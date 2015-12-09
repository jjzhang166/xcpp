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
1. �ݹ�Ļ�����
2. �����ź��� CXSemCount
3. CXReadWriteLock��д�ź���
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

/** @brief�������ź����������߳�ͬ�� */
class CXSemBinary : public CXSem
{
public:
	/**
	*  @brief���캯��
	*  @param [in] bInit �ź����ĳ�ʼ״̬(TRUE)���źų�ʼΪFULL��������ΪEMPTY
	*/

	CXSemBinary(SemState initState = SemState_Full);
	virtual ~CXSemBinary();

	/**
	*  �ȴ���ȡ�ź��������Է������ȴ�
	*  @param [in] timeOutMs ���ȴ���ȡ�ź����ĳ�ʱ������
	*  @return ��ȡ�ɹ���������TRUE��FALSE���ʾ��ȡ�ź���ʧ��
	*/
	virtual BOOL WaitForTake(UINT timeOutMs = WAIT_FOREVER);

	/**
	* @brief��ȡ�ź����������ȴ�
	* Take�൱��WaitForTake(WAIT_FOREVER);
	*  @return ��ȡ�ɹ���������TRUE���������FALSE��ϵͳ���ܳ����ˣ��������������FALSE
	*/
	BOOL Take();

	/**
	* �ͷ��ź���
	*  @return ��
	*/
	void Give();
private:
	sem_t m_sem;
	DISALLOW_COPY_AND_ASSIGN(CXSemBinary);
};

/** @brief��������������̻߳��� */
class CXSemMutex : public CXSem
{
public:
	CXSemMutex();
	virtual ~CXSemMutex();

	/**
	*  @brief�ȴ���ȡ�ź��������Է������ȴ�
	*  @param [in] timeOutMs ���ȴ���ȡ�ź����ĳ�ʱ������
	*  @return ��ȡ�ɹ���������TRUE��FALSE���ʾ��ȡ�ź���ʧ��
	*/
	virtual BOOL WaitForTake(UINT timeOutMs = WAIT_FOREVER);

	/**
	* @brief��ȡ�ź���\n
	* Take�൱��WaitForTake(WAIT_FOREVER);
	*  @return ��ȡ�ɹ���������TRUE���������FALSE��ϵͳ���ܳ����ˣ��������������FALSE
	*/
	virtual BOOL Take();

	void Give();
	/**
	* �ͷ��ź���
	*  @return ��
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

/** @brief������������Խ��̻��� */
class CXSemProcessMutex : public CXSem
{
public:
	CXSemProcessMutex();
	virtual ~CXSemProcessMutex();

	BOOL Open(const TCHAR* pszKey, BOOL bCreateFlag);

	void Close();

	/**
	*  @brief�ȴ���ȡ�ź��������Է������ȴ�
	*  @param [in] timeOutMs ���ȴ���ȡ�ź����ĳ�ʱ������
	*  @return ��ȡ�ɹ���������TRUE��FALSE���ʾ��ȡ�ź���ʧ��
	*/
	virtual BOOL WaitForTake(UINT timeOutMs = WAIT_FOREVER);

	/**
	* @brief��ȡ�ź���\n
	* Take�൱��WaitForTake(WAIT_FOREVER);
	*  @return ��ȡ�ɹ���������TRUE���������FALSE��ϵͳ���ܳ����ˣ��������������FALSE
	*/
	virtual BOOL Take();

	/**
	* �ͷ��ź���
	*  @return ��
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

/** @brief�ٽ������󣬸����࣬���CXSemMutex�����ź���ʹ�ã��ṩ�Զ������ͽ������� */
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

/** �����ź��������̹߳�����Դ@@@�����еĴ��� */
class CXSemCount : public CXSem
{
public:
	/**
	*  @brief ����һ�������ź���
	*  @param [in] nCount �ź��������Ŀ
	*  @param [in] initState �ź�����ʼ״̬,�ò�����linux����Ч������ΪSemState_Full
	*  !!!ע: ����linux�ź���ʵ�ֵ�ԭ��nCount��linux�±�ʾ��ʼ״̬�ź���Ŀ,��ʼ����full״̬
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