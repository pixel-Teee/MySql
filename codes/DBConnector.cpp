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

	//开始初始化，运行各个模板
	int DBConnector::StartRun(int id, Callback fun, Callback bfun)
	{
		if(mysql != nullptr) return -1;
		//0.实例化mysql连接器
		mysql = new MysqlConnector();
		//1.实例化redis连接器
		 
		//2.初始化数据
		m_ThreadID = id;
		m_CallBack = fun;
		m_BeginBack = bfun;
		//开辟数据，1024个buffer，每个1K
		m_BuffsPool.Init(500, 1024 * 32);

		//3.连接mysql
		bool isok = mysql->ConnectMysql(m_config->ip, m_config->username, m_config->userpass, m_config->dbname,  m_config->port);
		if (isok)
		{
			printf("mysql connect successfully...\n");
		}
		else
		{
			printf("mysql connect failed...\n");
		}

		//4.开启线程
		std::thread th(&DBConnector::Run, this);
		th.detach();

		return 0;
	}

	//主线程推送数据到工作线程
	//主线程有读写查询请求的时候，需要推送数据到工作线程
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

	//工作线程
	void DBConnector::Run()
	{
		printf("run thread...%d\n", m_ThreadID);
		if(m_BeginBack != nullptr) m_BeginBack(nullptr);

		while (true)
		{
			/*---上锁---，其它线程调用到这里，会阻塞*/
			std::unique_lock<std::mutex> lock(m_Mutex);
			while (m_Queue.empty())
			{
				/*---条件变量---*/
				/*---休眠---*/
				m_ConditionVar.wait(lock);
			}
			m_SwapQueue.swap(m_Queue);
			/*---解锁---*/
			lock.unlock();

			//处理队列串行访问数据
			while (!m_SwapQueue.empty())
			{
				DBBuffer* buff = m_SwapQueue.front();//访问第一个元素
				m_SwapQueue.pop();//弹出来，不要释放，放回去即可

				/*使用回调处理数据*/
				m_CallBack(buff);
				--m_WorkCount;
				m_BuffsPool.Push(buff);
			}
		}

		printf("exit thread...%d\n", m_ThreadID);
	}

}