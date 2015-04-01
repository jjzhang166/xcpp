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
	@brief ʹ��UDP�㲥
	@param [in] bEnale TRUEΪ������FALSEΪ�ر�
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL EnableBroadcast(BOOL bEnale) const;

	/**
	@brief ����㲥��
	@param [in] multiAddr �ಥ��ַ
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL JoinMultiGroup(ULONG multiAddr) const;

	/**
	@brief ���õ�ǰSOCKET�ಥTTL
	@param [in] ttl ���÷�Χ[1,255]
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	@see GetMulitcastTTL
	*/
	BOOL SetMulitcastTTL(int ttl) const;

	/**
	@brief ��ȡ��ǰSOCKET�ಥTTL
	@return ����TTLֵ���������-1��ʾʧ��
	@see SetMulitcastTTL
	*/
	int GetMulitcastTTL() const;

	/**
		Ŀǰ�ú��������н���af_family���ã��ұ���ΪAF_INET
		�����ʹ����������

	@brief ����һ��UDP�ͻ���
	@param inaddr ���뱾��SOCKET��ַ��
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	@see CreateServer  Destroy
	*/
	BOOL CreateClient(const sockaddr_in& inaddr);

	/**
	
		@brief ����һ��UDP Server ��SOCKET
		@param [in]inaddr Server SOCKET IP��port ����ȫΪ0
		@param [out]���������� inaddrȫΪ0����ϵͳ���������һ��UDP�˿ںţ���������������˿�
		@return �ɹ�����TRUE��ʧ�ܷ���FALSE
		@see CreateClient  Destroy
	*/
	BOOL CreateServer(const sockaddr_in& inaddr, sockaddr_in* outaddr);

	/**
	@brief ����UDP Socket��Server/Client)
	@param [in]inaddr Server SOCKET IP��port ����ȫΪ0
	@param [out]���������� inaddrȫΪ0����ϵͳ���������һ��UDP�˿ںţ���������������˿�
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	@see CreateClient  CreateServer
	*/
	void Destroy();

	/**
	@brief ����UDP���ճ�ʱ
	@param [in]nTimeOutMs �Ժ���Ϊ��λ�ĳ�ʱ��
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	@see CreateClient  CreateServer
	*/
	BOOL SetRxTimeOut(UINT nTimeOutMs);

	/**
	@brief ����udp��Ϣ
	@param [out]fromAddr Զ��addr
	@param [out]pBuffer �������ݻ�����
	@param [in]bufLen pBuffer��������С
	@return �ɹ�����ʵ�ʽ��յ����ֽ�����-1��ʾʧ�ܣ�0��ʾ��ʱ
	@see CreateClient  CreateServer
	*/
	ssize_t RecvFrom(sockaddr_in& fromAddr, void* pBuffer, size_t bufLen) const;

	/**
	@brief ����UDP Socket��Server/Client)
	@param [in]inaddr Server SOCKET IP��port ����ȫΪ0
	@param [out]���������� inaddrȫΪ0����ϵͳ���������һ��UDP�˿ںţ���������������˿�
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	@see CreateClient  CreateServer
	*/
	ssize_t SendTo(const sockaddr_in& remoteAddr, const void* pBuffer, size_t len) const;

	/**

	@brief �ȴ�����������
	@param [out]fromAddr ���ؽ��ܵ����ݵ�IP�Ͷ˿�
	@param [out]pBuffer �������ݻ�����
	@param [in] bufLen �������ݻ�����pBuffer��С
	@param [in] timeOutMs �ȴ���ʱ
	@return ���ؽ��յ������ݳ��ȣ�ʧ�ܷ���-1
	@see CreateClient  CreateServer GetLastSockError
	*/
	ssize_t WaitAndRecv(sockaddr_in& fromAddr, void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;

	/**

	@brief ��ȡ���;���״̬
	@param [in] timeOutMs �ȴ���ʱ
	@return ���ؽ��յ������ݳ��ȣ�ʧ�ܷ���-1
	@see CreateClient  CreateServer GetLastSockError
	@deprecated
	*/
	ssize_t WaitForSend(unsigned int timeOutMs) const;

	/**
	@brief �ȴ�ֱ���������
		@param[in] timeOutMs �ȴ���ʱ
		@return ����ʵ�ʷ������ݳ��ȣ�����ֵ==bufLen ��ʾ�ɹ�������ֵʧ��
		@see WaitAndRecv
	*/
	ssize_t WaitAndSendTo(const sockaddr_in& oAddr, const void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;
};

#endif

