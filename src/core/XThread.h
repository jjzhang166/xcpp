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
2. 维护所有用户创建的所有子线程的全局表

history :
2014/09/23			CHENQ	created
2014/09/27			CHENQ 	CXThread destructor -> virtual, fix thread quit free memery BUGS..
2014/10/11			CHENQ	Add staic void Thread::Sleep
2014/10/18			CHENQ	1. 实现CXThread::Wait函数
							2. 增加CXThreadEx 未来会替换CXThread
							3. 增加CXThreadWaiter 用以在主线程中等待子线程退出
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

/**@brief 线程类*/
/**\TODO:: 线程被异常销毁时(exit等)处理！！！*/
class CXThread
{
public:
	CXThread();
	virtual ~CXThread();

	//调用后子线程立即执行
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

/** @brief默认线程栈空间大小，如果不调用SetStackSize进行设置 */
#define XTHREAD_STACK_SIZE (1024*1024U)
class CXThreadEx
{
public:
	CXThreadEx();
	virtual ~CXThreadEx();

	/*
	*  启动线程，启动线程之后会在子线程中运行Run函数
	*  @return 成功返回TRUE，失败返回FALSE
	*/
	BOOL Start();

	/*
	*  @brief等待线程退出，注意本函数禁止在被创建的线程中调用!如果在用户输入的时间内退出则立即返回TRUE
	*  @param [in] timeOutMs 参数指定等待timeOutMs毫秒的超时。
	*  @return 如果等待的时间超过用户输入的timeOutMs线程仍然未运行结束则返回FALSE，其他情况返回TRUE
	*/
	BOOL Wait(ULONG timeOutMs=WAIT_FOREVER);

	/*
	*  @brief强制终结线程
	*  @param [in] ExitCode 设置退出代码，此后可以通过GetExitCode函数来获取退出代码，可以用于区分是否正常退出
	*  @return 如果等待的时间超过用户输入的timeOutMs线程仍然未运行结束则返回FALSE，其他情况返回TRUE
	*/
	BOOL Terminate(int ExitCode);

	/*
	*  @brief等待线程退出，注意本函数禁止在被创建的线程中调用!
	*  @param [in] nSizeInBytes 参数1说明
	*  @return 如果等待的时间超过用户输入的timeOutMs线程仍然未运行结束则返回FALSE，其他情况返回TRUE
	*/
	BOOL SetStackSize(UINT nSizeInBytes);

	/*
	* @brief返回线程是否正在运行
	* @return 正在运行返回TRUE，如果线程退出或者未运行返回FALSE
	*/
	inline BOOL IsRunning() const { return m_bRunning; }

	/*
	* @brief返回线程是否已经退出
	* @return 已经退出返回TRUE，如果线程未运行或者正在运行返回FALSE
	*/
	inline BOOL IsFinished() const { return m_bFinished; }

	/*
	* @brief返回线程退出代码
	* @return 返回线程退出代码\n
	* 如果线程正常退出返回Run函数结束时的返回值，如果你没有重载Run函数，默认返回0.\n
	* 当你调用Terminate来强制关闭线程后，调用此函数返回Terminate传入的退出代码
	*/
	inline int GetExitCode() const { return m_iExitCode; }
protected:
	/*
	* @brief线程实体函数，你需要重载此函数来实现线程，Run函数中的代码将会在被创建的线程中运行\n
	* 当线程退出时线程对象会保存Run函数的返回值。
	* @return 返回线程退出代码\n
	* 如果线程正常退出返回Run函数结束时的返回值，如果你没有重载Run函数，默认返回0.\n
	* 当你调用Terminate来强制关闭线程后，调用此函数返回Terminate传入的退出代码
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

/** @brief线程等待回收类
* 本类目的在父线程中等待子线程退出
*/
class CXThreadWaiter
{
public:
	explicit CXThreadWaiter();
	~CXThreadWaiter();

	/**
	* @brief 添加一个子线程指针到监控列表中
	* @param [in] pThread 子线程指针
	* @return 成功返回TRUE，失败返回FALSE
	*/
	BOOL Add(CXThread* pThread);

	/**
	* @brief 添加一个子线程指针到监控列表中,运算符重载
	* @param [in] pThread 子线程指针
	*/
	CXThreadWaiter& operator << (CXThread* pThread);

	/**
	* @brief 等待在监视列表中的所有线程退出\n
	* 本函数为阻塞函数
	* @param  无
	* @return 无
	*/
	void WaitAll();

	/**
	* @brief 删除监视列表中的线程对象(调用析构函数)\n
	*  如果线程尚未退出则调用CXThread::Terminate(-1) \n
	*  NOTE1 : 调用本函数前建议先调用WaitAll() \n
	*  NOTE2 : 加入到监控链表中的线程对象用户不能既调用delete又调用本函数\n
	*  NOTE3 : 如果线程对象为栈上的不能调用该函数
	* @param 无
	* @return 无
	*/
	void DeleteAll();

private:
	std::list<CXThread*> m_threadList;
	CXSemMutex m_mutex;

	DISALLOW_COPY_AND_ASSIGN(CXThreadWaiter);
};


#endif /*__XTHREAD_H__*/