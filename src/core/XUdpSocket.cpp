/*! XUdpSocket.h
********************************************************************************
description			:  A simple cross platform udp socket class
related files		:  XUdpSocket.cpp
create date			:  2014-09-10
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
2014/09/17			CHENQ	enable create as client or server,support Linux&windows
*******************************************************************************/

#include "stdafx.h"
#include "XSocket.h"
#include "XUdpSocket.h"

#ifndef OS_WIN
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

CXUdpSocket::CXUdpSocket()
{
}


CXUdpSocket::~CXUdpSocket()
{
	Destroy();
}

BOOL CXUdpSocket::CreateClient(const sockaddr_in& inaddr)
{
	if (GetSock() != INVALID_SOCKET)
		return TRUE;
	_ASSERT(inaddr.sin_family==AF_INET);//TODO::IPv6;
	SOCKET fd = socket(inaddr.sin_family, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == fd)
		return FALSE;

	return Attach(fd);
}

BOOL CXUdpSocket::EnableBroadcast(BOOL bEnale) const
{
	return setsockopt(GetSock(), SOL_SOCKET, SO_BROADCAST, (char*)&bEnale, sizeof(BOOL))==0;
}

BOOL CXUdpSocket::SetMulitcastTTL(int ttl) const
{
	return  0 == setsockopt((int)m_sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl));
}

int CXUdpSocket::GetMulitcastTTL() const
{
	int ttl = 0;
	socklen_t retLen = 0;
	int ret = getsockopt((int)m_sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, &retLen);
	if (ret == -1)
	{
		return -1;
	}

	return ttl;
}

BOOL CXUdpSocket::JoinMultiGroup(ULONG multiAddr) const
{
	if (!IN_CLASSD(ntohl(multiAddr)))
	{
		return FALSE;
	}

	struct ip_mreq maddr = { { 0 }, };
	maddr.imr_multiaddr.s_addr = multiAddr;

	return 0==setsockopt(GetSock(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&maddr, sizeof(maddr));
}

BOOL CXUdpSocket::CreateServer(const sockaddr_in& inaddr, sockaddr_in* pOutAddr)
{
	SOCKET sock = GetSock();
	if (sock != INVALID_SOCKET)
		return TRUE;

	SOCKET fd = socket(inaddr.sin_family, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == fd)
		return FALSE;

	if (0 != bind(fd, (const sockaddr*)&inaddr, sizeof(sockaddr_in)))
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

ssize_t CXUdpSocket::SendTo(const sockaddr_in& remoteAddr, const void* pBuffer, size_t len) const
{
	SOCKET sock = GetSock();
	_ASSERT(sock != INVALID_SOCKET);

	int sockLen = sizeof(remoteAddr);
	return sendto(sock, (const char*)pBuffer, (int)len, 0, (const sockaddr*)&remoteAddr, sockLen);
}

ssize_t CXUdpSocket::RecvFrom(sockaddr_in& fromAddr, void* pBuffer, size_t bufLen) const
{
	SOCKET sock = GetSock();
	_ASSERT(sock != INVALID_SOCKET);
	socklen_t sockLen = sizeof(fromAddr);
	ssize_t rxLen = ::recvfrom(sock, (char*)pBuffer, bufLen, 0, (sockaddr*)&fromAddr, &sockLen);
	if (rxLen < 0)
	{
		errno_t errCode = CXSocket::GetLastSocketError();
#ifdef OS_WIN
		if (errCode==WSAETIMEDOUT)
		{
			return 0;
		}
#else
		if (errCode==EWOULDBLOCK || errCode== EAGAIN)
		{
			return 0;
		}
#endif
	}
	return rxLen;
}

BOOL CXUdpSocket::SetRxTimeOut(UINT nTimeOutMs)
{
	struct timeval tv = {0};
	tv.tv_sec = nTimeOutMs/1000;
	tv.tv_usec = nTimeOutMs%1000;
	SOCKET sock = GetSock();

	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0)
	{
		printf("socket option  SO_RCVTIMEO not support\n");
		return FALSE;
	}
	return TRUE;
}

ssize_t CXUdpSocket::WaitAndRecv(sockaddr_in& fromAddr, void* pBuffer, size_t bufLen, unsigned int timeOutMs) const
{
	SOCKET sock = GetSock();
	_ASSERT(sock != INVALID_SOCKET);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(sock, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000000;
#else //win & posix
	struct timeval tp = { 0 };
	tp.tv_sec = timeOutMs/1000;
	tp.tv_usec = (timeOutMs%1000)*1000;
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}

	int sel = select(sock + 1, &set, NULL, NULL, pTp);
	if (sel == 1)
	{
		socklen_t len = sizeof(fromAddr);
		return recvfrom(sock, (char*)pBuffer, bufLen, 0, (sockaddr*)&fromAddr, &len);
	}
	else if (sel == 0)
	{
		return 0;
	}

	return -1;
}

ssize_t CXUdpSocket::WaitForSend(unsigned int timeOutMs) const
{
	SOCKET sock = GetSock();
	_ASSERT(sock != INVALID_SOCKET);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(sock, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_nsec = (timeOutMs % 1000) * 1000000;
#else
	struct timeval tp = { 0 };
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000;
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}

	int sel = select(sock + 1, &set, NULL, NULL, pTp);
	return sel;
}

ssize_t CXUdpSocket::WaitAndSendTo(const sockaddr_in& fromAddr, const void* pBuffer, size_t bufLen, unsigned int timeOutMs) const
{
	SOCKET sock = GetSock();
	_ASSERT(sock != INVALID_SOCKET);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(sock, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_nsec = (timeOutMs % 1000) * 1000000;
#else
	struct timeval tp = { 0 };
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000;
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}

	int sel = select(sock + 1, NULL, &set, NULL, pTp);
	if (sel == 1)
	{
		int len = sizeof(fromAddr);
		return sendto(sock, (const char*)pBuffer, bufLen, 0, (const sockaddr*)&fromAddr, len);
	}
	else if (sel == 0)
	{
		return 0;
	}

	return -1;
}

void CXUdpSocket::Destroy()
{
	SOCKET sock = GetSock();
	if (INVALID_SOCKET == sock)
		return;

	Close();
	Detach();
}