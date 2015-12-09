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
	/***@brief 将字符串转化为大写
	@param pszStr[in] 被格式化的字符串
	@return 返回首地址	
	*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrUpr)(char* pszStr);

	/***@brief 将字符串转化为小写
	@param pszStr[in] 被格式化的字符串
	@return 返回首地址
	*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrLwr)(char* pszStr);

	/***@brief	拷贝字符串
	@param pszDst[out]	指向目的缓冲区
	@param pszSrc[out]	指向要被拷贝的字符串缓冲区
	@param cchMax[in]	最大拷贝的字节数，包含终结字符串的'\0',\n 所以这个参数通常设置为pszDst的缓冲区大小
	void XStrReplace(char* pTxt, char cFrom, char cTo)*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrCpyN)(char* pszDst, const char* pszSrc, int cchMax);
#endif /*ENDIF OF OS_WIN*/
	/***@brief	替换字符串中的指定字符
	@param pTxt[in out]	要被替换字符串
	@param cFrom[out]	指定被替换的字符
	@param cTo[in]		指定替换的字符
	@return				返回pTxt,方便调用这使用本函数作为参数
	char* XStrReplace(char* pTxt, char cFrom, char cTo)*/
	char* XE_DEFINE_XESTRINGFUNCA(XStrReplace)(IN OUT char* pTxt, char cFrom, char cTo);

	/***@brief 截去字符串的前导和结尾的空格
	@param pIn[in out]	输入字符缓冲区
	@param ppOut[out]	输入指针，在函数执行完后，该指针指向处理后的字符串首地址
	void strPick(char* pIn, char** ppOut)*/
	void XE_DEFINE_XESTRINGFUNCA(XStrPick)(IN char* pIn, OUT char** ppOut);
	#define XStrPickA(pIn, ppOut)		*(ppOut) = XStrTrimA(pIn)

	/***@brief 截去字符串的前导和结尾的空格
	@param pIn[in out]	输入字符缓冲区
	@return	输入指针，在函数执行完后，该指针指向处理后的字符串首地址*/
	char* XE_DECLARE_XESTRINGFUNCA(XStrTrim)(IN OUT char* pIn);

	/** @brief 将一个字符串分离成若干个子串,保留前导和结尾的空格
	@param pString[in]
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCA(XStrSplit)(IN char* pString, OUT char** ppSubs, int iSubNum, char cDelimit);

	/** @brief 将一个字符串分离成若干个子串,并截去前导和结尾的空格
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int strSplitSubEx(char* pString, char** ppSubs, int iSubNum, char cDelimit); 
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitEx)(IN char* pString, OUT char** ppSubs, int iSubNum, char cDelimit);

	/** @brief 将一个字符串分离成若干个子串,并截去前导和结尾的空格,支持指定多种分隔符
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out] 输入缓冲区(将会被函数中被修改）
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int strSplitSubMdEx(char* pString, char** ppSubs, int iSubNum, const char cDelimits[], int iDelimits);
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitMd)(char* pString, char** ppSubs, int iSubNum, const char cDelimits[], int iDelimits);

	/** @brief 将一个字符串分离成若干个子串,并截去前导和结尾的空格,支持指定多种分隔符
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out] 输入缓冲区(将会被函数中被修改）
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitMdEx)(char* pString, char** ppSubs, int iSubNum, const char cDelimits[], int iDelimits);

	/** @brief 将一个字符串按照空格(' '或者'\t')分离成若干个子串,并截去前导和结尾的空格,支持指定多种分隔符
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out] 指定返回的指针数组(将会被函数中被修改）
	@param iSubNum[in]	ppSubs中指针数组大小
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCA(XStrSplitBySpace)(char* pString, char** ppSubs, int iSubNum);

	/** @brief 将一个浮点数格式化为指定精度的字符串
	@param value[in] value 要被格式化的浮点数
	@param iPrecision[in] 精度准确到小数后位数
	@param pszBuffer[out]	格式化的字符串缓冲区
	@param iBufSize[int]	格式化的字符串缓冲区长度
	@return	格式化的字符串首地址*/
	char* XE_DEFINE_XESTRINGFUNCA(XFormatFloat)(double value, int iPrecision, OUT char* pszBuffer, int iBufSize);

#endif /*ENDIF XE_CHAR_MODE&XE_CHAR_ANSI */

#if (XE_CHAR_MODE & XE_CHAR_UNICODE)

#ifndef OS_WIN
	/***@brief 将字符串转化为大写
	@param pszStr[in] 被格式化的字符串
	@return 返回首地址	
	*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrUpr)(wchar_t* pszStr);

	/***@brief 将字符串转化为小写
	@param pszStr[in] 被格式化的字符串
	@return 返回首地址
	*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrLwr)(wchar_t* pszStr);

	/***@brief	拷贝字符串
	@param pszDst[out]	指向目的缓冲区
	@param pszSrc[out]	指向要被拷贝的字符串缓冲区
	@param cchMax[in]	最大拷贝的字节数，包含终结字符串的'\0',\n 所以这个参数通常设置为pszDst的缓冲区大小
	void XStrReplace(char* pTxt, char cFrom, char cTo)*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrCpyN)(wchar_t* pszDst, const wchar_t* pszSrc, int cchMax);
#endif /*ENDIF OF OS_WIN*/


	/***@brief	替换字符串中的指定字符
	@param pTxt[in out]	要被替换字符串
	@param cFrom[out]	指定被替换的字符
	@param cTo[in]		指定替换的字符
	void XStrReplace(char* pTxt, char cFrom, char cTo)*/
	wchar_t* XE_DEFINE_XESTRINGFUNCW(XStrReplace)(IN OUT wchar_t* pTxt, wchar_t cFrom, wchar_t cTo);

	/***@brief 截去字符串的前导和结尾的空格
	@param pIn[in out]	输入字符缓冲区
	@param ppOut[out]	输入指针，在函数执行完后，该指针指向处理后的字符串首地址
	void strPick(char* pIn, char** ppOut)*/
	void XE_DEFINE_XESTRINGFUNCW(XStrPick)(IN wchar_t* pIn, OUT wchar_t** ppOut);
	#define XStrPickW(pIn, ppOut)		*(ppOut) = XStrTrimW(pIn)

	/***@brief 截去字符串的前导和结尾的空格
	@param pIn[in out]	输入字符缓冲区
	@return	输入指针，在函数执行完后，该指针指向处理后的字符串首地址*/
	wchar_t* XE_DECLARE_XESTRINGFUNCW(XStrTrim)(IN OUT wchar_t* pIn);

	/** @brief 将一个字符串分离成若干个子串,保留前导和结尾的空格
	@param pString[in]
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplit)(IN wchar_t* pString, OUT wchar_t** ppSubs, int iSubNum, wchar_t cDelimit);

	/** @brief 将一个字符串分离成若干个子串,并截去前导和结尾的空格
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out]
	@param iSubNum[in]
	@param cDelimit
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitEx)(IN wchar_t* pString, OUT wchar_t** ppSubs, int iSubNum, wchar_t cDelimit);

	/** @brief 将一个字符串分离成若干个子串,并截去前导和结尾的空格,支持指定多种分隔符
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out] 输入缓冲区(将会被函数中被修改）
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitMd)(wchar_t* pString, wchar_t** ppSubs, int iSubNum, const wchar_t cDelimits[], int iDelimits);

	/** @brief 将一个字符串分离成若干个子串,并截去前导和结尾的空格,支持指定多种分隔符
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out] 输入缓冲区(将会被函数中被修改）
	@param iSubNum[in]
	@param cDelimits[in]
	@param iDelimits[in]
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitMdEx)(wchar_t* pString, wchar_t** ppSubs, int iSubNum, const wchar_t cDelimits[], int iDelimits);

	/** @brief 将一个字符串按照空格(' '或者'\t')分离成若干个子串,并截去前导和结尾的空格,支持指定多种分隔符
	@param pString[in] 输入缓冲区(将会被函数中被修改）
	@param ppSubs[in out] 指定返回的指针数组(将会被函数中被修改）
	@param iSubNum[in]	ppSubs中指针数组大小
	@return	返回子字符串的个数，用户指定的数目小于实际字串数目，则返回用户指定返回数;*/
	int XE_DEFINE_XESTRINGFUNCW(XStrSplitBySpace)(wchar_t* pString, wchar_t** ppSubs, int iSubNum);

	/** @brief 将一个浮点数格式化为指定精度的字符串
	@param value[in] value 要被格式化的浮点数
	@param iPrecision[in] 精度准确到小数后位数
	@param pszBuffer[out]	格式化的字符串缓冲区
	@param iBufSize[int]	格式化的字符串缓冲区长度
	@return	格式化的字符串首地址*/
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