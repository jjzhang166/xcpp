/*! XUtils.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform utilitis tools
related files		:  stdafx.h XUtils.cpp
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
2014/10/11			CHENQ	move CXUtils::Sleep to CXThread::Sleep
							move CXUtils::GetLastSocketError to CXSocket::GetLastSocketError
2014/10/16			CHENQ	add CXUtils::GetTickCount for tickcount benifit function
*******************************************************************************/

#ifndef __UTILSX_H__
#define __UTILSX_H__

#include "XSTLMacro.h"
#include <vector>
#include <map>
#include <list>
#include <set>

#define SECOND_OF_HOUR (3600U)
#define SECOND_OF_DAY (86400U)
#define SECOND_OF_WEAK (604800U)

#define NSCOUNT_OF_SEC 1000000000U
#define NSCOUNT_OF_MS	1000000U

#define TIME_NS_TO_SEC(_u64Ns) ((_u64Ns)/NSCOUNT_OF_SEC)
#define TIME_SEC_TO_NS(_sec) ((UINT64)_sec*NSCOUNT_OF_SEC)
#define GET_NS_OF_NS64(_u64Ns) ((UINT32)(_u64Ns%NSCOUNT_OF_SEC))
#define TIME_NS64_TO_MS(_u64Ns) ((_u64Ns)/NSCOUNT_OF_MS)

class CSingleton
{
public:
	friend class CGarbo;

	CSingleton* Create()
	{
		if (sg_pInstance)
		{
			return CSingleton::sg_pInstance;
		}
		return new CSingleton();
	}

	CSingleton* Instance() { return CSingleton::sg_pInstance; }
protected:
	virtual ~CSingleton(){}
private:
	class CGarbo   //它的唯一工作就是在析构函数中删除CSingleton的实例  
	{
	public:
		CGarbo()
		{
		}
		~CGarbo()
		{
			if (CSingleton::sg_pInstance)
				delete CSingleton::sg_pInstance;
		}
	};

	CSingleton()//TODO:
	{
		sg_pInstance = NULL;
	}
	static CSingleton* sg_pInstance;
	static CGarbo m_gGarbo;  //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数
};

#ifndef OS_WIN
struct GUID 
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
};
#endif

class CXUtils
{
public:
	/** 随机生成GUID/UUID
	@brief 生成GUID/UUID
	@return 成功GUID，如果成员全为0，则获取失败
	*/
	static GUID BuildGuid();

	/**
	@brief 将UTC时间转化为struct tm结构体
	@param [in] t 传入的UTC时间
	@param [out] pTm 返回local时间tm结构体
	@return 成功返回TRUE，失败返回FALE
	*/
	static BOOL LocalTime(const time_t& t, struct tm* pTm);

	/**
	@brief 获取编译器版本号
	@return 返回编译器版本号字符串
	*/
	static std::string GetCompilerVersion();

	/**
	@brief 分割字符串
	@param [in] s 待分割字符串
	@param [in] delim 分割字符串 
	@param [out] pRetSubString 返回子串vector
	@return 返回分割后子串个数
	*/
	static size_t split(const std::string& s, const std::string& delim, std::vector<std::string>* pRetSubString);

	static size_t split(const std::string& s, const std::string& delim, std::list<std::string>* pRetSubString);

	/**
	@brief 转换宽字符字符串到窄字符字符串
	@return 窄字符字符串
	*/
	static XSTLStringA ws2s(const XSTLStringW& ws);

	/**
	@brief 转换窄字符字符串到宽字符字符串
	@return 宽字符字符串
	*/
	static XSTLStringW s2ws(const XSTLStringA& s);

	/**
	@brief 获取系统时间以纳米为单位
		具体精确程度依赖于操作系统，
		大部分桌面操作系统精度在10微妙到10毫秒级
	@return 返回当前系统以纳秒为单位的时间
	*/
	static UINT64 GetCurrentTimeNs64();

	/**
	@brief 获取系统时间以纳米为单位
	具体精确程度依赖于操作系统，
	大部分桌面操作系统精度在10微妙到10毫秒级
	@param pTv 当前struct time格式的时间
	@return 返回当前系统以纳秒为单位的时间
	*/
	static BOOL GetCurrentTimeTv(struct timeval* pTv);

	/**
	@brief 获取系统时间以毫秒为单位
		本函数仅可用于时间间隔测量、计数，不可用于时间表示\n
		因为UINT32 不足以表示UTC时间
	@return 返回当前系统以毫秒为单位的时间
	*/
	static UINT32 GetTickCount();

	/**
	@brief 获取当前错误码
	@return 返回错误码
	@deprecated 由于特殊的原因，这个函数可能会在将来的版本中取消。
	*/
	static errno_t GetLastError();

	/**
	@brief 获取当前SOCKET错误码
	@return 返回错误码
	@deprecated 由于特殊的原因，这个函数可能会在将来的版本中取消。
	*/
	static errno_t GetLastSocketError();

	/**
	@brief 判断文件是否存在
	@param [in] pszFilePath 文件路径
	@return 存在返回TRUE，不存在返回ＦＡＬＳＥ
	@deprecated 由于特殊的原因，这个函数可能会在将来的版本中取消。
	*/
	static BOOL IsFileExist(const char* pszFilePath);
};

#endif /*__UTILSX_H__*/