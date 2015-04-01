#include "stdafx.h"
#include "XApplication.h"

#include "XUdpSocket.h"
#include "XProcess.h"
#include "XLog.h"

#ifdef OS_WIN

#else
#include <sys/types.h>
//#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#define SINGLETON_PROCESS_USE_SOCKET		1
#define SINGLETON_PROCESS_USE_LOCKFILE		2
#define SIGNLETON_PROCESS_USE_SHAREDMEMORY	3
#define SIGNLETON_PROCESS_IMPLEMENT SINGLETON_PROCESS_USE_LOCKFILE

class CSingletonAppliaionObj
{
public:
	CSingletonAppliaionObj(const std::string& appName);
	~CSingletonAppliaionObj();

	BOOL Veriyfy() const { return m_bOk; }

private:
#ifdef OS_WIN
	HANDLE m_handle;
#else
	int m_handle;
#endif
	BOOL m_bOk;
};

#ifdef OS_WIN
#define LOCKFILE_PATH "Global\\"
CSingletonAppliaionObj::CSingletonAppliaionObj(const std::string& appName) :
	m_handle(NULL), m_bOk(FALSE)
{
	std::string instanceName = std::string(LOCKFILE_PATH) + appName;
	m_handle = CreateMutexA(NULL, FALSE, instanceName.c_str());
	m_bOk = !(GetLastError() == ERROR_ALREADY_EXISTS);
}

CSingletonAppliaionObj::~CSingletonAppliaionObj()
{
	if (m_handle != NULL)
	{
		m_handle = NULL;
		::CloseHandle(m_handle);
	}
}
#else
//#define LOCKFILE_PATH  "/var/run/"
#define LOCKFILE_PATH "/tmp/"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

static BOOL sTrylockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return (fcntl(fd, F_SETLK, &fl))==0;
}

CSingletonAppliaionObj::CSingletonAppliaionObj(const std::string& appName) :
	m_handle(0), m_bOk(FALSE)
{
	std::string instanceName = LOCKFILE_PATH + appName + ".pid";
	m_handle = open(instanceName.c_str(), O_RDWR | O_CREAT, LOCKMODE);
	if (m_handle < 0)
	{
		//m_bOk = FALSE;
		XLOG_PRINT(LOG_ERR, "<%s>Can't Open File %s, errno is %d", __FUNCTION__, instanceName.c_str(), errno);
		return;
	}

	if (!sTrylockfile(m_handle) && (errno == EACCES || errno == EAGAIN))
	{
		//m_bOk = FALSE; 
		XLOG_PRINT(LOG_ERR, "<%s>file: %s already locked", __FUNCTION__, instanceName.c_str());
		return ;
	}
	char szBuffer[32] = {0};
	int retVal = ftruncate(m_handle, 0);
	UNUSED(retVal);

	//¼æÈÝ64Î»ÏµÍ³
	_SPRINTF(szBuffer, sizeof(szBuffer)-1, "%lld", (long long int)getpid());
	retVal = write(m_handle, szBuffer, strlen(szBuffer) + 1);
	m_bOk = TRUE;
}

CSingletonAppliaionObj::~CSingletonAppliaionObj()
{
	if (m_handle > 0)
	{
		close(m_handle);
		m_handle = 0;
	}
}
#endif

CSingletonAppliaion::CSingletonAppliaion(const char* pszAppKey)
	: m_strKey(pszAppKey)
{
	if (NULL == pszAppKey)
	{
		m_strKey = CXProcess::GetCurrentExcuteNameA();
	}
	m_Obj = new (std::nothrow) CSingletonAppliaionObj(m_strKey);
	_ASSERT(m_Obj != NULL);
}

BOOL CSingletonAppliaion::Verify() const
{
	_ASSERT(m_Obj);
	return m_Obj->Veriyfy();
}

CSingletonAppliaion::~CSingletonAppliaion()
{
	if (NULL != m_Obj)
	{
		delete m_Obj;
	}
}
