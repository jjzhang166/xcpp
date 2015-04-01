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
	@brief ����TCP ServerSocket
	@param [in]inaddr ��Ҫ�󶨵�IP��ַ�Ͷ˿ڣ�����ȫ��Ϊ0������˿�Ϊ0����ϵͳ���������һ��
	@param [out]outaddr ����ʵ�ʰ󶨵Ķ˿�
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL Create(const sockaddr_in& inaddr, sockaddr_in* outaddr);

	/**
	@brief ��ʼ����SOCKET
	@param [in]listenMax ��������
		�����ڲ���ϵͳʵ�֣�windows��linux��ͬ
		linuxĬ�����511��
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL Listen(size_t listenMax);

	/**
	 ������Ϊ��������!!!
	@brief �ȴ��Է�����
	@param [out]pRemoteaddr����Զ�����ӿͻ���IP�Ͷ˿�
	@return ����ϵͳ����ĺ�Զ�����ӵ�SOCKET
	*/
	SOCKET Accept(sockaddr_in* pRemoteaddr);
};

class CXTcpSessionSocket : public CXTcpSocket
{
public:
	/**
	@brief ����TCP�ͻ���SOCKET
	@param [in]inaddr inaddr.family����ΪAF_INET
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL Create(const sockaddr_in& inaddr);

	/**
		�������൱��Create + Connect
	@brief ����TCP�ͻ���SOCKET��������Զ�̷�����
	@param [in]inaddr Զ�̷�����IP�Ͷ˿�
	@return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	BOOL CreateWithConnect(const sockaddr_in& inaddr);

	/**
		Ĭ�������SOCKETΪ�������ģ������ݷ������ϴ�ʱ�����������ܻᷢ�Ͳ�����
	@brief �������ݵ�Զ������
	@param [in]pBuffer ���������ݻ�����
	@param [in]bufLen ����������
	@return ����ʵ�ʷ��͵��ֽ���
	*/
	ssize_t Send(const void* pBuffer, size_t bufLen) const;

	/**
		Ĭ�������SOCKETΪ�������ģ�������û�����ݽ���ʱ����������-1
	@brief ��Զ��������������
	@param [out]pBuffer �������ݻ�����
	@param [in]bufLen ����������
	@return ����ʵ�ʽ��յ����ݳ���
	*/
	ssize_t Recv(void* pBuffer, size_t bufLen) const;

	/**
	@brief 	�ȴ�����Զ��������������
	@param [out]pBuffer �������ݻ�����
	@param [in]bufLen ����������
	@param timeOutMs ��ʱ�Ժ���Ϊ��λ
	@return ����ʵ�ʽ��յ����ݳ���
	*/
	//�жϵ�ǰsocket�Ƿ���Է��ͣ�ֱ����ʱ������1���Է��ͣ�����0����ʱ,����Զ��SOCKET�����رգ�����-1 �������Զ��SOCKET�쳣�˳�
	ssize_t WaitAndRecv(void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;

	/**
	@brief 	�ȴ�����������
	@param [out]pBuffer �������ݻ�����
	@param [in]bufLen ����������
	@param timeOutMs ��ʱ�Ժ���Ϊ��λ
	@return ����ʵ�ʷ��͵����ݳ���
	*/
	ssize_t WaitAndSend(const void* pBuffer, size_t bufLen, unsigned int timeOutMs) const;

	/**
	@brief 	�ȴ��ɷ�������
	@param timeOutMs ��ʱ�Ժ���Ϊ��λ
	@return ����0��ʾ��ʱ������1��ʾ�ɹ�������-1 ʧ��
	*/
	ssize_t WaitForSend(unsigned int timeOutMs) const;

	/**
	@brief �ȴ���������
	@param timeOutMs ��ʱ�Ժ���Ϊ��λ
	@return ����0��ʾ��ʱ������1��ʾ�ɹ�������-1 ʧ��
	*/
	ssize_t WaitForRecv(unsigned int timeOutMs) const;
};

#ifdef XCPP_DEVELOP_CODE
#if X_VERSION_VALUE >= X_VERSION_MAKE(0,5,0)
/*����SOCKET ��*/
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
