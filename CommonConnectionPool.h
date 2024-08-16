#pragma once
//ʵ�����ӳع���ģ��

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
	//��ȡ���ӳض���ʵ��
	static ConnecttionPool* getconnectionPool();

	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr< Connection>getconnection();
	 
private:
	ConnecttionPool();//���������캯��˽�л�


	bool loadConfigFile();//�������ļ��м���������

	//�����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	//����һ���µĶ�ʱ�̣߳�ɨ�賬��_maxIdleTimeʱ��Ŀ������ӣ����ж���Ŀ������ӻ���
	void scannerConnectionTask();

	string _ip;//mysql��ip��ַ
	unsigned short _port;//mysql��½�Ķ˿ں�3306
	string _username;//mysql��½���û���
	string _password;//mysql��¼����
	string _dbname;//���ӵ����ݿ�����
	int _initSize;//���ӳصĳ�ʼ������
	int _maxSize;//���ӳص����������
	int _maxIdleTime;//���ӳ�������ʱ��
	int _connectionTimeOut;//���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection*>_connectionQue;//�洢mysql���ӵĶ���
	mutex _queueMutex;//ά�����Ӷ��е��̰߳�ȫ������
	atomic_int _connectionCnt;//��¼������������connnection���ӵ�������

	condition_variable cv;//���������������������������̺߳������̵߳�ͨ��

};