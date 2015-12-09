/*! XDlist.h
********************************************************************************
                          #    #  ####  #####  #####  
                           #  #  #    # #    # #    # 
                            ##   #      #    # #    # 
                            ##   #      #####  #####  
                           #  #  #    # #      #      
                          #    #  ####  #      #      
--------------------------------------------------------------------------------
description			:  Doubly linked list data structure
create date			:  2014-11-28
author				:  CHENQ
version				:	---
--------------------------------------------------------------------------------
If you find any bugs, please e-mail me chennqqi@qq.com  
--------------------------------------------------------------------------------

history :
2014/11/28			CHENQ	created

TODO:	
1. macros			not finished
*******************************************************************************/

#ifndef __XDLIST_H__
#define __XDLIST_H__

struct xdlist_node
{
	struct xdlist_node* pPre;
	struct xdlist_node* pNext;
};

struct xdlist_list
{
	xdlist_node* pHead;	/*指向头结点*/
	xdlist_node* pTail;	/*指向尾节点*/
};

typedef struct xdlist_list X_DLIST;
typedef struct xdlist_node X_DNODE;

#define XDLIST_INIT(_pList) \
do \
{ \
	(_pList)->pHead = NULL; \
	(_pList)->pTail = NULL; \
} while (FALSE)

#define XDLIST_NEXT(_pNode)  (_pNode)->pNext
#define XDLIST_PRE(_pNode) (_pNode)->pPre

#define XDLIST_FIRST(_pList) (_pList)->pHead

#define XDLIST_IS_END(_pNode) ((_pNode)==NULL)
#define XDLIST_ISNOT_END(_pNode) ((_pNode)!=NULL)
#define XDLIST_IS_EMPTY(_pList) ((_pList)->pTail==NULL)


#define XDLIST_INSERT_HEAD(_pList,_pNode) \
do \
{ \
	(_pNode)->pNext = (_pList)->pNext; \
	(_pList)->pNext = (_pNode); \
} while (FALSE)

#define XDLIST_INSERT(_pList, _pPreNode, _pNewNode) \
do \
{ \
	/*SAVE _pNode->pNext*/X_DNODE* pNext = NULL; \
	X_DNODE* pPre = (_pPreNode); \
	if (NULL==pPre) \
	{ \
		pNext = (_pList)->pHead; \
		(_pList)->pHead = (_pNewNode); \
	} \
	else \
	{ \
		pNext = (_pPreNode)->pNext; \
		(_pPreNode)->pNext = (_pNewNode); \
	} \
	if (pNext == NULL) \
	{ \
		(_pList)->pTail = (_pNewNode); \
	} \
	else \
	{ \
		pNext->pPre = (_pNewNode); \
	} \
	(_pNewNode)->pNext = pNext; \
	(_pNewNode)->pPre = pPre; \
} while (FALSE)

#define XDLIST_ADD(_pList, _pNode) \
do \
{ \
	XDLIST_INSERT((_pList), (_pList)->pTail, (_pNode)); \
} while (FALSE)

#define XDLIST_REMOVE(_pList,_pNode) \
do \
{ \
	X_DNODE* pPre = (_pNode)->pPre; \
	X_DNODE* pNext = (_pNode)->pNext; \
	if (NULL==pPre) \
	{ \
		(_pList)->pHead = pNext; \
	} \
	else \
	{ \
		pPre->pNext = pNext; \
	} \
	if (NULL != pNext) \
	{ \
		pNext->pPre = pPre; \
	} \
	else \
	{ \
		(_pList)->pTail = pPre; \
	} \
}while (FALSE)

#define XDLIST_REMOVE_FIRST(_pList)  \
do \
{ \
	XDLIST_REMOVE(_pList, (_pList)->pHead); \
} while (FALSE)


#endif /*__XLIST_H__*/