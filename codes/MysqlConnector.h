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

		//执行查询，参数是查询的语句
		int ExecQuery(const std::string& pData);
		void GetQueryFields();
		//连接数据库，需要提供IP地址和端口号，数据库的账号，密码
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
			//获取错误码
			if(m_IsGetErrorCode)
				return mysql_errno(mysql);
			return -1000;//这个-1000是个随便的值
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
		//查询到的数据集
		MYSQL_RES* myres;
		//总共的行号
		int m_TotalRows;
		//总共的列号
		int m_TotalFields;

		char m_IP[datalen];//数据库IP
		char m_UserName[datalen];//数据库用户名
		char m_UserPassword[datalen];//数据库密码
		char m_DBName[datalen];//数据库名
		uint16_t m_Port;//数据库端口号
		bool m_IsConnect;//连接状态
		bool m_IsGetErrorCode;//是否获取错误码

		std::vector<std::string> fieldVec;//字段列表数据
		std::map<std::string, std::string> m_FieldsValue;//字段数据
	};
}