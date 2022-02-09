#include "UserData.h"

namespace app
{
	std::map<std::string, S_USER_MEMBER_BASE*> __AccountsName;
	std::map<int, S_USER_MEMBER_BASE*> __AccountsID;
	std::map<int, S_USER_BASE*> __OnlineUsersID;

	S_USER_MEMBER_BASE* FindMember(std::string name)
	{
		auto it = __AccountsName.find(name);
		if (it != __AccountsName.end()) return it->second;
		return nullptr;
	}
	S_USER_MEMBER_BASE* FindMember(int memid)
	{
		auto it = __AccountsID.find(memid);
		if(it != __AccountsID.end()) return it->second;
		return nullptr;
	}

	S_USER_BASE* FindUser(int memid)
	{
		auto it = __OnlineUsersID.find(memid);
		if(it != __OnlineUsersID.end()) return it->second;
		return nullptr;
	}

}