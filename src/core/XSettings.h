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

/**@brief 配置类，提供配置的读写功能
	调用者通过字符串（主码）来获取值
	优点:耦合性低，用户直接关联key/value就行了，类似于redis
	缺点: 1. key不能错，不能重复，否则返回意想不到的值
		  2. 暂时不支持注释
		  3. 暂时不支持Section

	@remarks:
		0. 请使用半角字符'='
		1. 如果一行存储多个值例如
			bookName=A,B,C,D
		  请采用String类型，且用户自行解析内容
		2. 如果有重名的Key，后者会覆盖前者
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

	/**	@brief 切换当前读取的section
		@param [in]pszSectionName要切换的section
		@return 成功返回TRUE，失败返回FAlSE

		该函数需要在Load之后调用，如果pszSectionName不存在则会返回FALSE，\n
		并会将读取索引切换到初始状态(指向hdr）
	*/
	BOOL SwitchSectionRead(const char* pszSectionName) const;
	BOOL SwitchSectionWrite(const char* pszSectionName);


	/**	@brief 增加一个section并且修改写索引指向当前section
	@param [in]pszSectionName要切换的section
	@return 成功返回TRUE，失败返回FAlSE
	如果section已存在则失败！本函数不会删除已有section
	*/
	BOOL AddSection(const char* pszSectionName);

	/** @brief 获取函数
		@param [in] pKey, 变量名，字符串，要求唯一 \n
		函数名标明了意思\n
		default值如果不存在时返回的默认值
		@return 返回结果
	*/
	int GetInt(const char* pKey, int iDefault=0) const;
	INT64 GetInt64(const char* pKey, INT64 i64Default=0) const;
	BOOL GetBoolean(const char* pKey, BOOL bDefault=FALSE) const;
	UINT GetUint(const char* pKey, UINT nDefault=0) const;
	std::string GetString(const char* pKey, const char* pszDefault) const;

	/** @brief 设置函数
		@param [in] pKey, 变量名，字符串，要求唯一 \n
		函数名标明了意思\n
		@param [in] value值,被设置的值
		@return 如果被设置的值的key不存在返回TRUE并设置该值，\n
		 如果存在直接返回FALSE，如果需要强制修改使用对应的Force函数
	*/
	BOOL SetInt(const char* pKey, int iValue);
	BOOL SetBoolean(const char* pKey, BOOL bValue);
	BOOL SetUint(const char* pKey, UINT nValue);
	BOOL SetInt64(const char* pKey, INT64 i64Value);
	BOOL SetString(const char* pKey, const char* pValue);

	/** @brief 设置函数
		@param [in] pKey, 变量名，字符串，要求唯一 \n
		函数名标明了意思\n
		@param [in] value值,被设置的值
		@return 如果被设置的值的key不存在返回TRUE并设置该值，\n
		如果存在返回FALSE并修改对应的值
	*/
	BOOL SetIntForce(const char* pKey, int iValue);
	BOOL SetBooleanForce(const char* pKey, BOOL bValue);
	BOOL SetUintForce(const char* pKey, UINT nValue);
	BOOL SetInt64Force(const char* pKey, INT64 i64Value);
	BOOL SetStringForce(const char* pKey, const char* pValue);

	/** @brief 打印函数
		本函数调用enumFuncPrint 将内存中配置信息打印到控制台(stdout)
	*/
	void DumpAll();

	static BOOL enumFuncPrint(const std::string& section,  const std::string& pKey, std::string& value);

	/** @brief 枚举每一个key、value
		@param pFunc回调函数，对每一个key、value调用回调\n
			如果回掉函数返回FALSE则结束回调过程
		@return 返回回调次数
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
	CXCfgFile简单配置文件操作类\n
	支持Key=Value结构
	不支持Section
	@remarks 本类在未来的版本会被移除，请使用CXSettings
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
