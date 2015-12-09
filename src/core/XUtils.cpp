/*! XUtils.h
********************************************************************************
description			:  A simple cross platform utilitis tools
related files		:  XAfx.h XUtils.h
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
*******************************************************************************/

#include "XAfx.h"
#include "XUtils.h"
#include <time.h>

#ifndef OS_WIN
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

GUID CXUtils::BuildGuid()
{
	GUID guid = {0};
#ifdef OS_WIN
	::CoCreateGuid(&guid);
#else
	char szUUID[64] = {0};
	int fd = open("/proc/sys/kernel/random/uuid", O_RDONLY);
	if (fd >= 0)
	{
		read(fd, szUUID, 36);
	}
	else
	{
		printf("Read UUID failed\n");
	}
	sscanf(szUUID, "%08x-%04x-%04x-%04x-%02x%02x%02x%02x",
		&guid.Data1, &guid.Data2, &guid.Data3, &guid.Data4[1],&guid.Data4[2],
		&guid.Data4[3],&guid.Data4[4],&guid.Data4[5],&guid.Data4[6],&guid.Data4[7]);
	close(fd);
#endif
	return guid;
}

std::string CXUtils::GetCompilerVersion()
{
#define MSC_VC6			"VC6.0"
#define MSC_VC7			"VC7.0	VS2002"	
#define MSC_VC7_1		"VC7.1	VS2003"
#define MSC_VC8			"VC8.0	VS2005"
#define MSC_VC9			"VC9.0	VS2008"
#define MSC_VC10		"VC10.0	VS2010"
#define MSC_VC11		"VC11.0	VS2011"
#define MSC_VC12		"VC12.0 VS2013"
#define MSC_VC_UNKNOWN	"VC unknownVersion"

#ifdef OS_WIN
	switch (_MSC_VER)
	{
	case 1200:	//vc6.0
		return MSC_VC6;

	case 1300:	//vc7.0		VS2002
		return MSC_VC7;

	case 1310:	//vc7.1		VS2003
		return MSC_VC7_1;

	case 1400:  //VC8.0		VS2005
		return MSC_VC8;

	case 1500:	//VC9.0		VS2008
		return MSC_VC9;

	case 1600:  //VC10.0	VS2010
		return MSC_VC10;

	case 1700:	//VC11.0	VS2011
		return MSC_VC11;

	case 1800:  //VC12.0	VS2013
		return MSC_VC12;

	default:
	case 1900:	//VC
		return MSC_VC_UNKNOWN;
	}

#elif defined(__VERSION__)&&defined(__GNUC__)
	return "gcc" __VERSION__;
#else
#error "Unknown compiler"
#endif
}

BOOL CXUtils::LocalTime(const time_t& t, struct tm* pTm)
{
	_ASSERT(pTm);
#ifdef OS_WIN
	return 0==localtime_s(pTm, &t);
#else
	return NULL != localtime_r(&t, pTm);
#endif
}

/*string ×ª»»Îª wstring */
std::wstring CXUtils::s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
#ifdef OS_WIN
	size_t _converted = 0;
	mbstowcs_s(&_converted, _Dest, _Dsize, _Source, _TRUNCATE);
#else
	mbstowcs(_Dest, _Source, _Dsize);
#endif
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

std::string CXUtils::ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);

#ifdef OS_WIN
	size_t _converted = 0;
	wcstombs_s(&_converted, _Dest, _Dsize, _Source, ws.size());
#else
	wcstombs(_Dest, _Source, _Dsize);
#endif

	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

UINT32 CXUtils::GetTickCount()
{
#ifdef OS_WIN
	return ::GetTickCount();
#elif defined(OS_POSIX)
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

UINT64 CXUtils::GetCurrentTimeNs64()
{
#ifdef OS_WIN
	union
	{
		long long ns100;
		FILETIME ft;
	}now;
	::GetSystemTimeAsFileTime(&now.ft);
	struct timeval tv = { 0 };
	tv.tv_usec = (long)((now.ns100 / 10LL) % 1000000LL);
	tv.tv_sec = (long)((now.ns100 - 116444736000000000LL) / 10000000LL);
#else
	struct timeval tv = { 0 };
	gettimeofday(&tv, NULL);
#endif
	return TIME_SEC_TO_NS(tv.tv_sec) + tv.tv_usec * 1000;
}

BOOL CXUtils::GetCurrentTimeTv(struct timeval* pTv)
{
	_ASSERT(pTv);
#ifdef OS_WIN
	union
	{
		long long ns100;
		FILETIME ft;
	}now;
	::GetSystemTimeAsFileTime(&now.ft);
	pTv->tv_usec = (long)((now.ns100 / 10LL) % 1000000LL);
	pTv->tv_sec = (long)((now.ns100 - 116444736000000000LL) / 10000000LL);
	return TRUE;
#else
	return 0==gettimeofday(pTv, NULL);
#endif
}

size_t CXUtils::split(const std::string& s, const std::string& delim, std::vector< std::string >* pRetSubString)
{
	size_t nCount = 0;
	_ASSERT(pRetSubString);

	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		pRetSubString->push_back(s.substr(last,index-last));
		nCount++;
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index!=std::string::npos && index-last>0)
	{
		pRetSubString->push_back(s.substr(last,index-last));
		nCount++;
	}
	return nCount;
}

size_t CXUtils::split(const std::string& s, const std::string& delim, std::list<std::string>* pRetSubString)
{
	size_t nCount = 0;
	_ASSERT(pRetSubString);

	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		pRetSubString->push_back(s.substr(last,index-last));
		nCount++;
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index!=std::string::npos && index-last>0)
	{
		pRetSubString->push_back(s.substr(last,index-last));
		nCount++;
	}
	return nCount;
}

errno_t CXUtils::GetLastError()
{
#ifdef OS_WIN
	return ::GetLastError();
#elif defined(OS_VXWORKS)
	return errnoGet();
#else
	return errno;
#endif
}

BOOL CXUtils::IsFileExist(const char* pszFilePath)
{
#ifdef OS_WIN
	return _access(pszFilePath, _A_NORMAL) != -1;
#else
	return access(pszFilePath, F_OK)!=-1;
#endif
}
