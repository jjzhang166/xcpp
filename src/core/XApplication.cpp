#include "XAfx.h"
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

class CSingletonAppPrivate
{
public:
	CSingletonAppPrivate() : m_handle(0), m_bOk(FALSE)
	{	}
	~CSingletonAppPrivate()
	{
		Release();
	}

	BOOL CreateByKey(const TCHAR* pszKey);

	void Release();

	BOOL Verify() const { return m_bOk; }

	XSTLString GetKey() const { return m_szKey;}

private:
	XSTLString m_szKey;
#ifdef OS_WIN
	HANDLE m_handle;
#else
	int m_handle;
#endif
	BOOL m_bOk;
};

#ifdef OS_WIN
#define LOCKFILE_PATH "Global\\"
BOOL CSingletonAppPrivate::CreateByKey(const TCHAR* pszKey)
{
	XSTLString instanceName = XSTLString(LOCKFILE_PATH) + XSTLString(pszKey);
	m_handle = CreateMutex(NULL, FALSE, instanceName.c_str());
	m_bOk = !(GetLastError() == ERROR_ALREADY_EXISTS);
	return m_handle!=NULL;
}

void CSingletonAppPrivate::Release()
{
	if (m_handle)
	{
		::CloseHandle(m_handle);
		m_handle = NULL;
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

BOOL CSingletonAppPrivate::CreateByKey(const TCHAR* pszKey)
{
	XSTLString instanceName = LOCKFILE_PATH + pszKey + ".pid";
	m_handle = open(instanceName.c_str(), O_RDWR | O_CREAT, LOCKMODE);
	if (m_handle < 0)
	{
		_ASSERT(FALSE);
		XLOG_PRINT(LOG_ERR, "<%s>Can't Open File %s, errno is %d", __FUNCTION__, instanceName.c_str(), errno);
		return FALSE;
	}

	if (!sTrylockfile(m_handle) && (errno == EACCES || errno == EAGAIN))
	{
		//m_bOk = FALSE; 
		XLOG_PRINT(LOG_ERR, "<%s>file: %s already locked", __FUNCTION__, instanceName.c_str());
		return TRUE;
	}
	TCHAR szBuffer[32] = {0};
	int retVal = ftruncate(m_handle, 0);
	UNUSED(retVal);

	//¼æÈÝ64Î»ÏµÍ³
	_SPRINTFEX(szBuffer, sizeof(szBuffer), "%lld", (long long int)getpid());
	retVal = write(m_handle, szBuffer, strlen(szBuffer) + 1);
	m_bOk = TRUE;
}

void CSingletonAppPrivate::Release()
{
	if (m_handle)
	{
		close(m_handle);
		m_handle = NULL;
	}
}
#endif

CSingletonApp::CSingletonApp(const TCHAR* pszAppKey)
{
	_ASSERT(pszAppKey);
	XCLASSPRIVATEVAL_CREATE(CSingletonApp);
	BOOL bCreateOK = m_pD->CreateByKey(pszAppKey);
	UNUSED(bCreateOK);
}

BOOL CSingletonApp::Verify() const
{
	_ASSERT(m_pD);
	return m_pD->Verify();
}

CSingletonApp::~CSingletonApp()
{
	XCLASSPRIVATEVAL_DELETE();
}
