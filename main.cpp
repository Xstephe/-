#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include"Connection.h"
#include"CommonConnectionPool.h"

int main()
{
	/*Connection conn;
	conn.connect("127.0.0.1", 3306, "root", "513513", "chat");*/

	//Connection conn;
	//char sql[1024] = { 0 };
	//sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//conn.connect("127.0.0.1", 3306, "root", "513513", "chat");
	//conn.update(sql);

	

	clock_t begin = clock();
	//ConnecttionPool* cp = ConnecttionPool::getconnectionPool();

	for (int i = 0; i < 1000; ++i)
	{
		/*Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
		conn.connect("127.0.0.1", 3306, "root", "513513", "chat");
		conn.update(sql);*/

		/*char sql[1024] = {0};
		shared_ptr<Connection>sp = cp->getconnection();
		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
		sp->update(sql);*/
	}

	//thread t1([]() {
	//	for (int i = 0; i < 250; ++i)
	//	{
	//		/*ConnecttionPool* cp = ConnecttionPool::getconnectionPool();
	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		shared_ptr<Connection>sp = cp->getconnection();
	//		sp->update(sql);*/

	//		Connection conn;
	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		conn.connect("127.0.0.1", 3306, "root", "513513", "chat");
	//		conn.update(sql);
	//	}
	//	});
	//thread t2([]() {
	//	for (int i = 0; i < 250; ++i)
	//	{
	//		/*ConnecttionPool* cp = ConnecttionPool::getconnectionPool();
	//		cchar sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		shared_ptr<Connection>sp = cp->getconnection();
	//		sp->update(sql);*/

	//		Connection con;
	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into account (name,money) values ('%s',%d)", "123", 1000);
	//		con.connect("127.0.0.1", 3306, "root", "513513", "itheima");
	//		con.update(sql);
	//	}
	//	});
	//thread t3([]() {
	//	for (int i = 0; i < 250; ++i)
	//	{
	//		/*ConnecttionPool* cp = ConnecttionPool::getconnectionPool();
	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		shared_ptr<Connection>sp = cp->getconnection();
	//		sp->update(sql);*/

	//		Connection conn;
	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		conn.connect("127.0.0.1", 3306, "root", "513513", "chat");
	//		conn.update(sql);
	//	}
	//	});
	//thread t4([]() {
	//	for (int i = 0; i < 250; ++i)
	//	{
	//		/*ConnecttionPool* cp = ConnecttionPool::getconnectionPool();

	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		shared_ptr<Connection>sp = cp->getconnection();*/
	//		//sp->update(sql);


	//		Connection conn;
	//		char sql[1024] = { 0 };
	//		sprintf(sql, "insert into user (name,age,sex) values ('%s',%d,'%s')", "xie zi ran ", 22, "male");
	//		conn.connect("127.0.0.1", 3306, "root", "513513", "chat");
	//		conn.update(sql);
	//		
	//	}
	//	});
	//t1.join();
	//t2.join();
	//t3.join();
	//t4.join();
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	return  0;
}


/*

������ δʹ�����ӳػ���ʱ��               ʹ�����ӳػ���ʱ��
1000   ���̣߳�9065ms   ���̣߳�2060ms     ���̣߳�2192ms  ���̣߳�775ms
5000   ���̣߳�48159ms  ���̣߳�9159ms    ���̣߳�10624ms ���̣߳�3426ms
10000  ���̣߳�83067ms  ���̣߳�20198ms    ���̣߳�21113ms ���̣߳�6328ms

*/


	
