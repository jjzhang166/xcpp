#include "stdafx.h"
#include "XThread.h"
#include <exception>

#ifdef OS_WIN
#include <process.h>
#else
#include <pthread.h>
#endif


CXThread::CXThread() : m_tHandle(0), m_semb(CXSem::SemState_Empty), m_bIsRunning(FALSE),
	m_bFinished(FALSE), m_pFunc(NULL), m_pTheadParam(NULL), m_iExitCode(0)
{
#ifdef OS_POSIX
	//pthread_mutex_init(&m_mutex, NULL);
	//pthread_cond_init(&m_cond, NULL);
#endif
}

CXThread::~CXThread()
{
	m_pTheadParam = NULL;//just for debug

	CXCriticalSection cs(&m_lock);
	if (m_bIsRunning || !m_bFinished)
	{
		printf("CXThread detroy while thread is still running. CXThread recommand you call CXThread::Wait before quit...\n");
		_ASSERT(FALSE);
	}
}

#if defined(OS_WIN)
typedef unsigned int(__stdcall *WIN_TASK_FUNCPTR)(void *);

unsigned WINAPI CXThread::wrapper_function(void* pArg)
#else
void* CXThread::wrapper_function(void* pArg)
#endif
{
	//子线程后执行
	CXThread* pThread = reinterpret_cast<CXThread *>(pArg);
	_ASSERT(pThread);

	CXCriticalSection cs(&pThread->m_lock);

#ifdef __cpp_exceptions
	try
	{
		// Call the actual client thread function
		pThread->m_pFunc(pThread->m_pTheadParam);
	}
	catch (...)
	{
		// Uncaught exceptions will terminate the application (default behavior
		// according to the C++0x draft)
		std::terminate();
	}
#else
	pThread->m_pFunc(pThread->m_pTheadParam);
#endif

	{
		pThread->m_bIsRunning = FALSE;
		pThread->m_bFinished = TRUE;
	}

#ifdef OS_POSIX
	pThread->m_semb.Give();
#endif
	return 0;
}

BOOL CXThread::CreateThread(THREAD_FUNCPTR pFunc, void* pArg, size_t stackSize)
{
	if (pFunc == NULL)
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	if (m_bIsRunning)
	{
		printf("Thread already created..\n");
		return FALSE;
	}

	m_bIsRunning = FALSE;
	m_pFunc = pFunc;
	m_pTheadParam = pArg;

	CXCriticalSection cs(&m_lock);
#if defined(OS_WIN)
	m_tHandle = (HANDLE)_beginthreadex(0, stackSize, (WIN_TASK_FUNCPTR)&CXThread::wrapper_function, (void*)this, 0, &m_tid);

	if (m_tHandle == NULL)
	{
		m_pFunc = NULL;
		m_bIsRunning = FALSE;
		m_bFinished = FALSE;
		m_pTheadParam = NULL;
		return FALSE;
	}

	m_bIsRunning = TRUE;
	return TRUE;

#elif defined(OS_POSIX)
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, stackSize);

	if (pthread_create(&m_tHandle, &attr, wrapper_function, (void *)this) != 0)//pthread_create成功返回0
		m_tHandle = 0;

	pthread_attr_destroy(&attr);

	if (m_tHandle == 0)
	{
		m_pFunc = NULL;
		m_bIsRunning = FALSE;
		m_bFinished = FALSE;
		m_pTheadParam = NULL;
		return FALSE;
	}
	return TRUE;
#endif
}

ThreadId CXThread::GetCurrentThreadId()
{
#ifdef OS_WIN
	return ::GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

void CXThread::Yeild()
{
#ifdef OS_WIN
	::Sleep(0);
#else
	//On Linux, this call always succeeds (but portable and future-proof applications should nevertheless handle a possible error return).
	error_t errorCode = pthread_yield();
	if (errorCode != 0)
	{
		printf("yield failed, errno = %d\n", errorCode);
	}
#endif
}

void CXThread::Sleep(UINT sleepMs)
{
#ifdef OS_WIN
	::Sleep(sleepMs);
#else
	if (sleepMs > 1000)
	{
		::sleep(sleepMs / 1000);
	}
	else
	{
		::usleep(sleepMs * 1000);
	}
#endif
}

BOOL CXThread::Terminate(int ExitCode)
{
	CXCriticalSection cs(&m_lock);
	if (m_bFinished || !m_bIsRunning)
	{
		return TRUE;
	}
	m_iExitCode = ExitCode;
#ifdef OS_WIN
	if (::TerminateThread(m_tHandle, ExitCode))
	{
		m_tHandle = 0;
		m_bFinished = TRUE;
		m_bIsRunning = FALSE;
		return TRUE;
	}
	return FALSE;
#else
	if (0 != pthread_cancel(m_tHandle))
	{
		return FALSE;
	}
	return m_semb.Take();
#endif
	return TRUE;
}

BOOL CXThread::IsRunning() const
{
	return m_bIsRunning;
}

BOOL CXThread::Wait(ULONG timeoutMs)
{
#ifdef OS_WIN
	if (m_tid == ::GetCurrentThreadId())
	{
		_ASSERT_X(FALSE, L"Try to wait thread quit on itself...!!!!\n");
		return FALSE;
	}

	do
	{
		if (m_bFinished || !m_bIsRunning)
		{
			return TRUE;
		}
	} while (FALSE);

	BOOL bRet = FALSE;
	switch (WaitForSingleObject(m_tHandle, timeoutMs)) 
	{
	case WAIT_OBJECT_0:
		bRet = TRUE;
		break;

	case WAIT_FAILED:
		_ASSERT_X(FALSE, L"XThread::wait: Wait wait failure");
		break;

	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
	default:
		break;
	}

	do
	{
		if (m_bFinished)
		{
			CXCriticalSection cs(&m_lock);
			CloseHandle(m_tHandle);
			m_tHandle = 0;
		}
		else
		{
			printf("CXThread(%d) maybe quit innormal\n", m_tHandle);
			CloseHandle(m_tHandle);
			m_tHandle = 0;
			m_iExitCode = -1;
			_ASSERT(FALSE);
		}
	} while (FALSE);
#else
	do
	{
		CXCriticalSection cs(&m_lock);
		if (m_bFinished || !m_bIsRunning)
		{
			return TRUE;
		}
	} while (FALSE);

	BOOL bRet = m_semb.WaitForTake(timeoutMs);
#endif
	return bRet;
}

#ifdef __XCPP_DEVELOP_CODE
#ifdef OS_WIN
unsigned WINAPI CXThreadEx::wrapperFunction(void* pArg)
#else
void* CXThreadEx::wrapperFunction(void* pArg)
#endif
{
	//子线程后执行
	CXThreadEx* pThread = reinterpret_cast<CXThreadEx *>(pArg);
	_ASSERT(pThread);

	try
	{
		pThread->m_iExitCode = pThread->Run();
	}
	catch (...)
	{
		// Uncaught exceptions will terminate the application (default behavior
		// according to the C++0x draft)
		std::terminate();
	}

	{
		pThread->m_bRunning = FALSE;
		pThread->m_bFinished = TRUE;
	}

#ifdef OS_POSIX
	pThread->m_semb.Give();
#endif
	return 0;
}

CXThreadEx::CXThreadEx() 
	: m_bRunning(FALSE), m_bFinished(FALSE), m_bStarted(FALSE), m_handle(INVALID_HANDLE_VALUE)
{

}

CXThreadEx::~CXThreadEx()
{

}

BOOL CXThreadEx::Start()
{
	if (m_bRunning)
	{
		printf("Thread already created..\n");
		return FALSE;
	}
	m_bRunning = FALSE;

	CXCriticalSection cs(&m_lock);

	UINT stackSize = XTHREAD_STACK_SIZE;//
#if defined(OS_WIN)
	m_tHandle = (HANDLE)_beginthreadex(0, stackSize, (WIN_TASK_FUNCPTR)&CXThreadX::wrapperFunction, (void*)this, 0, &m_tid);

	if (m_tHandle == NULL)
	{
		m_bRunning = FALSE;
		m_bFinished = FALSE;
		return FALSE;
	}

	m_bRunning = TRUE;
	return TRUE;

#elif defined(OS_POSIX)
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, stackSize);

	if (pthread_create(&m_tHandle, &attr, wrapper_function, (void *)this) != 0)//pthread_create成功返回0
		m_tHandle = 0;

	pthread_attr_destroy(&attr);

	if (m_tHandle == 0)
	{
		m_bRunning = FALSE;
		m_bFinished = FALSE;
		return FALSE;
	}
	return TRUE;
#endif
}

BOOL CXThreadEx::Terminate(int ExitCode)
{
	CXCriticalSection cs(&m_lock);
	if (m_bFinished || !m_bIsRunning)
	{
		return TRUE;
	}
	m_iExitCode = ExitCode;
#ifdef OS_WIN
	if (::TerminateThread(m_tHandle, ExitCode))
	{
		m_tHandle = 0;
		m_bFinished = TRUE;
		m_bIsRunning = FALSE;
		return TRUE;
	}
	return FALSE;
#else
	if (0 != pthread_cancel(m_tHandle))
	{
		return FALSE;
	}
	return m_semb.Take();
#endif
	return TRUE;}

BOOL CXThreadEx::Wait(ULONG timeOutMs)
{
#ifdef OS_WIN
	if (m_tid == ::GetCurrentThreadId())
	{
		_ASSERT_X(FALSE, "Try to wait thread quit on itself...!!!!\n");
		return FALSE;
	}

	do
	{
		if (m_bFinished || !m_bIsRunning)
		{
			return TRUE;
		}
	} while (FALSE);

	BOOL bRet = FALSE;
	switch (WaitForSingleObject(m_tHandle, timeoutMs))
	{
	case WAIT_OBJECT_0:
		bRet = TRUE;
		break;

	case WAIT_FAILED:
		_ASSERT_X(FALSE, "QThread::wait: Thread wait failure");
		break;

	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
	default:
		break;
	}

	do
	{
		if (m_bFinished)
		{
			CXCriticalSection cs(&m_lock);
			CloseHandle(m_tHandle);
			m_tHandle = 0;
		}
		else
		{
			printf("CXThread(%d) maybe quit innormal\n", m_tHandle);
			CloseHandle(m_tHandle);
			m_tHandle = 0;
			m_iExitCode = -1;
			_ASSERT(FALSE);
		}
	} while (FALSE);
#else
	do
	{
		CXCriticalSection cs(&m_lock);
		if (m_bFinished || !m_bIsRunning)
		{
			return TRUE;
		}
	} while (FALSE);

	BOOL bRet = m_semb.WaitForTake(timeoutMs);
#endif
	return bRet;
}

BOOL CXThreadEx::SetStackSize(UINT nSizeInBytes)
{
	CXCriticalSection cs(&m_lock);
	if (m_tHandle == NULL)
	{
	}
	return FALSE;
}
#endif

CXThreadWaiter::CXThreadWaiter()
{
}

BOOL CXThreadWaiter::Add(CXThread* pThread)
{
	_ASSERT(pThread);
	CXCriticalSection cs(&m_mutex);
	m_threadList.push_back(pThread);
	return TRUE;
}

CXThreadWaiter::~CXThreadWaiter()
{
}

CXThreadWaiter& CXThreadWaiter::operator << (CXThread* pThread)
{
	_ASSERT(pThread);
	Add(pThread);
	return *this;
}

void CXThreadWaiter::WaitAll()
{
	UINT nSize = m_threadList.size();
	UINT nIdx = 0;
	CXThread** ppThreads = (CXThread**)alloca(sizeof(CXThread*)* nSize);
	for (std::list<CXThread*>::iterator iter = m_threadList.begin();
		iter != m_threadList.end(); ++iter)
	{
		ppThreads[nIdx++] = (*iter);
	}

	for (UINT i = 0; i < nIdx; i++)
	{
		ppThreads[i]->Wait();
	}
}

void CXThreadWaiter::DeleteAll()
{
	UINT nSize = m_threadList.size();
	UINT nIdx = 0;
	CXThread** ppThreads = (CXThread**)alloca(sizeof(CXThread*)* nSize);

	for (std::list<CXThread*>::iterator iter = m_threadList.begin();
		iter != m_threadList.end(); ++iter)
	{
		ppThreads[nIdx++] = (*iter);
	}

	for (UINT i = 0; i < nIdx; i++)
	{
		if (ppThreads[i]->IsRunning())
		{
			ppThreads[i]->Terminate(-1);
		}
		delete ppThreads[i];
	}
	m_threadList.clear();
}

