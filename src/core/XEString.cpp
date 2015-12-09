//#include "XAfx.h"
#include "XAfx.h"

#if (XE_CHAR_MODE & XE_CHAR_ANSI)
#undef XE_CHAR_TYPEMODE
#define XE_CHAR_TYPEMODE A
#undef XE_CHAR_TYPE
#define XE_CHAR_TYPE char
#undef XE_DEFINE_XESTRINGFUNC
#define XE_DEFINE_XESTRINGFUNC XE_DEFINE_XESTRINGFUNCA

#undef XE_STRCHAR
#define XE_STRCHAR strchr
#undef XE_STRLEN
#define XE_STRLEN strlen
#undef XE_T
#define XE_T(__s) (__s)

#undef XE_SPRINTF 
#define XE_SPRINTF X_SPRINTFA

#include "XEString.hpp"
#endif


#if (XE_CHAR_MODE & XE_CHAR_UNICODE)
#undef XE_CHAR_TYPEMODE
#define XE_CHAR_TYPEMODE W
#undef XE_CHAR_TYPE
#define XE_CHAR_TYPE wchar_t

#undef XE_STRCHAR
#define XE_STRCHAR wcschr
#undef XE_STRLEN
#define XE_STRLEN wcslen
#undef XE_T
#define XE_T(__s) (L ## __s)

#undef XE_SPRINTF
#define XE_SPRINTF X_SPRINTFW

#undef XE_DEFINE_XESTRINGFUNC
#define XE_DEFINE_XESTRINGFUNC XE_DEFINE_XESTRINGFUNCW

#include "XEString.hpp"
#endif
