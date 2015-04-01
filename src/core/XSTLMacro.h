#ifndef __XSTLMACRO_H__
#define __XSTLMACRO_H__ 

#include <string>
#include <list>

typedef std::wstring XSTLStringW;
typedef std::string XSTLStringA;
typedef std::list<XSTLStringA> XSTLStringListA;
typedef std::list<XSTLStringW> XSTLStringListW;

#ifdef UNICODE

#define XSTLString XSTLStringW
#define XSTLStringList XSTLStringListW

#else

#define XSTLStringList XSTLStringListA
#define XSTLString XSTLStringA


#endif


#endif /*__XSTLMACRO_H__*/