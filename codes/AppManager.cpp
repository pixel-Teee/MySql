#include "AppManager.h"

#include "MysqlConnector.h"
#include <thread>

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

	int app::run()
	{
		initMysql();
		while (true)
		{
			//100∫¡√Î
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		return 0;
	}
}
