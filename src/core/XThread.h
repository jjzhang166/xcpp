/*! XThread.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform tcp socket class
related files		:  XThread.cpp
create date			:  2014-09-23
author				:  CHENQ
version				:  0.0.2
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------
TODO:
1. ThreadInterface: An Interface of a thread to show how to use CXThead...
2. ά�������û��������������̵߳�ȫ�ֱ�

history :
2014/09/23			CHENQ	created
2014/09/27			CHENQ 	CXThread destructor -> virtual, fix thread quit free memery BUGS..
2014/10/11			CHENQ	Add staic void Thread::Sleep
2014/10/18			CHENQ	1. ʵ��CXThread::Wait����
							2. ����CXThreadEx δ�����滻CXThread
							3. ����CXThreadWaiter ���������߳��еȴ����߳��˳�
*******************************************************************************/

#ifndef __XTHREAD_H__
#define __XTHREAD_H__

#ifdef OS_WIN
typedef HANDLE ThreadHandle;
typedef unsigned int ThreadId;
#elif defined(OS_POSIX)
#include <pthread.h>
typedef pthread_t ThreadId;
typedef pthread_t ThreadHandle;
#endif

#include "XSemaphore.h"
#include <vector>
#include <list>

typedef int (*THREAD_FUNCPTR)(void* pArg);

/**@brief �߳���*/
/**\TODO:: �̱߳��쳣����ʱ(exit��)��������*/
class CXThread
{
public:
	CXThread();
	virtual ~CXThread();

	//���ú����߳�����ִ��
	BOOL CreateThread(THREAD_FUNCPTR pFunc, void* pArg, size_t stackSize);

	static ThreadId GetCurrentThreadId();
	static void Sleep(UINT sleepMs);
	static void Yeild();

	BOOL Terminate(int ExitCode);
	BOOL IsRunning() const;
	BOOL IsFinished() const { return m_bFinished; }
	BOOL Wait(ULONG timeoutMs=WAIT_FOREVER);
	size_t GetStackSize() const;

	ThreadHandle GetThreadHandle() const { return m_tHandle; }
	ThreadId GetThreadId() const 
	{ 
#ifdef OS_WIN
		return m_tid; 
#else
		return m_tHandle;
#endif
	}

protected:
	virtual int Run() { return 0; }
private:
#ifdef OS_WIN
	static unsigned WINAPI wrapper_function(void* pArg);
#else
	static void* wrapper_function(void* pArg);
#endif

	size_t m_stackSize;
	ThreadHandle m_tHandle;
#ifdef OS_WIN
	ThreadId m_tid;
#endif
	CXSemBinary m_semb;

	CXSemMutex m_lock;

	BOOL m_bIsRunning;
	BOOL m_bFinished;

	THREAD_FUNCPTR m_pFunc;
	void* m_pTheadParam;

	int m_iExitCode;
#ifdef OS_POSIX
	//pthread_mutex_t m_mutex;
	//pthread_cond_t m_cond;
#endif
};

#if defined(XCPP_DEVELOP_CODE)

/** @briefĬ���߳�ջ�ռ��С�����������SetStackSize�������� */
#define XTHREAD_STACK_SIZE (1024*1024U)
class CXThreadEx
{
public:
	CXThreadEx();
	virtual ~CXThreadEx();

	/*
	*  �����̣߳������߳�֮��������߳�������Run����
	*  @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL Start();

	/*
	*  @brief�ȴ��߳��˳���ע�Ȿ������ֹ�ڱ��������߳��е���!������û������ʱ�����˳�����������TRUE
	*  @param [in] timeOutMs ����ָ���ȴ�timeOutMs����ĳ�ʱ��
	*  @return ����ȴ���ʱ�䳬���û������timeOutMs�߳���Ȼδ���н����򷵻�FALSE�������������TRUE
	*/
	BOOL Wait(ULONG timeOutMs=WAIT_FOREVER);

	/*
	*  @briefǿ���ս��߳�
	*  @param [in] ExitCode �����˳����룬�˺����ͨ��GetExitCode��������ȡ�˳����룬�������������Ƿ������˳�
	*  @return ����ȴ���ʱ�䳬���û������timeOutMs�߳���Ȼδ���н����򷵻�FALSE�������������TRUE
	*/
	BOOL Terminate(int ExitCode);

	/*
	*  @brief�ȴ��߳��˳���ע�Ȿ������ֹ�ڱ��������߳��е���!
	*  @param [in] nSizeInBytes ����1˵��
	*  @return ����ȴ���ʱ�䳬���û������timeOutMs�߳���Ȼδ���н����򷵻�FALSE�������������TRUE
	*/
	BOOL SetStackSize(UINT nSizeInBytes);

	/*
	* @brief�����߳��Ƿ���������
	* @return �������з���TRUE������߳��˳�����δ���з���FALSE
	*/
	inline BOOL IsRunning() const { return m_bRunning; }

	/*
	* @brief�����߳��Ƿ��Ѿ��˳�
	* @return �Ѿ��˳�����TRUE������߳�δ���л����������з���FALSE
	*/
	inline BOOL IsFinished() const { return m_bFinished; }

	/*
	* @brief�����߳��˳�����
	* @return �����߳��˳�����\n
	* ����߳������˳�����Run��������ʱ�ķ���ֵ�������û������Run������Ĭ�Ϸ���0.\n
	* �������Terminate��ǿ�ƹر��̺߳󣬵��ô˺�������Terminate������˳�����
	*/
	inline int GetExitCode() const { return m_iExitCode; }
protected:
	/*
	* @brief�߳�ʵ�庯��������Ҫ���ش˺�����ʵ���̣߳�Run�����еĴ��뽫���ڱ��������߳�������\n
	* ���߳��˳�ʱ�̶߳���ᱣ��Run�����ķ���ֵ��
	* @return �����߳��˳�����\n
	* ����߳������˳�����Run��������ʱ�ķ���ֵ�������û������Run������Ĭ�Ϸ���0.\n
	* �������Terminate��ǿ�ƹر��̺߳󣬵��ô˺�������Terminate������˳�����
	*/
	virtual int Run() { return 0; }

private:

#ifdef OS_WIN
	static unsigned WINAPI wrapperFunction(void* pArg);
#else
	static void* wrapperFunction(void* pArg);
#endif

private:
	size_t m_stackSize;
	CXSemMutex m_lock;
	BOOL m_bRunning;
	BOOL m_bFinished;
	BOOL m_bStarted;
	int m_iExitCode;
	ThreadHandle m_handle;
	ThreadId m_tid;
};
#endif /*__XCPP_DEVELOP_CODE*/

/** @brief�̵߳ȴ�������
* ����Ŀ���ڸ��߳��еȴ����߳��˳�
*/
class CXThreadWaiter
{
public:
	explicit CXThreadWaiter();
	~CXThreadWaiter();

	/**
	* @brief ���һ�����߳�ָ�뵽����б���
	* @param [in] pThread ���߳�ָ��
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL Add(CXThread* pThread);

	/**
	* @brief ���һ�����߳�ָ�뵽����б���,���������
	* @param [in] pThread ���߳�ָ��
	*/
	CXThreadWaiter& operator << (CXThread* pThread);

	/**
	* @brief �ȴ��ڼ����б��е������߳��˳�\n
	* ������Ϊ��������
	* @param  ��
	* @return ��
	*/
	void WaitAll();

	/**
	* @brief ɾ�������б��е��̶߳���(������������)\n
	*  ����߳���δ�˳������CXThread::Terminate(-1) \n
	*  NOTE1 : ���ñ�����ǰ�����ȵ���WaitAll() \n
	*  NOTE2 : ���뵽��������е��̶߳����û����ܼȵ���delete�ֵ��ñ�����\n
	*  NOTE3 : ����̶߳���Ϊջ�ϵĲ��ܵ��øú���
	* @param ��
	* @return ��
	*/
	void DeleteAll();

private:
	std::list<CXThread*> m_threadList;
	CXSemMutex m_mutex;

	DISALLOW_COPY_AND_ASSIGN(CXThreadWaiter);
};


#endif /*__XTHREAD_H__*/