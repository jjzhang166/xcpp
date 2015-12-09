#include "XAfx.h"
#include "XSemaphore.h"
#include "XUtils.h"
#include "XThread.h"
#include <fcntl.h>
/*******************************************************
1. �ٽ���	
	CRITICAL_SECTION  _critical
	����:Windows ���ٽ���ֻ�������̣߳��������ڽ���
	��غ���:InitializeCriticalSection��DeleteCriticalSection��EnterCriticalSection��ReleaseCriticalSection

2. �¼�Event
	����:�����߳�/����֮��ͬ������Ϊ:
	a. �����¼� �����ڽ��̺��߳�֮�����ͬ��
	b. �����¼� �������߳�֮��ͬ��
	��غ���:CreateEvent��OpenEvent��WatiForSingleObject��SerEvent��ResetEvent��WaitForMultiObjects��
3. ������mutex
	����:���Կ����ͬ��,Ҳ���������̼߳�ͬ�� ��������������֤����ֻ��һ��ʵ�����У�����������������
	��غ���:CreateMutex��OpenMutex��WaitForSingleObject��ReleaseMutex

	���������¼�����ͬ�� 
	�ο�����: http://blog.csdn.net/morewindows/article/details/7823572
	a. �������ġ�����������
		ռ��ĳ����Դ�Ľ���������ֹ�������ȴ�����Դ�Ľ����ܷ��֪��
		ϵͳ���Զ����û��������ü������ȴ��߳�(WaitForSingleObject)����WAIT_ABANDONED_0
	b. �¼����ܴ����������⣬���ܻᵼ�����޵ȴ�
	c. �ź���(windows����ָ�����ź���)���ܴ�������������

	�¼�(event)�뻥����(mutex)����

	�¼�(event) 
	�¼�������ͬ����λ����ȵ��̵߳ģ��¼���������ʹһ���߳����һ�����飬Ȼ��������߳����ʣ�µ����顣
	�¼���ʹ�ú����Զ� �¼��ļ���̬�����˹������Ƶģ���Mutex���ͷţ�releaseMetux�����һֱ���ڼ���̬��
	ֱ���߳�WaitForSingleObject�� �¼������������ƾ���Ķ�дģ�ͺ������ߺ�������ģ�͡�
	��Ӧ�ķ�ʽΪ�������ߵȴ������ߵ����ѣ����������������֪ͨ�����߽���������
	������(Mutex) 
	Mutex ��������ռ����Դ��һ�����ڵ�λ��ȵ����ڽ���ͬ����
	ÿ���̶߳����������ķ���һ����Դ�����Σ��������ĸ��̶߳���Դ���ʴ������ȴ���
	һ���߳�ֻ���� Mutex���ڼ���̬��ʱ����ʱ���������Դ�����Σ�
	�߳̿���ͨ��WaitForSingelObject���ȴ�Mutex���ڷ�����Դ���֮ ��ReleaseMutex�ͷ�Mutex����ʱMutex���ڼ���̬��
	Mutex���гɹ��ȴ��ĸ����ã��ڵȴ���Mutex��Mutex�Զ���Ϊδ ����̬��ֱ������ReleaseMutexʹMutex��Ϊ����̬Ϊֹ��
	�Զ��¼�Ҳ���гɹ��ȴ��ĸ����á��ֶ��¼�û�У�����ResetEventʹ�ֶ� �¼���Ϊδ����̬��
	���̺��߳�Ҳû�гɹ��ȴ��ĸ����á����̻߳��߽��̺�������ʱ���߳��ں˶����Ϊ����̬��
	��WaitForSingleObject�� û��ʹ�̻߳��߽��̵��ں˶����Ϊδ����̬��

4.	�ź���(windows����ָ�����ź���)
	


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
#ifdef OS_WIN//TODO:::�Ż�
	if (timeOutMs == WAIT_FOREVER)
	{
		::EnterCriticalSection(&m_handle);
		m_bLocked = TRUE;				//TODO:ԭ�Ӳ���
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
			CXThread::Sleep(1);//windows�ֱ�����10ms
		}
	}
#else
	if (timeOutMs == WAIT_FOREVER)
	{
		::pthread_mutex_lock(&m_handle);
		m_bLocked = TRUE;				//TODO:ԭ�Ӳ���
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
			CXThread::Sleep(1);//windows�ֱ�����10ms
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

//TODO:: ״̬û��ʾ�������쳣����
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
		//����������ֵ���ǳ�����mutex����������
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
