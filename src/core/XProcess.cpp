/*! XProcess.cpp
********************************************************************************
description			:  A simple cross platform process interface
related files		:  stdafx.h XUtils.h XProcess.h
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
2014/19/17			CHENQ   add create process in linux

note:
大多数操作系统进程都有类似的特性：
1. 决定子进程是否继承父进程的资源
	a. 环境变量
	b. 内存资源 (共享或者拷贝副本)
	c. 标准输入输出流 stdin stdout stderr

*******************************************************************************/


#include "stdafx.h"

#include <string>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "XSemaphore.h"
#include "XThread.h"
#include "XProcess.h"
#include "XLog.h"
#include "XUtils.h"

#define RAW_LOG XLOG_PRINT


#ifdef OS_WIN
#include <UserEnv.h>
const DWORD kProcessKilledExitCode = 1;
#pragma comment(lib, "userenv.lib")
#elif defined(OS_LINUX)
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/syscall.h>

#define SUBPROCESS_QUIT_BY_FATHER 0
#define SUBPROCESS_QUIT_BY_SELF 1
#define SUBPROCESS_QUIT_METHOD SUBPROCESS_QUIT_BY_SELF

#if SUBPROCESS_QUIT_METHOD==SUBPROCESS_QUIT_BY_FATHER
class CPrivateProcessSignalHandler
{
public:
	explicit CPrivateProcessSignalHandler() {}
	virtual ~CPrivateProcessSignalHandler(){}

	void AddProcess(const ProcessId& pid, const ProcessHandle& handle)
	{
		CXCriticalSection lock(&m_mutex);
		m_ChildProcessMap.insert(std::map<ProcessId, ProcessHandle>::value_type(pid, handle));
	}

	//子进程自动退出处理
	void HandleChildQuitSignal(int signo) 
	{
		UNUSED(signo);
		pid_t pid = 0;
		int stat = 0;
		while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0 ) 
		{
			RAW_LOG(LOG_DEBUG, "child %d exit by self\n", pid );
			CXCriticalSection lock(&m_mutex);
			std::map<ProcessId, ProcessHandle>::iterator iter = m_ChildProcessMap.find(pid);
			if (iter != m_ChildProcessMap.end())
			{
				//closeHandle();
				m_ChildProcessMap.erase(iter);
				break;
			}
		}
	}

	//父进程(自己)退出(收到用户控制台CtrlC,收到kill 9, kill 15)处理
	void HandleTerminateSignals(int signo)
	{
		UNUSED(signo);
		CXCriticalSection lock(&m_mutex);
		CloseAllChild();
	}

protected:
	void CloseAllChild()
	{
		std::map<ProcessId, ProcessHandle>::iterator iter = m_ChildProcessMap.begin();
		for ( ;iter != m_ChildProcessMap.end(); ++iter)
		{
			CXProcess::Kill(iter->second, -1, true);
			m_ChildProcessMap.erase(iter);
		}
		m_ChildProcessMap.clear();
	}
private:
	std::map<ProcessId, ProcessHandle> m_ChildProcessMap;
	CXSemMutex m_mutex;
};

static CPrivateProcessSignalHandler sg_procesHandler;

static void handleChildQuit(int signo) 
{
	sg_procesHandler.HandleChildQuitSignal(signo);
}

static void handleTerminate(int signo)
{
	sg_procesHandler.HandleTerminateSignals(signo);
}
#else
static void childAutoQuit(BOOL* pbCheckQuit)
{
	for (;*pbCheckQuit;)
	{
		pid_t ppid = getppid();
		pid_t pid = getpid();
		if (ppid == 1)
		{
			RAW_LOG(LOG_DEBUG, "child <%d> exit, parent is init(1)\n", pid);
			exit(-1);
		}
		sleep(2);
	}
}

static void handleChildQuit(int signo)
{
	pid_t pid = 0;
	int stat = 0;
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0 ) 
	{
		RAW_LOG(LOG_DEBUG, "child <%d> exit by self\n", pid );
	}
}
#endif

#endif

#ifdef OS_POSIX
#include "XFileHelper.h"
#include "XPosix_P.h"
#include "XLinux_P.h"
#endif

#if defined(OS_LINUX)
 static const rlim_t kSystemDefaultMaxFds = 8192;
 static const char kFDDir[] = "/proc/self/fd";
#elif defined(OS_MACOSX)
 static const rlim_t kSystemDefaultMaxFds = 256; 
static const char kFDDir[] = "/dev/fd";
#elif defined(OS_SOLARIS)
 static const rlim_t kSystemDefaultMaxFds = 8192;
 static const char kFDDir[] = "/dev/fd";
#elif defined(OS_FREEBSD)
 static const rlim_t kSystemDefaultMaxFds = 8192;
 static const char kFDDir[] = "/dev/fd";
#elif defined(OS_OPENBSD)
 static const rlim_t kSystemDefaultMaxFds = 256;
 static const char kFDDir[] = "/dev/fd";
#elif defined(OS_ANDROID)
 static const rlim_t kSystemDefaultMaxFds = 1024;
 static const char kFDDir[] = "/proc/self/fd";
#endif




//每个进程都有一个环境块，它包含了一套环境变量和它的值。
//命令处理器提供了一套命令，以显示其环境块或创建新的环境变量。
//命令处理器以继承命令处理器的环境变量启动程序。
//默认情况下，子进程继承父进程的环境变量。
//但是，你可以指定一个不同的环境，为子进程创建一个新的环境块和通过CreateProcess函数（或者CreateProcessAsUser函数）的参数传递一个指向它的指针。
//每个环境块包含以下格式的环境变量：
//
//
//VAR1 = 值1\0
//VAR2 = 值2\0
//VAR3 = 值3\0
//... ...
//VarN = ValueN\0\0
//
//
//环境变量的名称不能包含一个等号（ = ）。
//
//
//GetEnvironmentStrings函数返回指向调用进程的环境块的一个指针，这应该被视为一个只读块，不直接修改它。相反，使用SetEnvironmentVariable函数来改变一个环境变量。当你完成从GetEnvironmentStrings获得环境块，调用FreeEnvironmentStrings函数来释放块。
//
//
//调用SetEnvironmentVariable系统环境变量没有影响。用户可以添加或修改系统环境变量，使用控制面板。以编程方式添加或修改系统环境变量，将它们添加到HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\CurrentControlSet\Control\Session Manager\Environment 注册表项，然后广播一条lParam设置为字符串“Environment”的WM_SETTINGCHANGE消息（BroadcastSystemMessage）。这允许应用程序，如shell，更新你的设置。请注意，这个关键的环境变量的值被限制为1024个字符。
//
//
//GetEnvironmentVariable函数确定指定的变量是否定义是在调用进程的环境中，而且，如果是这样，它的值是什么。
//
//
//要检索一个给定的用户环境块的复制，使用CreateEnvironmentBlock函数，去释放一个由CreateEnvironmentBlock创建的环境块，使用DestroyEnvironmentBlock函数。这些函数引用了一个环境块，该环境块是一个有空结束符的Unicode文本数组，列表结尾使用两个空字符（\0\0）。
//
//
//为了扩大环境变量的字符串，使用ExpandEnvironmentStrings函数（或者ExpandEnvironmentStringsForUser函数）。
//
//
//进入 我的电脑 右击 属性 高级 环境变量 即可看到。
//
//以下是常见环境变量
//%ALLUSERSPROFILE% 局部 返回所有“用户配置文件”的位置。
//%APPDATA% 局部 返回默认情况下应用程序存储数据的位置。
//%CD% 局部 返回当前目录字符串。
//%CMDCMDLINE% 局部 返回用来启动当前的 Cmd.exe 的准确命令行。
//%CMDEXTVERSION% 系统 返回当前的“命令处理程序扩展”的版本号。
//%COMPUTERNAME% 系统 返回计算机的名称。
//%COMSPEC% 系统 返回命令行解释器可执行程序的准确路径。
//%DATE% 系统 返回当前日期。使用与 date / t 命令相同的格式。由 Cmd.exe 生成。有关 date 命令的详细信息，请参阅 Date。
//%ERRORLEVEL% 系统 返回最近使用过的命令的错误代码。通常用非零值表示错误。
//%HOMEDRIVE% 系统 返回连接到用户主目录的本地工作站驱动器号。基于主目录值的设置。用户主目录是在“本地用户和组”中指定的。
//%HOMEPATH% 系统 返回用户主目录的完整路径。基于主目录值的设置。用户主目录是在“本地用户和组”中指定的。
//%HOMESHARE% 系统 返回用户的共享主目录的网络路径。基于主目录值的设置。用户主目录是在“本地用户和组”中指定的。
//%LOGONSEVER% 局部 返回验证当前登录会话的域控制器的名称。
//%NUMBER_OF_PROCESSORS% 系统 指定安装在计算机上的处理器的数目。
//%OS% 系统 返回操作系统的名称。Windows 2000 将操作系统显示为 Windows_NT。
//%PATH% 系统 指定可执行文件的搜索路径。
//%PATHEXT% 系统 返回操作系统认为可执行的文件扩展名的列表。
//%PROCESSOR_ARCHITECTURE% 系统 返回处理器的芯片体系结构。值 : x86，IA64。
//%PROCESSOR_IDENTFIER% 系统 返回处理器说明。
//%PROCESSOR_LEVEL% 系统 返回计算机上安装的处理器的型号。
//%PROCESSOR_REVISION% 系统 返回处理器修订号的系统变量。
//%PROMPT% 局部 返回当前解释程序的命令提示符设置。由 Cmd.exe 生成。
//%RANDOM% 系统 返回 0 到 32767 之间的任意十进制数字。由 Cmd.exe 生成。
//%SYSTEMDRIVE% 系统 返回包含 Windows XP 根目录（即系统根目录）的驱动器。
//%SYSTEMROOT% 系统 返回 Windows XP 根目录的位置。
//%TEMP% and %TMP% 系统和用户 返回对当前登录用户可用的应用程序所使用的默认临时目录。有些应用程序需要 TEMP，而其它应用程序则需要 TMP。
//%TIME% 系统 返回当前时间。使用与 time / t 命令相同的格式。由 Cmd.exe 生成。有关 time 命令的详细信息，请参阅 Time。
//%USERDOMAIN% 局部 返回包含用户帐户的域的名称。
//%USERNAME% 局部 返回当前登录的用户的名称。
//%UserPrefix% 局部 返回当前用户的配置文件的位置。
//%WINDIR% 系统 返回操作系统目录的位置。

CXProcess::~CXProcess()
{
}

XProcessId CXProcess::GetCurrentProcessId()
{
#ifdef OS_WIN
	return ::GetCurrentProcessId();
#else
	return ::getpid();
#endif
}

void CXProcess::Init()
{
#ifdef OS_LINUX

#if SUBPROCESS_QUIT_METHOD==SUBPROCESS_QUIT_BY_FATHER
	signal(SIGKILL, &handleTerminate);
	signal(SIGINT, &handleTerminate);
	signal(SIGTERM, &handleTerminate);
#endif
	signal(SIGCHLD, &handleChildQuit);    
#endif
}

static CXThread* sg_pThread = NULL;
static BOOL sg_bCheckQuit = TRUE;
void CXProcess::InitProcessAttached()
{
#ifdef OS_WIN
#else
	 if (sg_pThread)
	 {
		 return;
	 }
	 sg_pThread = new CXThread();
	 sg_pThread->CreateThread((THREAD_FUNCPTR)&childAutoQuit, &sg_bCheckQuit, 16 * 1024);
#endif
}

#ifdef OS_POSIX
static int RunningOnValgrind() {return 0;}
static void ResetChildSignalHandlersToDefaults() 
{
	// The previous signal handlers are likely to be meaningless in the child's
	// context so we reset them to the defaults for now. http://crbug.com/44953
	// These signal handlers are set up at least in browser_main_posix.cc:
	// BrowserMainPartsPosix::PreEarlyInitialization and stack_trace_posix.cc:
	// EnableInProcessStackDumping.
	signal(SIGHUP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGABRT, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGBUS, SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGSYS, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
}
#endif

BOOL CXProcess::Kill(XProcessHandle process, int exit_code, bool wait)
{
#ifdef OS_WIN
	BOOL result = (TerminateProcess(process, exit_code) != FALSE);
	if (result && wait)
	{
		// The process may not end immediately due to pending I/O
		if (WAIT_OBJECT_0 != WaitForSingleObject(process, 60 * 1000))
			std::cout << "Error waiting for process exit";
	}
	else if (!result)
	{
		std::cout << "Unable to terminate process, errno is " << GetLastError();
	}
	return result;
#else
	ProcessId process_id = process;// linux don't have handle, pid==handle
	_ASSERT_X(process_id>1, " tried to kill invalid process_id") ;
	if (process_id <= 1)
		return FALSE;
	BOOL result = kill(process_id, SIGTERM) == 0;
	if (result && wait) 
	{
		int tries = 60;

		if (RunningOnValgrind()) 
		{
			// Wait for some extra time when running under Valgrind since the child
			// processes may take some time doing leak checking.
			tries *= 2;
		}

		unsigned sleep_ms = 4;

		// The process may not end immediately due to pending I/O
		bool exited = false;
		while (tries-- > 0) 
		{
			pid_t pid = HANDLE_EINTR(waitpid(process_id, NULL, WNOHANG));
			if (pid == process_id) 
			{
				exited = true;
				break;
			}
			if (pid == -1) 
			{
				if (errno == ECHILD) 
				{
					// The wait may fail with ECHILD if another process also waited for
					// the same pid, causing the process state to get cleaned up.
					exited = true;
					break;
				}
				std::cout << "Error waiting for process " << process_id;
			}

			usleep(sleep_ms * 1000);
			const unsigned kMaxSleepMs = 1000;
			if (sleep_ms < kMaxSleepMs)
				sleep_ms *= 2;
		}

		// If we're waiting and the child hasn't died by now, force it
		// with a SIGKILL.
		if (!exited)
			result = kill(process_id, SIGKILL) == 0;
	}

	if (!result)
	{
		std::cout << "Unable to terminate process " << process_id;
	}
	return result;
#endif 
}

XSTLString CXProcess::GetCurrentExcutePath()
{
#ifdef OS_WIN
	TCHAR szPath[MAX_PATH] = {0};
	if (::GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		return XSTLString(szPath);
	}
	return XSTLString(_T(""));
#else
	char szName[PATH_MAX] = {0};
	if (readlink("/proc/self/exe", szName, sizeof(szName)) > 0)
	{
		std::string str = szName;
		return XString16(CXUtils::s2ws(str));
	}
	return XString16(L"");
#endif
}

XSTLString CXProcess::GetCurrentExcuteName()
{
	XSTLString name = CXProcess::GetCurrentExcutePath();
	if (name.size() != 0)
	{
#ifdef OS_WIN
		size_t pos = name.find_last_of(_T("\\"));
#else
		size_t pos = name.find_last_of(_T("/"));
#endif
		name.erase(0, pos);
	}
	return name;
}

XSTLStringA CXProcess::GetCurrentExcuteNameA()
{
#if defined(UNICODE)
	XSTLStringW wname = GetCurrentExcuteName();
	return CXUtils::ws2s(wname);
#else
	return CXProcess::GetCurrentExcuteName();
#endif
}

BOOL CXProcess::IsProcessExit(XProcessHandle pHandle)
{
#ifdef OS_WIN
	DWORD exitCode = 0;
	BOOL bOk = GetExitCodeProcess(pHandle, &exitCode);
	if (bOk && exitCode==STILL_ACTIVE)
	{
		return TRUE;
	}
	return FALSE;
#else
	int status = kill(pHandle, 0);
	return status==0?TRUE:FALSE;
#endif
}

#if 0
BOOL CXProcess::IsExit()
{
	return CXProcess::IsProcessExit(m_pHandle);
}

BOOL CXProcess::Close()
{
	BOOL bRet = FALSE;
#ifdef OS_WIN
	bRet = ::CloseHandle(m_pHandle);
	m_pHandle = INVALID_HANDLE_VALUE;
#else
	close(m_pid);
	m_pid = 0;
	m_pHandle = 0;
	bRet = TRUE;
#endif
	return bRet;
}
#endif