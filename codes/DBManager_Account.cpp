#include "DBManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include "UserData.h"

namespace db
{
	/*---读取账号信息---*/
	//初始化读取
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
		printf("user_account size=%d time:%d 毫秒\n", (int)app::__AccountsID.size(), (int)duration.count());
	}

	//更新登录时间
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

		//从工作线程推送数据到主线程
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_1000);
		buf2->s(memid);
		buf2->e();
		__DBManager->PushToMainThread(buf2);
	}

	//注册账号
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

		/*----mysql插入的ID----*/
		int memid = mysql->GetMysql()->insert_id;

		//从工作线程推送数据到主线程
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_2000);
		buf2->s(memid);
		buf2->s(account, 100);
		buf2->s(password, 100);
		buf2->e();
		__DBManager->PushToMainThread(buf2);
	}
	
	//账号数据库所在工作线程的回调函数
	void DBManager::Thread_UserAccount(DBBuffer* buff)
	{
		uint16_t cmd;
		buff->init_r();
		buff->r(cmd);
		switch (cmd)
		{
		case CMD_1000://更新登录时间
			updateLoginTime(buff, DBAccount);
			break;
		case CMD_2000://注册登录
			regAccount(buff, DBAccount);
			break;
		}
	}
}