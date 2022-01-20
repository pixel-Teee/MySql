#pragma once

#include <string>

namespace db
{
	class DBBuffer
	{
	public:
		//当前长度
		int m_CurLength;
		//旧的长度
		int m_OldLength;
		//发送的尾
		int m_SendE;//封包数据
		//接受的尾
		int m_ReceE;//接受数据
		//缓冲区，主要用于推送数据
		char* m_Buf;
		//数据库连接器
		void* m_db;
	public:
		DBBuffer(int size)
		{
			m_SendE = 0;
			m_ReceE = 0;
			m_OldLength = size;
			m_CurLength = size;
			m_Buf = new char[m_CurLength];
			memset(m_Buf, 0, sizeof m_CurLength);

			m_db = nullptr;
		}

		~DBBuffer()
		{
			if(NULL != m_Buf)
				delete[] m_Buf;
		}

		inline void* GetDB(){ return m_db; }
		inline void SetDB(void* v){ m_db = v; }
		inline void Clear()
		{
			m_db = nullptr;
			m_SendE = 0;
			m_ReceE = 0;
			if (m_CurLength != m_OldLength)
			{
				if(NULL != m_Buf) delete m_Buf;

				//重新设置长度
				m_CurLength = m_OldLength;
				m_Buf = new char[m_OldLength];
			}
			memset(m_Buf, 0, m_OldLength);
		}

		inline void append(int size)
		{
			if(size <= m_OldLength) return;
			if(NULL != m_Buf) delete[] m_Buf;
			m_CurLength = size;
			m_Buf = new char[m_CurLength];
			memset(m_Buf, 0, m_CurLength);
		}

		//---封装到缓冲区---
		void b(const uint16_t cmd);
		void e();
		void s(const int8_t v);
		void s(const uint8_t v);
		void s(const int16_t v);
		void s(const uint16_t v);
		void s(const int v);
		void s(const int64_t v);
		void s(const bool v);
		void s(void* v, const int len);

		//---解析缓冲区---
		inline void init_r() { m_ReceE = 0; }
		void r(int8_t& v);
		void r(uint8_t& v);
		void r(int16_t& v);
		void r(uint16_t& v);
		void r(int& v);
		void r(int64_t& v);
		void r(bool& v);
		void r(void* v, const int len);
	};
}
