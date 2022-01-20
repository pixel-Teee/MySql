#pragma once
#include <concurrent_queue.h>
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
		S_DBXML* __GameDBXML;//��Ϸ���ݿ�����
		S_DBXML* __AccountXML;//�˺����ݿ�����

		Concurrency::concurrent_queue<DBBuffer*> __logicBuffs;//��ȫ�Ĳ�������
		ThreadSafePool __poolBuffs;//buff�����ڴ�أ����߳����������յ�

		std::vector<DBConnector*> DBRead;//��Ҷ�ȡ���ݿ����
		std::vector<DBConnector*> DBWrite;//���д���ݿ����
		DBConnector* DBAccount;//����˺�ȫ�����ݿ����

		DBConnector* GetDBSource(int type);
		DBBuffer* PopPool();

		void InitDB();

		void Thread_BeginAccount();
		void Thread_UserRead(DBBuffer* buff);
		void Thread_UserWrite(DBBuffer* buff);
		void Thread_UserAccount(DBBuffer* buff);
	};

	extern DBManager* __DBManager;
}