#pragma once

#include <string>
#include <vector>
#include <map>

#ifdef ____WIN32_
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "mysql.h"
#pragma comment(lib, "libmysql.lib")
#else
#include <stdio.h>
#include <mysql.h>
#endif

namespace db
{
	const int datalen = 32;
	class MysqlConnector
	{
	public:
		MysqlConnector();
		~MysqlConnector();

		//ִ�в�ѯ�������ǲ�ѯ�����
		int ExecQuery(const std::string& pData);
		void GetQueryFields();
		//�������ݿ⣬��Ҫ�ṩIP��ַ�Ͷ˿ںţ����ݿ���˺ţ�����
		bool ConnectMysql(const char* ip, const char* usename, const char* password, const char* dbname, int port = 3306);
		void Clear();
		bool ReConnect();
		void ClearRes();
		void CloseQuery();
		int QueryNext();

		inline int GetQueryRowNum() { return m_TotalRows; }
		inline int GetQueryFieldNum() { return m_TotalFields; }

		inline MYSQL* GetMysql() { return mysql;}
		inline MYSQL_RES* GetMyRes() { return myres; }
		inline int GetErrorCode()
		{
			//��ȡ������
			if(m_IsGetErrorCode)
				return mysql_errno(mysql);
			return -1000;//���-1000�Ǹ�����ֵ
		}

		inline const char* GetErrorStr()
		{
			return mysql_error(mysql);
		}
		
		void r(const char* key, uint8_t& value);
		void r(const char* key, int8_t& value);
		void r(const char* key, uint16_t& value);
		void r(const char* key, int16_t& value);
		void r(const char* key, uint32_t& value);
		void r(const char* key, int32_t& value);
		void r(const char* key, int64_t& value);
		void r(const char* key, char* value);
		void r(const char* key, std::string& value);
	private:
		MYSQL* mysql;
		//��ѯ�������ݼ�
		MYSQL_RES* myres;
		//�ܹ����к�
		int m_TotalRows;
		//�ܹ����к�
		int m_TotalFields;

		char m_IP[datalen];//���ݿ�IP
		char m_UserName[datalen];//���ݿ��û���
		char m_UserPassword[datalen];//���ݿ�����
		char m_DBName[datalen];//���ݿ���
		uint16_t m_Port;//���ݿ�˿ں�
		bool m_IsConnect;//����״̬
		bool m_IsGetErrorCode;//�Ƿ��ȡ������

		std::vector<std::string> fieldVec;//�ֶ��б�����
		std::map<std::string, std::string> m_FieldsValue;//�ֶ�����
	};
}