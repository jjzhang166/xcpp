/*! XUdpSocket.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform udp socket class
related files		:  XUdpSocket.h
create date			:  2014-09-10
author				:  CHENQ
version				:  v0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
2014/09/25			CHERQ	CXUdpSocket inherited from CXSocket
*******************************************************************************/

#ifndef __XUDPSOCKET_H__
#define __XUDPSOCKET_H__

#include "XSocket.h"

/** UDP Socket*/
class CXUdpSocket : public CXSocket
{
public:
	CXUdpSocket();
	virtual ~CXUdpSocket();

	/**
	@brief 使能UDP广播
	@param [in] bEnale TRUE为开启，FALSE为关闭
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL EnableBroadcast(BOOL bEnale) const;

	/**
	@brief 加入广播组
	@param [in] multiAddr 多播地址
	@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL JoinMultiGroup(ULONG multiAddr) const;

	/**
	@brief 设置当前SOCKET多播TTL
	@param [in] ttl 设置范围[1,255]
	@return 成功返回TRUE，失败返回FALSE
	@see GetMulitcastTTL
	*/
	BOOL SetMulitcastTTL(int ttl) const;

	/**
	@brief 获取当前SOCKET多播TTL
	@return 返回TTL值，如果返回-1表示失败
	@see SetMulitcastTTL
	*/
	int GetMulitcastTTL() const;

	/**
		目前该函数参数中仅仅af_family有用，且必须为AF_INET
		如果不使用了请销毁

	@brief 创建一个UDP客户端
	@param inaddr 输入本地SOCKET地址，
	@return 成功返回TRUE，失败返回FALSE
	@see CreateServer  Destroy
	*/
	BOOL CreateClient(const sockaddr_in& inaddr);

	/**
	
		@brief 创建一个UDP Server 的SOCKET
		@param [in]inaddr Server SOCKET IP和port 可以全为0
		@param [out]如果输入参数 inaddr全为0，则系统会随机分配一个UDP端口号，本参数返回这个端口
		@return 成功返回TRUE，失败返回FALSE
		@see CreateClient  Destroy
	*/
	BOOL CreateServer(const sockaddr_in& inaddr, sockaddr_in* outaddr);

	/**
	@brief 销毁UDP Socket（Server/Client)
	@param [in]inaddr Server SOCKET IP和port 可以全为0
	@param [out]如果输入参数 inaddr全为0，则系统会随机分配一个UDP端口号，本参数返回这个端口
	@return 成功返回TRUE，失败返回FALSE
	@see CreateClient  CreateServer
	*/
	void Destroy();

	/**
	@brief 设置UDP接收超时
	@param [in]nTimeOutMs 以毫秒为单位的超时数
	@return 成功返回TRUE，失败返回FALSE
	@see CreateClient  CreateServer
	*/
	BOOL SetRxTimeOut(UINT nTimeOutMs);

	/**
	@brief 接收udp消息
	@param [out]fromAddr 远程addr
	@param [out]pBuffer 返回数据缓冲区
	@param [in]bufLen pBuffer缓冲区大小
	@return 成功返回实际接收到的字节数，-1表示失败，0表示超时
	@see CreateClient  CreateServer
	*/
	ssize_t RecvFrom(sockaddr_in& fromAddr, void* pBuffer, size_t bufLen) const;

	/**
	@brief 销毁UDP Socket（Server/Client)
	@param [in]inaddr Server SOCKET IP和port 可以全为0
	@param [out]如果输入参数 inaddr全为0，则系统会随机分配一个UDP端口号，本参数返回这个端口
	@return 成功返回TRUE，失败返回FALSE
	@see CreateClient  CreateServer
	*/
	ssize_t SendTo(const sockaddr_in& remoteAddr, const void* pBuffer, size_t len) const;

	/**

	@brief 等待并接受数据
	@param [out]fromAddr 返回接受到数据的IP和端口
	@param [out]pBuffer 接受数据缓冲区
	@param [in] bufLen 接受数据缓冲区pBuffer大小
	@param [in] timeOutMs 等待超时
	@return 返回接收到的数据长度，失败返回-1
	@see CreateClient  CreateServer GetLastSockError
	*/
	ssize_t WaitAndRecv(sockaddr_in& fromAddr, void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;

	/**

	@brief 获取发送就绪状态
	@param [in] timeOutMs 等待超时
	@return 返回接收到的数据长度，失败返回-1
	@see CreateClient  CreateServer GetLastSockError
	@deprecated
	*/
	ssize_t WaitForSend(unsigned int timeOutMs) const;

	/**
	@brief 等待直到发送完成
		@param[in] timeOutMs 等待超时
		@return 返回实际发送数据长度，返回值==bufLen 表示成功，其他值失败
		@see WaitAndRecv
	*/
	ssize_t WaitAndSendTo(const sockaddr_in& oAddr, const void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;
};

#endif

