# Connection Pool 
 **使用到的技术点**
```
MySQL数据库编程、单例模式、queue队列容器、C++11多线程编程、线程互斥、线程同步通信和 unique_lock、
基于CAS的原子整形、智能指针shared_ptr、lambda表达式、生产者-消费者线程模型
```

 **其他**
```
--该项目在window平台下完成，采用的工具是VS2022
--该项目需要进行以下配置
--1.右键项目 - C/C++ - 常规 - 附加包含目录，填写mysql.h头文件的路径 
--2.右键项目 - 链接器 - 常规 - 附加库目录，填写libmysql.lib的路径 
--3.右键项目 - 链接器 - 输入 - 附加依赖项，填写libmysql.lib库的名字 
--4.把libmysql.dll动态链接库（Linux下后缀名是.so库）放在工程目录下
```
