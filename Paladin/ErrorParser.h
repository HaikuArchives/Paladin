#ifndef ERROR_PARSER_H
#define ERROR_PARSER_H

#include "ObjectList.h"

#include <Message.h>
#include <List.h>
#include <Locker.h>
#include <String.h>

enum {
	ERROR_MSG = 0,
	ERROR_WARNING = 1,
	ERROR_ERROR = 2
};

class error_msg
{
public:
			error_msg(void);
	void	PrintToStream(void);
	
	BString path;
	int32	line;
	BString	error;
	int8	type;
	BString	rawdata;
};

class ErrorList : public BLocker
{
public:
							ErrorList(const char *string = NULL);
							ErrorList(const ErrorList &from);
							~ErrorList(void);
			ErrorList &		operator=(const ErrorList &from);
			
			void			Append(const ErrorList &from);
			
			int32			CountWarnings(void);
			error_msg *		GetNextWarning(void);
			
			int32			CountErrors(void);
			error_msg *		GetNextError(void);
			
			error_msg *		GetNextItem(void);

			void			Rewind(void);
			
			void			Flatten(BMessage &msg);
			void			Unflatten(BMessage &msg);
	
	BObjectList<error_msg>	msglist;

private:
			int32		fIndex;
};

void	ParseGCCErrors(const char *string, ErrorList &list);
void	ParseLDErrors(const char *string, ErrorList &list);
void	ParseRCErrors(const char *string, ErrorList &list);
void	ParseLexErrors(const char *string, ErrorList &list);
void	ParseYaccErrors(const char *string, ErrorList &list);
void	ParseRezErrors(const char *string, ErrorList &list);
void	ParseIntoLines(const char *string, ErrorList &list);

#endif
