/*! XLog.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform log file 
related files		:  stdafx.h XLog.cpp
create date			:  2014-09-10
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created


TODO:
1. print/log header build by Policy
*******************************************************************************/

#ifndef __XLOG_H__
#define __XLOG_H__


#define __INC_XLOG

class CXLogFileBinary;
class CXLogFileStream;


class CXLog
{
public:
	enum LOG_LEVEL
	{
		LOG_None = 0,	/**<NONE ��ʼ״̬																*/
		LOG_Emergency,		/**<�����������Ҫ����֪ͨ������Ա��								*/
		LOG_Alert,		/**<Ӧ�ñ��������������⣬��ϵͳ���ݿⱻ�ƻ���ISP���Ӷ�ʧ��			*/
		LOG_Critical,		/**<��Ҫ�������Ӳ�̴��󣬱������Ӷ�ʧ��							*/
		LOG_Error,		/**<���󣬲��Ƿǳ���������һ��ʱ�����޸����ɡ�						*/
		LOG_Warning,	/**<������Ϣ�����Ǵ��󣬱���ϵͳ����ʹ����85%�ȡ�					*/
		LOG_Notice,		/**<���Ǵ��������Ҳ����Ҫ��������								*/
		LOG_Infomation,		/**<�鱨��Ϣ��������ϵͳ��Ϣ������ɧ�ű��棬�������ݵȣ�����Ҫ����*/
		LOG_Debug,		/**<������ϸ�Ŀ����鱨����Ϣ��ͨ��ֻ�ڵ���һ������ʱʹ�á�			*/
		LOG_Max,		/**<MAX																*/
	};

	CXLog();
	virtual ~CXLog();


	//Interface
	static ssize_t PrintOnly(LOG_LEVEL level, const char* pFmt, ...);

	static const char* LogLevel2String(LOG_LEVEL level);
	static LOG_LEVEL String2LogLevel(const char* pString);


	static void SetDebugLevel(LOG_LEVEL level) { m_snDefaultDbgLevel = level; };
	static LOG_LEVEL GetDebugLevel() { return m_snDefaultDbgLevel; }
	static void SetFp(FILE* fp) { m_sfpLog = fp; }
	static void StaicFlush();
protected:
	static LOG_LEVEL m_snDefaultDbgLevel;
	static FILE* m_sfpLog;
};


#ifndef LOG_EMERG
#define LOG_EMERG CXLog::LOG_Emergency
#endif

#ifndef LOG_ALERT
#define LOG_ALERT CXLog::LOG_Alert
#endif

#ifndef LOG_CRIT
#define LOG_CRIT CXLog::LOG_Critical
#endif

#ifndef LOG_ERR
#define LOG_ERR CXLog::LOG_Error
#endif

//benifit for user
#ifndef LOG_ERROR
#define LOG_ERROR LOG_ERR
#endif

#ifndef LOG_WARNING
#define LOG_WARNING CXLog::LOG_Warning
#endif

#ifndef LOG_NOTICE
#define LOG_NOTICE CXLog::LOG_Notice
#endif

#ifndef LOG_INFO
#define LOG_INFO CXLog::LOG_Infomation
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG CXLog::LOG_Debug
#endif

#ifndef XPRINTF
#define XPRINTF CXLog::PrintOnly
#endif

#ifndef LOG_MUST
#define LOG_MUST CXLog::LOG_None
#endif

#define X_FLUSH_LOG  CXLog::StaicFlush

#ifdef OS_WIN

#define XLOG_PRINT(__val,...) \
do	\
{	\
	if (CXLog::GetDebugLevel() >= (__val)) \
	{  \
		XPRINTF(__val, __VA_ARGS__); \
	} \
} \
while (FALSE)

#else

#define XLOG_PRINT(__val,_args...) \
do \
{ \
	if (CXLog::GetDebugLevel() >= (__val)) \
	{ \
		XPRINTF(__val, ##_args); \
	}\
} \
while (FALSE)

#endif

#endif /*__XLOG_H__*/