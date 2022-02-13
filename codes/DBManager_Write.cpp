#include "DBManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "UserData.h"

namespace db
{
	//д���� ��������
	void do_7000(DBBuffer* buff, DBConnector* db)
	{
		int kind = 0;//0 ���� 1 ����;����
		auto mysql = db->GetMysqlConnector();
		auto redis = db->GetRedisConnector();

		//��������
		app::S_USER_BASE user;
		buff->r(kind);
		buff->r(&user, sizeof(app::S_USER_BASE));

		//1����ɾ�� redis
		std::string sss = "del user_data:" + std::to_string(user.memid);
		int err = redis->RedisCommand(sss.c_str());
		printf("redis 7000 delete key...%d %d\n", user.memid, err);
	
		//2��д��mysql
		int curtime = (int)time(NULL);
		std::stringstream sql;
		sql << "update user_data set curhp = " << user.curhp << ","
			<< "maxhp = " << user.maxhp << ","
			<< "speed = " << user.speed << ","
			<< "pos_x = " << user.pos_x << ","
			<< "pos_y = " << user.pos_y << ","
			<< "pos_z = " << user.pos_z << ","
			<< "rot_x = " << user.rot_x << ","
			<< "rot_y = " << user.rot_y << ","
			<< "rot_z = " << user.rot_z << ","
			<< "logintime = " << curtime << " where memid = " << user.memid << ";";
		err = mysql->ExecQuery(sql.str());
		if (err != 0)
		{
			printf("mysql 7000 update err...%d %s\n", err, mysql->GetErrorStr());

			auto buf2 = __DBManager->PopPool();
			buf2->b(CMD_7000);
			buf2->s(1);
			buf2->s(kind);
			buf2->s(user.memid);
			buf2->e();
			__DBManager->PushToMainThread(buf2);
			return;
		}
		//����д��redis����
		std::string sss2 = "HMSET user_data:" + std::to_string(user.memid) + 
			" memid " + std::to_string(user.memid) +
			" curhp " + std::to_string(user.curhp) + 
			" maxhp " + std::to_string(user.maxhp) + 
			" speed " + std::to_string(user.speed) + 
			" nick " + user.nick + 
			" pos_x " + std::to_string(user.pos_x) + 
			" pos_y " + std::to_string(user.pos_y) +
			" pos_z " + std::to_string(user.pos_z) +
			" rot_x " + std::to_string(user.rot_x) +
			" rot_y " + std::to_string(user.rot_y) +
			" rot_z " + std::to_string(user.rot_z) +
			" logintime " + std::to_string(curtime); 

		int isok = 0;
		int err1 = redis->RedisCommand(sss2.c_str());
		std::string sss_expire = "expire user_data:" + std::to_string(user.memid) + " 100";
		if (err1 != 0)
		{
			printf("redis 7000 update err...%d\n", err1);
			isok = 2;
		}
		else
		{
			redisReply* reply = (redisReply*)redis->getReply();
			if (reply->type != REDIS_REPLY_STATUS)
			{
				isok = 3;
			}
		}
		int err2 = redis->RedisCommand(sss_expire.c_str());


		printf("mysql 7000 update successfully...%d-%d \n", user.memid, curtime);
		auto buf2 = __DBManager->PopPool();
		buf2->b(CMD_7000);
		buf2->s(isok);
		buf2->s(kind);
		buf2->s(user.memid);
		buf2->e();
		__DBManager->PushToMainThread(buf2);
	}

	void DBManager::Thread_UserWrite(DBBuffer* buff)
	{
		auto db = (DBConnector*)buff->GetDB();
		if (db == nullptr)
		{
			printf("Thread_UserWrite is error:\n");
			return;
		}

		uint16_t cmd;
		buff->init_r();
		buff->r(cmd);
		switch (cmd)
		{
		case CMD_7000:
			do_7000(buff, db);
			break;
		}
	}
}