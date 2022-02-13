#include "AppTest.h"
#include "DBManager.h"

#include "UserData.h"

namespace app
{
	//获取玩家数据
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

	//3、访问其它玩家数据
	void test_OtherUserData(int memid)
	{
		auto user = FindUser(memid);
		if (user != NULL)
		{
			printf("user online...直接获取数据%d %d\n", memid, __LINE__);
			return;
		}

		auto db = __DBManager->GetDBSource(ETT_USERREAD);
		auto buff = db->PopBuffer();
		buff->b(CMD_6000);
		buff->s(memid);
		buff->e();
		db->PushToWorkThread(buff);
	}


	//1、测试登录
	void test_Login(std::string account, std::string password)
	{
		//1.从自定义内存红黑树里面查找
		//找账号
		auto mem = FindMember(account);
		if (mem == nullptr)
		{
			printf("testLogin mem == nulptr...%d\n", __LINE__);
			return;
		}

		//找密码
		int err = strcmp(password.c_str(), mem->password);
		if (err != 0)
		{
			printf("testLogin password err...%d\n", __LINE__);
			return;
		}

		mem->timeLastLogin = time(NULL);
		printf("testLogin successfully...%d\n", __LINE__);

		//从主线程推送数据到工作线程
		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->b(CMD_1000);//表示更新
		buff->s(mem->ID);//ID
		buff->e();
		//从主线程推送到工作线程
		db->PushToWorkThread(buff);

		//获取登录数据
		test_GetUserData(mem->ID);
	}

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

		//8、集合set
		//Set是String类型的无序集合，集合成员是唯一的，这就意味着集合不能出现重复的数据
		//集合是通过哈希表实现的，所以添加、删除、查找的复杂度都是O(1)
		sss = "sadd key:xxxx 100";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("sadd...type:%d %d\n", reply->type, (int)reply->integer);

		sss = "SMEMBERS key:xxxx";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("SMEMBERS...type:%d %d\n", reply->type, (int)reply->elements);

		//11、有序集合
		//有序集合和集合一样也是string类型元素的集合，且不允许重复的成员
		// 不同的是每个元素都会关联一个double类型的分数，redis通过分数为集合中的元素从小到大排序
		// 有序集合成员是唯一的，但是分数可以重复
		// 集合是通过哈希表实现的，所以添加、删除、查找的复杂的都是O(1)
		sss = "zadd key:zzzz 100 kkksssal";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("zadd...type:%d %d\n", reply->type, (int)reply->integer);

		sss = "ZRANGE key:zzzz 0 -1 WITHSCORES";
		redis->RedisCommand(sss.c_str());
		reply = (redisReply*)redis->getReply();

		printf("ZRANGE...type:%d %d\n", reply->type, (int)reply->elements);

		//12、事务
		//单个redis命令的执行是原子性的，但Redis没有在事务上增加任何维持原子的机制，
		//所以Redis事务的执行并不是原子的
		//事务可以理解为一个打包的批量执行脚本，但批量指令并非是原子化的操作
		//中间某条指令的失败不会导致前面已做的指令回滚，也不会造成后续的指令不做
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

		//四、集合(set)
		//sadd REDIS_REPLY_INTEGER reply->integer(1设置成功 0设置失败)
		//SMEMBERS REDIS_REPLY_ARRAY reply->elements(数组个数) reply->element(数组)

		//五、有序集合(sorted set)
		//zadd REDIS_REPLY_INTEGER reply->integer(1设置成功 0设置失败)
		//ZRANGE REDIS ARRAY reply->elements(数组个数) reply->element(数组)

		//事务
		//MULTI 开始事务 REDIS_REPLY_STATUS reply->str("OK")
		//EXEC 执行事务 REDIS_REPLY_ARRAY reply->elements(数组个数) reply->element(数组)
	}

	void testData()
	{
		test_Login("gfp", "gfp001");
		//testRedis();
		test_OtherUserData(102);
	}

}