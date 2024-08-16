#define _CRT_SECURE_NO_WARNINGS
#include"CommonConnectionPool.h"
#include"public.h"


//�̰߳�ȫ����������ģʽ�ӿ�
ConnecttionPool* ConnecttionPool::getconnectionPool()
{
	 static ConnecttionPool pool;//static��̬�ֲ���������һ�����е����д���Ž��г�ʼ���������Զ��������ͷ�������֤���̰߳�ȫ
	 return &pool;
}


//�������ļ��м���������
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
		if (index == -1)//��Ч��������
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

//���ӳصĹ���
ConnecttionPool::ConnecttionPool()
{
	//����������
	if (!loadConfigFile())
	{
		return;
	}

	//������ʼ����������
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();//ˢ��һ�¿�ʼ���е���ʼʱ��
		_connectionQue.push(p);
		_connectionCnt++;
	}

	//����һ���̣߳���Ϊ���ӵ�������
	thread produce(std::bind(&ConnecttionPool::produceConnectionTask, this)); //�̺߳�������c�ӿڣ���Ҫ�������󶨳�Ա�����������Ա����������Ϊ�̺߳���
	produce.detach();//����Ϊ�����߳�

	//����һ���µĶ�ʱ�̣߳�ɨ�賬��_maxIdleTimeʱ��Ŀ������ӣ����ж���Ŀ������ӻ���
	thread scanner(std::bind(&ConnecttionPool::scannerConnectionTask, this));
	scanner.detach();//����Ϊ�����߳�

}

//�����ڶ������߳��У�ר�Ÿ�������������
void ConnecttionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<std::mutex>lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);//���в��գ��˴��������߳̽���ȴ�״̬��
		}

		//����û�дﵽ���ޣ������µ����ӣ����Ҷ���Ϊ��
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();//ˢ��һ�¿�ʼ���е���ʼʱ��
			_connectionQue.push(p);
			_connectionCnt++;
		}
		cv.notify_all();//֪ͨ�������߳̿��������� 
	}
}

//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
shared_ptr< Connection>ConnecttionPool::getconnection()
{
	unique_lock<std::mutex>lock(_queueMutex);
	while (_connectionQue.empty())
	{

		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut)))
		{
			//��ʱ�������ֶ���Ϊ��
			if (_connectionQue.empty())
			{
				LOG("��ȡ�������ӳ�ʱ��...��ȡ����ʧ��");
				return nullptr;
			}
		}
	}

	//��ȡһ�����õ�����
	//��Ҫ�Զ���shared_ptr����Դ�ͷŵķ�ʽ����Connection�黹��������
	shared_ptr<Connection>sp(_connectionQue.front(), [&](Connection* conf)
		{
			//�������ڷ�����Ӧ���߳��е��õģ�����һ��Ҫ���Ƕ��е��̰߳�ȫ
			unique_lock<std::mutex>lock(_queueMutex);
			conf->refreshAliveTime();//ˢ��һ�¿�ʼ���е���ʼʱ��
			_connectionQue.push(conf);
		});
	_connectionQue.pop();
	cv.notify_all();//�����������Ժ�֪ͨ�������̼߳��һ�£��������Ϊ�գ���֪ͨ����������
	return sp;

}


//ɨ�賬��_maxIdleTimeʱ��Ŀ������ӣ����ж���Ŀ������ӻ���
void ConnecttionPool::scannerConnectionTask()
{
	for (;;)
	{
		//ͨ��sleepģ�ⶨʱЧ��
		this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

		//ɨ���������У��ͷŶ��������
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
				break;//��ͷ������û�г���_maxIdleTime���������ӿ϶�û��
			}
		}
	}
}
