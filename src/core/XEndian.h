#ifndef __XENDIAN_H__
#define __XENDIAN_H__

/*! XList.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  handle byte endian of XCPP
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com  
--------------------------------------------------------------------------------

history :
2014/09/10			CHENQ	created


TODO:
1. use cpu code to increase performance

*******************************************************************************/


#ifndef __BYTE_ORDER
#error "unknown platform"
#endif

#define __swap16(x) \
	((UINT16)(\
	(((UINT16)(x)& (UINT16)0x00ffU) << 8) | \
	(((UINT16)(x)& (UINT16)0xff00U) >> 8)))

#define __swap32(x) \
	((UINT32)(\
	(((UINT32)(x)& (UINT32)0x000000ff) << 24) | \
	(((UINT32)(x)& (UINT32)0x0000ff00) << 8) | \
	(((UINT32)(x)& (UINT32)0x00ff0000) >> 8) | \
	(((UINT32)(x)& (UINT32)0xff000000) >> 24)))

#define __swap64(ll) ((UINT64)(((ll) >> 56) | \
	(((ll)& 0x00ff000000000000) >> 40) | \
	(((ll)& 0x0000ff0000000000) >> 24) | \
	(((ll)& 0x000000ff00000000) >> 8) | \
	(((ll)& 0x00000000ff000000) << 8) | \
	(((ll)& 0x0000000000ff0000) << 24) | \
	(((ll)& 0x000000000000ff00) << 40) | \
	(((ll) << 56))))

#define betoh32x htobe32x
#define betoh16x htobe16x
#define betoh64x htobe64x

#if _BYTE_ORDER==_LITTILE_ENDIAN
#define htole32x(val) (val)
#define htole16x(val) (val)

#define htobe32x(val) __swap32(val)
#define htobe16x(val) __swap16(val)
#define htobe64x(val) __swap64(val)

#else

#define htole32x(val) __swap32(val)
#define htole16x(val) __swap16(val)

#define htobe32x(val)
#define htobe16x(val)
#endif

#endif