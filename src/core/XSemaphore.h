/*! XLock.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross lock for thread sync
related files		:  XLock.cpp
create date			:  2014-09-24
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/23			CHENQ	created

\TODO::
1. 递归的互斥量
2. 计数信号量 CXSemCount
3. CXReadWriteLock读写信号量
*******************************************************************************/
#ifndef __XLOCK_H__
#define __XLOCK_H__

#include "XSTLMacro.h"
#ifdef OS_WIN
typedef HANDLE sem_t;
#endif

#ifdef OS_LINUX
#include <semaphore.h>
#endif

class CXSem
{
public:
	enum SemState
	{
		SemState_Empty = 0,
		SemState_Full,
	};

	CXSem(){}
	virtual ~CXSem(){}
	//virtual BOOL Take(UINT timeOutMs) = 0;

	virtual BOOL Take() = 0;
	virtual BOOL WaitForTake(UINT timeOutMs) = 0;
	virtual void Give()=0;
};

/** @brief二进制信号量，用以线程同步 */
class CXSemBinary : public CXSem
{
public:
	/**
	*  @brief构造函数
	*  @param [in] bInit 信号量的初始状态(TRUE)则信号初始为FULL，否则则为EMPTY
	*/

	CXSemBinary(SemState initState = SemState_Full);
	virtual ~CXSemBinary();

	/**
	*  等待获取信号量，用以非阻塞等待
	*  @param [in] timeOutMs 最大等待获取信号量的超时毫秒数
	*  @return 获取成功立即返回TRUE，FALSE则表示获取信号量失败
	*/
	virtual BOOL WaitForTake(UINT timeOutMs = WAIT_FOREVER);

	/**
	* @brief获取信号量，阻塞等待
	* Take相当于WaitForTake(WAIT_FOREVER);
	*  @return 获取成功立即返回TRUE，如果返回FALSE则系统可能出错了，正常情况不返回FALSE
	*/
	BOOL Take();

	/**
	* 释放信号量
	*  @return 无
	*/
	void Give();
private:
	sem_t m_sem;
	DISALLOW_COPY_AND_ASSIGN(CXSemBinary);
};

/** @brief互斥号量，用以线程互斥 */
class CXSemMutex : public CXSem
{
public:
	CXSemMutex();
	virtual ~CXSemMutex();

	/**
	*  @brief等待获取信号量，用以非阻塞等待
	*  @param [in] timeOutMs 最大等待获取信号量的超时毫秒数
	*  @return 获取成功立即返回TRUE，FALSE则表示获取信号量失败
	*/
	virtual BOOL WaitForTake(UINT timeOutMs = WAIT_FOREVER);

	/**
	* @brief获取信号量\n
	* Take相当于WaitForTake(WAIT_FOREVER);
	*  @return 获取成功立即返回TRUE，如果返回FALSE则系统可能出错了，正常情况不返回FALSE
	*/
	virtual BOOL Take();

	void Give();
	/**
	* 释放信号量
	*  @return 无
	*/

private:
	volatile BOOL m_bLocked;
#ifdef OS_WIN
	CRITICAL_SECTION m_handle;
#elif defined(OS_POSIX)
	pthread_mutex_t m_handle;
#endif
	DISALLOW_COPY_AND_ASSIGN(CXSemMutex);
};

/** @brief互斥号量，用以进程互斥 */
class CXSemProcessMutex : public CXSem
{
public:
	CXSemProcessMutex();
	virtual ~CXSemProcessMutex();

	BOOL Open(const TCHAR* pszKey, BOOL bCreateFlag);

	void Close();

	/**
	*  @brief等待获取信号量，用以非阻塞等待
	*  @param [in] timeOutMs 最大等待获取信号量的超时毫秒数
	*  @return 获取成功立即返回TRUE，FALSE则表示获取信号量失败
	*/
	virtual BOOL WaitForTake(UINT timeOutMs = WAIT_FOREVER);

	/**
	* @brief获取信号量\n
	* Take相当于WaitForTake(WAIT_FOREVER);
	*  @return 获取成功立即返回TRUE，如果返回FALSE则系统可能出错了，正常情况不返回FALSE
	*/
	virtual BOOL Take();

	/**
	* 释放信号量
	*  @return 无
	*/
	void Give();

	XSTLString GetKey() const { return m_strKey; }

private:
	XSTLString m_strKey;
	volatile BOOL m_bLocked;

#ifdef OS_WIN
	HANDLE m_handle;
#elif defined(OS_POSIX)
	sem_t* m_handle;
	BOOL m_bCreate;
#endif
	DISALLOW_COPY_AND_ASSIGN(CXSemProcessMutex);
};

/** @brief临界区对象，辅助类，配合CXSemMutex互斥信号量使用，提供自动加锁和解锁操作 */
class CXCriticalSection
{
public:
	CXCriticalSection(CXSemMutex* pMutex) : m_pSem(pMutex)
	{
		m_pSem->Take();
	}
	virtual ~CXCriticalSection()
	{
		m_pSem->Give();
	}
private:
	CXSem* m_pSem;
	DISALLOW_COPY_AND_ASSIGN(CXCriticalSection);
};

/** 计数信号量，多线程共享资源@@@开发中的代码 */
class CXSemCount : public CXSem
{
public:
	/**
	*  @brief 创建一个计数信号量
	*  @param [in] nCount 信号量最大数目
	*  @param [in] initState 信号量初始状态,该参数在linux下无效，总是为SemState_Full
	*  !!!注: 由于linux信号量实现的原因，nCount在linux下表示初始状态信号数目,初始总是full状态
	*/
	CXSemCount(UINT nCount, SemState initState=SemState_Full);

	virtual BOOL Take();
	virtual BOOL WaitForTake(UINT timeOutMs);
	virtual void Give();
	virtual ~CXSemCount();
private:
	sem_t m_sem;
	DISALLOW_COPY_AND_ASSIGN(CXSemCount);
};

XCLASSPRIVATE_FORWARD_DECLARE(CXSemReadWrite);
class CXSemReadWrite
{
public:
	explicit CXSemReadWrite();
	~CXSemReadWrite();

	BOOL WaitForTakeRead(UINT timeOutMs);
	BOOL WaitForTakeWrite(UINT timeOutMs);
	void TakeRead();
	void TakeWrite();
	void Give();

private:
	XCLASSPRIVATE_DECLARE(CXSemReadWrite);
	DISALLOW_COPY_AND_ASSIGN(CXSemReadWrite);
};

class CXSemReadLocker
{
public:
	CXSemReadLocker(CXSemReadWrite* pRwLock) : m_p(pRwLock)
	{
		_ASSERT(pRwLock);
		pRwLock->TakeRead();
	}
	~CXSemReadLocker()
	{
		m_p->Give();
		m_p = NULL;
	}
private:
	CXSemReadWrite* m_p;
};

class CXSemWriteLocker
{
public:
	CXSemWriteLocker(CXSemReadWrite* pRwLock) : m_p(pRwLock)
	{
		_ASSERT(pRwLock);
		pRwLock->TakeWrite();
	}
	~CXSemWriteLocker()
	{
		m_p->Give();
		m_p = NULL;
	}
private:
	CXSemReadWrite* m_p;
};

#endif /*__XLOCK_H__*/