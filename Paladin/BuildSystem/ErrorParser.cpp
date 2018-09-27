#include "ErrorParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <OS.h>

error_msg::error_msg(void)
	:	line(-1),
		type(-1)
{
}

void
error_msg::PrintToStream(void)
{
	BString out;
	if (type == ERROR_MSG)
		out << "MSG: ";
	else if (type == ERROR_WARNING)
		out << "WARNING: ";
	else
		out << "ERROR: ";
	
	if (path.CountChars() > 0)
		out << path << ": ";
	
	if (line >= 0)
		out << line << ": ";
	
	out << error << "\n";
	
	printf("%s\n",out.String());
}


ErrorList::ErrorList(const char *string)
	:	msglist(20,true),
		fIndex(0)
{
}


ErrorList::ErrorList(const ErrorList &from)
	:	msglist(20,true),
		fIndex(0)
{
	*this = from;
}


ErrorList::~ErrorList(void)
{
}


ErrorList &
ErrorList::operator=(const ErrorList &from)
{
	msglist.MakeEmpty();
	Append(from);
	return *this;
}


void
ErrorList::Append(const ErrorList &from)
{
	for (int32 i = 0; i < from.msglist.CountItems(); i++)
	{
		error_msg *msg = (error_msg*)from.msglist.ItemAt(i);
		error_msg *newmsg = new error_msg;
		*newmsg = *msg;
		msglist.AddItem(newmsg);
	}
}


int32
ErrorList::CountWarnings(void)
{
	int32 count = 0;
	for (int32 i = 0; i < msglist.CountItems(); i++)
	{
		error_msg *msg = (error_msg*)msglist.ItemAt(i);
		if (msg->type == ERROR_ERROR)
			continue;
		if (msg->type == ERROR_NOTE)
			continue;
		if (msg->type == ERROR_UNKNOWN)
			continue;
		
		if (msg->type == ERROR_MSG)
		{
			int32 index = i + 1;
			
			error_msg *next = (error_msg*)msglist.ItemAt(index);
			while (next && next->type == ERROR_MSG)
			{
				index++;
				next = (error_msg*)msglist.ItemAt(index);
			}
			
			if (next && next->type == ERROR_ERROR)
				continue;
		}
		count++;
	}
	return count;
}


error_msg *
ErrorList::GetNextWarning(void)
{
	error_msg *msg = NULL;
	for (int32 i = fIndex; i < msglist.CountItems(); i++)
	{
		error_msg *item = (error_msg*)msglist.ItemAt(i);
		
		if (item->type == ERROR_ERROR)
			continue;
		if (item->type == ERROR_NOTE)
			continue;
		if (item->type == ERROR_UNKNOWN)
			continue;
		
		if (item->type == ERROR_MSG)
		{
			int32 index = i + 1;
			
			error_msg *next = (error_msg*)msglist.ItemAt(index);
			while (next && next->type == ERROR_MSG)
			{
				index++;
				next = (error_msg*)msglist.ItemAt(index);
			}
			
			if (next && next->type == ERROR_ERROR)
				continue;
		}
		msg = item;
		fIndex = i + 1;
		break;
	}
	return msg;
}


int32
ErrorList::CountErrors(void)
{
	int32 count = 0;
	for (int32 i = 0; i < msglist.CountItems(); i++)
	{
		error_msg *msg = (error_msg*)msglist.ItemAt(i);
		if (msg->type == ERROR_ERROR)
			count++;
	}
	return count;
}


error_msg *
ErrorList::GetNextError(void)
{
	error_msg *msg = NULL;
	for (int32 i = fIndex; i < msglist.CountItems(); i++)
	{
		error_msg *item = (error_msg*)msglist.ItemAt(i);
		
		if (item->type == ERROR_WARNING)
			continue;
		if (item->type == ERROR_NOTE)
			continue;
		if (item->type == ERROR_UNKNOWN)
			continue;
		
		if (item->type == ERROR_MSG)
		{
			int32 index = i + 1;
			
			error_msg *next = (error_msg*)msglist.ItemAt(index);
			while (next && next->type == ERROR_MSG)
			{
				index++;
				next = (error_msg*)msglist.ItemAt(index);
			}
			
			if (next && next->type == ERROR_WARNING)
				continue;
		}
		msg = item;
		fIndex = i + 1;
		break;
	}
	return msg;
}


error_msg *
ErrorList::GetNextItem(void)
{
	return (error_msg*)msglist.ItemAt(fIndex++);
}


void
ErrorList::Rewind(void)
{
	fIndex = 0;
}


void
ErrorList::Flatten(BMessage &msg)
{
	msg.MakeEmpty();
	for (int32 i = 0; i < msglist.CountItems(); i++)
	{
		error_msg *error = (error_msg*)msglist.ItemAt(i);
		msg.AddString("path",error->path);
		msg.AddInt32("line",error->line);
		msg.AddString("error",error->error);
		msg.AddInt8("type",error->type);
		msg.AddString("rawdata",error->rawdata);
	}
}


void
ErrorList::Unflatten(BMessage &msg)
{
	msglist.MakeEmpty();
	Rewind();
	
	type_code code;
	int32 count;
		
	// There will always be raw data in a valid error list
	if (msg.GetInfo("rawdata",&code,&count) != B_OK || count < 1)
		return;
	
	for (int32 i = 0; i < count; i++)
	{
		error_msg *error = new error_msg;
		if (msg.FindString("rawdata",i,&error->rawdata) != B_OK)
		{
			delete error;
			return;
		}
		
		// Certain cases will have an invalid path, such as the beginning of the
		// second line of a multiline "undefined reference to..." ld error
		if (msg.FindString("path",i,&error->path) != B_OK)
			error->path = "";
		
		if (msg.FindInt32("line",i,&error->line) != B_OK)
			error->line = -1;
		
		if (msg.FindString("error",i,&error->error) != B_OK)
			error->error = "";
		
		// every error_msg item MUST have a type
		if (msg.FindInt8("type",i,&error->type) != B_OK)
		{
			delete error;
			return;
		}
		msglist.AddItem(error);
	}
}


BString
ErrorList::AsString(void)
{
	BString out;
	for (int32 i = 0; i < msglist.CountItems(); i++)
	{
		error_msg *item = msglist.ItemAt(i);
		out << item->rawdata << "\n";
	}
	
	return out;
}


void
ParseGCCErrors(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;
	
	int32 length = strlen(string);
	if (length < 1)
		return;
	
	char *data = new char[length + 1];
	sprintf(data,"%s",string);
	
	char *item = strtok(data,"\n");
	while (item)
	{
		error_msg *msg = new error_msg;
		msg->rawdata = item;
		list.msglist.AddItem(msg);
		item = strtok(NULL,"\n");
	}
	
	int8 errorPrev = ERROR_UNSET;
	for (int32 i = 0; i < list.msglist.CountItems(); i++)
	{
		error_msg *msg = (error_msg*)list.msglist.ItemAt(i);
		
		if (msg->rawdata.CountChars() < 1)
			continue;
		
		int32 startpos = 0;
		int32 endpos = msg->rawdata.FindFirst(":");
		msg->rawdata.CopyInto(msg->path,startpos,endpos);
		
		// Now we have to do a little fancy guesswork
		if (isdigit(msg->rawdata[endpos + 1]))
		{
			startpos = endpos;
			
			BString temp;
			int32 numberIndex = startpos + 1;
			while (isdigit(msg->rawdata[numberIndex]))
			{
				temp += msg->rawdata[numberIndex];
				numberIndex++;
				if (numberIndex >= msg->rawdata.CountChars())
					break;
			}
			msg->line = atol(temp.String());
			endpos += temp.CountChars();
		}
		else
			msg->line = -1;
		
		// adding 2 instead of one because there is always a space after the final 
		// colon in the event there is an error message, which is usually
		if (endpos + 2 < msg->rawdata.CountChars()) {
			msg->error = msg->rawdata.String() + endpos + 2;
		}
		
		/*if ((msg->line < 0 && msg->error.IFindFirst("error") < 0) ||
			(msg->error.CountChars() < 1)) {
			if (-1 != errorPrev) {
				msg->type = errorPrev;
			} else {
				msg->type = ERROR_MSG;
			}
		} else*/ if (msg->rawdata.IFindFirst("warning:") >= 0) {
			msg->type = ERROR_WARNING;
		} else if (msg->rawdata.IFindFirst("error:") >= 0) {
			msg->type = ERROR_ERROR;
		} else {
			//msg->type = ERROR_UNKNOWN;
			// if not known, mark as previous or unknown
			if (ERROR_UNSET != errorPrev) {
				msg->type = errorPrev;
			} else {
				msg->type = ERROR_UNKNOWN;
			}
		}
			
		errorPrev = msg->type;
	}
	
	delete [] data;
}


void
ParseLDErrors(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;

	int32 length = strlen(string);
	if (length < 1)
		return;
	
	char *data = new char[length + 1];
	sprintf(data,"%s",string);
	
	char *item = strtok(data,"\n");
	while (item)
	{
		error_msg *msg = new error_msg;
		msg->rawdata = item;
		list.msglist.AddItem(msg);
		item = strtok(NULL,"\n");
	}
	
	int8 errorPrev = ERROR_UNSET;
	for (int32 i = 0; i < list.msglist.CountItems(); i++)
	{
		error_msg *msg = (error_msg*)list.msglist.ItemAt(i);
		
		if (msg->rawdata.CountChars() < 1)
			continue;
		
		int32 startpos = 0;
		int32 endpos = msg->rawdata.FindFirst(":");
		if (endpos > startpos)
			msg->rawdata.CopyInto(msg->path,startpos,endpos);
		
		// The linker doesn't use line numbers
		msg->line = -1;
		
		// adding 2 instead of one because there is always a space after the final colon
		msg->error = msg->rawdata.String() + endpos + 2;
		
		if (msg->error.IFindFirst("warning:") >= 0) {
			msg->type = ERROR_WARNING;
		} else if (msg->error.IFindFirst("error:") >= 0) {
			msg->type = ERROR_ERROR;
		} else if (msg->error.IFindFirst("undefined") >= 0) {
			msg->type = ERROR_ERROR;
		} else {
			if (ERROR_UNSET != errorPrev) {
				msg->type = errorPrev;
			} else {
				msg->type = ERROR_NOTE;
			}
		}
			
		errorPrev = msg->type;
	}
	delete [] data;
}


void
ParseRCErrors(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;

	int32 length = strlen(string);
	if (length < 1)
		return;
	
	debugger("ParseRCErrors unimplemented");
}


void
ParseLexErrors(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;

	int32 length = strlen(string);
	if (length < 1)
		return;
	
	debugger("ParseRCErrors unimplemented");
}


void
ParseYaccErrors(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;

	int32 length = strlen(string);
	if (length < 1)
		return;
	
	debugger("ParseRCErrors unimplemented");
}


void
ParseRezErrors(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;

	int32 length = strlen(string);
	if (length < 1)
		return;
	
	char *data = new char[length + 1];
	sprintf(data,"%s",string);
	
	char *item = strtok(data,"\n");
	error_msg *msg = NULL;
	while (item)
	{
		if (strstr(item,"### Rez") != item && strstr(item,"#----") != item)
		{
			msg = new error_msg;
			msg->rawdata = item;
			list.msglist.AddItem(msg);
		}
		item = strtok(NULL,"\n");
	}
	
	for (int32 i = 0; i < list.msglist.CountItems(); i++)
	{
		msg = (error_msg*)list.msglist.ItemAt(i);
		
		if (msg->rawdata.CountChars() < 1)
			continue;
		
		int32 startpos = 0;
		int32 endpos = 0;
		
		if (msg->rawdata.FindFirst("File \"") == 0)
		{
			startpos = 6;
			endpos = msg->rawdata.FindFirst("\"",startpos);
			msg->rawdata.CopyInto(msg->path,startpos,endpos - startpos);
			
			startpos = msg->rawdata.FindFirst("\";");
			if (startpos > 0)
			{
				startpos++;
				startpos = msg->rawdata.IFindFirst("Line ");
				if (startpos > 0)
					msg->line = atol(msg->rawdata.String() + startpos + 5);
			}
		}
		else
		{
			msg->error = msg->rawdata;
			msg->line = 0;
		}
		
		
		if (msg->line < 0 && msg->error.IFindFirst("error") < 0)
			msg->type = ERROR_MSG;
		else if (msg->error.IFindFirst("warning:") >= 0)
			msg->type = ERROR_WARNING;
		else if (msg->error.IFindFirst("note:") >= 0)
			msg->type = ERROR_NOTE;
		else if (msg->error.IFindFirst("error:") >= 0)
			msg->type = ERROR_ERROR;
		else
			msg->type = ERROR_UNKNOWN;
	}
	delete [] data;
}


void
ParseIntoLines(const char *string, ErrorList &list)
{
	list.msglist.MakeEmpty();
	if (!string)
		return;

	int32 length = strlen(string);
	if (length < 1)
		return;
	
	char *data = new char[length + 1];
	sprintf(data,"%s",string);
	
	char *item = strtok(data,"\n");
	while (item)
	{
		error_msg *msg = new error_msg;
		msg->rawdata = item;
		msg->type = ERROR_MSG;
		list.msglist.AddItem(msg);
		item = strtok(NULL,"\n");
	}
	
	delete [] data;
}
