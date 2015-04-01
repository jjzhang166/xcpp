/*! Application
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
create date			:  2014-09-10
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com
--------------------------------------------------------------------------------

TODO:
1. will be abolished in next version
2. singleton process
*******************************************************************************/


#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "XUdpSocket.h"


/**
	@brief µ¥Àý½ø³Ì
	@detail 
*/
#include <string>

class CSingletonAppliaionObj;
class CSingletonAppliaion
{
public:
	CSingletonAppliaion(const char* pszAppKey=NULL);
	virtual ~CSingletonAppliaion();

	BOOL Verify() const;

private:
	std::string m_strKey;
	CSingletonAppliaionObj* m_Obj;
};

#endif /*__APPLICATION_H__*/