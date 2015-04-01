/*! XSettings.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform setting file
related files		:  stdafx.h XSettings.cpp
create date			:  2014-10-11
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/10/11			CHENQ	created


TODO:				
2014/1011/			1. 
*******************************************************************************/


#ifndef __XSETTINGS_H__
#define __XSETTINGS_H__
#include <string>
#include <map>
#include <vector>

class CXSettingFile
{
public:
	enum	Format { NativeFormat, IniFormat, InvalidFormat };
	enum	Scope { UserScope, SystemScope };
	enum	Status { NoError, AccessError, FormatError };

	CXSettingFile() {}
	virtual ~CXSettingFile() {}

	virtual BOOL Read(const std::string& file_path) = 0;
	virtual BOOL Save(const std::string& file_path) const = 0;

protected:
	virtual void OnLoaded() {}
};

/**@brief �����࣬�ṩ���õĶ�д����
	������ͨ���ַ��������룩����ȡֵ
	�ŵ�:����Եͣ��û�ֱ�ӹ���key/value�����ˣ�������redis
	ȱ��: 1. key���ܴ������ظ������򷵻����벻����ֵ
		  2. ��ʱ��֧��ע��
		  3. ��ʱ��֧��Section

	@remarks:
		0. ��ʹ�ð���ַ�'='
		1. ���һ�д洢���ֵ����
			bookName=A,B,C,D
		  �����String���ͣ����û����н�������
		2. �����������Key�����߻Ḳ��ǰ��
*/
typedef BOOL (*ENUM_SETTING_FUNC)(const std::string& section, const std::string& key, std::string& value);

class CXSettings
{
public:
	typedef std::map<std::string, std::string> XSettingSection;
	typedef std::map<std::string, XSettingSection> XSETTING_MAP;

	CXSettings();
	virtual ~CXSettings();

	BOOL Load(const char* pszSettingFile, UINT nMaxLine = 512U);
	BOOL Save(const char* pszSettingFile);

	/**	@brief �л���ǰ��ȡ��section
		@param [in]pszSectionNameҪ�л���section
		@return �ɹ�����TRUE��ʧ�ܷ���FAlSE

		�ú�����Ҫ��Load֮����ã����pszSectionName��������᷵��FALSE��\n
		���Ὣ��ȡ�����л�����ʼ״̬(ָ��hdr��
	*/
	BOOL SwitchSectionRead(const char* pszSectionName) const;
	BOOL SwitchSectionWrite(const char* pszSectionName);


	/**	@brief ����һ��section�����޸�д����ָ��ǰsection
	@param [in]pszSectionNameҪ�л���section
	@return �ɹ�����TRUE��ʧ�ܷ���FAlSE
	���section�Ѵ�����ʧ�ܣ�����������ɾ������section
	*/
	BOOL AddSection(const char* pszSectionName);

	/** @brief ��ȡ����
		@param [in] pKey, ���������ַ�����Ҫ��Ψһ \n
		��������������˼\n
		defaultֵ���������ʱ���ص�Ĭ��ֵ
		@return ���ؽ��
	*/
	int GetInt(const char* pKey, int iDefault=0) const;
	INT64 GetInt64(const char* pKey, INT64 i64Default=0) const;
	BOOL GetBoolean(const char* pKey, BOOL bDefault=FALSE) const;
	UINT GetUint(const char* pKey, UINT nDefault=0) const;
	std::string GetString(const char* pKey, const char* pszDefault) const;

	/** @brief ���ú���
		@param [in] pKey, ���������ַ�����Ҫ��Ψһ \n
		��������������˼\n
		@param [in] valueֵ,�����õ�ֵ
		@return ��������õ�ֵ��key�����ڷ���TRUE�����ø�ֵ��\n
		 �������ֱ�ӷ���FALSE�������Ҫǿ���޸�ʹ�ö�Ӧ��Force����
	*/
	BOOL SetInt(const char* pKey, int iValue);
	BOOL SetBoolean(const char* pKey, BOOL bValue);
	BOOL SetUint(const char* pKey, UINT nValue);
	BOOL SetInt64(const char* pKey, INT64 i64Value);
	BOOL SetString(const char* pKey, const char* pValue);

	/** @brief ���ú���
		@param [in] pKey, ���������ַ�����Ҫ��Ψһ \n
		��������������˼\n
		@param [in] valueֵ,�����õ�ֵ
		@return ��������õ�ֵ��key�����ڷ���TRUE�����ø�ֵ��\n
		������ڷ���FALSE���޸Ķ�Ӧ��ֵ
	*/
	BOOL SetIntForce(const char* pKey, int iValue);
	BOOL SetBooleanForce(const char* pKey, BOOL bValue);
	BOOL SetUintForce(const char* pKey, UINT nValue);
	BOOL SetInt64Force(const char* pKey, INT64 i64Value);
	BOOL SetStringForce(const char* pKey, const char* pValue);

	/** @brief ��ӡ����
		����������enumFuncPrint ���ڴ���������Ϣ��ӡ������̨(stdout)
	*/
	void DumpAll();

	static BOOL enumFuncPrint(const std::string& section,  const std::string& pKey, std::string& value);

	/** @brief ö��ÿһ��key��value
		@param pFunc�ص���������ÿһ��key��value���ûص�\n
			����ص���������FALSE������ص�����
		@return ���ػص�����
	*/
	int EnumValue(ENUM_SETTING_FUNC pFunc = enumFuncPrint);

private:
	const XSettingSection* GetSecionPtrR(const char* pszKey) const ;
	XSettingSection* GetSecionPtrW(const char* pszKey);

private:
	mutable XSETTING_MAP m_mapVal;
	mutable XSETTING_MAP::iterator m_iterRead;
	XSETTING_MAP::iterator m_iterWrite;
};

/**
	CXCfgFile�������ļ�������\n
	֧��Key=Value�ṹ
	��֧��Section
	@remarks ������δ���İ汾�ᱻ�Ƴ�����ʹ��CXSettings
*/
class CXCfgFile
{
public:
	CXCfgFile();
	~CXCfgFile();

	bool Read(const std::string& file_path);
	bool Save(const std::string& file_path);

	std::string get_string(const std::string& key, const char* pDefautValue = "");
	unsigned int get_int(const std::string& key, UINT nDefaultValue=0);
	BOOL get_boolean(const std::string& key, BOOL bDefault = FALSE);

	bool set_string(const std::string& key, const std::string& value);
	bool set_int(const std::string& key, const unsigned int value);
	bool set_boolean(const std::string& key, const BOOL bValue);
private:
	void trim_left_right(std::string &str);
	void set_vector_string(const std::string& key, const std::string& value);

private:
	std::vector<std::string> m_data;
	std::map<std::string, std::string> m_map;
};

#endif /*__XSETTINGS_H__*/
