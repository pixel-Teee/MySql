#pragma once
#include <map>
#include <string>
#define USER_MAX_MEMBER 20

namespace app
{
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
#pragma pack(pop, packing)

	extern std::map<std::string, S_USER_MEMBER_BASE*> __AccountsName;
	extern std::map<int, S_USER_MEMBER_BASE*> __AccountsID;
	extern S_USER_MEMBER_BASE* FindMember(std::string name);
	extern S_USER_MEMBER_BASE* FindMember(int memid);
}