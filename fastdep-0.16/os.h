#ifndef __OS_H__
#define __OS_H__

#ifdef WIN32
static const char  cPathSep = '\\' ;
static const char* sPathSep = "\\" ;
#else
static const char  cPathSep = '/' ;
static const char* sPathSep = "/" ;
#endif

#endif // __OS_H__
