#include "DBManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "UserData.h"

namespace db
{
	/*---读取账号信息---*/
	//初始化读取
	void DBManager::Thread_BeginAccount()
	{
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

			printf("%d-%d %s-%s %d-%d\n", mem->ID, mem->state, mem->name, mem->password, mem->timeCreate, mem->timeLastLogin);
		}
	}
	
	void DBManager::Thread_UserAccount(DBBuffer* buff)
	{

	}
}