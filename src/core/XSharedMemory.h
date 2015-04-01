/*! XSharedMemory.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  A simple cross platform shared memory class
related files		:  XSharedMemory.cpp
create date			:  2015-01-20
author				:  CHENQ
version				:  0.0.1
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

history :
2015/1/20 CHENQ created
*******************************************************************************/
#ifndef __X_SHAREDMEMORY_H__
#define __X_SHAREDMEMORY_H__

#include "XSTLMacro.h"

XCLASSPRIVATE_FORWARD_DECLARE(CXShareMemory);
class CXShareMemory
{
public:
	enum AccessMode
	{
		AccessRO = 0,	/*< ReadOnly*/
		AccessRW,	/*< ReadWrite*/
	};

	enum ErrorCode
	{
		//TODO:::
	};

	explicit CXShareMemory(const TCHAR* pszMemKey);
	virtual ~CXShareMemory();

	BOOL Create(size_t sizeofMem, AccessMode mode = AccessRW);

	BOOL Attach(AccessMode mode = AccessRW);

	BOOL SetKey(const TCHAR* pszKey);

	BOOL Detach();

	BOOL IsAttached();

	BOOL Lock();

	BOOL UnLock();

	//GET functions
	inline size_t GetSize() const { return m_size; }
	inline XSTLString GetKey() const { return m_strKey; }
	inline AccessMode GetAccessMode() const { return m_mode; }

private:
	AccessMode m_mode;
	size_t m_size;
	XSTLString m_strKey;
	XCLASSPRIVATE_DECLARE(CXShareMemory);
};



#endif /*__X_SHAREDMEMORY_H__*/