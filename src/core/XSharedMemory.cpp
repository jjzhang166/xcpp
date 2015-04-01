#include "stdafx.h"

#include "XSharedMemory.h"
#include "XSemaphore.h"

//TODO::
class CXShareMemoryPrivate
{
public:
	CXShareMemoryPrivate(CXShareMemory* p) : m_pSM(p), m_pMem(NULL), m_pHandle(0)
	{	} 
	~CXShareMemoryPrivate();

	BOOL Create(size_t sizeofMem, const TCHAR* pszKey);
	BOOL Attach(CXShareMemory::AccessMode mode);
	BOOL Detach();

	void Close();
	void* GetPtr() {return m_pMem;}
	const void* GetConstPtr(){return m_pMem;}

	BOOL Lock();
	BOOL UnLock();

	inline BOOL IsAttached() const { return m_pSM != NULL; }
	static XSTLString BuildSharedMemoryMutexString(const XSTLString& keybase);
#ifndef OS_WIN
	static int CreateUnixSharedFile(const XSTLString& keyFileName);
	static key_t BuildUnixSharedmemoryKey(const XSTLString& keyFileName);
#endif

private:
	CXSemProcessMutex m_mutex;
	CXShareMemory* m_pSM;
	void* m_pMem;
#ifdef OS_WIN
	HANDLE m_pHandle;
#else
	XSTLString m_strFileName;
	int m_pHandle;
	BOOL m_bCreateFile;
#endif
};

BOOL CXShareMemoryPrivate::Lock()
{
	return m_mutex.Take();
}

BOOL CXShareMemoryPrivate::UnLock()
{
	m_mutex.Give();
	return FALSE;
}

XSTLString CXShareMemoryPrivate::BuildSharedMemoryMutexString(const XSTLString& keybase)
{
	XSTLString s = keybase;
	s += _T("_$XShareMemory");
	return s;
}

CXShareMemoryPrivate::~CXShareMemoryPrivate()
{
	Close();
}

#ifdef OS_WIN
BOOL CXShareMemoryPrivate::Create(size_t sizeofMem, const TCHAR* pszKey)
{
	// Create the file mapping.
	HANDLE pHandle = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, sizeofMem, pszKey);

	// hand is valid when it already exists unlike unix so explicitly check
	if (GetLastError() == ERROR_ALREADY_EXISTS || pHandle==NULL)
		return FALSE;

	XSTLString key = pszKey;
	XSTLString mutexKey = CXShareMemoryPrivate::BuildSharedMemoryMutexString(key);
	if (!m_mutex.Open(mutexKey.c_str(), TRUE))
	{
		Close();
		return FALSE;
	}

	m_pHandle = pHandle;
	return TRUE;
}

void CXShareMemoryPrivate::Close()
{
	if (m_pHandle != NULL)
	{
		CloseHandle(m_pHandle);
		m_pHandle = NULL;
	}
}

BOOL CXShareMemoryPrivate::Attach(CXShareMemory::AccessMode mode)
{
	if (m_pMem != NULL)
	{
		return FALSE;
	}

	int permissions = (mode == CXShareMemory::AccessRO ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS);
	void* pMem = (void *)MapViewOfFile(m_pHandle, permissions, 0, 0, 0);
	if (NULL == pMem) 
	{
		//setErrorString(QLatin1String("QSharedMemory::attach"));
		//cleanHandle();
		return FALSE;
	}

	// Grab the size of the memory we have been given (a multiple of 4K on windows)
	MEMORY_BASIC_INFORMATION meminfo = {0};
	if (!VirtualQuery(pMem, &meminfo, sizeof(meminfo))) 
	{
		// Windows doesn't set an error code on this one,
		// it should only be a kernel memory error.
		return FALSE;
	}
	//TODO:::
	size_t size = meminfo.RegionSize;
	UNUSED(size);
	return TRUE;
}

BOOL CXShareMemoryPrivate::Detach()
{
	//TODO::
	if (NULL == m_pMem)
	{
		return FALSE;
	}

	UnmapViewOfFile(m_pMem);
	m_pMem = NULL;
	Close();
	return TRUE;
}
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/shm.h>
int CXShareMemoryPrivate::CreateUnixSharedFile(const XSTLString& sharedmemFileName)
{
	/*
	key_t ftok( const char * fname, int id )
	函数的参数fname必须要输入一个存在的文件
	*/
	XSTLString fileName("./");
	fileName  += sharedmemFileName;
	int fd = open(fileName.c_str(), O_EXCL | O_CREAT | O_RDWR, 0640);
	if (fd == -1)
	{
		return errno==EEXIST?0:-1;
	}
	else
	{
		close(fd);
		fd = 0;
	}
	return 1;
}

key_t CXShareMemoryPrivate::BuildUnixSharedmemoryKey(const XSTLString& sharedmemFileName)
{
	key_t shmKey = ftok(sharedmemFileName.c_str(), 'X');
	return shmKey;
}

/*
#warnning!!!
本函数在调用过工程中间，其他线程切换了工作目录会导致失败!!!
*/
BOOL CXShareMemoryPrivate::Create(size_t sizeofMem, const TCHAR* pszKey)
{
	XSTLString shareFile = CXShareMemoryPrivate::BuildSharedMemoryMutexString(pszKey);
	int createFileRet = CXShareMemoryPrivate::CreateUnixSharedFile(shareFile);
	if (createFileRet == -1)
	{
		return FALSE;
	}

	m_bCreateFile = (createFileRet==1);

	key_t shmKey = CXShareMemoryPrivate::BuildUnixSharedmemoryKey(shareFile);
	// Create the file mapping.
	if (-1 == shmget(shmKey, sizeofMem, 0666 | IPC_CREAT | IPC_EXCL)) 
	{
		_tremove(shareFile.c_str());
		return FALSE;
	}

	XSTLString key = pszKey;
	XSTLString mutexKey = CXShareMemoryPrivate::BuildSharedMemoryMutexString(key);
	if (!m_mutex.Open(mutexKey.c_str(), TRUE))
	{
		Close();
		return FALSE;
	}

	m_pHandle = shmKey;
	return TRUE;
}

void CXShareMemoryPrivate::Close()
{
	if (m_pHandle != 0)
	{
		Detach();

		if (m_bCreateFile)
		{
			_tremove(m_strFileName.c_str());
			m_bCreateFile = FALSE;

			if (shmctl(m_pHandle, IPC_RMID, NULL) == -1) 
			{
				_ASSERT(FALSE);
				//TRACE("");
				//TODO::
			}
		}
		m_pHandle = 0;
	}
}

BOOL CXShareMemoryPrivate::Attach(CXShareMemory::AccessMode mode)
{
	if (m_pMem != NULL)
	{
		return FALSE;
	}

	int permissions = (mode == CXShareMemory::AccessRO ? SHM_RDONLY : 0);
	void* pMem = shmat(m_pHandle, (void*)0, permissions); 
	if (NULL == pMem)
	{
		//setErrorString(QLatin1String("QSharedMemory::attach"));
		//cleanHandle();
		return FALSE;
	}
	m_pMem = pMem;
	return TRUE;
}

BOOL CXShareMemoryPrivate::Detach()
{
	//TODO::
	if (NULL == m_pMem)
	{
		return FALSE;
	}

	if (shmdt(m_pMem) == -1)  
	{  
		//TODO:::
		fprintf(stderr, "shmdt failed\n");  
		return FALSE;
	}  

	m_pMem = NULL;
	Close();
	return TRUE;
}
#endif


CXShareMemory::CXShareMemory(const TCHAR* pszKey) : m_mode(AccessRW), m_size(0), m_strKey(pszKey)
{
	XCLASSPRIVATE_CREATE_INIT(CXShareMemory, m_pD);
	UNUSED(m_pD);
}

CXShareMemory::~CXShareMemory()
{

}

BOOL CXShareMemory::Create(size_t sizeofMem, AccessMode mode)
{
	if (sizeofMem <= 0)
	{
		return FALSE;
	}
	//PrivateCreate

	return Attach(mode);
}

BOOL CXShareMemory::Attach(AccessMode mode)
{
	if (IsAttached())
	{
		return FALSE;
	}

	return m_pD->Attach(mode);
}

BOOL CXShareMemory::SetKey(const TCHAR* pszKey)
{
	_ASSERT(pszKey);
	if (IsAttached())
	{
		return FALSE;
	}
	m_strKey = pszKey;
	return TRUE;
}

BOOL CXShareMemory::Detach()
{
	return m_pD->UnLock();
}

BOOL CXShareMemory::IsAttached()
{
	return m_pD->IsAttached();
}

BOOL CXShareMemory::Lock()
{
	return m_pD->Lock();
}

BOOL CXShareMemory::UnLock()
{
	return m_pD->UnLock();
}
