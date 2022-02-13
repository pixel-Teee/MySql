#include "AppTest.h"
#include "DBManager.h"

#include "UserData.h"

namespace app
{
	//��ȡ�������
	void test_GetUserData(int memid)
	{
		auto account = app::FindMember(memid);
		
		if (account == nullptr)
		{
			printf("user err_online..%d %d\n", memid, __LINE__);
			return;
		}

		auto user = FindUser(memid);
		if (user != NULL)
		{
			printf("user err_online..%d %d\n", memid, __LINE__);
			return;
		}

		auto db = __DBManager->GetDBSource(ETT_USERREAD);
		auto buff = db->PopBuffer();

		buff->b(CMD_3000);
		buff->s(memid);
		buff->s((int)account->redis_state);
		buff->e();
		db->PushToWorkThread(buff);
	}

	//3�����������������
	void test_OtherUserData(int memid)
	{
		auto user = FindUser(memid);
		if (user != NULL)
		{
			printf("user online...ֱ�ӻ�ȡ����%d %d\n", memid, __LINE__);
			return;
		}

		auto db = __DBManager->GetDBSource(ETT_USERREAD);
		auto buff = db->PopBuffer();
		buff->b(CMD_6000);
		buff->s(memid);
		buff->e();
		db->PushToWorkThread(buff);
	}


	//1�����Ե�¼
	void test_Login(std::string account, std::string password)
	{
		//1.���Զ����ڴ������������
		//���˺�
		auto mem = FindMember(account);
		if (mem == nullptr)
		{
			printf("testLogin mem == nulptr...%d\n", __LINE__);
			return;
		}

		//������
		int err = strcmp(password.c_str(), mem->password);
		if (err != 0)
		{
			printf("testLogin password err...%d\n", __LINE__);
			return;
		}

		mem->timeLastLogin = time(NULL);
		printf("testLogin successfully...%d\n", __LINE__);

		//�����߳��������ݵ������߳�
		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->b(CMD_1000);//��ʾ����
		buff->s(mem->ID);//ID
		buff->e();
		//�����߳����͵������߳�
		db->PushToWorkThread(buff);

		//��ȡ��¼����
		test_GetUserData(mem->ID);
	}

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

		//8������set
		//Set��String���͵����򼯺ϣ����ϳ�Ա��Ψһ�ģ������ζ�ż��ϲ��ܳ����ظ�������
		//������ͨ����ϣ��ʵ�ֵģ�������ӡ�ɾ�������ҵĸ��Ӷȶ���O(1)
		sss = "sadd key:xxxx 100";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("sadd...type:%d %d\n", reply->type, (int)reply->integer);

		sss = "SMEMBERS key:xxxx";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("SMEMBERS...type:%d %d\n", reply->type, (int)reply->elements);

		//11�����򼯺�
		//���򼯺Ϻͼ���һ��Ҳ��string����Ԫ�صļ��ϣ��Ҳ������ظ��ĳ�Ա
		// ��ͬ����ÿ��Ԫ�ض������һ��double���͵ķ�����redisͨ������Ϊ�����е�Ԫ�ش�С��������
		// ���򼯺ϳ�Ա��Ψһ�ģ����Ƿ��������ظ�
		// ������ͨ����ϣ��ʵ�ֵģ�������ӡ�ɾ�������ҵĸ��ӵĶ���O(1)
		sss = "zadd key:zzzz 100 kkksssal";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("zadd...type:%d %d\n", reply->type, (int)reply->integer);

		sss = "ZRANGE key:zzzz 0 -1 WITHSCORES";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("ZRANGE...type:%d %d\n", reply->type, (int)reply->elements);

		//12������
		//����redis�����ִ����ԭ���Եģ���Redisû���������������κ�ά��ԭ�ӵĻ��ƣ�
		//����Redis�����ִ�в�����ԭ�ӵ�
		//����������Ϊһ�����������ִ�нű���������ָ�����ԭ�ӻ��Ĳ���
		//�м�ĳ��ָ���ʧ�ܲ��ᵼ��ǰ��������ָ��ع���Ҳ������ɺ�����ָ���
		sss = "MULTI";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		printf("MULTI...type:%d %s\n", reply->type, reply->str);

		sss = "set a1 111";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		printf("set...type:%d %s\n", reply->type, reply->str);

		sss = "get a2";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		printf("get...type:%d %s\n", reply->type, reply->str);

		sss = "EXEC";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();
		printf("EXEC...type:%d %d\n", reply->type, (int)reply->elements);

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

		//�ġ�����(set)
		//sadd REDIS_REPLY_INTEGER reply->integer(1���óɹ� 0����ʧ��)
		//SMEMBERS REDIS_REPLY_ARRAY reply->elements(�������) reply->element(����)

		//�塢���򼯺�(sorted set)
		//zadd REDIS_REPLY_INTEGER reply->integer(1���óɹ� 0����ʧ��)
		//ZRANGE REDIS ARRAY reply->elements(�������) reply->element(����)

		//����
		//MULTI ��ʼ���� REDIS_REPLY_STATUS reply->str("OK")
		//EXEC ִ������ REDIS_REPLY_ARRAY reply->elements(�������) reply->element(����)
	}

	void testData()
	{
		test_Login("gfp", "gfp001");
		//testRedis();
		test_OtherUserData(102);
	}

}