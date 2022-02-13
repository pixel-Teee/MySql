#include "DBManager.h"
#include "AppLogin.h"

extern void InitRedisDll();

namespace db
{
	DBManager* __DBManager = nullptr;

	DBManager::DBManager()
	{
		DBAccount = nullptr;
		__GameDBXML = nullptr;
		__AccountXML = nullptr;
	}

	DBManager::~DBManager()
	{
		if (__GameDBXML != nullptr) delete __GameDBXML;
		if (__AccountXML != nullptr) delete __AccountXML;
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

	//��ʼ��
	void DBManager::InitDB()
	{
		//����mysql��
		mysql_library_init(0, NULL, NULL);
		//����redis��
		InitRedisDll();

		//��ʼ�����ճ�
		__poolBuffs.Init(1000, 1024 * 32);
		//��ʼ��xml
		__GameDBXML = new S_DBXML();

#ifdef ____WIN32_
		sprintf(__GameDBXML->ip, "127.0.0.1");
		sprintf(__GameDBXML->username, "root");
		sprintf(__GameDBXML->userpass, "757602");
		sprintf(__GameDBXML->dbname, "sakila");
		__GameDBXML->port = 3306;
#else
		sprintf(__GameDBXML->ip, "192.168.1.102");
		sprintf(__GameDBXML->username, "root");
		sprintf(__GameDBXML->userpass, "757602");
		sprintf(__GameDBXML->dbname, "myaccount");
		__GameDBXML->port = 3306;
#endif
		//��ʼ���˺����ݿ�����
		__AccountXML = new S_DBXML();

#ifdef ____WIN32_
		sprintf(__AccountXML->ip, "127.0.0.1");
		sprintf(__AccountXML->username, "root");
		sprintf(__AccountXML->userpass, "757602");
		sprintf(__AccountXML->dbname, "sakila");
		__AccountXML->port = 3306;
#else
		sprintf(__AccountXML->ip, "192.168.1.102");
		sprintf(__AccountXML->username, "root");
		sprintf(__AccountXML->userpass, "757602");
		sprintf(__AccountXML->dbname, "myaccount");
		__AccountXML->port = 3306;
#endif
		/*�����µĿɵ��ö���*/
		auto readfun = std::bind(&DBManager::Thread_UserRead, this, std::placeholders::_1);

		auto writefun = std::bind(&DBManager::Thread_UserWrite, this, std::placeholders::_1);

		auto accountfun = std::bind(&DBManager::Thread_UserAccount, this, std::placeholders::_1);

		auto begin_account = std::bind(&DBManager::Thread_BeginAccount, this);

		int count = 1;
		/*�߳�ID*/
		int r_id = 1000;
		int w_id = 2000;

		DBRead.reserve(3);
		DBWrite.reserve(3);

		//�������߳�
		for (int i = 0; i < count; ++i)
		{
			auto d = new db::DBConnector(__GameDBXML);
			DBRead.emplace_back(d);
			//�����߳�
			d->StartRun(r_id++, readfun, nullptr);
		}

		//д�����߳�
		for (int i = 0; i < count; ++i)
		{
			auto d = new db::DBConnector(__GameDBXML);
			DBWrite.emplace_back(d);
			//�����߳�
			d->StartRun(w_id++, writefun, nullptr);
		}

		//�˺����ݿ��߳�
		DBAccount = new db::DBConnector(__AccountXML);
		DBAccount->StartRun(3000, accountfun, begin_account);

		
	}

	//���»�ȡ�����߳����ͳ���������
	void DBManager::update()
	{
		while (!__logicBuffs.empty())
		{
			DBBuffer* buff = nullptr;
			__logicBuffs.try_pop(buff);
			if(buff == nullptr)break;
			
			//���ø��߼���Ԫ
			app::OnDBCommand(buff);

			//ʹ����ϣ�������ճ�
			__poolBuffs.Push(buff);
		}
	}

	//�ṩ�������ݵ����߳�
	void DBManager::PushToMainThread(DBBuffer* buffer)
	{
		if(buffer == nullptr) return;
		__logicBuffs.push(buffer);
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
}

