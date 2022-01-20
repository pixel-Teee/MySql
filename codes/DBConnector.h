#pragma once
#include <string>
#include <functional>
#include <atomic>
#include <queue>

#include <mutex>//互斥锁
#include <thread>

#include "DBBuffer.h"
#include "ThreadSafeQueue.h"
#include "MysqlConnector.h"

namespace db
{
	typedef std::function<void(DBBuffer*)> Callback;

	//访问数据库的一些结构
	//---从配置文件里面读取---
	struct S_DBXML
	{
		int port;//端口号
		char ip[20];//IP
		char username[20];//数据库账号
		char userpass[20];//数据库密码
		char dbname[20];//数据库名字

		S_DBXML(){ memset(this, 0, sizeof(*this)); }
	};

	//开启线程类型
	enum E_THREAD_TYPE
	{
		ETT_USERREAD = 0x01,
		ETT_USERWRITE = 0x02,
		ETT_ACCOUNT = 0x03//账号线程，专门处理账号的
	};

	//DB连接器
	class DBConnector
	{
	public:
		DBConnector(S_DBXML* cfg);
		~DBConnector();
	private:
		S_DBXML*		m_config;//连接数据库的配置文件
		MysqlConnector* mysql;//mysql实例对象

		//线程ID，当前属于哪个线程
		int m_ThreadID;
		Callback m_CallBack;//回调函数
		Callback m_BeginBack;//初始化回调函数

		std::mutex m_Mutex;//互斥锁
		std::queue<DBBuffer*> m_Queue;//串行访问队列
		std::queue<DBBuffer*> m_SwapQueue;//交换数据队列
		std::condition_variable m_ConditionVar;//条件变量

		ThreadSafePool m_BuffsPool;//内存回收池
		atomic<int> m_WorkCount;//当前工作数量，队列中的数据数量
	public:
		//线程ID，两个回调
		int StartRun(int id, Callback fun, Callback bfun = nullptr);
		void PushToWorkThread(DBBuffer* buffer);
		DBBuffer* PopBuffer();
		
		inline MysqlConnector* GetMysqlConnector(){ return mysql; }
		inline int GetThreadID(){ return m_ThreadID; }
		inline int GetWorkCount()const { return m_WorkCount;}

		//运行线程
		void Run();
	};
}