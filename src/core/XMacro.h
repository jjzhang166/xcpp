/*! XMarco.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform marco defines..
related files		:  none
create date			:  2014-09-20
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

#ifndef __XMARRCO_H__
#define __XMARRCO_H__

todo list:
1. ��ֹ�����麯��
2. ��ֹĬ�Ͽ������캯���͸�ֵ����


history :
2014/09/20			CHENQ	created
*******************************************************************************/
#ifndef __XMACRCO_H__
#define __XMACRCO_H__

/*** @brief _Print���庯��
	@detail
	_VFPRINTF��_VSPRINTF��_SPRINTF�ȱ�׼��������ڲ�ͬ�����¿����в�ͬ��ʵ�֡� \n
	Ϊ�˷�ֹԽ�����ʹ��ָ����������С�ĺ���snprintf�ȡ�
	�����ڲ�ͬƽ̨��size����˼��ͬ��Ϊ�˱�֤����Ľ�׳�Խ���,���뻺���������һ��
	char szBuffer[buffSize];
	_SPRINTF(szBufffer, sizeof(szBuffer)-1, pFmt,...)



*** @brief ����UNICODE��MBCS��ANSI


	1���ڱ�̵�ʱ��ʹ��TCHAR�������ͣ��������ܹ�����Ԥ�����Ķ��壬����ת��ΪANSI������Unicode��
	2��Ԥ�����_MBCS��_UNICODE��UNICODE��_MBCS�Ƕ��ֽں�ANSI�ַ����ı���ꡣ��ʱTCHAR��ת��Ϊchar��_UNICODE��UNICODE��Unicode�����Ԥ����꣬TCHAR��ת��Ϊwchar_t��
	3��_UNICODE��UNICODE��_MBCS�����ڱ����ʱ��ͬʱ�����塣
	4��_UNICODE������C�����ڿ��ͷ�ļ���UNICODE������Windowsͷ�ļ���һ��ͬʱ�����������ꡣ

*/

#ifdef __cplusplus
#define X_EXTERN_C extern "C"
#else
#define X_EXTERN_C extern
#endif

#define X_EOS '\0'
#define X_EOSW L'\0'
#define X_EOSA '\0'
										
#define XUINT_ROUNDUP(_uintnum, _align) ((((_uintnum)+((_align)-1))/(_align))*(_align))
#define XUINT_ROUNDDOWN(_uintnum, _align) ((((_uintnum)/(_align))*(_align))

#define XUINT_MEM_ROUNDUP(_uintnum, _align) (((_uintnum)+(_align))&(~((_align)-1)))
#define XUINT_MEM_ROUNDDOWN(_uintnum, _align) ((_uintnum)&(~((_align)-1)))

#define XUINT_VALUE_ROUNDUP(_uintnum, _align) ((((_uintnum)+((_align)-1))/(_align))*(_align))
#define XUINT_VALUE_ROUNDDOWN(_uintnum, _align) ((((_uintnum)/(_align))*(_align))


#define XE_CHAR_ANSI		0x01
#define XE_CHAR_UNICODE		0x02
#define XE_CHAR_MODE		(XE_CHAR_ANSI|XE_CHAR_UNICODE)

#define XE_DEFINE_XESTRINGFUNCA(__name) __name##A
#define XE_DEFINE_XESTRINGFUNCW(__name) __name##W
#define XE_DECLARE_XESTRINGFUNCA XE_DEFINE_XESTRINGFUNCA
#define XE_DECLARE_XESTRINGFUNCW XE_DEFINE_XESTRINGFUNCW

#ifdef OS_WIN

#define XStrCpyN lstrcpyn
#define XStrCpyNA lstrcpynA
#define XStrCpyNW lstrcpynW

//defines about UNICODE
#ifdef _UNICODE
#ifndef UNICODE
#error "You should define both UNICODE and _UNICODE in project"
//#define UNICODE         // UNICODE is used by Windows headers
#endif
#endif

#ifdef UNICODE
#ifndef _UNICODE
#error "You should define both UNICODE and _UNICODE in project"
//#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
#endif
#endif

//TODO: Unicode
#define SPRINTF_S sprintf_s
#define SSCANF_S sscanf_s
#define _VSPRINTF vsprintf_s
#define _VFPRINTF vfprintf_s
#define _VNFPRINTF _vsnprintf_s
#define _SPRINTF sprintf_s
#define X_SPRINTFA sprintf_s

/*
#warnning X_SPRINTFW �ڶ�������Ӧ�ü�ȥsizeof(wchar_t)
*/
#define X_SPRINTFW swprintf_s

#define _MKDIR _tmkdir

#define CHECK_ARGUMENT_KEY(argvalue, key) (lstrcmpi((argvalue), _T(key))==0)

#else //OS_WIN end non-win enable

#include "XEString.h"

#define SPRINTF_S snprintf
#define SSCANF_S sscanf
#define _VSPRINTF vsprintf
#define _VFPRINTF vfprintf
#define _VNFPRINTF vsnprintf
#define _SPRINTF snprintf
#define _MKDIR(_szpath) mkdir((_szpath), 0777)
#define _access access
#define _fileno fileno
#define _close close
#define _read read
#define _write write
#define _fdopen fdopen

#define _TCHAR char
#define _tmain main
#define CHECK_ARGUMENT_KEY(argvalue, key) (strcasecmp((argvalue), (key))==0)

#define X_SPRINTFA snprintf
#define X_SPRINTFW swprintf

#ifdef UNICODE
#define XStrCpyN XStrCpyNW
#else
#define XStrCpyN XStrCpyNA
#endif


#endif //end of OS_WIN


#ifndef LOCAL
#define LOCAL static
#endif

/*


*/

/**/
#ifdef __INC_XLOG
#define DBG_PRINT CXLog::PrintOnly
#endif

#ifdef __cplusplus

/*ѧϰQt*/

#define XCLASS_FORWARD_DECLARE(__class,__Ext) class __class##__Ext;
#define XCLASSPRIVATE_FORWARD_DECLARE(__class) XCLASS_FORWARD_DECLARE(__class,Private)

#define XOBJPTR_DECLARE(__type,__val) __type* __val
#define XCLASSPRIVATE_DECLARE(__class) XOBJPTR_DECLARE(__class##Private, m_pD);

#define XRIVATEOBJ_DECLARE(_class) 

#define XOBJPTR_CREAT(__type,__val) __type* __val = new (std::nothrow) __type()
#define XCLASSPRIVATE_CREATE(__class,__val) XOBJPTR_CREAT(__class##Private,__val)

#define XOBJPTR_CREAT_WITHPTR(__type,__val,__ptr) __type* __val = new (std::nothrow) __type(__ptr);
#define XCLASSPRIVATE_CREATE_INIT(__class, __val) XOBJPTR_CREAT_WITHPTR(__class##Private, __val, this)

#define XOBJPTR_DELETE(__val) delete __val
#define XCLASSPRIVATE_DELETE(__val) XOBJPTR_DELETE(__val)

#endif /*end of __cplusplus*/

#endif /*__XMACRCO_H__*/