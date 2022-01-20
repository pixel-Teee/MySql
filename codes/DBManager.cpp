#include "DBManager.h"

namespace db
{

	DBManager::DBManager()
	{

	}

	DBManager::~DBManager()
	{

	}

	//获取工作线程工作量最小的线程实例
	DBConnector* DBManager::GetDBSource(int type)
	{
		if (type == ETT_USERREAD)
		{
			auto it = DBRead.begin();
			db::DBConnector* db = *it;
			int value = db->GetWorkCount();
			if(value == 0) return db;

			//找出最小的工作数量的工作线程
			for (; it != DBRead.end(); ++it)
			{
				db::DBConnector* pdb = *it;
				if (pdb->GetWorkCount() < db->GetWorkCount())
				{
					db = pdb;
				}
			}
			return db;
		}

		if (type == ETT_USERWRITE)
		{
			auto it = DBWrite.begin();
			db::DBConnector* db = *it;
			int value = db->GetWorkCount();
			if (value == 0) return db;

			//找出最小的工作数量的工作线程
			for (; it != DBWrite.end(); ++it)
			{
				db::DBConnector* pdb = *it;
				if (pdb->GetWorkCount() < db->GetWorkCount())
				{
					db = pdb;
				}
			}
			return db;
		}

		return nullptr;
	}

	//获取回收对象dbbuffer
	DBBuffer* DBManager::PopPool()
	{
		/*主线程的buffer池*/
		auto buff = __poolBuffs.Pop();
		buff->Clear();
		buff->SetDB(nullptr);
		return buff;
	}

	//初始化
	void DBManager::InitDB()
	{
		//初始化回收池
		__poolBuffs.Init(1000, 1024 * 32);
		//初始化xml
		__GameDBXML = new S_DBXML();

		sprintf(__GameDBXML->ip, "127.0.0.1");
		sprintf(__GameDBXML->username, "root");
		sprintf(__GameDBXML->userpass, "757602");
		sprintf(__GameDBXML->dbname, "mygamedb");
		__GameDBXML->port = 3306;

		//初始化账号数据库配置
		__AccountXML = new S_DBXML();

		sprintf(__AccountXML->ip, "127.0.0.1");
		sprintf(__AccountXML->username, "root");
		sprintf(__AccountXML->userpass, "757602");
		sprintf(__AccountXML->dbname, "mygamedb");
		__AccountXML->port = 3306;

		/*生成新的可调用对象*/
		auto readfun = std::bind(&DBManager::Thread_UserRead, this, std::placeholders::_1);

		auto writefun = std::bind(&DBManager::Thread_UserWrite, this, std::placeholders::_1);

		auto accountfun = std::bind(&DBManager::Thread_UserWrite, this, std::placeholders::_1);

		/*线程ID*/
		int r_id = 1000;
		int w_id = 2000;

		DBRead.reserve(3);
		DBWrite.reserve(3);

		//读工作线程
		for (int i = 0; i < 3; ++i)
		{
			auto d = new db::DBConnector(__GameDBXML);
			DBRead.emplace_back(d);
			//运行线程
			d->StartRun(r_id++, readfun, nullptr);
		}

		//写工作线程
		for (int i = 0; i < 3; ++i)
		{
			auto d = new db::DBConnector(__GameDBXML);
			DBWrite.emplace_back(d);
			//运行线程
			d->StartRun(w_id++, writefun, nullptr);
		}

		//账号数据库线程
		DBAccount = new db::DBConnector(__AccountXML);
		DBAccount->StartRun(3000, accountfun, nullptr);
	}

}