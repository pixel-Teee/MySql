#include "AppLogin.h"
#include "DBManager.h"

namespace app
{
	////ģ���¼
	//void onLogin_1000()
	//{
	//	std::string account = "gfp";
	//	std::string password = "gfp001";

	//	//1.���Զ����ڴ������������
	//	//���˺�
	//	auto mem = FindMember(account);
	//	if (mem == nullptr)
	//	{
	//		printf("onLogin_1000 mem == nulptr...%d\n", __LINE__);
	//		return;
	//	}

	//	mem->timeLastLogin = time(NULL);
	//	//������
	//	int err = strcmp(password.c_str(), mem->password);
	//	if (err != 0)
	//	{
	//		printf("onLogin_1000 password err...%d\n", __LINE__);
	//		return;
	//	}

	//	//�����߳��������ݵ������߳�
	//	auto db = __DBManager->DBAccount;
	//	auto buff = db->PopBuffer();
	//	buff->b(CMD_1000);//��ʾ����
	//	buff->s(mem->ID);//ID
	//	buff->e();
	//	//�����߳����͵������߳�
	//	db->PushToWorkThread(buff);

	//	printf("onLogin_1000 successfully...%d\n", __LINE__);
	//}

	void onRegister_2000()
	{
		std::string account = "zb";
		std::string password = "zb004";

		//1.���Զ����ڴ������������
		auto mem = FindMember(account);
		if (mem != nullptr)
		{
			printf("onRegister_2000 mem != nullptr..%d\n", __LINE__);
			return;
		}

		//�����߳��������ݵ������߳�
		//ע���˺�����
		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->b(CMD_2000);
		buff->s((int)account.size());
		buff->s((char*)account.c_str(), account.size());
		buff->s((int)password.size());
		buff->s((char*)password.c_str(), password.size());
		buff->e();
		//�����߳����͵������߳�
		db->PushToWorkThread(buff);
	}

	//ģ����Ϸ���ݵĴ���
	void on_3000()
	{
		int memid = 102;
		//��ȡ���̣߳���������С��
		auto db = __DBManager->GetDBSource(ETT_USERREAD);
		auto buff = db->PopBuffer();
		buff->b(CMD_3000);
		buff->s(memid);
		buff->e();
		//���͵������߳�
		db->PushToWorkThread(buff);
	}

	void db_1000(DBBuffer* buff)
	{
		int memid;
		buff->r(memid);

		printf("db_1000 successfully..%d %d\n", memid, __LINE__);
	}

	void db_2000(DBBuffer* buff)
	{
		int memid;
		char account[100];
		char password[100];
		memset(account, 0, 100);
		memset(password, 0, 100);
		buff->r(memid);
		buff->r(account, 100);
		buff->r(password, 100);
		printf("db_2000 successfully..%d %s %s %d\n", memid, account, password, __LINE__);

		app::S_USER_MEMBER_BASE* mem = new app::S_USER_MEMBER_BASE();
		mem->ID = memid;
		memcpy(mem->name, account, 20);
		memcpy(mem->password, password, 20);
		mem->timeCreate = time(NULL);
		//�����ڴ浱��
		__AccountsID.insert(std::make_pair(memid, mem));
	}

	//��ȡ�û�����
	void db_3000(DBBuffer* buff)
	{
		int errcmd = 0;
		buff->r(errcmd);
		if (errcmd != 0)
		{
			printf("db_3000 failed..%d %d\n", errcmd, __LINE__);
			return;
		}
		//��ȡ�������
		app::S_USER_BASE* user = new app::S_USER_BASE();
		buff->r(user, sizeof(app::S_USER_BASE));
		user->logintime = (int)time(NULL);
		user->Temp_SaveTime = (int)time(NULL);
		user->Temp_SaveThreadID = 0;
		user->Temp_SaveCount = 0;

		//���� redis״̬
		auto mem = FindMember(user->memid);
		if(mem != NULL) mem->redis_state = ERS_FREE;

		//��֤ �ڲ�����
		auto d = FindUser(user->memid);
		if (d != NULL)
		{
			printf("user_err online2...%d %d\n", user->memid, __LINE__);
			delete user;
			user = 0;
			return;
		}

		//��ӵ������������
		app::__OnlineUsersID.insert(std::make_pair(user->memid, user));

		printf("db_3000 successfully..%d-%d-%d %s %d\n", user->memid, user->curhp, user->maxhp, user->nick, __LINE__);
	}

	void db_4000(DBBuffer* buff)
	{
		int errcmd = 0;
		buff->r(errcmd);
		if (errcmd != 0)
		{
			printf("db_4000 failed...%d %d\n", errcmd, __LINE__);
			return;
		}
		printf("db_4000 successfully...%d %d\n", errcmd, __LINE__);
	}

	void db_6000(DBBuffer* buff)
	{
		int errcmd = 0;
		buff->r(errcmd);

		if (errcmd != 0)
		{
			printf("db_6000 failed...%d %d\n", errcmd, __LINE__);
			return;
		}

		//��ȡ�������
		app::S_USER_BASE* user = new app::S_USER_BASE();
		buff->r(user, sizeof(app::S_USER_BASE));

		printf("db_6000 successfully...%d-%d-%d %s %d\n", user->memid, user->curhp, user->maxhp, user->nick, __LINE__);

	}

	//��������
	void db_7000(DBBuffer* buff)
	{
		int errcmd = 0;
		int kind = 0;
		int memid = 0;
		buff->r(errcmd);
		buff->r(kind);
		buff->r(memid);
		auto mem = FindMember(memid);

		//д������ʧ��
		if (errcmd != 0)
		{
			//if(errcmd == 1)д��־�������ļ���¼һ��
			printf("db_7000 failed..%d %d\n", errcmd, __LINE__);
			if(mem != NULL) mem->redis_state = ERS_FAILED;
		}
		else
		{
			if(mem != NULL) mem->redis_state = ERS_FREE;
		}

		//���������ڱ����ID
		auto user = FindUser(memid);
		if (user != NULL)
		{
			printf("db_7000 save successfully... %d [%d %d] %d\n",
			kind, user->Temp_SaveThreadID, user->Temp_SaveCount, __LINE__);
			user->clearThreadID();
			printf("db_7000 clear successfully..%d [%d %d] %d\n", kind, user->Temp_SaveThreadID, user->Temp_SaveCount, __LINE__);
		}

		if(kind == ESE_TIMESAVE) return;
		//����ɾ��
		auto it = __OnlineUsersID.find(memid);
		if(it != __OnlineUsersID.end())
		{
			auto user = it->second;
			if (user != nullptr)
			{
				delete user;
			}
			__OnlineUsersID.erase(it);
		}
	}

	//-----------------------------------------------------
	//�����̷߳��ص����̵߳��е�����
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
			case CMD_2000:
				db_2000(buff);
				break;
			case CMD_3000:
				db_3000(buff);
				break;;
			case CMD_4000:
				db_4000(buff);
				break;
			case CMD_6000:
				db_6000(buff);
				break;
			case CMD_7000:
				db_7000(buff);
				break;
		}
	}
}