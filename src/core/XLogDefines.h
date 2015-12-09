/*! XUtils.h
********************************************************************************
description			:  Define data structure of XLOG
related files		:  XLog.cpp
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created
*******************************************************************************/

#ifndef __XLOG_DEFINES_H__
#define __XLOG_DEFINES_H__

struct xlog_header
{
	char szHdrFlag[4];
	UINT32 endianFlag;
	UINT32 codecFlag;
	UINT32 res[5];
};//32B

typedef struct xlog_header XLOG_HDR;

struct xlog_item
{
	UINT32 logLevel;
	UINT32 nTimeHigh;
	UINT32 nTimeLow;
	char szDat[256-12];
};//256B

typedef struct xlog_item XLOG_ITEM;


#endif /*__XLOG_DEFINES_H__*/