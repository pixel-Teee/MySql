#include "DBManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include "UserData.h"

namespace db
{
	/*---��ȡ�˺���Ϣ---*/
	//��ʼ����ȡ
	void DBManager::Thread_BeginAccount()
	{
		auto start = std::chrono::steady_clock::now();

		auto mysql = DBAccount->GetMysqlConnector();
		stringstream sql;
		sql << "select * from user_account;";

		int error = mysql->ExecQuery(sql.str());
		if (error != 0)
		{
			printf("[thread:%d] beginAccount error:%d-%s line:%d\n", DBAccount->GetThreadID(), mysql->GetErrorCode(), mysql->GetErrorStr(), __LINE__);
			return;
		}
		
		int row = mysql->GetQueryRowNum();
		for (int i = 0; i < row; ++i)
		{
			int state = 0;
			app::S_USER_MEMBER_BASE* mem = new app::S_USER_MEMBER_BASE();

			mysql->r("id", mem->ID);
			mysql->r("state", state);
			mysql->r("username", mem->name);
			mysql->r("password", mem->password);
			mysql->r("createtime", mem->timeCreate);
			mysql->r("logintime", mem->timeLastLogin);

			mem->state = state;

			std::string key(mem->name);
			app::__AccountsName.insert(std::make_pair(key, mem));
			app::__AccountsID.insert(std::make_pair(mem->ID, mem));
			mysql->QueryNext();

			//printf("%d-%d %s-%s %d-%d\n", mem->ID, mem->state, mem->name, mem->password, mem->timeCreate, mem->timeLastLogin);
		}

		auto current = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
		printf("user_account size=%d time:%d ����\n", (int)app::__AccountsID.size(), (int)duration.count());
	}

	//���µ�¼ʱ��
	void updateLoginTime(DBBuffer* buff, DBConnector* db)
	{
		int memid;
		buff->r(memid);

		int currtime = time(NULL);

		stringstream sql;
		sql << "update user_account set logintime = " << 
		currtime << " where id = " << memid << ";";

		auto mysql = db->GetMysqlConnector();
		int ret = mysql->ExecQuery(sql.str());

		if (ret != 0)
		{
			printf("mysql 1000 :%s %d line:%d\n", mysql->GetErrorStr(), ret, __LINE__);
			return;
		}

		printf("mysql update logintime successfully...\n");

		//�ӹ����߳��������ݵ����߳�
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_1000);
		buf2->s(memid);
		buf2->e();
		__DBManager->PushToMainThread(buf2);
	}

	//ע���˺�
	void regAccount(DBBuffer* buff, DBConnector* db)
	{
		int len1;
		int len2;
		char account[100];
		char password[100];
		memset(account, 0, 100);
		memset(password, 0, 100);

		buff->r(len1);
		buff->r(account, len1);
		buff->r(len2);
		buff->r(password, len2);

		int createtime = time(NULL);
		stringstream sql;
		sql << "insert user_account(username, password, createtime, logintime) values("
		<< "'" << account << "','" << password << "'," << createtime << "," << createtime << ");";

		auto mysql = db->GetMysqlConnector();
		int ret = mysql->ExecQuery(sql.str());
		if (ret)
		{
			printf("mysql 2000 err:%s %d line:%d\n", mysql->GetErrorStr(), ret, __LINE__);
			return;
		}

		printf("mysql 2000 successfully...\n");

		/*----mysql�����ID----*/
		int memid = mysql->GetMysql()->insert_id;

		//�ӹ����߳��������ݵ����߳�
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_2000);
		buf2->s(memid);
		buf2->s(account, 100);
		buf2->s(password, 100);
		buf2->e();
		__DBManager->PushToMainThread(buf2);
	}
	
	//�˺����ݿ����ڹ����̵߳Ļص�����
	void DBManager::Thread_UserAccount(DBBuffer* buff)
	{
		uint16_t cmd;
		buff->init_r();
		buff->r(cmd);
		switch (cmd)
		{
		case CMD_1000://���µ�¼ʱ��
			updateLoginTime(buff, DBAccount);
			break;
		case CMD_2000://ע���¼
			regAccount(buff, DBAccount);
			break;
		}
	}
}