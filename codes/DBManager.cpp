#include "DBManager.h"

namespace db
{

	DBManager::DBManager()
	{

	}

	DBManager::~DBManager()
	{

	}

	//��ȡ�����̹߳�������С���߳�ʵ��
	DBConnector* DBManager::GetDBSource(int type)
	{
		if (type == ETT_USERREAD)
		{
			auto it = DBRead.begin();
			db::DBConnector* db = *it;
			int value = db->GetWorkCount();
			if(value == 0) return db;

			//�ҳ���С�Ĺ��������Ĺ����߳�
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

			//�ҳ���С�Ĺ��������Ĺ����߳�
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

	//��ȡ���ն���dbbuffer
	DBBuffer* DBManager::PopPool()
	{
		/*���̵߳�buffer��*/
		auto buff = __poolBuffs.Pop();
		buff->Clear();
		buff->SetDB(nullptr);
		return buff;
	}

	//��ʼ��
	void DBManager::InitDB()
	{
		//��ʼ�����ճ�
		__poolBuffs.Init(1000, 1024 * 32);
		//��ʼ��xml
		__GameDBXML = new S_DBXML();

		sprintf(__GameDBXML->ip, "127.0.0.1");
		sprintf(__GameDBXML->username, "root");
		sprintf(__GameDBXML->userpass, "757602");
		sprintf(__GameDBXML->dbname, "mygamedb");
		__GameDBXML->port = 3306;

		//��ʼ���˺����ݿ�����
		__AccountXML = new S_DBXML();

		sprintf(__AccountXML->ip, "127.0.0.1");
		sprintf(__AccountXML->username, "root");
		sprintf(__AccountXML->userpass, "757602");
		sprintf(__AccountXML->dbname, "mygamedb");
		__AccountXML->port = 3306;

		/*�����µĿɵ��ö���*/
		auto readfun = std::bind(&DBManager::Thread_UserRead, this, std::placeholders::_1);

		auto writefun = std::bind(&DBManager::Thread_UserWrite, this, std::placeholders::_1);

		auto accountfun = std::bind(&DBManager::Thread_UserWrite, this, std::placeholders::_1);

		/*�߳�ID*/
		int r_id = 1000;
		int w_id = 2000;

		DBRead.reserve(3);
		DBWrite.reserve(3);

		//�������߳�
		for (int i = 0; i < 3; ++i)
		{
			auto d = new db::DBConnector(__GameDBXML);
			DBRead.emplace_back(d);
			//�����߳�
			d->StartRun(r_id++, readfun, nullptr);
		}

		//д�����߳�
		for (int i = 0; i < 3; ++i)
		{
			auto d = new db::DBConnector(__GameDBXML);
			DBWrite.emplace_back(d);
			//�����߳�
			d->StartRun(w_id++, writefun, nullptr);
		}

		//�˺����ݿ��߳�
		DBAccount = new db::DBConnector(__AccountXML);
		DBAccount->StartRun(3000, accountfun, nullptr);
	}

}