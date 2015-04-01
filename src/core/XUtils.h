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
	class CGarbo   //����Ψһ��������������������ɾ��CSingleton��ʵ��  
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
	static CGarbo m_gGarbo;  //����һ����̬��Ա�������������ʱ��ϵͳ���Զ�����������������
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
	/** �������GUID/UUID
	@brief ����GUID/UUID
	@return �ɹ�GUID�������ԱȫΪ0�����ȡʧ��
	*/
	static GUID BuildGuid();

	/**
	@brief ��UTCʱ��ת��Ϊstruct tm�ṹ��
	@param [in] t �����UTCʱ��
	@param [out] pTm ����localʱ��tm�ṹ��
	@return �ɹ�����TRUE��ʧ�ܷ���FALE
	*/
	static BOOL LocalTime(const time_t& t, struct tm* pTm);

	/**
	@brief ��ȡ�������汾��
	@return ���ر������汾���ַ���
	*/
	static std::string GetCompilerVersion();

	/**
	@brief �ָ��ַ���
	@param [in] s ���ָ��ַ���
	@param [in] delim �ָ��ַ��� 
	@param [out] pRetSubString �����Ӵ�vector
	@return ���طָ���Ӵ�����
	*/
	static size_t split(const std::string& s, const std::string& delim, std::vector<std::string>* pRetSubString);

	static size_t split(const std::string& s, const std::string& delim, std::list<std::string>* pRetSubString);

	/**
	@brief ת�����ַ��ַ�����խ�ַ��ַ���
	@return խ�ַ��ַ���
	*/
	static XSTLStringA ws2s(const XSTLStringW& ws);

	/**
	@brief ת��խ�ַ��ַ��������ַ��ַ���
	@return ���ַ��ַ���
	*/
	static XSTLStringW s2ws(const XSTLStringA& s);

	/**
	@brief ��ȡϵͳʱ��������Ϊ��λ
		���徫ȷ�̶������ڲ���ϵͳ��
		�󲿷��������ϵͳ������10΢�10���뼶
	@return ���ص�ǰϵͳ������Ϊ��λ��ʱ��
	*/
	static UINT64 GetCurrentTimeNs64();

	/**
	@brief ��ȡϵͳʱ��������Ϊ��λ
	���徫ȷ�̶������ڲ���ϵͳ��
	�󲿷��������ϵͳ������10΢�10���뼶
	@param pTv ��ǰstruct time��ʽ��ʱ��
	@return ���ص�ǰϵͳ������Ϊ��λ��ʱ��
	*/
	static BOOL GetCurrentTimeTv(struct timeval* pTv);

	/**
	@brief ��ȡϵͳʱ���Ժ���Ϊ��λ
		��������������ʱ������������������������ʱ���ʾ\n
		��ΪUINT32 �����Ա�ʾUTCʱ��
	@return ���ص�ǰϵͳ�Ժ���Ϊ��λ��ʱ��
	*/
	static UINT32 GetTickCount();

	/**
	@brief ��ȡ��ǰ������
	@return ���ش�����
	@deprecated ���������ԭ������������ܻ��ڽ����İ汾��ȡ����
	*/
	static errno_t GetLastError();

	/**
	@brief ��ȡ��ǰSOCKET������
	@return ���ش�����
	@deprecated ���������ԭ������������ܻ��ڽ����İ汾��ȡ����
	*/
	static errno_t GetLastSocketError();

	/**
	@brief �ж��ļ��Ƿ����
	@param [in] pszFilePath �ļ�·��
	@return ���ڷ���TRUE�������ڷ��أƣ��̣ӣ�
	@deprecated ���������ԭ������������ܻ��ڽ����İ汾��ȡ����
	*/
	static BOOL IsFileExist(const char* pszFilePath);
};

#endif /*__UTILSX_H__*/