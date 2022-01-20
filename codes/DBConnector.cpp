#include "DBConnector.h"

namespace db
{
	
	DBConnector::DBConnector(S_DBXML* cfg)
	{
		mysql = nullptr;
		m_config = cfg;
	}

	DBConnector::~DBConnector()
	{

	}

	//��ʼ��ʼ�������и���ģ��
	int DBConnector::StartRun(int id, Callback fun, Callback bfun)
	{
		if(mysql != nullptr) return -1;
		//0.ʵ����mysql������
		mysql = new MysqlConnector();
		//1.ʵ����redis������
		 
		//2.��ʼ������
		m_ThreadID = id;
		m_CallBack = fun;
		m_BeginBack = bfun;
		//�������ݣ�1024��buffer��ÿ��1K
		m_BuffsPool.Init(500, 1024 * 32);

		//3.����mysql
		bool isok = mysql->ConnectMysql(m_config->ip, m_config->username, m_config->userpass, m_config->dbname,  m_config->port);
		if (isok)
		{
			printf("mysql connect successfully...\n");
		}
		else
		{
			printf("mysql connect failed...\n");
		}

		//4.�����߳�
		std::thread th(&DBConnector::Run, this);
		th.detach();

		return 0;
	}

	//���߳��������ݵ������߳�
	//���߳��ж�д��ѯ�����ʱ����Ҫ�������ݵ������߳�
	void DBConnector::PushToWorkThread(DBBuffer* buff)
	{
		if(buff == nullptr) return;

		std::unique_lock<std::mutex> lock(m_Mutex);

		bool isnotify = m_Queue.empty();
		m_Queue.push(buff);
		lock.unlock();

		++m_WorkCount;
		if(isnotify) m_ConditionVar.notify_one();
	}

	DBBuffer* DBConnector::PopBuffer()
	{
		auto buff = m_BuffsPool.Pop();
		buff->Clear();
		buff->SetDB(this);
		return buff;
	}

	//�����߳�
	void DBConnector::Run()
	{
		printf("run thread...%d\n", m_ThreadID);
		if(m_BeginBack != nullptr) m_BeginBack(nullptr);

		while (true)
		{
			/*---����---�������̵߳��õ����������*/
			std::unique_lock<std::mutex> lock(m_Mutex);
			while (m_Queue.empty())
			{
				/*---��������---*/
				/*---����---*/
				m_ConditionVar.wait(lock);
			}
			m_SwapQueue.swap(m_Queue);
			/*---����---*/
			lock.unlock();

			//������д��з�������
			while (!m_SwapQueue.empty())
			{
				DBBuffer* buff = m_SwapQueue.front();//���ʵ�һ��Ԫ��
				m_SwapQueue.pop();//����������Ҫ�ͷţ��Ż�ȥ����

				/*ʹ�ûص���������*/
				m_CallBack(buff);
				--m_WorkCount;
				m_BuffsPool.Push(buff);
			}
		}

		printf("exit thread...%d\n", m_ThreadID);
	}

}