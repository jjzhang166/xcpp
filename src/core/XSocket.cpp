#include "stdafx.h"
#include "XSocket.h"
#include "XUtils.h"

#ifdef WINVER
#pragma comment(lib, "WS2_32")
#endif

#ifdef OS_WIN
class CSocketXInitOnce
{
public:
	CSocketXInitOnce()
	{
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 0), &info))
		{
			throw "Could not start WSA";
		}
	}
	~CSocketXInitOnce()
	{
		WSACleanup();
	}
};

static CSocketXInitOnce s_sockInitOnceObj;
#endif


void CXSocket::Init()
{
	//do nothing, to make sure linker link this file, to call CSocketXInitOnce' constructor
}

BOOL CXSocket::Connect(const sockaddr_in& remoteAddr) const
{
	_ASSERT(m_sockfd != INVALID_SOCKET);
	if (m_sockfd == INVALID_SOCKET)
		return FALSE;
	socklen_t len = sizeof(remoteAddr);
	return ::connect(m_sockfd, (const sockaddr*)&remoteAddr, len)==0 ? TRUE : FALSE;
}

BOOL CXSocket::SetRxBufferSize(size_t sizeInBytes) const
{
	_ASSERT(m_sockfd != INVALID_SOCKET);
	int rxLen = (int)sizeInBytes;
	int len = sizeof(rxLen);
	return 0 == setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, (const char *)&rxLen, len);
}

ssize_t CXSocket::GetTxBufferSize() const
{
	_ASSERT(m_sockfd != INVALID_SOCKET);
	int sizeInBytes = 0;
	socklen_t len = sizeof(sizeInBytes);
	getsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&sizeInBytes, &len);
	return sizeInBytes;
}

errno_t CXSocket::GetLastSocketError()
{
#ifdef OS_WIN
	return WSAGetLastError();
#elif defined(OS_VXWORKS)
	return errnoGet();
#else
	return errno;
#endif
}

ssize_t CXSocket::GetRxBufferSize() const
{
	_ASSERT(m_sockfd != INVALID_SOCKET);
	int sizeInBytes = 0;
	socklen_t len = sizeof(sizeInBytes);
	getsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&sizeInBytes, &len);
	return sizeInBytes;
}

#ifndef OS_WIN
BOOL CXSocket::BindToDevice(const char* pDeviceName) const
{
	_ASSERT(pDeviceName);
#ifdef OS_VXWORKS
	/*vxworks 网卡长度参数固定为16 vxworks version <=6.9*/
	int interfaceNameLen = 16;
#else
	int interfaceNameLen = strlen(pDeviceName);
#endif
	return  (pDeviceName!=NULL && setsockopt((int)m_sockfd, SOL_SOCKET, 
		SO_BINDTODEVICE, (char*)pDeviceName, interfaceNameLen) == 0);
}

int CXSocket::GetVr() const
{
#ifdef OS_VXWORKS
	int vr = 0;
	int recvLen = sizeof(vr);
	if (getsockopt((int)m_sockfd, SOL_SOCKET, SO_X_VR, (char*)&vr, &recvLen) < 0)
	{
		m_iErrorNo = errnoGet();
		return -1;
	}
	return vr;
#else
	return FALSE;
#endif
}

BOOL CXSocket::SetPortReuse(BOOL bTrue) const
{
#ifdef OS_VXWORKS
	return setsockopt((int)m_sockfd, SOL_SOCKET, SO_REUSEPORT, (char*)&bReuse, sizeof(BOOL));
#else
	//TODO::
	return FALSE;
#endif
}

BOOL CXSocket::SetVr(int vr) const
{
#ifdef OS_VXWORKS
	if (setsockopt((int)m_sockfd, SOL_SOCKET, SO_X_VR, (char*)&vr, sizeof(vr)) < 0)
	{
		m_iErrorNo = errnoGet();
		return FALSE;
	}
	return TRUE;
#else
	return FALSE;
#endif
}
#endif

BOOL CXSocket::GetSockName(sockaddr_in* pLocalAddr) const
{
	_ASSERT(pLocalAddr);
	socklen_t len = sizeof(sockaddr_in);
	return 0 == getsockname((int)m_sockfd, (sockaddr*)pLocalAddr, &len);
}

int CXSocket::GetTTL() const
{
	int ttl = 0;
	socklen_t retLen = 0;
	int ret = getsockopt((int)m_sockfd, IPPROTO_IP, IP_TTL, (char*)&ttl, &retLen);
	if (ret == -1)
	{
		return -1;
	}

	return ttl;
}

BOOL CXSocket::SetAddrReuse(BOOL bTrue) const
{
	BOOL bReuse = bTrue;
	return (setsockopt((int)m_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof(BOOL)) == 0);
}

BOOL CXSocket::SetTTL(int ttl) const
{
#ifdef OS_WIN
	//TODO:::
	UNUSED(ttl);
	return FALSE;
#else

#ifdef OS_VXWORKS
	UINT8 ttlValue = ttl;
	int ttlSize = sizeof(UINT8);
#else
	int ttlValue = ttl;
	int ttlSize = sizeof(ttl);

#endif
	int ret = setsockopt((int)m_sockfd, IPPROTO_IP, IP_TTL, (char*)&ttlValue, ttlSize);
	if (ret == -1)
	{
		//errnoGet();
		return FALSE;
	}

	return TRUE;
#endif
}

BOOL CXSocket::SetTxBufferSize(size_t sizeInBytes) const
{
	_ASSERT(m_sockfd != INVALID_SOCKET);
	int rxLen = (int)sizeInBytes;
	int len = sizeof(rxLen);
	return 0 == setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, (const char *)&rxLen, len);
}

BOOL CXSocket::Attach(SOCKET fd)
{
	_ASSERT(m_sockfd == INVALID_SOCKET);
	if (m_sockfd != INVALID_SOCKET)
		return FALSE;

	m_sockfd = fd;
	return TRUE;
}

//在windows下socket不支持_read/_write,考虑到兼容性recv/send
ssize_t CXSocket::Read(void* pBuffer, size_t bufLen) const
{
	return ::recv(m_sockfd, (char*)pBuffer, bufLen, 0);
}

ssize_t CXSocket::Write(const void* pBuffer, size_t dataLen) const
{
	return ::send(m_sockfd, (const char*)pBuffer, dataLen, 0);
}

void CXSocket::Detach()
{
	m_sockfd = INVALID_SOCKET;
}

static inline void Ms2Tv(UINT timeOutMs, struct timeval* pTv)
{
	pTv->tv_sec = timeOutMs / 1000;
	pTv->tv_usec = (timeOutMs % 1000) * 1000;
}

static inline UINT Tv2Ms(const struct timeval* pTv)
{
	return pTv->tv_sec * 1000 + pTv->tv_usec / 1000;
}

UINT CXSocket::WaitForSingleSocket(const SOCKET& sockFd, UINT timeOutMs)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockFd, &set);

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000000;
#else //win & posix
	struct timeval tp = { 0 };
	Ms2Tv(timeOutMs, &tp);
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}

#ifndef OS_WIN
__RETRY:
#endif

	int sel = select(sockFd+1, &set, NULL, NULL, pTp);
	if (sel == 0)
	{
		return WAIT_TIMEOUT;
	}
	else if (sel > 0)
	{
		if (FD_ISSET(sockFd, &set))
		{
			return WAIT_OBJECT_0;
		}
		return WAIT_FAILED;
	}
#ifndef OS_WIN
	//TODO::: windows 下也有可能返回
	//https://lists.secondlife.com/pipermail/jira-notify/2012-July/341563.html
	else if (CXSocket::GetLastSocketError() == EINTR)
	{
		goto __RETRY;
	}
#endif

	return WAIT_FAILED;
}


UINT CXSocket::WaitForMultiSocket(const SOCKET sockets[], UINT nCount, BOOL bWaitAll, UINT timeOutMs)
{
	SOCKET sockMax = 0;
	//SOCKET* pSock = (SOCKET*)alloca(sizeof(SOCKET)*nCount);
	//memcpy(pSock, sockets, sizeof(SOCKET)*nCount);

	//升序排列
	//qsort(pSock, nCount, sizeof(SOCKET), &socketCmp);

	fd_set set;
	FD_ZERO(&set);
	for (UINT i = 0; i < nCount; i++)
	{
		if (sockMax < sockets[i])
		{
			sockMax = sockets[i];
		}
		FD_SET(sockets[i], &set);
	}

#ifdef OS_VXWORKS
	struct timespec tp = { 0 };
	struct timespec* pTp = &tp;
	tp.tv_sec = timeOutMs / 1000;
	tp.tv_usec = (timeOutMs % 1000) * 1000000;
#else //win & posix
	struct timeval tp = { 0 };
	Ms2Tv(timeOutMs, &tp);
	struct timeval* pTp = &tp;
#endif
	if (timeOutMs == WAIT_FOREVER)
	{
		pTp = NULL;
	}
	UINT tickCountS = 0;
__RETRY:
	tickCountS = CXUtils::GetTickCount();
	
	int sel = select(sockMax + 1, &set, NULL, NULL, pTp);
	if (sel == 0)
	{
		return WAIT_TIMEOUT;
	}
	else if (sel > 0)
	{
		if (bWaitAll)
		{
			UINT tickCountC = CXUtils::GetTickCount();

			for (UINT i = 0; i < nCount; i++)
			{
				if (!FD_ISSET(sockets[i], &set))
				{
					if (pTp == NULL)
					{
						goto __RETRY;
					}
					else
					{
						UINT nPassed = (tickCountC - tickCountS);
						UINT nWaitMs = Tv2Ms(pTp);
						if (nPassed <= nWaitMs)
						{
							return WAIT_TIMEOUT;
						}
						else
						{
							nWaitMs -= nPassed;
							Ms2Tv(nWaitMs, pTp);
							goto __RETRY;
						}
					}
				}
			}
			return WAIT_OBJECT_0;
		}
		else
		{
			for (UINT i = 0; i < nCount; i++)
			{
				if (FD_ISSET(sockets[i], &set))
				{
					return WAIT_OBJECT_0 + i;
				}
			}
			return WAIT_FAILED;
		}
	}
#ifndef OS_WIN
	//TODO::: windows 下也有可能返回
	//https://lists.secondlife.com/pipermail/jira-notify/2012-July/341563.html
	else if (CXSocket::GetLastSocketError() == EINTR)
	{
		goto __RETRY;
	}
#endif

	return WAIT_FAILED;
}
