#include "AppTest.h"
#include "DBManager.h"

#include "UserData.h"

namespace app
{
	//测试5种数据类型
	void testRedis()
	{
		auto redis = __DBManager->DBAccount->GetRedisConnector();
		redisReply* reply = nullptr;

		//1、hash hmeset
		std::string sss = "HMSET user_data:1000 memid 100 curhp 20";
		int err = redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		if(reply == nullptr) return;
		printf("hmset...type:%d %s\n", reply->type, reply->str);

		//2. hash hmget
		sss = "HMGET user_data:1000 memid curhp";
		err = redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		if(reply == nullptr) return;

		int length = 0;
		redis->value(0, length);
		//数组形式 elements:元素个数
		printf("hmget...type:%d %d %d\n", reply->type, (int)reply->elements, length);

		sss = "hset user_data:1000 maxhp 2000";
		err = redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		if(reply == nullptr) return;

		printf("hset...type:%d %d \n", reply->type, (int)reply->integer);

		sss = "hget user_data:1000 maxhp";
		err = redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		if (reply == nullptr) return;

		printf("hget...type:%d %s \n", reply->type, reply->str);

		//4、string
		sss = "set aaa 111";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("set...type:%d %s\n", reply->type, reply->str);

		//5、获取字符串
		sss = "get aaa";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("get...type:%d %s\n", reply->type, reply->str);

		//6、删除key 返回 REDIS_REPLY_INTEGER 成功返回1 失败返回0
		sss = "del user_data:1001";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("del...type:%d %d\n", reply->type, (int)reply->integer);

		//6、设置过期时间
		sss = "expire user_data:1000 1000";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("expire...type:%d %d\n", reply->type, (int)reply->integer);

		//7、设置列表
		sss = "lpush kkk 100";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("list...type:%d %d\n", reply->type, (int)reply->integer);

		sss = "lrange kkk 0 -1";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("list...type:%d %d\n", reply->type, (int)reply->elements);
		//总结
		//一、hash
		//hmset REDIS_REPLY_STATUS reply->str("OK")
		//hmget REDIS_REPLY_ARRAY reply->elements(数组个数) reply->element(数组)
		//hset  REDIS_REPLY_INTEGER reply->integer
		//hget  REDIS_REPLY_STRING reply->str / REDIS_REPLY_NIL 查询数据不存在
		//del/expire REDIS_REPLY_INTEGER reply->integer(删除或设置成功 0 key不存在)
		
		//二、string
		//set REDIS_REPLY_STATUS reply->str("OK")
		//get REDIS_REPLY_STRING reply->str /不存在REDIS_REPLY_NIL

		//三、list
		//lpush REDIS_REPLY_INTEGER reply->integer(设置List长度)
		//lrange REDIS_REPLY_INTEGER reply->elements(数组个数) reply->element(数组)
	}

	void testData()
	{
		testRedis();
	}

}