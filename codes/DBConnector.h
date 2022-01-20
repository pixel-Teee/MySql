#pragma once
#include <string>
#include <functional>
#include <atomic>
#include <queue>

#include <mutex>//������
#include <thread>

#include "DBBuffer.h"
#include "ThreadSafeQueue.h"
#include "MysqlConnector.h"

namespace db
{
	typedef std::function<void(DBBuffer*)> Callback;

	//�������ݿ��һЩ�ṹ
	//---�������ļ������ȡ---
	struct S_DBXML
	{
		int port;//�˿ں�
		char ip[20];//IP
		char username[20];//���ݿ��˺�
		char userpass[20];//���ݿ�����
		char dbname[20];//���ݿ�����

		S_DBXML(){ memset(this, 0, sizeof(*this)); }
	};

	//�����߳�����
	enum E_THREAD_TYPE
	{
		ETT_USERREAD = 0x01,
		ETT_USERWRITE = 0x02,
		ETT_ACCOUNT = 0x03//�˺��̣߳�ר�Ŵ����˺ŵ�
	};

	//DB������
	class DBConnector
	{
	public:
		DBConnector(S_DBXML* cfg);
		~DBConnector();
	private:
		S_DBXML*		m_config;//�������ݿ�������ļ�
		MysqlConnector* mysql;//mysqlʵ������

		//�߳�ID����ǰ�����ĸ��߳�
		int m_ThreadID;
		Callback m_CallBack;//�ص�����
		Callback m_BeginBack;//��ʼ���ص�����

		std::mutex m_Mutex;//������
		std::queue<DBBuffer*> m_Queue;//���з��ʶ���
		std::queue<DBBuffer*> m_SwapQueue;//�������ݶ���
		std::condition_variable m_ConditionVar;//��������

		ThreadSafePool m_BuffsPool;//�ڴ���ճ�
		atomic<int> m_WorkCount;//��ǰ���������������е���������
	public:
		//�߳�ID�������ص�
		int StartRun(int id, Callback fun, Callback bfun = nullptr);
		void PushToWorkThread(DBBuffer* buffer);
		DBBuffer* PopBuffer();
		
		inline MysqlConnector* GetMysqlConnector(){ return mysql; }
		inline int GetThreadID(){ return m_ThreadID; }
		inline int GetWorkCount()const { return m_WorkCount;}

		//�����߳�
		void Run();
	};
}