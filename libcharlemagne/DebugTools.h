#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

#include <stdio.h>

#define ENABLE_DEBUGTOOLS

extern unsigned long gDebugLevel;

#define TRACE_CREATE	1
#define	TRACE_DESTROY	2

#ifdef ENABLE_DEBUGTOOLS
	#define STRACE(x,y) if (gDebugLevel & y){ printf x; }
#else
	#define STRACE(x,y) /* */
#endif

#endif
