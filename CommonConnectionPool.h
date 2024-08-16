#pragma once
//实现连接池功能模块

#include<iostream>
#include<string>
#include<queue>
#include"Connection.h"
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
using namespace std;


class ConnecttionPool
{
public:
	//获取连接池对象实例
	static ConnecttionPool* getconnectionPool();

	//给外部提供接口，从连接池中获取一个可用的空闲连接
	shared_ptr< Connection>getconnection();
	 
private:
	ConnecttionPool();//单例，构造函数私有化


	bool loadConfigFile();//从配置文件中加载配置项

	//运行在独立的线程中，专门负责生产新连接
	void produceConnectionTask();

	//启动一个新的定时线程，扫描超过_maxIdleTime时间的空闲连接，进行多余的空闲连接回收
	void scannerConnectionTask();

	string _ip;//mysql的ip地址
	unsigned short _port;//mysql登陆的端口号3306
	string _username;//mysql登陆的用户名
	string _password;//mysql登录密码
	string _dbname;//连接的数据库名称
	int _initSize;//连接池的初始连接量
	int _maxSize;//连接池的最大连接量
	int _maxIdleTime;//连接池最大空余时间
	int _connectionTimeOut;//连接池获取连接的超时时间

	queue<Connection*>_connectionQue;//存储mysql连接的队列
	mutex _queueMutex;//维护连接队列的线程安全队列锁
	atomic_int _connectionCnt;//记录连接所创建的connnection连接的总数量

	condition_variable cv;//设置条件变量，用于连接生产线程和消费线程的通信

};