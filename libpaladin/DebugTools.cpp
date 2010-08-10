#include "DebugTools.h"
#include <stdio.h>
#include <Alert.h>
#include <iostream>
#include <String.h>
#include <Errors.h>

int gPrintDebugMode = 0;

void DebugAlert(const char *msg, const char *title)
{
#ifdef USE_TRACE_TOOLS
	(new BAlert(title, msg ,"OK",NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT))->Go();
#endif
}

void PrintStatus(status_t value)
{
#ifdef USE_TRACE_TOOLS
	// Function which simply translates a returned status code into a string
	// and dumps it to stdout
	BString outstr;
	switch(value)
	{
		// General errors
		case B_OK:
			outstr="B_OK ";
			break;
		case B_NO_MEMORY:
			outstr="B_NO_MEMORY ";
			break;
		case B_IO_ERROR:
			outstr="B_IO_ERROR ";
			break;
		case B_PERMISSION_DENIED:
			outstr="B_PERMISSION_DENIED ";
			break;
		case B_BAD_INDEX:
			outstr="B_BAD_INDEX ";
			break;
		case B_BAD_TYPE:
			outstr="B_BAD_TYPE ";
			break;
		case B_BAD_VALUE:
			outstr="B_BAD_VALUE ";
			break;
		case B_MISMATCHED_VALUES:
			outstr="B_MISMATCHED_VALUES ";
			break;
		case B_NAME_NOT_FOUND:
			outstr="B_NAME_NOT_FOUND ";
			break;
		case B_NAME_IN_USE:
			outstr="B_NAME_IN_USE ";
			break;
		case B_TIMED_OUT:
			outstr="B_TIMED_OUT ";
			break;
		case B_INTERRUPTED:
			outstr="B_INTERRUPTED ";
			break;
		case B_WOULD_BLOCK:
			outstr="B_WOULD_BLOCK ";
			break;
		case B_CANCELED:
			outstr="B_CACNCELED ";
			break;
		case B_NO_INIT:
			outstr="B_NO_INIT ";
			break;
		case B_BUSY:
			outstr="B_BUSY ";
			break;
		case B_NOT_ALLOWED:
			outstr="B_NOT_ALLOWED ";
			break;
		case B_ERROR:
			outstr="B_ERROR ";
			break;

		// Kernel Kit errors
		case B_BAD_SEM_ID:
			outstr="B_BAD_SEM_ID ";
			break;
		case B_NO_MORE_SEMS:
			outstr="B_NO_MORE_SEMS ";
			break;
		case B_BAD_THREAD_ID:
			outstr="B_BAD_THREAD_ID ";
			break;
		case B_NO_MORE_THREADS:
			outstr="B_NO_MORE_THREADS ";
			break;
		case B_BAD_THREAD_STATE:
			outstr="B_BAD_THREAD_STATE ";
			break;
		case B_BAD_TEAM_ID:
			outstr="B_BAD_TEAM_ID ";
			break;
		case B_NO_MORE_TEAMS:
			outstr="B_NO_MORE_TEAMS ";
			break;
		case B_BAD_PORT_ID:
			outstr="B_BAD_PORT_ID ";
			break;
		case B_NO_MORE_PORTS:
			outstr="B_NO_MORE_PORTS ";
			break;
		case B_BAD_IMAGE_ID:
			outstr="B_BAD_IMAGE_ID ";
			break;
		case B_BAD_ADDRESS:
			outstr="B_BAD_ADDRESS ";
			break;
		case B_NOT_AN_EXECUTABLE:
			outstr="B_NOT_AN_EXECUTABLE ";
			break;
		case B_MISSING_LIBRARY:
			outstr="B_MISSING_LIBRARY ";
			break;
		case B_MISSING_SYMBOL:
			outstr="B_MISSING_SYMBOL ";
			break;
		case B_DEBUGGER_ALREADY_INSTALLED:
			outstr="B_DEBUGGER_ALREADY_INSTALLED ";
			break;

		// Storage Kit Errors
		case B_FILE_ERROR:
			outstr="B_FILE_ERROR";
			break;
		case B_FILE_NOT_FOUND:
			outstr="B_FILE_NOT_FOUND ";
			break;
		case B_ENTRY_NOT_FOUND:
			outstr="B_ENTRY_NOT_FOUND ";
			break;
		case B_NAME_TOO_LONG:
			outstr="B_NAME_TOO_LONG";
			break;
		case B_NOT_A_DIRECTORY:
			outstr="B_NOT_A_DIRECTORY ";
			break;
		case B_DIRECTORY_NOT_EMPTY:
			outstr="B_DIRECTORY_NOT_EMPTY ";
			break;
		case B_DEVICE_FULL:
			outstr="B_DEVICE_FULL ";
			break;
		case B_READ_ONLY_DEVICE:
			outstr="B_READ_ONLY_DEVICE ";
			break;
		case B_IS_A_DIRECTORY:
			outstr="B_IS_A_DIRECTORY ";
			break;
		case B_NO_MORE_FDS:
			outstr="B_NO_MORE_FDS";
			break;
		case B_CROSS_DEVICE_LINK:
			outstr="B_CROSS_DEVICE_LINK ";
			break;
		case B_LINK_LIMIT:
			outstr="B_LINK_LIMIT ";
			break;
		case B_BUSTED_PIPE:
			outstr="B_BUSTED_PIPE ";
			break;
		case B_UNSUPPORTED:
			outstr="B_UNSUPPORTED ";
			break;
		case B_PARTITION_TOO_SMALL:
			outstr="B_PARTITION_TOO_SMALL ";
			break;
		
		// Application Kit errors
		case B_BAD_REPLY:
			outstr="B_BAD_REPLY ";
			break;
		case B_DUPLICATE_REPLY:
			outstr="B_DUPLICATE_REPLY ";
			break;
		case B_MESSAGE_TO_SELF:
			outstr="B_MESSAGE_TO_SELF ";
			break;
		case B_BAD_HANDLER:
			outstr="B_BAD_HANDLER ";
			break;
		case B_ALREADY_RUNNING:
			outstr="B_ALREADY_RUNNING ";
			break;
		case B_LAUNCH_FAILED:
			outstr="B_LAUNCH_FAILED ";
			break;
		case B_AMBIGUOUS_APP_LAUNCH:
			outstr="B_AMBIGUOUS_APP_LAUNCH ";
			break;
		case B_UNKNOWN_MIME_TYPE:
			outstr="B_UNKNOWN_MIME_TYPE ";
			break;
		case B_BAD_SCRIPT_SYNTAX:
			outstr="B_BAD_SCRIPT_SYNTAX ";
			break;
		case B_LAUNCH_FAILED_NO_RESOLVE_LINK:
			outstr="B_LAUNCH_FAILED_NO_RESOLVE_LINK ";
			break;
		case B_LAUNCH_FAILED_EXECUTABLE:
			outstr="B_LAUNCH_FAILED_EXECUTABLE ";
			break;
		case B_LAUNCH_FAILED_APP_NOT_FOUND:
			outstr="B_LAUNCH_FAILED_APP_NOT_FOUND ";
			break;
		case B_LAUNCH_FAILED_APP_IN_TRASH:
			outstr="B_LAUNCH_FAILED_APP_IN_TRASH ";
			break;
		case B_LAUNCH_FAILED_NO_PREFERRED_APP:
			outstr="B_LAUNCH_FAILED_NO_PREFERRED_APP ";
			break;
		case B_LAUNCH_FAILED_FILES_APP_NOT_FOUND:
			outstr="B_LAUNCH_FAILED_FILES_APP_NOT_FOUND ";
			break;
		case B_BAD_MIME_SNIFFER_RULE:
			outstr="B_BAD_MIME_SNIFFER_RULE ";
			break;
		
		default:
			outstr="undefined status value in debugtools::PrintStatus() ";
			break;
	}
	printf("Status: %s\n",outstr.String());
#endif
}

void PrintStatusToStream(status_t value)
{
#ifdef USE_TRACE_TOOLS
	// Function which simply translates a returned status code into a string
	// and dumps it to stdout
	BString outstr;
	switch(value)
	{
		case B_OK:
			outstr="B_OK ";
			break;
		case B_NAME_NOT_FOUND:
			outstr="B_NAME_NOT_FOUND ";
			break;
		case B_BAD_VALUE:
			outstr="B_BAD_VALUE ";
			break;
		case B_ERROR:
			outstr="B_ERROR ";
			break;
		case B_TIMED_OUT:
			outstr="B_TIMED_OUT ";
			break;
		case B_NO_MORE_PORTS:
			outstr="B_NO_MORE_PORTS ";
			break;
		case B_WOULD_BLOCK:
			outstr="B_WOULD_BLOCK ";
			break;
		case B_BAD_PORT_ID:
			outstr="B_BAD_PORT_ID ";
			break;
		case B_BAD_TEAM_ID:
			outstr="B_BAD_TEAM_ID ";
			break;
		default:
			outstr="undefined status value in debugtools::PrintStatusToStream() ";
			break;
	}
	printf("Status: %s\n",outstr.String());
#endif
}

void PrintMessageCode(int32 code)
{
#ifdef USE_TRACE_TOOLS
	// Used to translate BMessage message codes back to a character
	// format

	printf("BMessage '%c%c%c%c'\n",(char)((code & 0xFF000000) >>  24),
		(char)((code & 0x00FF0000) >>  16),
		(char)((code & 0x0000FF00) >>  8),
		(char)((code & 0x000000FF)) );
#endif
}
