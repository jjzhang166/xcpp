/*! XSocket.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform socket defines
related files		:  XSocket.h
create date			:  2014-09-22
author				:  CHENQ
version				:	v0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/22			CHENQ	created
2014/09/26			CHENQ	add connect&SNDBUF&RXBUF functions

/TODO:
1. socket 读写时需要检测 EAGAIN、EWOULDBLOCK 和 EINTER
	异步模式下去读一个没有数据的描述符会返回-1，并设置错误码为EAGAIN
	EINTER 错误码，当前阻塞IO被系统调用打断，如accept建立连接打断select，此时select返回0
*******************************************************************************/

#ifndef __XSOCKET_H__
#define __XSOCKET_H__


#ifndef OS_WIN
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#ifndef OS_WIN
typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define closesocket close 
#define UDP_RX_BUF_MAX	262142U	//linux系统可设置的最大值

#define WAIT_TIMEOUT	(0x1111)
#define WAIT_FAILED		(~(UINT)0)
#define WAIT_OBJECT_0	0
#else
typedef int socklen_t;
#endif

class CXSocket
{
public:
	enum
	{
		Socket_NoBlock = 0,	/**< 非阻塞SOCKET		*/
		Socket_Block,		/**< 阻塞SOCKET			*/
	};

	/* @brief 初始化SOCKET环境
	* 初始化SOCKET使用环境
	* @deprecated 将来可能会移除
	*/
	static void Init();

	/**
	* @brief 返回当前SOCKET错误码
	*/
	static errno_t GetLastSocketError();

	CXSocket() : m_sockfd(INVALID_SOCKET){}
	virtual ~CXSocket(){}

	/**
	* @brief连接到远程SOCKET
	* @param [in] remoteAddr 远程SOCKET IPv4的IP和端口
	* @return 成功返回TRUE，失败返回FALSE \n
	* 错误码可以通过@see GetLastSocketError()
	*/
	BOOL Connect(const sockaddr_in& remoteAddr) const;

	/**
	* @brief关闭SOCKET
	*/
	void Close() { closesocket(m_sockfd); }

	/**
	* @brief获取接收缓冲区大小
	* @return 成功返回缓冲区大小\n
		失败返回-1 @see GetLastSocketError()
	*/
	ssize_t GetTxBufferSize() const;

	/**
	* @brief获取发送缓冲区大小
	* @return 成功返回缓冲区大小\n
	失败返回-1 @see GetLastSocketError
	*/
	ssize_t GetRxBufferSize() const;

	/**
	* @brief获取发送缓冲区大小
	* @return 成功返回缓冲区大小\n
	* 失败返回-1 @see GetLastSocketError
	* @see SetRxBufferSize
	*/

	/**
	* @brief设置发送缓冲区大小
	* @return 成功返回缓冲区大小\n
	*	失败返回-1 @see GetLastSocketError
	* @see GetRxBufferSize
	*/
	BOOL SetRxBufferSize(size_t sizeInBytes) const;

	/**
	* @brief设置发送缓冲区大小
	* @return 成功返回缓冲区大小\n
	* 失败返回-1 @see GetLastSocketError()
	*  @see GetRxBufferSize
	*/
	BOOL SetTxBufferSize(size_t sizeInBytes) const;

	/**
	@brief 获取SOCKET TTL
	@return 返回当前SOCKET的 TTL，函数失败返回-1，成功返回当前的TTL
	@see SetTTL
	*/
	int GetTTL() const;

	/**
	@brief 设置SOCKET TTL
	@param [in]ttl 要设置的TTL，注意按照bsd socket规范TTL的范围是[1,255]
	@return 设置当前SOCKET的 TTL，函数失败返回FALSE，成功返回当前的TRUE
	@see GetTTL
	*/
	BOOL SetTTL(int ttl) const;

	/**
	@brief 设置SOCKET Addr Reuse (TCP&UDP)

	SO_REUSEADDR可以用在以下四种情况下。 (摘自《Unix网络编程》卷一，即UNPv1) \n
    1、当有一个有相同本地地址和端口的socket1处于TIME_WAIT状态时，而你启 \n
		动的程序的socket2要占用该地址和端口，你的程序就要用到该选项。 \n
    2、SO_REUSEADDR允许同一port上启动同一服务器的多个实例(多个进程)。但
		每个实例绑定的IP地址是不能相同的。在有多块网卡或用IP Alias技术的 \n
		机器可以测试这种情况。 
    3、SO_REUSEADDR允许单个进程绑定相同的端口到多个socket上，但每个socket \n
		绑定的ip地址不同。这和2很相似，区别请看UNPv1。 
    4、SO_REUSEADDR允许完全相同的地址和端口的重复绑定。但这只用于UDP的多播，
		不用于TCP。 (UDP Only)

	@param [in]bTrue TRUE 允许重用，FALSE禁止重用
	@return 函数失败返回FALSE，成功返回当前的TRUE
	*/
	BOOL SetAddrReuse(BOOL bTrue) const;

#ifndef OS_WIN
	/**
		@brief获取当前虚拟路由号
		@return 当前路由号
	*/
	int GetVr() const;

	/**
		@brief设置当前虚拟路由号
		@param [in] vr 路由号
	*/
	BOOL SetVr(int vr) const;

	/**
	@brief 设置端口重用\n
		windows下不支持该函数，SetAddrReuse替代

	@param [in]bTrue TRUE 允许重用，FALSE禁止重用
	@return 函数失败返回FALSE，成功返回当前的TRUE
	*/
	BOOL SetPortReuse(BOOL bTrue) const;

	/**
	@brief 绑定SOCKET到指定的网卡设备上
		使用环境：
		1. 有系统有多个网卡（包含虚拟网卡）
		2，如果BIND到任意地址SOCKET主动发起连接有可能连接失败
			这个取决于默认网关，和操作系统指定的网卡
		3. 该函数需要拥有root权限

	@param [in]pDeviceName 绑定的网卡名称
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL BindToDevice(const char* pDeviceName) const;
#endif

	/**
	@brief 获取一个套接字的名字

		它用于一个已捆绑或已连接套接字s，本地地址将被返回 \n
		本调用特别适用于如下情况：未调用bind()就调用了connect()，\n
		这时唯有getsockname()调用可以获知系统内定的本地地址。\n
		在返回时，namelen参数包含了名字的实际字节数
	@param [out]pLocalAddr 返回的sockaddr_in数据指针
	@return 函数失败返回FALSE，成功返回当前的TRUE
	*/
	BOOL GetSockName(sockaddr_in* pLocalAddr) const;

	/**
	@brief 附加一个SOCKET，
		只有附加了正确SOCKET之后本类的函数才可以调用成功
		一个CXSocket对象只能附加一个SOCKET\n
		当已经附加了一个socket，再次调用本函数会返回FALSE
		如果需要操作新的socket请先调用
	@param [in] fd 被附加的SOCKET，支持TCP/UDP的SCOKET
	@return 成功返回TRUE，失败返回FALSE
	@see Detach  GetSock
	*/
	BOOL Attach(SOCKET fd);

	/**
	@brief 解除当前附加的SOCKET\n
		详见Attach
	@see Attach
	*/
	void Detach();

	/**
	@brief读取SOCKET数据
	@param [out] pBuffer 返回SOCKET数据的缓冲区
	@param [in] bufLen 缓冲区pBuffer的大小
	@return 成功获取到数据的长度,返回值是有符号的不会大于bufLen
			失败返回-1
	@see Write
	*/
	virtual ssize_t Read(void* pBuffer, size_t bufLen) const;

	/**
	@brief发送SOCKET数据
	@param [in] pBuffer 待发送SOCKET数据的缓冲区
	@param [in] bufLen 缓冲区pBuffer的大小
	@return 返回成功发送的字节数
		失败返回-1或者小于dataLen
	@see Read
	*/
	virtual ssize_t Write(const void* pBuffer, size_t dataLen) const;

	//virtual BOOL Connect(const sockaddr_in& addr) = 0;

	/**
	@brief 等待多个描述符可读，类似于Windows的WaitForMultiObjects

	@param [in] sockets 等待的描述符指针
	@param [in] nCount sockets缓冲区中元素的个数
	@param [in] bWaitAll 是否等待全部就绪
	@param [in] nTimeOutMs 最长等待时间以毫秒为单位，如果nTimeOutMs为WAIT_FOREVER则永久等待
	@return bWaitAll为TRUE时，只有当所有描述符都就绪或者超时了才会返回\n
				返回值 WAIT_OBJECT_0 表明全部就绪\n
				返回值 WAIT_TIMEOUT 表明超时\n
				返回值 WAIT_FAILED 表明出错\n
			bWaitAll为FALSE时，只要有任何SOCKET读就绪或者超时结束则返回
				返回值 [WAIT_OBJECT_0 ~ WAIT_OBJECT_0+nCount-1] 就绪SOCKET的索引值和WAIT_OBJECT_0的和 \n
				返回值 WAIT_TIMEOUT 表明超时\n
				返回值 WAIT_FAILED 表明出错\n
	@deprecated 本函数仅用于Read，与函数名称不太贴切，未来会修改
	*/
	static UINT WaitForMultiSocket(const SOCKET sockets[], UINT nCount, BOOL bWaitAll, UINT nTimeOutMs);

	static UINT WaitForSingleSocket(const SOCKET& sockets, UINT nTimeOutMs);

	/*
	*  @brief返回当前SOCKET是否有效
	*  @return 有效返回TRUE，无效返回FALSE
	*/
	inline BOOL IsValid() const { return m_sockfd != INVALID_SOCKET; }

	/*
	*  @brief返回当前SOCKET
	*  @return 当前SOCKET，INVALID_SOCKET表示未附加SOCKET
	*  @see Attach
	*/
	inline SOCKET GetSock() const { return m_sockfd; }

protected:
	SOCKET m_sockfd;
};



#endif  /*__XSOCKET_H__*/