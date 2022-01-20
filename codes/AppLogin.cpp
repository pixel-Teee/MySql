#include "AppLogin.h"
#include "DBManager.h"

namespace app
{
	//ģ���¼
	extern void onLogin_1000()
	{
		std::string account = "gfp";
		std::string password = "gfp001";

		//1.���Զ����ڴ������������
		//���˺�
		auto mem = FindMember(account);
		if (mem == nullptr)
		{
			printf("onLogin_1000 mem == nulptr...%d\n", __LINE__);
			return;
		}

		mem->timeLastLogin = time(NULL);
		//������
		int err = strcmp(password.c_str(), mem->password);
		if (err != 0)
		{
			printf("onLogin_1000 password err...%d\n", __LINE__);
			return;
		}

		//�����߳��������ݵ������߳�
		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->b(CMD_1000);//��ʾ����
		buff->s(mem->ID);//ID
		buff->e();
		//�����߳����͵������߳�
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
		}
	}
}