#include "DBManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "UserData.h"

namespace db
{
	//读请求
	void do_3000(DBBuffer* buff, DBConnector* db)
	{
		int memid = 0;
		buff->r(memid);

		auto mysql = db->GetMysqlConnector();
		std:stringstream sql;
		sql << "select * from user_data where memid = " << memid << ";";

		int err = mysql->ExecQuery(sql.str());
		if (err != 0)
		{
			printf("mysql 3000 query err...%d\n", err);
			/*报告错误的包*/
			auto buf2 = __DBManager->PopPool();
			buf2->b(CMD_3000);
			buf2->s(1);
			buf2->e();
			__DBManager->PushToMainThread(buf2);
			return;
		}
		int row = mysql->GetQueryRowNum();
		if (row != 1)
		{
			printf("mysql 3000 row != 1...%d\n", row);
			auto buf2 = __DBManager->PopPool();
			buf2->b(CMD_3000);
			buf2->s(2);
			buf2->e();
			__DBManager->PushToMainThread(buf2);
			return;
		}

		int id = 0;
		int curhp = 0;
		int maxhp = 0;
		char nick[20];
		memset(nick, 0, 20);
		mysql->r("id", id);
		mysql->r("curhp", curhp);
		mysql->r("maxhp", maxhp);
		mysql->r("nick", nick);
		mysql->CloseQuery();
		printf("mysql 3000 read %d-%d-%d-%d nick:%s\n", id, memid, curhp, maxhp, nick);
		
		//从工作线程推送到主线程
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_3000);
		buf2->s(0);
		buf2->s(id);
		buf2->s(memid);
		buf2->s(curhp);
		buf2->s(maxhp);
		buf2->s(nick, 20);
		buf2->e();
		__DBManager->PushToMainThread(buf2);
	}
	//工作线程里面读
	void DBManager::Thread_UserRead(DBBuffer* buff)
	{
		auto db = (DBConnector*)buff->GetDB();
		if (db == nullptr)
		{
			printf("Thread_UserRead is error:\n");
			return;
		}

		uint16_t cmd;

		buff->init_r();
		buff->r(cmd);

		switch (cmd)
		{
		case CMD_3000:
			do_3000(buff, db);
			break;
		}
	}


}