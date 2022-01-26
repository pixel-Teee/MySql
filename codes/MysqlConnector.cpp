#include "MysqlConnector.h"

namespace db
{

	MysqlConnector::MysqlConnector()
	{
		mysql = NULL;
		myres = NULL;

		m_TotalRows = 0;
		m_TotalFields = 0;
		m_IsConnect = false;
		m_IsGetErrorCode = false;
	}

	MysqlConnector::~MysqlConnector()
	{
		if (mysql)
		{
			mysql_close(mysql);
			mysql = NULL;
		}
		if (myres)
		{
			myres = NULL;
		}
	}

	//����mysql������
	bool MysqlConnector::ConnectMysql(const char* ip, const char* usename, const char* password, const char* dbname, int port /*= 3306*/)
	{
		strcpy(m_IP, ip);
		strcpy(m_UserName, usename);
		strcpy(m_UserPassword, password);
		strcpy(m_DBName, dbname);

		m_Port = port;

		Clear();

		if (ReConnect() == false) return false;
		return true;
	}

	void MysqlConnector::Clear()
	{
		m_IsConnect = false;
		if (mysql)
		{
			mysql_close(mysql);
			mysql = NULL;
		}
		if(myres) myres = NULL;
	}

	//����
	bool MysqlConnector::ReConnect()
	{
		//����֮ǰpingһ��
		if (m_IsConnect)
		{	
			m_IsGetErrorCode = true;
			//�����������������
			int error = mysql_ping(mysql);
			if(error == 0) return true;
			m_IsGetErrorCode = false;
			return false;
		}

		//��ʼ������
		mysql = mysql_init(NULL);
		if(mysql == NULL) return false;

		char value = 1;
		int f_outtime = 5;
		mysql_options(mysql, MYSQL_OPT_RECONNECT, &value);

		mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &f_outtime);

		mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");

		//mysql_real_connect����һ�����
		auto m = mysql_real_connect(mysql, m_IP, m_UserName, m_UserPassword, m_DBName, m_Port, NULL, CLIENT_MULTI_STATEMENTS);
		if (m == NULL)
		{
			printf("mysql reconnect error:%s \n", GetErrorStr());
			m_IsConnect = false;
			m_IsGetErrorCode = false;
			return false;
		}
		mysql_options(mysql, MYSQL_OPT_RECONNECT, &value);

		mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &f_outtime);

		m_IsConnect = true;

		m_IsGetErrorCode = true;

		return true;
	}

	//����������Ϊ��ѯ�ļ��ϴ�����ڴ��е�
	void MysqlConnector::ClearRes()
	{
		bool isnext = (myres != NULL);
		if (myres)
		{
			mysql_free_result(myres);
			myres = NULL;
		}

		if (isnext)
		{
			while (mysql_next_result(mysql) == 0)
			{
				//��ȡ��ǰ�Ľ����
				myres = mysql_store_result(mysql);
				myres = NULL;
			}
		}
		else
		{
			do 
			{
				myres = mysql_store_result(mysql);
				if (myres)
				{
					mysql_free_result(myres);
					myres = NULL;
				}
				
			} while (mysql_next_result(mysql) == 0);
		}
		myres = NULL;
	}

	void MysqlConnector::CloseQuery()
	{
		m_FieldsValue.clear();
	}

	//ִ�в�ѯ
	//SELECT:��ѯ���
	//INSERT:����
	//UPDATE:����
	//DELETE:ɾ��
	int MysqlConnector::ExecQuery(const std::string& pData)
	{
		if(pData.size() < 1) return -1;
		ClearRes();
		//Pingһ��
		if (ReConnect() == false) return -2;

		int ret = mysql_real_query(mysql, pData.c_str(), pData.size());

		if(ret != 0) return ret;

		if((pData.find("select") == -1) && (pData.find("SELECT") == -1)) return ret;

		myres = mysql_store_result(mysql);

		if (myres == nullptr)
		{
			printf("mysql ExecQuery error...%s\n", pData.c_str());
			return -1;
		}

		m_TotalFields = mysql_num_fields(myres);
		m_TotalRows = static_cast<int>(mysql_affected_rows(mysql));

		GetQueryFields();

		return 0;
	}

	//��ȡ������䵽������
	void MysqlConnector::GetQueryFields()
	{
		fieldVec.clear();
		m_FieldsValue.clear();

		MYSQL_ROW curRow = mysql_fetch_row(myres);
		if(!curRow) return;

		//ÿһ�е�����
		MYSQL_FIELD* field = NULL;
		int num = mysql_num_fields(myres);
		int i = 0;
		//��field����fields
		while (field = mysql_fetch_field(myres))
		{
			auto tempres = curRow[i];

			if (tempres)
			{
				std::string sss(field->name, field->name_length);

				fieldVec.push_back(sss);//��ӵ������Ƶ�����

				m_FieldsValue[sss] = tempres;//��������䵽�����
				//m_FieldsValue.insert(std::make_pair(sss, tempres));
			}
			++i;
			if(i >= num) break;
		}
	}

	int MysqlConnector::QueryNext()
	{
		m_FieldsValue.clear();

		//��ȡ��һ�е�����
		if (myres == nullptr)
		{
			printf("mysql ExecQuery error...\n");
			return -1;
		}

		MYSQL_ROW curRow = mysql_fetch_row(myres);

		if(!curRow) return -1;

		int size = fieldVec.size();
		for (int i = 0; i < size; ++i)
		{
			auto tempres = curRow[i];
			if (tempres)
			{
				m_FieldsValue[fieldVec[i]] = tempres;
			}
		}
		return 0;
	}

	void MysqlConnector::r(const char* key, uint8_t& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (uint8_t)atoi(it->second.data());
		}
	}

	void MysqlConnector::r(const char* key, int8_t& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (int8_t)atoi(it->second.data());
		}
	}

	void MysqlConnector::r(const char* key, uint16_t& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (uint16_t)atoi(it->second.data());
		}
	}

	void MysqlConnector::r(const char* key, int16_t& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (int16_t)atoi(it->second.data());
		}
	}

	void MysqlConnector::r(const char* key, uint32_t& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (uint32_t)atoi(it->second.data());
		}
	}

	void MysqlConnector::r(const char* key, int32_t& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (int32_t)atoi(it->second.data());
		}
	}

	void MysqlConnector::r(const char* key, int64_t& value)
	{
		/*
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = (int64_t)_atoi64(it->second.data());
		}
		*/
	}	

	void MysqlConnector::r(const char* key, std::string& value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			value = it->second.data();
		}
	}

	void MysqlConnector::r(const char* key, char* value)
	{
		auto it = m_FieldsValue.find(key);
		if (it != m_FieldsValue.end())
		{
			strcpy(value, it->second.data());
		}
	}

}