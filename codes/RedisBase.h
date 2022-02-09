#pragma once

class RedisBase
{
public:
	virtual ~RedisBase(){}
	virtual int Connect(const char* ip, int port) = 0;//����
	virtual int ReConnect() = 0;//�������ӣ��Զ�����
	virtual bool IsConnect() = 0;

	virtual int RedisCommand(const char* cmd) = 0;
	virtual void Clear() = 0;
	virtual char* value(int index, int& length) = 0;
};

#ifdef ____WIN32_
	extern "C" _declspec(dllexport) RedisBase* NewRedis();
#else
	extern "C" RedisBase* NewRedis();
#endif