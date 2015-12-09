/*! XTcpSocket.cpp
********************************************************************************
description			:  A simple cross platform tcp socket class
related files		:  XTcpSocket.h
create date			:  2014-09-22
author				:  CHENQ
version				:  0.0.2
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/22			CHENQ	created
2014/09/26			CHENQ	fix CXTcpSessionSocket::Create failed BUG
							CXTcpSessionSocket::WaitAndSend separate buffer to 2048 to avoid send failed
*******************************************************************************/

#include "XAfx.h"
#include "XSocket.h"
#include "XTcpSocket.h"
#include "XLog.h"
#include "XUtils.h"

CXTcpSocket::~CXTcpSocket()
{
	Destroy();
}

BOOL CXTcpSessionSocket::Create(const sockaddr_in& inaddr)
{
	if (GetSock() != INVALID_SOCKET)
		return TRUE;

	SOCKET fd = socket(inaddr.sin_family, SOCK_STREAM, 0);
	if (INVALID_SOCKET == fd)
		return FALSE;

	return Attach(fd);
}

BOOL CXTcpSessionSocket::CreateWithConnect(const sockaddr_in& inaddr)
{
	if (GetSock() != INVALID_SOCKET)
		return TRUE;

	if (inaddr.sin_addr.s_addr == 0 || inaddr.sin_port == 0)
	{
		XPRINTF(LOG_DEBUG, "CreateClient failed, ip or port equals zero. Maybe you want to create a server, use CXTcpSocket::CreateServer instand\n");
		return FALSE;
	}
	SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == fd)
		return FALSE;

	socklen_t len = sizeof(inaddr);
	if (connect(fd, (const sockaddr*)&inaddr, len) != 0)
	{
		XPRINTF(LOG_DEBUG, "connect to server failed ,errno (%d)\n", CXSocket::GetLastSocketError());
		closesocket(fd);
		return FALSE;
	}
	return Attach(fd);
}

BOOL CXTcpServerSocket::Create(const sockaddr_in& inaddr, sockaddr_in* pOutAddr)
{
	//TODO::异常处理
	if (GetSock() != INVALID_SOCKET)
		return TRUE;

	SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == fd)
		return FALSE;

	if (0 != bind(fd, (const sockaddr*)&inaddr, sizeof(sockaddr_in)))
	{
		closesocket(fd);
		return FALSE;
	}
	if (::listen(fd, SOMAXCONN) != 0)
	{
		closesocket(fd);
		return FALSE;
	}
	if (pOutAddr)
	{
		socklen_t len = sizeof(sockaddr_in);
		getsockname(fd, (sockaddr*)pOutAddr, &len);
	}

	return Attach(fd);
}

void CXTcpSocket::Destroy()
{
	closesocket(GetSock());
	Detach();
}

ssize_t CXTcpSessionSocket::Send(const void* pBuffer, size_t len) const
{
	SOCKET fd = GetSock();
	_ASSERT(fd != INVALID_SOCKET);

	return send(fd, (const char*)pBuffer, len, 0);
}

SOCKET CXTcpServerSocket::Accept(sockaddr_in* pRemoteaddr)
{
	SOCKET fd = GetSock();
	_ASSERT(fd != INVALID_SOCKET);
	sockaddr_in remoteaddr = { 0 };
	socklen_t len = sizeof(remoteaddr);
	SOCKET retFd = accept(fd, (sockaddr*)&remoteaddr, &len);
	if (pRemoteaddr)
	{
		*pRemoteaddr = remoteaddr;
	}
	return retFd;
}

BOOL CXTcpServerSocket::Listen(size_t maxClient)
{
	SOCKET fd = GetSock();
	_ASSERT(fd != INVALID_SOCKET);

	return 0==listen(fd, maxClient);
}

ssize_t CXTcpSessionSocket::WaitAndRecv(void* pBuffer, size_t bufLen, unsigned int timeOutMs) const
{
	SOCKET fd = GetSock();
	_ASSERT(fd != INVALID_SOCKET);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000000;
#else //win & posix
	struct timeval tp = { 0 };
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000;
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}

	int sel = select(fd + 1, &set, NULL, NULL, pTp);
	if (sel == 1)
	{
		ssize_t ret = recv(fd, (char*)pBuffer, bufLen, 0);
		if (ret <= 0)
		{
			return -1;//远程连接断开
		}
		return ret;
	}
	else if (sel == 0)
	{
		return 0;
	}

	return -1;
}

//判断当前socket是否可以发送，直到超时，返回1可以发送，返回0，超时，返回-1 出错
ssize_t CXTcpSessionSocket::WaitForSend(unsigned int timeOutMs) const
{
	SOCKET fd = GetSock();
	_ASSERT(fd != INVALID_SOCKET);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000000;
#else //win & posix
	struct timeval tp = { 0 };
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000;
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}

	int sel = select(fd + 1, NULL, &set, NULL, pTp);
	return sel;
}

ssize_t CXTcpSessionSocket::WaitAndSend(const void* pBuffer, size_t bufLen, unsigned int timeOutMs) const
{
	SOCKET fd = GetSock();
	_ASSERT(fd != INVALID_SOCKET);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000000;
#else //win & posix
	struct timeval tp = { 0 };
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000;
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}
	const char* pSrc = (const char*)pBuffer;
	size_t offset = 0;
	for (; offset<bufLen;)
	{
		char szTxBuffer[2048] = { 0 };

		size_t copyLen = (bufLen-offset)>sizeof(szTxBuffer) ? sizeof(szTxBuffer) : (bufLen-offset);
		memcpy(szTxBuffer, &pSrc[offset], copyLen);

		int sel = select(fd + 1, NULL, &set, NULL, pTp);
		if (sel <= 0)
		{
		//	printf("select faile, errno=%d\n", CXSocket::GetLastSocketError());
			_ASSERT(FALSE);
			return offset;
		}

		if ((ssize_t)copyLen != send(fd, (const char*)szTxBuffer, copyLen, 0))
		{
			//printf("Send failed, errno=%d\n", CXSocket::GetLastSocketError());
			_ASSERT(FALSE);
			return offset;
		}
		offset += copyLen;
	}
	return offset;
}
