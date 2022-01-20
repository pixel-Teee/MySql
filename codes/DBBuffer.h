#pragma once

#include <string>

namespace db
{
	class DBBuffer
	{
	public:
		//��ǰ����
		int m_CurLength;
		//�ɵĳ���
		int m_OldLength;
		//���͵�β
		int m_SendE;//�������
		//���ܵ�β
		int m_ReceE;//��������
		//����������Ҫ������������
		char* m_Buf;
		//���ݿ�������
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

				//�������ó���
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

		//---��װ��������---
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

		//---����������---
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
