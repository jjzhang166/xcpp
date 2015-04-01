/*! XTcpSocket.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform tcp socket class
related files		:  XUdpSocket.h
create date			:  2014-09-22
author				:  CHENQ
version				:  v0.0.2
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/22			CHENQ	created
2014/09/26			CHENQ	CXTcpSocket destructor -> virutal
*******************************************************************************/


#ifndef __XTCPSOCKET_H__
#define __XTCPSOCKET_H__

#include "XSocket.h"

/** TCP Socket*/
class CXTcpSocket : 	public CXSocket
{
public:
	CXTcpSocket() {}
	virtual ~CXTcpSocket();
	virtual void Destroy();
};

/** TCP Server Socket*/
class CXTcpServerSocket : public CXTcpSocket
{
public:
	/**
	@brief 创建TCP ServerSocket
	@param [in]inaddr 需要绑定的IP地址和端口，可以全部为0，如果端口为0，则系统会随机分配一个
	@param [out]outaddr 返回实际绑定的端口
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL Create(const sockaddr_in& inaddr, sockaddr_in* outaddr);

	/**
	@brief 开始监听SOCKET
	@param [in]listenMax 最大监听数
		依赖于操作系统实现，windows和linux不同
		linux默认最大511个
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL Listen(size_t listenMax);

	/**
	 本函数为阻塞函数!!!
	@brief 等待对方连接
	@param [out]pRemoteaddr返回远程连接客户端IP和端口
	@return 返回系统分配的和远程连接的SOCKET
	*/
	SOCKET Accept(sockaddr_in* pRemoteaddr);
};

class CXTcpSessionSocket : public CXTcpSocket
{
public:
	/**
	@brief 创建TCP客户端SOCKET
	@param [in]inaddr inaddr.family必须为AF_INET
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL Create(const sockaddr_in& inaddr);

	/**
		本函数相当于Create + Connect
	@brief 创建TCP客户端SOCKET并且连接远程服务器
	@param [in]inaddr 远程服务器IP和端口
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL CreateWithConnect(const sockaddr_in& inaddr);

	/**
		默认情况下SOCKET为非阻塞的，当数据发送量较大时，本函数可能会发送不完整
	@brief 发送数据到远程主机
	@param [in]pBuffer 待发送数据缓冲区
	@param [in]bufLen 缓冲区长度
	@return 返回实际发送的字节数
	*/
	ssize_t Send(const void* pBuffer, size_t bufLen) const;

	/**
		默认情况下SOCKET为非阻塞的，所以在没有数据接收时本函数返回-1
	@brief 从远程主机接收数据
	@param [out]pBuffer 接收数据缓冲区
	@param [in]bufLen 缓冲区长度
	@return 返回实际接收的数据长度
	*/
	ssize_t Recv(void* pBuffer, size_t bufLen) const;

	/**
	@brief 	等待并从远程主机接收数据
	@param [out]pBuffer 接收数据缓冲区
	@param [in]bufLen 缓冲区长度
	@param timeOutMs 超时以毫秒为单位
	@return 返回实际接收的数据长度
	*/
	//判断当前socket是否可以发送，直到超时，返回1可以发送，返回0，超时,或者远程SOCKET主动关闭，返回-1 出错或者远程SOCKET异常退出
	ssize_t WaitAndRecv(void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;

	/**
	@brief 	等待并发送数据
	@param [out]pBuffer 接收数据缓冲区
	@param [in]bufLen 缓冲区长度
	@param timeOutMs 超时以毫秒为单位
	@return 返回实际发送的数据长度
	*/
	ssize_t WaitAndSend(const void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;

	/**
	@brief 	等待可发送数据
	@param timeOutMs 超时以毫秒为单位
	@return 返回0表示超时，返回1表示成功，返回-1 失败
	*/
	ssize_t WaitForSend(unsigned int timeOutMs) const;

	/**
	@brief 等待接收数据
	@param timeOutMs 超时以毫秒为单位
	@return 返回0表示超时，返回1表示成功，返回-1 失败
	*/
	ssize_t WaitForRecv(unsigned int timeOutMs) const;
};

#ifdef XCPP_DEVELOP_CODE
#if X_VERSION_VALUE >= X_VERSION_MAKE(0,5,0)
/*管理SOCKET 组*/
class CXTcpSessionGroup
{
public://TODO::
	BOOL AddSocket(SOCKET sock);
	BOOL RemoveSocket(SOCKET sock);

	BOOL PrePare(fd_set& set);
};
#endif
#endif

#endif /*__XTCPSOCKET_H__*/
