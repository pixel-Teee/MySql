#pragma once
#include <concurrent_queue.h>
#include "DBBuffer.h"
#include <atomic>

using namespace std;
using namespace db;

//----线程安全回收池 内存池，重复使用DDBuffer，存放到队列中----
//----安全的并发队列----
class ThreadSafePool
{
private:
	Concurrency::concurrent_queue<DBBuffer*> buffs;//安全的并发队列
	//buffer长度
	int m_BufferSize;
	//队列最大数量
	int m_MaxCount;
	//记录当前数量
	atomic_int m_Count;
public:
	ThreadSafePool(){}

	~ThreadSafePool()
	{
		while (!buffs.empty())
		{
			DBBuffer* buff = NULL;

			//尝试性去获取一下
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

	/*----拿出一个Buffer----*/
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
			/*----多个线程获取数据的时候可能为空----*/
			buffs.try_pop(buff);
			if (buff == nullptr)
			{
				buff = new DBBuffer(m_BufferSize);
				/*----清一下数据----*/
				buff->Clear();
			}
			else
			{
				/*----数量减一----*/
				--m_Count;
			}
		}
		return buff;
	}

	//添加数据
	/*----用完添加回去----*/
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
