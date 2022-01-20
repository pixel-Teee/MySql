#include "AppLogin.h"
#include "DBManager.h"

namespace app
{
	//模拟登录
	extern void onLogin_1000()
	{
		std::string account = "gfp";
		std::string password = "gfp001";

		//1.从自定义内存红黑树里面查找
		//找账号
		auto mem = FindMember(account);
		if (mem == nullptr)
		{
			printf("onLogin_1000 mem == nulptr...%d\n", __LINE__);
			return;
		}

		mem->timeLastLogin = time(NULL);
		//找密码
		int err = strcmp(password.c_str(), mem->password);
		if (err != 0)
		{
			printf("onLogin_1000 password err...%d\n", __LINE__);
			return;
		}

		//从主线程推送数据到工作线程
		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->b(CMD_1000);//表示更新
		buff->s(mem->ID);//ID
		buff->e();
		//从主线程推送到工作线程
		db->PushToWorkThread(buff);

		printf("onLogin_1000 successfully...%d\n", __LINE__);
	}

	void db_1000(DBBuffer* buff)
	{
		int memid;
		buff->r(memid);

		printf("db_1000 successfully..%d %d\n", memid, __LINE__);
	}

	//-----------------------------------------------------
	//工作线程返回到主线程当中的数据
	void OnDBCommand(void* buf)
	{
		DBBuffer* buff = (DBBuffer*)buf;

		uint16_t cmd;
		buff->init_r();
		buff->r(cmd);

		switch (cmd)
		{
			case CMD_1000:
				db_1000(buff);
				break;
		}
	}
}