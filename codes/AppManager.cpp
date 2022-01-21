#include "AppManager.h"

#include "DBManager.h"
#include "AppLogin.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace app
{
	db::MysqlConnector* mysqlconnect = NULL;

	void initMysql()
	{
		//1.º”‘ÿmysqlø‚
		mysql_library_init(0, NULL, NULL);

		mysqlconnect = new db::MysqlConnector();

		bool isconnect = mysqlconnect->ConnectMysql("127.0.0.1", "root", "757602", "sakila", 3306);

		if (isconnect)
		{
			printf("mysql connect successfully...\n");
		}
		else
		{
			printf("mysql connect failed...\n");
		}

		std::stringstream sql;

		sql << "select * from sakila.actor;";

		int error = mysqlconnect->ExecQuery(sql.str());

		if (error != 0)
		{
			printf("mysql query err...%d\n", error);
			return;
		}

		int row = mysqlconnect->GetQueryRowNum();
		for (int i = 0; i < row; ++i)
		{
			
			printf("mysql row...%d\n", i);

			int id = 0;
			std::string first_name;
			mysqlconnect->r("id", id);
			mysqlconnect->r("first_name", first_name);
			
			mysqlconnect->QueryNext();
			printf("mysql data->%d->%s", id, first_name.c_str());
		}
		
	}
	/*-------------------------------------------------*/
	/*-------------------------------------------------*/
	/*-------------------------------------------------*/
	void update()
	{
		if(__DBManager == NULL) return;
		__DBManager->update();
	}

	void Init()
	{

	}

	int app::run()
	{
		__DBManager = new DBManager();
		__DBManager->InitDB();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		app::onLogin_1000();
		app::onRegister_2000();
		app::on_3000();
		while (true)
		{
			update();
			//100∫¡√Î
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		return 0;
	}
}
