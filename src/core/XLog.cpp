/*! XLog.cpp
********************************************************************************
description			:  A simple cross platform log file implement
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
#include "XLog.h"
#include "XLogDefines.h"
#include "XUtils.h"

#include <time.h>
#include <stdarg.h>

static const char* sg_szlevelPtr[] =
{
	"EMERG",
	"ALERT",
	"CRIT",
	"ERR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG"
};

static struct DEBUG_STRING_TABLE
{
	CXLog::LOG_LEVEL nDbgLevel;
	const char* pszValue;
	size_t len;
}sg_dbgStringTable[] =
{
	{ LOG_EMERG, "EMERG", 5 },
	{ LOG_ALERT, "ALEART",6},
	{ LOG_CRIT, "CRIT", 4},
	{ LOG_WARNING, "WARNING" ,4},
	{ LOG_NOTICE, "NOTICE" ,4},
	{ LOG_ERR, "ERROR" ,3},
	{ LOG_INFO, "INFOMATION" ,4},
	{ LOG_DEBUG, "DEBUG", 5 },
	{ LOG_DEBUG, "DBG", 3 },
};

#ifdef _DEBUG
CXLog::LOG_LEVEL CXLog::m_snDefaultDbgLevel = LOG_DEBUG;
#else
CXLog::LOG_LEVEL CXLog::m_snDefaultDbgLevel = LOG_ERROR;
#endif

FILE* CXLog::m_sfpLog = stdout;

CXLog::CXLog()
{
}

CXLog::~CXLog()
{
	
}

const char* CXLog::LogLevel2String(LOG_LEVEL level)
{
	for (UINT i = 0; i < _countof(sg_dbgStringTable); i++)
	{
		DEBUG_STRING_TABLE* pTab = &sg_dbgStringTable[i];
		if (level == pTab->nDbgLevel)
		{
			return pTab->pszValue;
		}
	}
	return "NONE";
}

void CXLog::StaicFlush()
{
	fflush(CXLog::m_sfpLog);
}

CXLog::LOG_LEVEL CXLog::String2LogLevel(const char* pString)
{
	for (UINT i = 0; i < _countof(sg_dbgStringTable); i++)
	{
		DEBUG_STRING_TABLE* pTab = &sg_dbgStringTable[i];
		if (_strnicmp(pTab->pszValue, pString, pTab->len) == 0)
		{
			return pTab->nDbgLevel;
		}
	}
	return CXLog::LOG_None;
}

ssize_t CXLog::PrintOnly(LOG_LEVEL level, const char* pFmt, ...)
{
	UNUSED(level);
	va_list valist;

	va_start(valist, pFmt);
	int count = _VFPRINTF(CXLog::m_sfpLog, pFmt, valist);
	fprintf(CXLog::m_sfpLog, "\n");
	va_end(valist);

	return count;
}
