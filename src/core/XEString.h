/*! XEString.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform Enhance c string
related files		:  XEString.cpp
create date			:  2015-03-10
author				:  CHENQ
version				:	v0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2015/03/10			CHENQ	created from substring.h strproc.h

/TODO: 
*******************************************************************************/

#ifndef __XESTRING_H__
#define __XESTRING_H__


#ifdef __cplusplus
extern "C" {
#endif

#if (XE_CHAR_MODE & XE_CHAR_ANSI)

#ifndef OS_WIN
	/***@brief ���ַ���ת��Ϊ��д
	@param pszStr[in] ����ʽ�����ַ���
	@return �����׵�ַ	
	*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrUpr)(char* pszStr);

	/***@brief ���ַ���ת��ΪСд
	@param pszStr[in] ����ʽ�����ַ���
	@return �����׵�ַ
	*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrLwr)(char* pszStr);

	/***@brief	�����ַ���
	@param pszDst[out]	ָ��Ŀ�Ļ�����
	@param pszSrc[out]	ָ��Ҫ���������ַ���������
	@param cchMax[in]	��󿽱����ֽ����������ս��ַ�����'\0',\n �����������ͨ������ΪpszDst�Ļ�������С
	void XStrReplace(char* pTxt, char cFrom, char cTo)*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrCpyN)(char* pszDst, const char* pszSrc, int cchMax);
#endif /*ENDIF OF OS_WIN*/
	/***@brief	�滻�ַ����е�ָ���ַ�
	@param pTxt[in out]	Ҫ���滻�ַ���
	@param cFrom[out]	ָ�����滻���ַ�
	@param cTo[in]		ָ���滻���ַ�
	@return				����pTxt,���������ʹ�ñ�������Ϊ����
	char* XStrReplace(char* pTxt, char cFrom, char cTo)*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrReplace)(IN OUT char* pTxt, char cFrom, char cTo);

	/***@brief ��ȥ�ַ�����ǰ���ͽ�β�Ŀո�
	@param pIn[in out]	�����ַ�������
	@param ppOut[out]	����ָ�룬�ں���ִ����󣬸�ָ��ָ�������ַ����׵�ַ
	void strPick(char* pIn, char** ppOut)*/
	void XE_DEFINE_XESTRINGFUNCA(XStrPick)(IN char* pIn, OUT char** ppOut);
	#define XStrPickA(pIn, ppOut)		*(ppOut) = XStrTrimA(pIn)

	/***@brief ��ȥ�ַ�����ǰ���ͽ�β�Ŀո�
	@param pIn[in out]	�����ַ�������
	@return	����ָ�룬�ں���ִ����󣬸�ָ��ָ�������ַ����׵�ַ*/
	char* XE_DECLARE_XESTRINGFUNCA(XStrTrim)(IN OUT char* pIn);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ǰ���ͽ�β�Ŀո�
	@param pString[in]
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCA(XStrSplit)(IN char* pString, OUT char** ppSubs, int iSubNum, char cDelimit);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int strSplitSubEx(char* pString, char** ppSubs, int iSubNum, char cDelimit); 
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitEx)(IN char* pString, OUT char** ppSubs, int iSubNum, char cDelimit);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�,֧��ָ�����ַָ���
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out] ���뻺����(���ᱻ�����б��޸ģ�
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int strSplitSubMdEx(char* pString, char** ppSubs, int iSubNum, const char cDelimits[], int iDelimits);
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitMd)(char* pString, char** ppSubs, int iSubNum, const char cDelimits[], int iDelimits);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�,֧��ָ�����ַָ���
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out] ���뻺����(���ᱻ�����б��޸ģ�
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitMdEx)(char* pString, char** ppSubs, int iSubNum, const char cDelimits[], int iDelimits);

	/** @brief ��һ���ַ������տո�(' '����'\t')��������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�,֧��ָ�����ַָ���
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out] ָ�����ص�ָ������(���ᱻ�����б��޸ģ�
	@param iSubNum[in]	ppSubs��ָ�������С
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitBySpace)(char* pString, char** ppSubs, int iSubNum);

	/** @brief ��һ����������ʽ��Ϊָ�����ȵ��ַ���
	@param value[in] value Ҫ����ʽ���ĸ�����
	@param iPrecision[in] ����׼ȷ��С����λ��
	@param pszBuffer[out]	��ʽ�����ַ���������
	@param iBufSize[int]	��ʽ�����ַ�������������
	@return	��ʽ�����ַ����׵�ַ*/
	char* XE_DEFINE_XESTRINGFUNCA(XFormatFloat)(double value, int iPrecision, OUT char* pszBuffer, int iBufSize);

#endif /*ENDIF XE_CHAR_MODE&XE_CHAR_ANSI */

#if (XE_CHAR_MODE & XE_CHAR_UNICODE)

#ifndef OS_WIN
	/***@brief ���ַ���ת��Ϊ��д
	@param pszStr[in] ����ʽ�����ַ���
	@return �����׵�ַ	
	*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrUpr)(wchar_t* pszStr);

	/***@brief ���ַ���ת��ΪСд
	@param pszStr[in] ����ʽ�����ַ���
	@return �����׵�ַ
	*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrLwr)(wchar_t* pszStr);

	/***@brief	�����ַ���
	@param pszDst[out]	ָ��Ŀ�Ļ�����
	@param pszSrc[out]	ָ��Ҫ���������ַ���������
	@param cchMax[in]	��󿽱����ֽ����������ս��ַ�����'\0',\n �����������ͨ������ΪpszDst�Ļ�������С
	void XStrReplace(char* pTxt, char cFrom, char cTo)*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrCpyN)(wchar_t* pszDst, const wchar_t* pszSrc, int cchMax);
#endif /*ENDIF OF OS_WIN*/


	/***@brief	�滻�ַ����е�ָ���ַ�
	@param pTxt[in out]	Ҫ���滻�ַ���
	@param cFrom[out]	ָ�����滻���ַ�
	@param cTo[in]		ָ���滻���ַ�
	void XStrReplace(char* pTxt, char cFrom, char cTo)*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrReplace)(IN OUT wchar_t* pTxt, wchar_t cFrom, wchar_t cTo);

	/***@brief ��ȥ�ַ�����ǰ���ͽ�β�Ŀո�
	@param pIn[in out]	�����ַ�������
	@param ppOut[out]	����ָ�룬�ں���ִ����󣬸�ָ��ָ�������ַ����׵�ַ
	void strPick(char* pIn, char** ppOut)*/
	void XE_DEFINE_XESTRINGFUNCW(XStrPick)(IN wchar_t* pIn, OUT wchar_t** ppOut);
	#define XStrPickW(pIn, ppOut)		*(ppOut) = XStrTrimW(pIn)

	/***@brief ��ȥ�ַ�����ǰ���ͽ�β�Ŀո�
	@param pIn[in out]	�����ַ�������
	@return	����ָ�룬�ں���ִ����󣬸�ָ��ָ�������ַ����׵�ַ*/
	wchar_t* XE_DECLARE_XESTRINGFUNCW(XStrTrim)(IN OUT wchar_t* pIn);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ǰ���ͽ�β�Ŀո�
	@param pString[in]
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplit)(IN wchar_t* pString, OUT wchar_t** ppSubs, int iSubNum, wchar_t cDelimit);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitEx)(IN wchar_t* pString, OUT wchar_t** ppSubs, int iSubNum, wchar_t cDelimit);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�,֧��ָ�����ַָ���
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out] ���뻺����(���ᱻ�����б��޸ģ�
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitMd)(wchar_t* pString, wchar_t** ppSubs, int iSubNum, const wchar_t cDelimits[], int iDelimits);

	/** @brief ��һ���ַ�����������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�,֧��ָ�����ַָ���
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out] ���뻺����(���ᱻ�����б��޸ģ�
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitMdEx)(wchar_t* pString, wchar_t** ppSubs, int iSubNum, const wchar_t cDelimits[], int iDelimits);

	/** @brief ��һ���ַ������տո�(' '����'\t')��������ɸ��Ӵ�,����ȥǰ���ͽ�β�Ŀո�,֧��ָ�����ַָ���
	@param pString[in] ���뻺����(���ᱻ�����б��޸ģ�
	@param ppSubs[in out] ָ�����ص�ָ������(���ᱻ�����б��޸ģ�
	@param iSubNum[in]	ppSubs��ָ�������С
	@return	�������ַ����ĸ������û�ָ������ĿС��ʵ���ִ���Ŀ���򷵻��û�ָ��������;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitBySpace)(wchar_t* pString, wchar_t** ppSubs, int iSubNum);

	/** @brief ��һ����������ʽ��Ϊָ�����ȵ��ַ���
	@param value[in] value Ҫ����ʽ���ĸ�����
	@param iPrecision[in] ����׼ȷ��С����λ��
	@param pszBuffer[out]	��ʽ�����ַ���������
	@param iBufSize[int]	��ʽ�����ַ�������������
	@return	��ʽ�����ַ����׵�ַ*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XFormatFloat)(double value, int iPrecision, OUT wchar_t* pszBuffer, int iBufSize);

#endif /*ENDIF XE_CHAR_MODE&XE_CHAR_UNICODE */

#ifdef __cplusplus
}
#endif

#ifdef UNICODE
#define XStrUpr XStrUprW
#define XStrLwr XStrLwrW
#define XStrSplit XStrSplitW
#define XStrSplitEx XStrSplitExW
#define XStrSplitMd XStrSplitMdW
#define XStrSplitMdEx XStrSplitMdExW
#else
#define XStrUpr XStrUprA
#define XStrLwr XStrLwrA
#define XStrSplit XStrSplitA
#define XStrSplitEx XStrSplitExA
#define XStrSplitMd XStrSplitMdA
#define XStrSplitMdEx XStrSplitMdExA
#endif

#endif /*__XESTRING_H__*/