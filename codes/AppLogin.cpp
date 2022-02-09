#include "AppLogin.h"
#include "DBManager.h"

namespace app
{
	////模拟登录
	//extern void onLogin_1000()
	//{
	//	std::string account = "gfp";
	//	std::string password = "gfp001";

	//	//1.从自定义内存红黑树里面查找
	//	//找账号
	//	auto mem = FindMember(account);
	//	if (mem == nullptr)
	//	{
	//		printf("onLogin_1000 mem == nulptr...%d\n", __LINE__);
	//		return;
	//	}

	//	mem->timeLastLogin = time(NULL);
	//	//找密码
	//	int err = strcmp(password.c_str(), mem->password);
	//	if (err != 0)
	//	{
	//		printf("onLogin_1000 password err...%d\n", __LINE__);
	//		return;
	//	}

	//	//从主线程推送数据到工作线程
	//	auto db = __DBManager->DBAccount;
	//	auto buff = db->PopBuffer();
	//	buff->b(CMD_1000);//表示更新
	//	buff->s(mem->ID);//ID
	//	buff->e();
	//	//从主线程推送到工作线程
	//	db->PushToWorkThread(buff);

	//	printf("onLogin_1000 successfully...%d\n", __LINE__);
	//}

	//void onRegister_2000()
	//{
	//	std::string account = "zb";
	//	std::string password = "zb004";

	//	//1.从自定义内存红黑树里面查找
	//	auto mem = FindMember(account);
	//	if (mem != nullptr)
	//	{
	//		printf("onRegister_2000 mem != nullptr..%d\n", __LINE__);
	//		return;
	//	}

	//	//从主线程推送数据到工作线程
	//	//注册账号密码
	//	auto db = __DBManager->DBAccount;
	//	auto buff = db->PopBuffer();
	//	buff->b(CMD_2000);
	//	buff->s((int)account.size());
	//	buff->s((char*)account.c_str(), account.size());
	//	buff->s((int)password.size());
	//	buff->s((char*)password.c_str(), password.size());
	//	buff->e();
	//	//从主线程推送到工作线程
	//	db->PushToWorkThread(buff);
	//}

	////模拟游戏数据的处理
	//void on_3000()
	//{
	//	int memid = 102;
	//	//获取读线程，工作量最小的
	//	auto db = __DBManager->GetDBSource(ETT_USERREAD);
	//	auto buff = db->PopBuffer();
	//	buff->b(CMD_3000);
	//	buff->s(memid);
	//	buff->e();
	//	//推送到工作线程
	//	db->PushToWorkThread(buff);
	//}

	//void db_1000(DBBuffer* buff)
	//{
	//	int memid;
	//	buff->r(memid);

	//	printf("db_1000 successfully..%d %d\n", memid, __LINE__);
	//}

	//void db_2000(DBBuffer* buff)
	//{
	//	int memid;
	//	char account[100];
	//	char password[100];
	//	memset(account, 0, 100);
	//	memset(password, 0, 100);
	//	buff->r(memid);
	//	buff->r(account, 100);
	//	buff->r(password, 100);
	//	printf("db_2000 successfully..%d %s %s %d\n", memid, account, password, __LINE__);

	//	app::S_USER_MEMBER_BASE* mem = new app::S_USER_MEMBER_BASE();
	//	mem->ID = memid;
	//	memcpy(mem->name, account, 20);
	//	memcpy(mem->password, password, 20);
	//	mem->timeCreate = time(NULL);
	//	//插入内存当中
	//	__AccountsID.insert(std::make_pair(memid, mem));
	//}

	//void db_3000(DBBuffer* buff)
	//{
	//	int errcmd = 0;
	//	buff->r(errcmd);
	//	if (errcmd != 0)
	//	{
	//		printf("db_3000 failed..%d %d\n", errcmd, __LINE__);
	//		return;
	//	}
	//	int id;
	//	int memid;
	//	int curhp;
	//	int maxhp;
	//	char nick[20];
	//	buff->r(id);
	//	buff->r(memid);
	//	buff->r(curhp);
	//	buff->r(maxhp);
	//	buff->r(nick, 20);
	//	printf("db_3000 successfully...%d %d, %d %d %s %d\n", id, memid, curhp, maxhp, nick, __LINE__);
	//}

	////-----------------------------------------------------
	////工作线程返回到主线程当中的数据
	//void OnDBCommand(void* buf)
	//{
	//	DBBuffer* buff = (DBBuffer*)buf;

	//	uint16_t cmd;
	//	buff->init_r();
	//	buff->r(cmd);

	//	switch (cmd)
	//	{
	//		case CMD_1000:
	//			db_1000(buff);
	//			break;
	//		case CMD_2000:
	//			db_2000(buff);
	//			break;
	//		case CMD_3000:
	//			db_3000(buff);
	//			break;;
	//	}
	//}
}