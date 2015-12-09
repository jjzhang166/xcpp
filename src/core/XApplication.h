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

History:
2015/12/6 chenq change CSingletonAppPrivate To CSingletonApplicationPrivate

TODO:
*******************************************************************************/


#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "XSTLMacro.h"
/**
	@brief µ¥Àý½ø³Ì
	@detail 
*/

XCLASSPRIVATE_FORWARD_DECLARE(CSingletonApp);
class CSingletonApp
{
public:
	CSingletonApp(const TCHAR* pszAppKey=NULL);
	virtual ~CSingletonApp();

	BOOL Verify() const;

private:
	XCLASSPRIVATE_DECLARE(CSingletonApp);
	DISALLOW_COPY_AND_ASSIGN(CSingletonApp);
};

#endif /*__APPLICATION_H__*/