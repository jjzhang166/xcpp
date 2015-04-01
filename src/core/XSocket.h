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
1. socket ��дʱ��Ҫ��� EAGAIN��EWOULDBLOCK �� EINTER
	�첽ģʽ��ȥ��һ��û�����ݵ��������᷵��-1�������ô�����ΪEAGAIN
	EINTER �����룬��ǰ����IO��ϵͳ���ô�ϣ���accept�������Ӵ��select����ʱselect����0
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
#define UDP_RX_BUF_MAX	262142U	//linuxϵͳ�����õ����ֵ

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
		Socket_NoBlock = 0,	/**< ������SOCKET		*/
		Socket_Block,		/**< ����SOCKET			*/
	};

	/* @brief ��ʼ��SOCKET����
	* ��ʼ��SOCKETʹ�û���
	* @deprecated �������ܻ��Ƴ�
	*/
	static void Init();

	/**
	* @brief ���ص�ǰSOCKET������
	*/
	static errno_t GetLastSocketError();

	CXSocket() : m_sockfd(INVALID_SOCKET){}
	virtual ~CXSocket(){}

	/**
	* @brief���ӵ�Զ��SOCKET
	* @param [in] remoteAddr Զ��SOCKET IPv4��IP�Ͷ˿�
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE \n
	* ���������ͨ��@see GetLastSocketError()
	*/
	BOOL Connect(const sockaddr_in& remoteAddr) const;

	/**
	* @brief�ر�SOCKET
	*/
	void Close() { closesocket(m_sockfd); }

	/**
	* @brief��ȡ���ջ�������С
	* @return �ɹ����ػ�������С\n
		ʧ�ܷ���-1 @see GetLastSocketError()
	*/
	ssize_t GetTxBufferSize() const;

	/**
	* @brief��ȡ���ͻ�������С
	* @return �ɹ����ػ�������С\n
	ʧ�ܷ���-1 @see GetLastSocketError
	*/
	ssize_t GetRxBufferSize() const;

	/**
	* @brief��ȡ���ͻ�������С
	* @return �ɹ����ػ�������С\n
	* ʧ�ܷ���-1 @see GetLastSocketError
	* @see SetRxBufferSize
	*/

	/**
	* @brief���÷��ͻ�������С
	* @return �ɹ����ػ�������С\n
	*	ʧ�ܷ���-1 @see GetLastSocketError
	* @see GetRxBufferSize
	*/
	BOOL SetRxBufferSize(size_t sizeInBytes) const;

	/**
	* @brief���÷��ͻ�������С
	* @return �ɹ����ػ�������С\n
	* ʧ�ܷ���-1 @see GetLastSocketError()
	*  @see GetRxBufferSize
	*/
	BOOL SetTxBufferSize(size_t sizeInBytes) const;

	/**
	@brief ��ȡSOCKET TTL
	@return ���ص�ǰSOCKET�� TTL������ʧ�ܷ���-1���ɹ����ص�ǰ��TTL
	@see SetTTL
	*/
	int GetTTL() const;

	/**
	@brief ����SOCKET TTL
	@param [in]ttl Ҫ���õ�TTL��ע�ⰴ��bsd socket�淶TTL�ķ�Χ��[1,255]
	@return ���õ�ǰSOCKET�� TTL������ʧ�ܷ���FALSE���ɹ����ص�ǰ��TRUE
	@see GetTTL
	*/
	BOOL SetTTL(int ttl) const;

	/**
	@brief ����SOCKET Addr Reuse (TCP&UDP)

	SO_REUSEADDR��������������������¡� (ժ�ԡ�Unix�����̡���һ����UNPv1) \n
    1������һ������ͬ���ص�ַ�Ͷ˿ڵ�socket1����TIME_WAIT״̬ʱ�������� \n
		���ĳ����socket2Ҫռ�øõ�ַ�Ͷ˿ڣ���ĳ����Ҫ�õ���ѡ� \n
    2��SO_REUSEADDR����ͬһport������ͬһ�������Ķ��ʵ��(�������)����
		ÿ��ʵ���󶨵�IP��ַ�ǲ�����ͬ�ġ����ж����������IP Alias������ \n
		�������Բ������������ 
    3��SO_REUSEADDR���������̰���ͬ�Ķ˿ڵ����socket�ϣ���ÿ��socket \n
		�󶨵�ip��ַ��ͬ�����2�����ƣ������뿴UNPv1�� 
    4��SO_REUSEADDR������ȫ��ͬ�ĵ�ַ�Ͷ˿ڵ��ظ��󶨡�����ֻ����UDP�Ķಥ��
		������TCP�� (UDP Only)

	@param [in]bTrue TRUE �������ã�FALSE��ֹ����
	@return ����ʧ�ܷ���FALSE���ɹ����ص�ǰ��TRUE
	*/
	BOOL SetAddrReuse(BOOL bTrue) const;

#ifndef OS_WIN
	/**
		@brief��ȡ��ǰ����·�ɺ�
		@return ��ǰ·�ɺ�
	*/
	int GetVr() const;

	/**
		@brief���õ�ǰ����·�ɺ�
		@param [in] vr ·�ɺ�
	*/
	BOOL SetVr(int vr) const;

	/**
	@brief ���ö˿�����\n
		windows�²�֧�ָú�����SetAddrReuse���

	@param [in]bTrue TRUE �������ã�FALSE��ֹ����
	@return ����ʧ�ܷ���FALSE���ɹ����ص�ǰ��TRUE
	*/
	BOOL SetPortReuse(BOOL bTrue) const;

	/**
	@brief ��SOCKET��ָ���������豸��
		ʹ�û�����
		1. ��ϵͳ�ж����������������������
		2�����BIND�������ַSOCKET�������������п�������ʧ��
			���ȡ����Ĭ�����أ��Ͳ���ϵͳָ��������
		3. �ú�����Ҫӵ��rootȨ��

	@param [in]pDeviceName �󶨵���������
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL BindToDevice(const char* pDeviceName) const;
#endif

	/**
	@brief ��ȡһ���׽��ֵ�����

		������һ����������������׽���s�����ص�ַ�������� \n
		�������ر����������������δ����bind()�͵�����connect()��\n
		��ʱΨ��getsockname()���ÿ��Ի�֪ϵͳ�ڶ��ı��ص�ַ��\n
		�ڷ���ʱ��namelen�������������ֵ�ʵ���ֽ���
	@param [out]pLocalAddr ���ص�sockaddr_in����ָ��
	@return ����ʧ�ܷ���FALSE���ɹ����ص�ǰ��TRUE
	*/
	BOOL GetSockName(sockaddr_in* pLocalAddr) const;

	/**
	@brief ����һ��SOCKET��
		ֻ�и�������ȷSOCKET֮����ĺ����ſ��Ե��óɹ�
		һ��CXSocket����ֻ�ܸ���һ��SOCKET\n
		���Ѿ�������һ��socket���ٴε��ñ������᷵��FALSE
		�����Ҫ�����µ�socket���ȵ���
	@param [in] fd �����ӵ�SOCKET��֧��TCP/UDP��SCOKET
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	@see Detach  GetSock
	*/
	BOOL Attach(SOCKET fd);

	/**
	@brief �����ǰ���ӵ�SOCKET\n
		���Attach
	@see Attach
	*/
	void Detach();

	/**
	@brief��ȡSOCKET����
	@param [out] pBuffer ����SOCKET���ݵĻ�����
	@param [in] bufLen ������pBuffer�Ĵ�С
	@return �ɹ���ȡ�����ݵĳ���,����ֵ���з��ŵĲ������bufLen
			ʧ�ܷ���-1
	@see Write
	*/
	virtual ssize_t Read(void* pBuffer, size_t bufLen) const;

	/**
	@brief����SOCKET����
	@param [in] pBuffer ������SOCKET���ݵĻ�����
	@param [in] bufLen ������pBuffer�Ĵ�С
	@return ���سɹ����͵��ֽ���
		ʧ�ܷ���-1����С��dataLen
	@see Read
	*/
	virtual ssize_t Write(const void* pBuffer, size_t dataLen) const;

	//virtual BOOL Connect(const sockaddr_in& addr) = 0;

	/**
	@brief �ȴ�����������ɶ���������Windows��WaitForMultiObjects

	@param [in] sockets �ȴ���������ָ��
	@param [in] nCount sockets��������Ԫ�صĸ���
	@param [in] bWaitAll �Ƿ�ȴ�ȫ������
	@param [in] nTimeOutMs ��ȴ�ʱ���Ժ���Ϊ��λ�����nTimeOutMsΪWAIT_FOREVER�����õȴ�
	@return bWaitAllΪTRUEʱ��ֻ�е��������������������߳�ʱ�˲Ż᷵��\n
				����ֵ WAIT_OBJECT_0 ����ȫ������\n
				����ֵ WAIT_TIMEOUT ������ʱ\n
				����ֵ WAIT_FAILED ��������\n
			bWaitAllΪFALSEʱ��ֻҪ���κ�SOCKET���������߳�ʱ�����򷵻�
				����ֵ [WAIT_OBJECT_0 ~ WAIT_OBJECT_0+nCount-1] ����SOCKET������ֵ��WAIT_OBJECT_0�ĺ� \n
				����ֵ WAIT_TIMEOUT ������ʱ\n
				����ֵ WAIT_FAILED ��������\n
	@deprecated ������������Read���뺯�����Ʋ�̫���У�δ�����޸�
	*/
	static UINT WaitForMultiSocket(const SOCKET sockets[], UINT nCount, BOOL bWaitAll, UINT nTimeOutMs);

	static UINT WaitForSingleSocket(const SOCKET& sockets, UINT nTimeOutMs);

	/*
	*  @brief���ص�ǰSOCKET�Ƿ���Ч
	*  @return ��Ч����TRUE����Ч����FALSE
	*/
	inline BOOL IsValid() const { return m_sockfd != INVALID_SOCKET; }

	/*
	*  @brief���ص�ǰSOCKET
	*  @return ��ǰSOCKET��INVALID_SOCKET��ʾδ����SOCKET
	*  @see Attach
	*/
	inline SOCKET GetSock() const { return m_sockfd; }

protected:
	SOCKET m_sockfd;
};



#endif  /*__XSOCKET_H__*/