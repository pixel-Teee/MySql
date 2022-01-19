#pragma once
#include <concurrent_queue.h>
#include "DBBuffer.h"
#include <atomic>

using namespace std;
using namespace db;

//----�̰߳�ȫ���ճ� �ڴ�أ��ظ�ʹ��DDBuffer����ŵ�������----
//----��ȫ�Ĳ�������----
class ThreadSafePool
{
private:
	Concurrency::concurrent_queue<DBBuffer*> buffs;//��ȫ�Ĳ�������
	//buffer����
	int m_BufferSize;
	//�����������
	int m_MaxCount;
	//��¼��ǰ����
	atomic_int m_Count;
public:
	ThreadSafePool(){}

	~ThreadSafePool()
	{
		while (!buffs.empty())
		{
			DBBuffer* buff = NULL;

			//������ȥ��ȡһ��
			buffs.try_pop(buff);
			if (NULL != buff)
			{
				delete buff;
			}
	
	
		}
	}

	inline void Init(int num, int size)
	{
		m_Count = 0;
		m_MaxCount = num;
		m_BufferSize = size;
	}

	inline int GetCount()
	{
		return m_Count;
	}

	/*----�ó�һ��Buffer----*/
	inline DBBuffer* Pop()
	{
		DBBuffer* buff = nullptr;
		if (buffs.empty() == true)
		{
			buff = new DBBuffer(m_BufferSize);
			buff->Clear();
		}
		else
		{
			/*----����̻߳�ȡ���ݵ�ʱ�����Ϊ��----*/
			buffs.try_pop(buff);
			if (buff == nullptr)
			{
				buff = new DBBuffer(m_BufferSize);
				/*----��һ������----*/
				buff->Clear();
			}
			else
			{
				/*----������һ----*/
				--m_Count;
			}
		}
		return buff;
	}

	//�������
	/*----������ӻ�ȥ----*/
	void Push(DBBuffer* buff)
	{
		if(buff == nullptr) return;
		if(m_Count > m_MaxCount)
		{
			delete buff;
			return;
		}
		buff->Clear();
		buffs.push(buff);
		++m_Count;
	}
};
