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

	/* @brief 运行一个程序
		@detail 函数被调用后当前线程被阻塞，直到子进程返回\n
			
		@param [in] cmd,执行程序名称（或者全路径）,
		@param [in] paramList,参数列表
		@param [out] pReturn, 可以利用这个参数返回子进程的标准数据文本
		@return //TODO::
	*/
	static int Execute(const XSTLString& cmd, XSTLString* pReturn = NULL);
	static int Execute(const XSTLString& cmd, const XSTLStringList& paramList, XSTLString* pReturn=NULL);

	/* @brief 运行一个程序
		@detail 函数启动子进程后立即返回，主进程退出后子进程继续执行
	@param [in] cmd,执行程序名称（或者全路径）,
	@param [in] paramList,参数列表
	@param [out] pRunText, 返回
	@return //TODO::
	*/
	static int StartDetached(const XSTLString& cmd);
	static int StartDetached(const XSTLString& cmd, const XSTLStringList& paramList);


	/**
		@brief 初始化进程环境
			如果需要创建子进程，请务必使用该函数，否则您需要自己编写代码来防止形成僵尸进程	
	*/
	static void Init();

	/**
	@brief 初始化进程环境
		如果父进程退出，子进程自动退出;
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
