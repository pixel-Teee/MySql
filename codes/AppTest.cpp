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

		//总结
		//一、hash
		//hmset REDIS_REPLY_STATUS reply->str("OK")
		//hmget REDIS_REPLY_ARRAY reply->elements(数组个数) reply->element(数组)
	}

	void testData()
	{
		testRedis();
	}

}