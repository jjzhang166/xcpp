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
���������ϵͳ���̶������Ƶ����ԣ�
1. �����ӽ����Ƿ�̳и����̵���Դ
	a. ��������
	b. �ڴ���Դ (������߿�������)
	c. ��׼��������� stdin stdout stderr

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

	//�ӽ����Զ��˳�����
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

	//������(�Լ�)�˳�(�յ��û�����̨CtrlC,�յ�kill 9, kill 15)����
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




//ÿ�����̶���һ�������飬��������һ�׻�������������ֵ��
//��������ṩ��һ���������ʾ�价����򴴽��µĻ���������
//��������Լ̳���������Ļ���������������
//Ĭ������£��ӽ��̼̳и����̵Ļ���������
//���ǣ������ָ��һ����ͬ�Ļ�����Ϊ�ӽ��̴���һ���µĻ������ͨ��CreateProcess����������CreateProcessAsUser�������Ĳ�������һ��ָ������ָ�롣
//ÿ��������������¸�ʽ�Ļ���������
//
//
//VAR1 = ֵ1\0
//VAR2 = ֵ2\0
//VAR3 = ֵ3\0
//... ...
//VarN = ValueN\0\0
//
//
//�������������Ʋ��ܰ���һ���Ⱥţ� = ����
//
//
//GetEnvironmentStrings��������ָ����ý��̵Ļ������һ��ָ�룬��Ӧ�ñ���Ϊһ��ֻ���飬��ֱ���޸������෴��ʹ��SetEnvironmentVariable�������ı�һ������������������ɴ�GetEnvironmentStrings��û����飬����FreeEnvironmentStrings�������ͷſ顣
//
//
//����SetEnvironmentVariableϵͳ��������û��Ӱ�졣�û�������ӻ��޸�ϵͳ����������ʹ�ÿ�����塣�Ա�̷�ʽ��ӻ��޸�ϵͳ������������������ӵ�HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\CurrentControlSet\Control\Session Manager\Environment ע����Ȼ��㲥һ��lParam����Ϊ�ַ�����Environment����WM_SETTINGCHANGE��Ϣ��BroadcastSystemMessage����������Ӧ�ó�����shell������������á���ע�⣬����ؼ��Ļ���������ֵ������Ϊ1024���ַ���
//
//
//GetEnvironmentVariable����ȷ��ָ���ı����Ƿ������ڵ��ý��̵Ļ����У����ң����������������ֵ��ʲô��
//
//
//Ҫ����һ���������û�������ĸ��ƣ�ʹ��CreateEnvironmentBlock������ȥ�ͷ�һ����CreateEnvironmentBlock�����Ļ����飬ʹ��DestroyEnvironmentBlock��������Щ����������һ�������飬�û�������һ���пս�������Unicode�ı����飬�б��βʹ���������ַ���\0\0����
//
//
//Ϊ�����󻷾��������ַ�����ʹ��ExpandEnvironmentStrings����������ExpandEnvironmentStringsForUser��������
//
//
//���� �ҵĵ��� �һ� ���� �߼� �������� ���ɿ�����
//
//�����ǳ�����������
//%ALLUSERSPROFILE% �ֲ� �������С��û������ļ�����λ�á�
//%APPDATA% �ֲ� ����Ĭ�������Ӧ�ó���洢���ݵ�λ�á�
//%CD% �ֲ� ���ص�ǰĿ¼�ַ�����
//%CMDCMDLINE% �ֲ� ��������������ǰ�� Cmd.exe ��׼ȷ�����С�
//%CMDEXTVERSION% ϵͳ ���ص�ǰ�ġ�����������չ���İ汾�š�
//%COMPUTERNAME% ϵͳ ���ؼ���������ơ�
//%COMSPEC% ϵͳ ���������н�������ִ�г����׼ȷ·����
//%DATE% ϵͳ ���ص�ǰ���ڡ�ʹ���� date / t ������ͬ�ĸ�ʽ���� Cmd.exe ���ɡ��й� date �������ϸ��Ϣ������� Date��
//%ERRORLEVEL% ϵͳ �������ʹ�ù�������Ĵ�����롣ͨ���÷���ֵ��ʾ����
//%HOMEDRIVE% ϵͳ �������ӵ��û���Ŀ¼�ı��ع���վ�������š�������Ŀ¼ֵ�����á��û���Ŀ¼���ڡ������û����顱��ָ���ġ�
//%HOMEPATH% ϵͳ �����û���Ŀ¼������·����������Ŀ¼ֵ�����á��û���Ŀ¼���ڡ������û����顱��ָ���ġ�
//%HOMESHARE% ϵͳ �����û��Ĺ�����Ŀ¼������·����������Ŀ¼ֵ�����á��û���Ŀ¼���ڡ������û����顱��ָ���ġ�
//%LOGONSEVER% �ֲ� ������֤��ǰ��¼�Ự��������������ơ�
//%NUMBER_OF_PROCESSORS% ϵͳ ָ����װ�ڼ�����ϵĴ���������Ŀ��
//%OS% ϵͳ ���ز���ϵͳ�����ơ�Windows 2000 ������ϵͳ��ʾΪ Windows_NT��
//%PATH% ϵͳ ָ����ִ���ļ�������·����
//%PATHEXT% ϵͳ ���ز���ϵͳ��Ϊ��ִ�е��ļ���չ�����б�
//%PROCESSOR_ARCHITECTURE% ϵͳ ���ش�������оƬ��ϵ�ṹ��ֵ : x86��IA64��
//%PROCESSOR_IDENTFIER% ϵͳ ���ش�����˵����
//%PROCESSOR_LEVEL% ϵͳ ���ؼ�����ϰ�װ�Ĵ��������ͺš�
//%PROCESSOR_REVISION% ϵͳ ���ش������޶��ŵ�ϵͳ������
//%PROMPT% �ֲ� ���ص�ǰ���ͳ����������ʾ�����á��� Cmd.exe ���ɡ�
//%RANDOM% ϵͳ ���� 0 �� 32767 ֮�������ʮ�������֡��� Cmd.exe ���ɡ�
//%SYSTEMDRIVE% ϵͳ ���ذ��� Windows XP ��Ŀ¼����ϵͳ��Ŀ¼������������
//%SYSTEMROOT% ϵͳ ���� Windows XP ��Ŀ¼��λ�á�
//%TEMP% and %TMP% ϵͳ���û� ���ضԵ�ǰ��¼�û����õ�Ӧ�ó�����ʹ�õ�Ĭ����ʱĿ¼����ЩӦ�ó�����Ҫ TEMP��������Ӧ�ó�������Ҫ TMP��
//%TIME% ϵͳ ���ص�ǰʱ�䡣ʹ���� time / t ������ͬ�ĸ�ʽ���� Cmd.exe ���ɡ��й� time �������ϸ��Ϣ������� Time��
//%USERDOMAIN% �ֲ� ���ذ����û��ʻ���������ơ�
//%USERNAME% �ֲ� ���ص�ǰ��¼���û������ơ�
//%UserPrefix% �ֲ� ���ص�ǰ�û��������ļ���λ�á�
//%WINDIR% ϵͳ ���ز���ϵͳĿ¼��λ�á�

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