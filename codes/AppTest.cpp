#include "AppTest.h"
#include "DBManager.h"

#include "UserData.h"

namespace app
{
	//����5����������
	void testRedis()
	{
		auto redis = __DBManager->DBAccount->GetRedisConnector();
		redisReply* reply = nullptr;

		//1��hash hmeset
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
		//������ʽ elements:Ԫ�ظ���
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

		//4��string
		sss = "set aaa 111";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("set...type:%d %s\n", reply->type, reply->str);

		//5����ȡ�ַ���
		sss = "get aaa";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("get...type:%d %s\n", reply->type, reply->str);

		//6��ɾ��key ���� REDIS_REPLY_INTEGER �ɹ�����1 ʧ�ܷ���0
		sss = "del user_data:1001";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("del...type:%d %d\n", reply->type, (int)reply->integer);

		//6�����ù���ʱ��
		sss = "expire user_data:1000 1000";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("expire...type:%d %d\n", reply->type, (int)reply->integer);

		//7�������б�
		sss = "lpush kkk 100";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("list...type:%d %d\n", reply->type, (int)reply->integer);

		sss = "lrange kkk 0 -1";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("list...type:%d %d\n", reply->type, (int)reply->elements);
		//�ܽ�
		//һ��hash
		//hmset REDIS_REPLY_STATUS reply->str("OK")
		//hmget REDIS_REPLY_ARRAY reply->elements(�������) reply->element(����)
		//hset  REDIS_REPLY_INTEGER reply->integer
		//hget  REDIS_REPLY_STRING reply->str / REDIS_REPLY_NIL ��ѯ���ݲ�����
		//del/expire REDIS_REPLY_INTEGER reply->integer(ɾ�������óɹ� 0 key������)
		
		//����string
		//set REDIS_REPLY_STATUS reply->str("OK")
		//get REDIS_REPLY_STRING reply->str /������REDIS_REPLY_NIL

		//����list
		//lpush REDIS_REPLY_INTEGER reply->integer(����List����)
		//lrange REDIS_REPLY_INTEGER reply->elements(�������) reply->element(����)
	}

	void testData()
	{
		testRedis();
	}

}