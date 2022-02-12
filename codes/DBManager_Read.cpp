#include "DBManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "UserData.h"

#include <chrono>
using namespace chrono;

namespace db
{
	//读请求
	void do_3000(DBBuffer* buff, DBConnector* db)
	{
		int redis_state = 0;
		int memid = 0;
		buff->r(memid);
		buff->r(redis_state);

		auto start = std::chrono::steady_clock::now();
		//一、查询账号 redis状态
		if (redis_state == 0)
		{
			//1、定义查询字段
			std::string sss = "HMGET user_data:" + std::to_string(memid) + " memid curhp maxhp speed nick pos_x pos_y pos_z rot_x rot_z";
			auto redis = db->GetRedisConnector();
			int err = redis->RedisCommand(sss.c_str());
			//没有错误
			if (err == 0)
			{
				int length = 0;
				redis->value(0, length);

				//查询成功
				if (length > 0)
				{
					app::S_USER_BASE user;
					user.memid = atoi(redis->value(0, length));
					user.curhp = atoi(redis->value(1, length));
					user.maxhp = atoi(redis->value(2, length));
					user.speed = atoi(redis->value(3, length));
					std::string nick = redis->value(4, length);

					if (length > 0)
					{
						memset(user.nick, 0, 20);
						memcpy(user.nick, nick.c_str(), 20);
					}
					user.pos_x = atoi(redis->value(5, length));
					user.pos_y = atoi(redis->value(6, length));
					user.pos_z = atoi(redis->value(7, length));
					user.rot_x = atoi(redis->value(8, length));
					user.rot_y = atoi(redis->value(9, length));
					user.rot_z = atoi(redis->value(10, length));
					redis->Clear();

					auto current = std::chrono::steady_clock::now();
					auto duration = duration_cast<milliseconds>(current - start);

					printf("redis 3000 read %d-%d-%d nick:%s time:%d 毫秒\n", user.memid, user.curhp, user.maxhp, user.nick, (int)duration.count());

					//从工作线程推送数据到主线程
					auto buf2 = __DBManager->PopPool();
					buf2->b(CMD_3000);
					buf2->s(0);
					buf2->s(&user, sizeof(user));
					buf2->e();
					__DBManager->PushToMainThread(buf2);
					return;
				}
			}
			//2、redis查询失败
			printf("redis 3000 find failed...find mysql...\n");

		}
		else
		{
			//1、删除redis中的数据
			std::string sss = "del user_data:" + std::to_string(memid);
			auto redis = db->GetRedisConnector();
			int err = redis->RedisCommand(sss.c_str());

			printf("redis 3000 delete key...%d\n", memid);
		}

		//二、查询mysql
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
			//返回一个错误码
			buf2->s(2);
			buf2->e();
			__DBManager->PushToMainThread(buf2);
			return;
		}

		//定义玩家数据结构
		app::S_USER_BASE user;
		memset(user.nick, 0, 20);
		mysql->r("memid", user.memid);
		mysql->r("curhp", user.curhp);
		mysql->r("maxhp", user.maxhp);
		mysql->r("speed", user.speed);
		mysql->r("pos_x", user.pos_x);
		mysql->r("pos_y", user.pos_y);
		mysql->r("pos_z", user.pos_z);
		mysql->r("rot_x", user.rot_x);
		mysql->r("rot_y", user.rot_y);
		mysql->r("rot_z", user.rot_z);
		mysql->r("nick", user.nick);
		user.logintime = (int)time(NULL);

		mysql->CloseQuery();

		//三、写入redis数据
		std::string sss = "HMSET user_data:" + std::to_string(memid) + 
							" memid " + std::to_string(memid) + 
							" curhp " + std::to_string(user.curhp) + 
							" maxhp " + std::to_string(user.speed) + 
							" nick " + user.nick + 
							" pos_x " + std::to_string(user.pos_x) + 
							" pos_y " + std::to_string(user.pos_y) + 
							" pos_z " + std::to_string(user.pos_z) +
							" rot_x " + std::to_string(user.rot_x) + 
							" rot_y " + std::to_string(user.rot_y) + 
							" rot_z " + std::to_string(user.rot_z) + 
							" logintime " + std::to_string(user.logintime);

		//printf("mysql 3000 read %d-%d-%d-%d nick:%s\n", id, memid, curhp, maxhp, nick);
		
		auto redis = db->GetRedisConnector();
		int err1 = redis->RedisCommand(sss.c_str());
		std::string sss_expire = "expire user_data:" + std::to_string(memid) + 
		" 100";
		int err2 = redis->RedisCommand(sss_expire.c_str());

		auto current = std::chrono::steady_clock::now();
		auto duration = duration_cast<milliseconds>(current - start);
		printf("mysql 3000 read %d-%d-%d nick:%s time:%d 毫秒 redis:%d-%d\n", user.memid, user.curhp, user.maxhp, user.nick, (int)duration.count(), err1, err2);
		//从工作线程推送数据到主线程
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_3000);
		buf2->s(0);
		buf2->s(&user, sizeof(user));
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