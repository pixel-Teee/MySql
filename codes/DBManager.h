#pragma once
#ifdef ____WIN32_
#include <concurrent_queue.h>
using namespace Concurrency;
#else
#include <tbb/tbb.h>
using namespace tbb;
#endif

#include "DBConnector.h"
#include "DBBuffer.h"

namespace db
{
	class DBManager
	{
	public:
		DBManager();
		~DBManager();

	public:
		S_DBXML* __GameDBXML;//游戏数据库配置
		S_DBXML* __AccountXML;//账号数据库配置

		concurrent_queue<DBBuffer*> __logicBuffs;//安全的并发队列
		ThreadSafePool __poolBuffs;//buff回收内存池，主线程中用来回收的

		std::vector<DBConnector*> DBRead;//玩家读取数据库操作
		std::vector<DBConnector*> DBWrite;//玩家写数据库操作
		DBConnector* DBAccount;//玩家账号全局数据库操作

		DBConnector* GetDBSource(int type, int tid = 0);
		DBBuffer* PopPool();

		void InitDB();
		void update();
		void PushToMainThread(DBBuffer* buffer);

		void Thread_BeginAccount();
		void Thread_UserRead(DBBuffer* buff);
		void Thread_UserWrite(DBBuffer* buff);
		void Thread_UserAccount(DBBuffer* buff);
	};

	extern DBManager* __DBManager;
}