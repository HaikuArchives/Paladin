#ifndef _DEBUGTOOLS_H_
#define _DEBUGTOOLS_H_

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <stdio.h>

/*
	Debug Levels:
		1	Basic build and project information
		2	Project loading and saving and more detailed build/project info
*/

extern int gPrintDebugMode;

#ifndef STRACE

#define USE_TRACE_TOOLS

#ifdef USE_TRACE_TOOLS
	#define STRACE(x,y) if (gPrintDebugMode >= x) printf y
#else
	#define STRACE(x,y) /* */
#endif

#endif // ifndef STRACE

void DebugAlert(const char *msg, const char *title="Debug");
void PrintStatusToStream(status_t value);
void PrintStatus(status_t value);
void PrintColorSpaceToStream(color_space value);
void PrintMessageCode(int32 code);
void TranslateMessageCodeToStream(int32 code);

#endif
