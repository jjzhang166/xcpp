/*! XProcess.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform process interface
related files		:  stdafx.h XUtils.h XProcess.cpp
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com  
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
*******************************************************************************/

#ifndef __XPROCESS_H__
#define __XPROCESS_H__

#include "XSTLMacro.h"

#if defined(OS_WIN)
typedef HANDLE XProcessHandle;
typedef DWORD XProcessId;
#elif defined(OS_POSIX)
// On POSIX, our ProcessHandle will just be the PID.
typedef pid_t XProcessHandle;
typedef pid_t XProcessId;
#endif  // defined(OS_WIN)

class CXProcess
{
public:
	CXProcess();
	virtual ~CXProcess();

	/* @brief ����һ������
		@detail ���������ú�ǰ�̱߳�������ֱ���ӽ��̷���\n
			
		@param [in] cmd,ִ�г������ƣ�����ȫ·����,
		@param [in] paramList,�����б�
		@param [out] pReturn, ��������������������ӽ��̵ı�׼�����ı�
		@return //TODO::
	*/
	static int Execute(const XSTLString& cmd, XSTLString* pReturn = NULL);
	static int Execute(const XSTLString& cmd, const XSTLStringList& paramList, XSTLString* pReturn=NULL);

	/* @brief ����һ������
		@detail ���������ӽ��̺��������أ��������˳����ӽ��̼���ִ��
	@param [in] cmd,ִ�г������ƣ�����ȫ·����,
	@param [in] paramList,�����б�
	@param [out] pRunText, ����
	@return //TODO::
	*/
	static int StartDetached(const XSTLString& cmd);
	static int StartDetached(const XSTLString& cmd, const XSTLStringList& paramList);


	/**
		@brief ��ʼ�����̻���
			�����Ҫ�����ӽ��̣������ʹ�øú�������������Ҫ�Լ���д��������ֹ�γɽ�ʬ����	
	*/
	static void Init();

	/**
	@brief ��ʼ�����̻���
		����������˳����ӽ����Զ��˳�;
	*/
	static void InitProcessAttached();


	static XSTLString GetWorkingDir();

	static BOOL IsProcessExit(XProcessHandle pHandle);
	static BOOL Kill(XProcessHandle process, int exit_code, bool wait);
	static void Terminate(XProcessHandle pHandle);

	static XProcessHandle GetCurrentProcessHandle();
	static XProcessId GetCurrentProcessId();
	static XSTLString GetCurrentExcuteName();
	static XSTLStringA GetCurrentExcuteNameA();

	static XSTLString GetCurrentExcutePath();
	
#ifdef OS_WIN
	XProcessId GetProcessIdByHandle(HANDLE Process);
#endif
	
private:
	XProcessHandle m_pHandle;
	XProcessId m_pid;
};
#endif // endof
