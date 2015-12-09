#include "XAfx.h"
#include "XSemaphore.h"
#include "XUtils.h"
#include "XThread.h"
#include <fcntl.h>
/*******************************************************
1. 临界区	
	CRITICAL_SECTION  _critical
	简述:Windows 的临界区只能用于线程，不能用于进程
	相关函数:InitializeCriticalSection、DeleteCriticalSection、EnterCriticalSection、ReleaseCriticalSection

2. 事件Event
	简述:用于线程/进程之间同步，分为:
	a. 命名事件 可用于进程和线程之间进行同步
	b. 匿名事件 仅用于线程之间同步
	相关函数:CreateEvent、OpenEvent、WatiForSingleObject、SerEvent、ResetEvent、WaitForMultiObjects、
3. 互斥量mutex
	简述:可以跨进程同步,也可以用来线程间同步 ，还可以用来保证程序只有一个实例运行（创建命名互斥量）
	相关函数:CreateMutex、OpenMutex、WaitForSingleObject、ReleaseMutex

	互斥量和事件的异同？ 
	参考博客: http://blog.csdn.net/morewindows/article/details/7823572
	a. 互斥量的“遗弃”问题
		占有某种资源的进程意外终止后，其它等待该资源的进程能否感知。
		系统会自动重置互斥量引用计数，等待线程(WaitForSingleObject)返回WAIT_ABANDONED_0
	b. 事件不能处理遗弃问题，可能会导致无限等待
	c. 信号量(windows下特指技术信号量)不能处理“遗弃”问题

	事件(event)与互斥量(mutex)区别

	事件(event) 
	事件是用来同步地位不相等的线程的，事件可以用来使一个线程完成一件事情，然后另外的线程完成剩下的事情。
	事件的使用很灵活，自动 事件的激发态是由人工来控制的，而Mutex在释放（releaseMetux）后就一直处于激发态，
	直到线程WaitForSingleObject。 事件可以用来控制经典的读写模型和生产者和消费者模型。
	相应的方式为，生成者等待消费者的消费，再消费者消费完后通知生产者进行生产。
	互斥量(Mutex) 
	Mutex 是排他的占有资源，一般用于地位相等的现在进行同步，
	每个线程都可以排他的访问一个资源或代码段，不存在哪个线程对资源访问存在优先次序。
	一个线程只能在 Mutex处于激发态的时候访问被保护的资源或代码段，
	线程可以通过WaitForSingelObject来等待Mutex，在访问资源完成之 后，ReleaseMutex释放Mutex，此时Mutex处于激发态。
	Mutex具有成功等待的副作用，在等待到Mutex后，Mutex自动变为未 激发态，直到调用ReleaseMutex使Mutex变为激发态为止。
	自动事件也具有成功等待的副作用。手动事件没有，必须ResetEvent使手动 事件变为未激发态。
	进程和线程也没有成功等待的副作用。当线程或者进程函数返回时，线程内核对象变为激发态，
	但WaitForSingleObject并 没有使线程或者进程的内核对象变为未激发态。

4.	信号量(windows下特指技术信号量)
	


*******************************************************/
#if defined(OS_POSIX)||defined(OS_LINUX)
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
#else
#error "need posix c>=200112L"
#endif
#endif

CXSemBinary::CXSemBinary(SemState initState)
{
#ifdef OS_LINUX
	int ret = sem_init(&m_sem, 0, initState==SemState_Full?1:0);
	if (ret != 0)
	{
		printf("sem_init failed, errno=%d\n", errno);
		_ASSERT(FALSE);
	}
#else
	m_sem = CreateEvent(NULL, FALSE, initState==SemState_Full?TRUE:FALSE, NULL);
	if (m_sem == INVALID_HANDLE_VALUE)
	{
		printf("CreateEvent failed, errno=%d\n", GetLastError());
		_ASSERT(FALSE);
	}
#endif
}

CXSemBinary::~CXSemBinary()
{
#ifdef OS_LINUX
	sem_destroy(&m_sem);
#else
	CloseHandle(m_sem);
#endif
}

BOOL CXSemBinary::Take()
{
#ifdef OS_LINUX
	if (sem_wait(&m_sem) == 0)
		return TRUE;
#else
	if (::WaitForSingleObject(m_sem, INFINITE) == WAIT_OBJECT_0)
	{
		//ResetEvent(m_sem);
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CXSemBinary::WaitForTake(UINT timeOutMs)
{
#ifdef OS_LINUX
	if (timeOutMs == WAIT_FOREVER)
	{
		return sem_wait(&m_sem) == 0;
	}
	else
	{
		struct timespec tp = {0,0};
		tp.tv_sec = (timeOutMs/1000) + time(NULL);
		tp.tv_nsec = (timeOutMs%1000) * 1000000;
		return 0==sem_timedwait(&m_sem, &tp);
	}
#elif defined(OS_WIN)
	if (::WaitForSingleObject(m_sem, timeOutMs) == WAIT_OBJECT_0)
	{
		//ResetEvent(m_sem);
		return TRUE;
	}
#endif
	return FALSE;
}


void CXSemBinary::Give()
{
#ifdef OS_LINUX
	sem_post(&m_sem);
#elif defined(OS_WIN)
	::SetEvent(m_sem);
#endif
}

CXSemMutex::CXSemMutex()
{
#ifdef OS_WIN
	::InitializeCriticalSection(&m_handle);
#elif defined(OS_POSIX)
	pthread_mutex_init(&m_handle, NULL);
#endif
}

CXSemMutex::~CXSemMutex()
{
#ifdef OS_WIN
	::DeleteCriticalSection(&m_handle);
#elif defined(OS_POSIX)
	pthread_mutex_destroy(&m_handle);
#endif
}

BOOL CXSemMutex::Take()
{
#ifdef OS_WIN
	::EnterCriticalSection(&m_handle);
	return TRUE;
#else
	pthread_mutex_lock(&m_handle);
	return TRUE;
#endif
}

BOOL CXSemMutex::WaitForTake(UINT timeOutMs)
{
#ifdef OS_WIN//TODO:::优化
	if (timeOutMs == WAIT_FOREVER)
	{
		::EnterCriticalSection(&m_handle);
		m_bLocked = TRUE;				//TODO:原子操作
	}
	else
	{
		for (UINT i = 0; i < timeOutMs; i++)
		{
			BOOL bLock = TryEnterCriticalSection(&m_handle);
			if (bLock)
			{
				m_bLocked = TRUE;
				break;
			}
			CXThread::Sleep(1);//windows分辨率是10ms
		}
	}
#else
	if (timeOutMs == WAIT_FOREVER)
	{
		::pthread_mutex_lock(&m_handle);
		m_bLocked = TRUE;				//TODO:原子操作
	}
	else
	{
		for (UINT i = 0; i < timeOutMs; i++)
		{
			int ret = pthread_mutex_trylock(&m_handle);
			if (ret == 0)
			{
				m_bLocked = TRUE;
				break;
			}
			CXThread::Sleep(1);//windows分辨率是10ms
		}
	}
#endif
	return m_bLocked;
}

void CXSemMutex::Give()
{
#ifdef OS_WIN
	::LeaveCriticalSection(&m_handle);
	m_bLocked = FALSE;
#else
	pthread_mutex_unlock(&m_handle);
#endif
}

#ifdef OS_WIN
CXSemProcessMutex::CXSemProcessMutex() : m_handle(NULL)
{

}

CXSemProcessMutex::~CXSemProcessMutex()
{
	Close();
}

BOOL CXSemProcessMutex::Open(const TCHAR* pszKey, BOOL bCreateFlag)
{
	if (m_handle != NULL || pszKey == NULL)
	{
		return FALSE;
	}

	HANDLE pHandle = 0;
	if (bCreateFlag)
	{
		pHandle = CreateMutex(NULL, FALSE, pszKey);
	}
	else
	{
		pHandle = OpenMutex(MUTEX_MODIFY_STATE, FALSE, pszKey);
	}
	if (pHandle == NULL)
	{
		return FALSE;
	}

	m_handle = pHandle;
	m_strKey = pszKey;
	return TRUE;
}

//TODO:: 状态没表示完整，异常处理
BOOL CXSemProcessMutex::WaitForTake(UINT timeOutMs)
{
	if (m_handle == NULL)
	{
		return FALSE;
	}
	if (timeOutMs = WAIT_FOREVER)
	{
		return Take();
	}

	UINT ret = WaitForSingleObject(m_handle, timeOutMs);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		return TRUE;

	case WAIT_TIMEOUT:
		return FALSE;

	case WAIT_ABANDONED:
	case WAIT_FAILED:
	default:
		return FALSE;
	}
	return FALSE;
}

BOOL CXSemProcessMutex::Take()
{
	UINT ret = WaitForSingleObject(m_handle, INFINITE);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		return TRUE;

	case WAIT_ABANDONED:
		//如果返回这个值就是出发了mutex的遗弃问题
		return FALSE;

	case WAIT_FAILED:
	case WAIT_TIMEOUT:
	default:
		return FALSE;
	}
	return FALSE;
}

void CXSemProcessMutex::Give()
{
	_ASSERT(m_handle);
	ReleaseMutex(m_handle);
}

void CXSemProcessMutex::Close()
{
	if (m_handle!=NULL)
	{
		CloseHandle(m_handle);
		m_strKey = _T("");
		m_handle = NULL;
	}
}
#else
CXSemProcessMutex::CXSemProcessMutex() : m_handle(0), m_bCreate(FALSE)
{

}

CXSemProcessMutex::~CXSemProcessMutex()
{
	Close();
}

BOOL CXSemProcessMutex::Open(const TCHAR* pszKey, BOOL bCreateFlag)
{
	if (m_handle != NULL)
	{
		return FALSE;
	}

	int oflag = bCreateFlag? O_CREAT : 0;
	sem_t* pHandle = sem_open(pszKey, oflag, 0644, 1);
	if (pHandle == NULL)
	{
		return FALSE;
	}

	m_handle = pHandle;
	m_bCreate = bCreateFlag;
	return TRUE;
}

BOOL CXSemProcessMutex::WaitForTake(UINT timeOutMs)
{
	_ASSERT(m_handle);

	if (timeOutMs==WAIT_FOREVER)
	{
		return Take();
	}
	else
	{
		struct timespec tp = {0,0};
		tp.tv_sec = timeOutMs/1000;
		tp.tv_nsec = timeOutMs % 1000000;

		return (sem_timedwait(m_handle, &tp) == 0);
	}
	return FALSE;
}

BOOL CXSemProcessMutex::Take()
{
	_ASSERT(m_handle);
	return 0==sem_wait(m_handle);
}

void CXSemProcessMutex::Give()
{
	_ASSERT(m_handle);
	sem_post(m_handle);
}

void CXSemProcessMutex::Close()
{
	if (m_handle)
	{
		sem_close(m_handle);
		m_handle = NULL;
		if (m_bCreate)
		{
			sem_unlink(m_strKey.c_str());
			m_bCreate = FALSE;
		}
		m_strKey = _T("");
	}
}
#endif

CXSemCount::CXSemCount(UINT nCount, SemState initState)
{
#ifdef OS_LINUX
	UNUSED(initState);//Linux Not support
	int ret = sem_init(&m_sem, 0, nCount);
	if (ret != 0)
	{
		printf("sem_init failed, errno=%d\n", errno);
		_ASSERT(FALSE);
	}
#else
	m_sem = CreateSemaphore(NULL, initState==SemState_Full?nCount:0, nCount, NULL);
	if (m_sem == NULL)
	{
		printf("CreateSemaphore failed, errno=%d\n", GetLastError());
		_ASSERT(FALSE);
	}
#endif
}

BOOL CXSemCount::Take()
{
#ifdef OS_WIN
	return WaitForTake(WAIT_FOREVER);
#else
	if (sem_wait(&m_sem) == 0)
		return TRUE;
#endif
	return FALSE;
}

BOOL CXSemCount::WaitForTake(UINT timeOutMs)
{
#ifdef OS_WIN
	_ASSERT(m_sem != NULL);
	DWORD ret = ::WaitForSingleObject(m_sem, timeOutMs);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		return TRUE;

	case WAIT_TIMEOUT:
		return FALSE;

	case WAIT_ABANDONED:
	case WAIT_FAILED:
	default:
		_ASSERT(FALSE);
		break;
	}
#else
	if (timeOutMs==WAIT_FOREVER)
	{
		return (sem_wait(&m_sem) == 0);
	}
	else
	{
		struct timespec abs_timeout = {0,0};
		abs_timeout.tv_sec = (timeOutMs / 1000) + time(NULL);//ABSti
		abs_timeout.tv_nsec = timeOutMs % 1000000;

		return (sem_timedwait(&m_sem, &abs_timeout) == 0);
	}
#endif
	return FALSE;
}

void CXSemCount::Give()
{
#ifdef OS_WIN
	::ReleaseSemaphore(m_sem, 1, NULL);
#else
	sem_post(&m_sem);
#endif
}

CXSemCount::~CXSemCount()
{
#ifdef OS_WIN
	CloseHandle(m_sem);
#else
	sem_destroy(&m_sem);
#endif
}

//////////////////////////////////////////////////////////////////////////
//ReadWriteLocker
//////////////////////////////////////////////////////////////////////////
#ifdef OS_WIN
class CXSemReadWritePrivate
{
public:
	CXSemReadWritePrivate() : m_nReader(0), m_hWMutex(0), m_hREvent(0)
	{
		if (!(m_hREvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			_ASSERT(FALSE);
		}

		if (! (m_hWMutex = CreateMutex(NULL, FALSE, NULL))) 
		{
			CloseHandle(m_hREvent);
			m_hREvent = NULL;
			_ASSERT(FALSE);
		}
	}
	~CXSemReadWritePrivate()
	{
		if (m_hWMutex)
		{
			CloseHandle(m_hWMutex);
			m_hWMutex = NULL;
		}

		if (m_hREvent)
		{
			CloseHandle(m_hREvent);
			m_hREvent = NULL;
		}		
	}

	BOOL WaitForTakeRead(UINT timeOutMs)
	{
		UINT retVal = WaitForSingleObject(m_hWMutex, timeOutMs);

		switch (retVal)
		{
		case WAIT_FAILED:
		case WAIT_TIMEOUT:
		}

		/* We've successfully acquired the writer mutex, we can't be locked
		* for write, so it's OK to add the reader lock.  The writer mutex
		* doubles as race condition protection for the readers counter.   
		*/
		InterlockedIncrement(&m_nReader);

		if (!ResetEvent(m_hREvent))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		if (!ReleaseMutex(m_hWMutex))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	BOOL WaitForTakeWrite(UINT timeOutMs)
	{
		UINT retVal = WaitForSingleObject(m_hWMutex, timeOutMs);

		switch (retVal)
		{
		case WAIT_FAILED:
		case WAIT_TIMEOUT:
		}

		/* We've successfully acquired the writer mutex, we can't be locked
		* for write, so it's OK to add the reader lock.  The writer mutex
		* doubles as race condition protection for the readers counter.   
		*/
		InterlockedIncrement(&m_nReader);

		if (!ResetEvent(m_hREvent))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		if (!ReleaseMutex(m_hWMutex))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	void TakeRead()
	{
		WaitForTakeRead(WAIT_FOREVER);
	}
	void TakeWrite()
	{
		WaitForTakeWrite(WAIT_FOREVER);
	}
	void Give()
	{
		UINT nVal = 0;

		/* First, guess that we're unlocking a writer */
		if (!ReleaseMutex(m_hWMutex))
		{
			nVal = GetLastError();
		}

		if (nVal == ERROR_NOT_OWNER) 
		{
			/* Nope, we must have a read lock */
			if (m_nReader>0 && !InterlockedDecrement(&m_nReader) &&
				! SetEvent(m_hREvent)) 
			{
//				ASSERT(FALSE);
			}
			else 
			{
				nVal = 0;
			}
		}
	}
private:
	ULONG m_nReader;
	HANDLE m_hWMutex;
	HANDLE m_hREvent;
};
#elif defined(OS_LINUX)
public:
	CXSemReadWritePrivate() : m_nReader(0), m_hWMutex(0), m_hREvent(0)
	{
		int stat = pthread_rwlock_init(&m_rwLock, NULL);
		_ASSERT(stat > 0);
	}
	~CXSemReadWritePrivate()
	{
		if (m_hWMutex)
		{
			CloseHandle(m_hWMutex);
			m_hWMutex = NULL;
		}

		if (m_hREvent)
		{
			CloseHandle(m_hREvent);
			m_hREvent = NULL;
		}		
	}

	BOOL WaitForTakeRead(UINT timeOutMs)
	{
		UINT retVal = WaitForSingleObject(m_hWMutex, timeOutMs);

		switch (retVal)
		{
		case WAIT_FAILED:
		case WAIT_TIMEOUT:
		}

		/* We've successfully acquired the writer mutex, we can't be locked
		* for write, so it's OK to add the reader lock.  The writer mutex
		* doubles as race condition protection for the readers counter.   
		*/
		InterlockedIncrement(&m_nReader);

		if (!ResetEvent(m_hREvent))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		if (!ReleaseMutex(m_hWMutex))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	BOOL WaitForTakeWrite(UINT timeOutMs)
	{
		UINT retVal = WaitForSingleObject(m_hWMutex, timeOutMs);

		switch (retVal)
		{
		case WAIT_FAILED:
		case WAIT_TIMEOUT:
		}

		/* We've successfully acquired the writer mutex, we can't be locked
		* for write, so it's OK to add the reader lock.  The writer mutex
		* doubles as race condition protection for the readers counter.   
		*/
		InterlockedIncrement(&m_nReader);

		if (!ResetEvent(m_hREvent))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		if (!ReleaseMutex(m_hWMutex))
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	void TakeRead()
	{
		WaitForTakeRead(WAIT_FOREVER);
	}
	void TakeWrite()
	{
		WaitForTakeWrite(WAIT_FOREVER);
	}
	void Give()
	{
		UINT nVal = 0;

		/* First, guess that we're unlocking a writer */
		if (!ReleaseMutex(m_hWMutex))
		{
			nVal = GetLastError();
		}

		if (nVal == ERROR_NOT_OWNER) 
		{
			/* Nope, we must have a read lock */
			if (m_nReader>0 && !InterlockedDecrement(&m_nReader) &&
				! SetEvent(m_hREvent)) 
			{
//				ASSERT(FALSE);
			}
			else 
			{
				nVal = 0;
			}
		}
	}
private:
	apr_thread_rwlock_t* m_rwLock;

}

apr_pool_cleanup_register(new_rwlock->pool,
	(void *)new_rwlock, thread_rwlock_cleanup,
	apr_pool_cleanup_null);

*rwlock = new_rwlock;
#endif

CXSemReadWrite::CXSemReadWrite()
{
	XCLASSPRIVATEVAL_CREATE(CXSemReadWrite);
}

CXSemReadWrite::~CXSemReadWrite()
{
	XCLASSPRIVATE_DELETE();
}

BOOL CXSemReadWrite::WaitForTakeRead(UINT timeOutMs)
{
	return m_pD->WaitForTakeRead(timeOutMs);
}

BOOL CXSemReadWrite::WaitForTakeWrite(UINT timeOutMs)
{
	return m_pD->WaitForTakeWrite(timeOutMs);
}

void CXSemReadWrite::TakeRead()
{
	m_pD->TakeRead();
}

void CXSemReadWrite::TakeWrite()
{
	m_pD->TakeWrite();
}

void CXSemReadWrite::Give()
{
	m_pD->Give();
}
