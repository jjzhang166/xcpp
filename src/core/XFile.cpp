#include "stdafx.h"
#include "XFile.h"
#ifdef OS_WIN
#include <direct.h>
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include "XEString.h"
#endif

TCHAR* CXFilePath::Unix2Dos(TCHAR* pszPath)
{
	_ASSERT(pszPath);
	TCHAR* pIter = pszPath;
	for (; *pIter!=EOS; pIter++)
	{
		if (*pIter == _T('/'))
		{
			*pIter = _T('\\');
		}
	}

	return pszPath;
}

XSTLString CXFilePath::Unix2Dos(const XSTLString& unixPath)
{
	char* pszNewString = _strdup(unixPath.c_str());
	_ASSERT(pszNewString);
	if (NULL == pszNewString)
	{
		return _T("");
	}
	CXFilePath::Unix2Dos(pszNewString);
	XSTLString dosString(pszNewString);
	free(pszNewString);
	return dosString;
}

XSTLString CXFilePath::Dos2Unix(const XSTLString& dosPath)
{
	char* pszNewString = _strdup(dosPath.c_str());
	_ASSERT(pszNewString);
	if (NULL == pszNewString)
	{
		return _T("");
	}
	CXFilePath::Dos2Unix(pszNewString);
	XSTLString dosString(pszNewString);
	free(pszNewString);
	return dosString;
}

BOOL CXFilePath::IsFilePathExist(const TCHAR* pszPathFile)
{
	 _ASSERT(pszPathFile);
	 //win7 通过测试
	 //centos 6.2 经过测试无论任何权限，文件或者目录只要存在必定返回0

	 //TODO noneUnicode;
	 return _taccess(pszPathFile, 0) == 0;
}

TCHAR* CXFilePath::Dos2Unix(TCHAR* pszPath)
{
	_ASSERT(pszPath);
	TCHAR* pIter = pszPath;
	for (; *pIter != EOS; pIter++)
	{
		if (*pIter == _T('\\'))
		{
			*pIter = _T('/');
		}
	}

	return pszPath;
}

TCHAR* CXFilePath::NormalizeFileName(TCHAR* pszFileName)
{
	TCHAR* pItem = pszFileName;
	for (;; pItem++)
	{
		switch (*pItem)
		{
		case EOS:
			return pszFileName;

		case _T('\\'):
		case _T('/'):
		case _T(':'):
		case _T('*'):
		case _T('?'):
		case _T('<'):
		case _T('>'):
		case _T('|'):
			*pItem = _T('#');
			break;

		default:
			break;
		}
	}
	return pszFileName;
}

BOOL CXFilePath::IsDirExist(const TCHAR* pszDir)
{
#ifdef OS_WIN
	DWORD dwAttr = GetFileAttributes(pszDir);
	if (0xFFFFFFFF == dwAttr)
	{
		return FALSE;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		return TRUE;
	}
#else
	DIR* pDir = opendir(pszDir);
	if (pDir==NULL)
	{
		return FALSE;
	}
	else
	{
		closedir(pDir);
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CXFilePath::IsPathDir(const TCHAR* pszPath)
{
#ifdef OS_WIN
	DWORD dwAttr = GetFileAttributes(pszPath);
	if (0xFFFFFFFF == dwAttr)
	{
		return FALSE;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		return TRUE;
	}
#else
	struct stat st = { 0 };
	stat(pszPath, &st);
	if (S_ISDIR(st.st_mode))
	{
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CXFilePath::IsPathFile(const TCHAR* pszPath)
{
#ifdef OS_WIN
	DWORD dwAttr = GetFileAttributes(pszPath);
	if (0xFFFFFFFF == dwAttr)
	{
		return FALSE;
	}
	if (0 == (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		return TRUE;
	}
#else
	struct stat st = { 0 };
	stat(pszPath, &st);
	if (S_ISREG(st.st_mode))
	{
		return TRUE;
	}
#endif
	return FALSE;
}

TCHAR* CXFilePath::GetFileDir(TCHAR* pszPath)
{
	_ASSERT(pszPath);

#ifdef OS_WIN
	DWORD dwAttr = GetFileAttributes(pszPath);
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		return pszPath;
	}
#else
	struct stat st = { 0 };
	stat(pszPath, &st);
	if (S_ISDIR(st.st_mode))
	{
		return pszPath;
	}
#endif
	int len = lstrlen(pszPath);
	/*
		dir_1/dir_11/dir_111/file_1
		dir_1/dir_11/dir_22/d
		0-->>>>len
	
	*/
	len--;
	while (len > 0)
	{
		if (pszPath[len - 1] == _T('\\') || pszPath[len - 1] == _T('/'))
		{
			pszPath[len] = EOS;
		}
	}
	return pszPath;
}

CXFile::CXFile() : m_fp(NULL)
{
}

CXFile::~CXFile()
{
	Close();
}

BOOL CXFile::OpenWithCreateDir(const TCHAR* pszFileName, const TCHAR* pszFlags)
{
	TCHAR szPath[_MAX_PATH] = { 0 };
	XStrCpyN(szPath, pszFileName, sizeof(szPath));
	X_FILEPATH_GETDIR(szPath);
	if (!X_FILEPATH_ISDIREXIST(szPath))
	{
		_MKDIR(szPath);
	}

	return Open(pszFileName, pszFlags);
}

ssize_t CXFile::OutString(const char* pFmt, ...)
{
	_ASSERT(m_fp);
	va_list valist;
	va_start(valist, pFmt);
	//TODO:: level
	int count = _VFPRINTF(m_fp, pFmt, valist);
	va_end(valist);
	return count;
}

size_t CXFile::GetSize() const
{
/*
#warning
!!!对于大于4G的文件这么操作可能有问题
*/
	_ASSERT(m_fp);
	long pos = ftell(m_fp);
	fseek(m_fp, 0, SEEK_END);
	size_t size = ftell(m_fp);
	fseek(m_fp, pos, SEEK_SET);

	return size;
}

size_t CXFile::GetOffset() const
{
	_ASSERT(m_fp);
	return ftell(m_fp);
}

BOOL CXFile::Open(const TCHAR* pszFileName, const TCHAR* pszFlags, int mode)
{
	_ASSERT(m_fp == NULL && pszFileName && pszFlags);

#ifdef OS_WIN
	//TODO:::: wide string

	//!!!windows fopen_s(&fp, "newfile.txt", "rw, ccs= encoding ");
	FILE* fp = _tfsopen(pszFileName, pszFlags, mode);
	if (fp == NULL)
	{
		return FALSE;
	}
#else
	UNUSED(mode);
	FILE* fp = fopen(pszFileName, pszFlags);
	if (fp == NULL)
	{
		return FALSE;
	}
#endif
	m_fp = fp;
	return TRUE;
}

BOOL CXFile::Attach(FILE* fp)
{
	if (m_fp != NULL || fp==NULL)
	{
		return FALSE;
	}

	m_fp = fp;
	return TRUE;
}

void CXFile::Detach()
{
	m_fp = NULL;
}

void CXFile::Flush()
{
	_ASSERT(m_fp);
	fflush(m_fp);
}

void CXFile::Close()
{
	if (m_fp != NULL)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
}

ssize_t CXFile::Write(const void* pBuffer, size_t bufLen)
{
	return (ssize_t)fwrite(pBuffer, 1, bufLen, m_fp);
}

ssize_t CXFile::Read(void* pBuffer, size_t bufLen)
{
	return (ssize_t)fread(pBuffer, 1, bufLen, m_fp);
}

CXFileFd::CXFileFd() : m_fd(-1)
{
}


CXFileFd::~CXFileFd()
{
	Close();
}

BOOL CXFileFd::OpenWithCreateDir(const TCHAR* pszFileName, int flags, int mode)
{
	TCHAR szPath[_MAX_PATH] = { 0 };
	XStrCpyN(szPath, pszFileName, sizeof(szPath));
	X_FILEPATH_GETDIR(szPath);
	if (!X_FILEPATH_ISDIREXIST(szPath))
	{
		_MKDIR(szPath);
	}

	return Open(pszFileName, flags, mode);
}

ssize_t CXFileFd::OutString(const char* pFmt, ...)
{
	_ASSERT(m_fd != -1);

	FILE* fp = _fdopen(m_fd, "w+");
	if (fp == NULL)
	{
		return -1;
	}
	va_list valist;
	va_start(valist, pFmt);
	//TODO:: level
	int count = _VFPRINTF(fp, pFmt, valist);
	va_end(valist);

	fclose(fp);
	return count;
}

BOOL CXFileFd::Open(const TCHAR* pszFileName, int flags, int mode)
{
	_ASSERT(m_fd == -1 && pszFileName);

#ifdef OS_WIN
	//TODO: unicode

	//!!!windows fopen_s(&fp, "newfile.txt", "rw, ccs= encoding ");
	int fd = -1;
	errno_t errorCode = _tsopen_s(&fd, pszFileName, flags, _SH_DENYNO, mode);
	UNUSED(mode);
	if (errorCode != 0 || fd < 0)
	{
		return FALSE;
	}
#else
	int fd = open(pszFileName, flags, mode);
	if (fd == -1)
	{
		return FALSE;
	}
#endif
	m_fd = fd;
	return TRUE;
}

BOOL CXFileFd::Attach(FILE* fp)
{
	if (fp == NULL)
	{
		return FALSE;
	}

#ifdef OS_WIN
	int fd = _fileno(fp);
#elif (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE)
	int fd = _fileno(fp);
#else
	//TODO:: not support
	return FALSE;
#endif
	return Attach(fd);
}

BOOL CXFileFd::Attach(int fd)
{
	if (m_fd != -1 || fd == -1)
	{
		return FALSE;
	}

	m_fd = fd;
	return TRUE;
}

void CXFileFd::Detach()
{
	m_fd = -1;
}

void CXFileFd::Flush()
{
	//_ASSERT(m_fp);
	//fflush(m_fp);
	//NO NEED
	_ASSERT(m_fd != -1);
#ifdef OS_WIN
#else
	//TODO::
	fsync(m_fd);
#endif
}

void CXFileFd::Close()
{
	if (m_fd != -1)
	{
		_close(m_fd);
		m_fd = -1;
	}
}

ssize_t CXFileFd::Write(const void* pBuffer, size_t bufLen)
{
	return (ssize_t)_write(m_fd, pBuffer, bufLen);
}

ssize_t CXFileFd::Read(void* pBuffer, size_t bufLen)
{
	return (ssize_t)_read(m_fd, pBuffer, bufLen);
}