#pragma once
#include <map>
#include <string>
#define USER_MAX_MEMBER 20
#define CMD_1000 1000
#define CMD_2000 2000
#define CMD_3000 3000

#ifdef ____WIN32_
#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

/* This is the reply object returned by redisCommand() */
typedef struct redisReply {
	int type; /* REDIS_REPLY_* */
	int64_t integer; /* The integer when type is REDIS_REPLY_INTEGER */
	int len; /* Length of string */
	char* str; /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
	size_t elements; /* number of elements, for REDIS_REPLY_ARRAY */
	struct redisReply** element; /* elements vector for REDIS_REPLY_ARRAY */
} redisReply;
#endif

namespace app
{
	//redis状态
	enum E_REDIS_STATE
	{
		ERS_FREE = 0,
		ERS_FAILED = 1
	};

	enum E_SAVE_EATSE
	{
		ESE_LEAVE = 0,
		ESE_TIMESAVE = 1
	};

//---1个字节的对齐---
#pragma pack(push, packing)
#pragma pack(1)

	struct S_USER_MEMBER_BASE
	{
		int ID;//账号唯一标记
		int8_t state;
		char name[USER_MAX_MEMBER];
		char password[USER_MAX_MEMBER];
		int  timeCreate;//账号创建时间
		int  timeLastLogin;//上次登录时间
	};
	//用户数据
	struct S_USER_BASE
	{
		int memid;
		int curhp;
		int maxhp;
		int speed;
		int pos_x;
		int pos_y;
		int pos_z;
		int rot_x;
		int rot_y;
		int rot_z;
		int logintime;
		char nick[USER_MAX_MEMBER];

		int Temp_SaveTime;//自动保存数据 时间
		int Temp_SaveThreadID;//正在保存使用的线程ID
		int Temp_SaveCount;//正在保存使用数量

		inline void clearThreadID()
		{
			--Temp_SaveCount;
			if (Temp_SaveCount > 0) return;
			Temp_SaveThreadID = 0;
			Temp_SaveCount = 0;
		}

		inline void addThreadID(int tid)
		{
			++Temp_SaveCount;
			Temp_SaveThreadID = tid;
		}
	};
#pragma pack(pop, packing)

	extern std::map<std::string, S_USER_MEMBER_BASE*> __AccountsName;
	extern std::map<int, S_USER_MEMBER_BASE*> __AccountsID;
	extern std::map<int, S_USER_BASE*> __OnlineUsersID;

	extern S_USER_MEMBER_BASE* FindMember(std::string name);
	extern S_USER_MEMBER_BASE* FindMember(int memid);

	extern S_USER_BASE* FindUser(int memid);
}