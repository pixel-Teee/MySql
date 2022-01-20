#pragma once
#include <map>
#include <string>
#define USER_MAX_MEMBER 20

namespace app
{
//---1���ֽڵĶ���---
#pragma pack(push, packing)
#pragma pack(1)

	struct S_USER_MEMBER_BASE
	{
		int ID;//�˺�Ψһ���
		int8_t state;
		char name[USER_MAX_MEMBER];
		char password[USER_MAX_MEMBER];
		int  timeCreate;//�˺Ŵ���ʱ��
		int  timeLastLogin;//�ϴε�¼ʱ��
	};
#pragma pack(pop, packing)

	extern std::map<std::string, S_USER_MEMBER_BASE*> __AccountsName;
	extern std::map<int, S_USER_MEMBER_BASE*> __AccountsID;
	extern S_USER_MEMBER_BASE* FindMember(std::string name);
	extern S_USER_MEMBER_BASE* FindMember(int memid);
}