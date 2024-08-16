#define _CRT_SECURE_NO_WARNINGS
#include"CommonConnectionPool.h"
#include"public.h"


//线程安全的懒汉单例模式接口
ConnecttionPool* ConnecttionPool::getconnectionPool()
{
	 static ConnecttionPool pool;//static静态局部变量，第一次运行到这行代码才进行初始化，并且自动加锁和释放锁，保证了线程安全
	 return &pool;
}


//从配置文件中加载配置项
bool ConnecttionPool::loadConfigFile()
{
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}

	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int index = str.find("=", 0);
		if (index == -1)//无效的配置项
		{
			continue;
		}
		int endindex = str.find("\n", index);
		string key = str.substr(0, index);
		string value = str.substr(index + 1, endindex - index - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize= atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeOut = atoi(value.c_str());
		}

	}
	return true;
}

//连接池的构造
ConnecttionPool::ConnecttionPool()
{
	//加载配置项
	if (!loadConfigFile())
	{
		return;
	}

	//创建初始数量的连接
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();//刷新一下开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}

	//启动一个线程，作为连接的生产者
	thread produce(std::bind(&ConnecttionPool::produceConnectionTask, this)); //线程函数都是c接口，需要绑定器，绑定成员方法，否则成员方法不能作为线程函数
	produce.detach();//设置为分离线程

	//启动一个新的定时线程，扫描超过_maxIdleTime时间的空闲连接，进行多余的空闲连接回收
	thread scanner(std::bind(&ConnecttionPool::scannerConnectionTask, this));
	scanner.detach();//设置为分离线程

}

//运行在独立的线程中，专门负责生产新连接
void ConnecttionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<std::mutex>lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);//队列不空，此处生产者线程进入等待状态，
		}

		//连接没有达到上限，创建新的连接，并且队列为空
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();//刷新一下开始空闲的起始时间
			_connectionQue.push(p);
			_connectionCnt++;
		}
		cv.notify_all();//通知消费者线程可以消费了 
	}
}

//给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr< Connection>ConnecttionPool::getconnection()
{
	unique_lock<std::mutex>lock(_queueMutex);
	while (_connectionQue.empty())
	{

		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut)))
		{
			//超时醒来发现队列为空
			if (_connectionQue.empty())
			{
				LOG("获取空闲连接超时了...获取连接失败");
				return nullptr;
			}
		}
	}

	//获取一个可用的连接
	//需要自定义shared_ptr的资源释放的方式，把Connection归还到队列中
	shared_ptr<Connection>sp(_connectionQue.front(), [&](Connection* conf)
		{
			//这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全
			unique_lock<std::mutex>lock(_queueMutex);
			conf->refreshAliveTime();//刷新一下开始空闲的起始时间
			_connectionQue.push(conf);
		});
	_connectionQue.pop();
	cv.notify_all();//消费完连接以后，通知生产者线程检查一下，如果队列为空，则通知生产者生产
	return sp;

}


//扫描超过_maxIdleTime时间的空闲连接，进行多余的空闲连接回收
void ConnecttionPool::scannerConnectionTask()
{
	for (;;)
	{
		//通过sleep模拟定时效果
		this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

		//扫描整个队列，释放多余的连接
		unique_lock<std::mutex>lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() > (_maxIdleTime*1000))
			{
				_connectionQue.pop(); 
				_connectionCnt--;
				delete p;
			}
			else {
				break;//队头的连接没有超过_maxIdleTime，其他连接肯定没有
			}
		}
	}
}
