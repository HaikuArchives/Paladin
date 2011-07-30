#include "LaunchHelper.h"

#include <File.h>
#include <Roster.h>
#include <stdio.h>
#include <stdlib.h>

#define HAIKU_PIPE_HACK

ArgList::ArgList(void)
	:	fArgList(20, true)
{
}


ArgList::ArgList(const char *string)
	:	fArgList(20, true)
{
	ParseToArgs(string);
}


ArgList::ArgList(const BString &str)
	:	fArgList(20, true)
{
	ParseToArgs(str.String());
}


ArgList::ArgList(const ArgList &list)
	:	fArgList(20, true)
{
	*this = list;
}


ArgList::~ArgList(void)
{
}


ArgList &
ArgList::operator <<(const char *string)
{
	AddArg(string);
	return *this;
}


ArgList &
ArgList::operator <<(const BString &string)
{
	AddArg(string.String());
	return *this;
}


ArgList &
ArgList::operator <<(const long &value)
{
	BString string;
	string << value;
	AddArg(string.String());
	return *this;
}


ArgList &
ArgList::operator =(const char *string)
{
	ParseToArgs(string);
	return *this;
}


ArgList &
ArgList::operator =(const BString &str)
{
	ParseToArgs(str.String());
	return *this;
}


ArgList &
ArgList::operator =(const ArgList &list)
{
	fArgList.MakeEmpty();
	
	for (int32 i = 0; i < list.fArgList.CountItems(); i++)
	{
		BString *item = list.fArgList.ItemAt(i);
		if (item)
			fArgList.AddItem(new BString(*item));
	}
	
	return *this;
}


ArgList &
ArgList::AddArg(const char *string)
{
	if (string && strlen(string) > 0)
	{
		BString *str = new BString(string);
		fArgList.AddItem(str);
	}
	return *this;
}


ArgList &
ArgList::AddList(const ArgList &list)
{
	for (int32 i = 0; i < list.CountArgs(); i++)
	{
		BString *arg = list.ArgAt(i);
		if (arg)
			AddArg(arg->String());
	}
	return *this;
}


BString *
ArgList::ArgAt(int32 index) const
{
	return fArgList.ItemAt(index);
}


BString *
ArgList::RemoveArg(int32 index)
{
	BString *item = fArgList.RemoveItemAt(index);
	return item;
}


void
ArgList::RemoveArg(BString *string)
{
	fArgList.RemoveItem(string);
}


int32
ArgList::CountArgs(void) const
{
	return fArgList.CountItems();
}


void
ArgList::MakeEmpty(void)
{
	fArgList.MakeEmpty();
}


void
ArgList::ParseToArgs(const char *string)
{
	if (!string)
		return;
	
	int32 length = strlen(string);
	if (length < 1)
		return;
	
	bool escapeMode = false;
	bool openSingleQuote = false;
	bool openDoubleQuote = false;
	BString arg;
	
	for (int32 i = 0; i < length; i++)
	{
		char c = string[i];
		
		if (escapeMode)
		{
			arg << c;
			escapeMode = false;
			continue;
		}
		
		switch (c)
		{
			case '\\':
			{
				escapeMode = true;
				break;
			}
			case '\'':
			{
				if (openDoubleQuote)
					arg << c;
				else
					openSingleQuote = !openSingleQuote;
				break;
			}
			case '"':
			{
				if (openSingleQuote)
					arg << c;
				else
					openDoubleQuote = !openDoubleQuote;
				break;
			}
			case ' ':
			{
				if (openDoubleQuote || openSingleQuote)
					arg << c;
				else
				{
					AddArg(arg.String());
					arg = "";
				}
				break;
			}
			default:
			{
				arg << c;
				break;
			}
		}
	}
	
	if (arg.CountChars() > 0)
		AddArg(arg.String());
}


BString
ArgList::AsString(void)
{
	BString out;
	for (int32 i = 0; i < CountArgs(); i++)
		out << " " << ArgAt(i)->String();
	
	return out;
}


void
ArgList::PrintToStream(void)
{
	printf("%s\n",AsString().String());
}


LaunchHelper::LaunchHelper(void)
	:	fTeamID(-1)
{
}


LaunchHelper::LaunchHelper(const char *type)
	:	fType(type),
		fTeamID(-1)
{
}


LaunchHelper::LaunchHelper(entry_ref &ref)
	:	fRef(ref),
		fTeamID(-1)
{
}


LaunchHelper &
LaunchHelper::operator =(const LaunchHelper &from)
{
	ArgList::operator =(from);
	fType = from.fType;
	fRef = from.fRef;
	fTeamID = from.fTeamID;
	
	return *this;
}


void
LaunchHelper::SetRef(const char *path)
{
	BEntry entry(path);
	if (entry.InitCheck() == B_OK && entry.Exists())
	{
		entry.GetRef(&fRef);
		fType = "";
	}
}


void
LaunchHelper::SetRef(entry_ref &ref)
{
	fRef = ref;
	fType = "";
}


void
LaunchHelper::SetType(const char *type)
{
	fType = type;
	fRef = entry_ref();
}


void
LaunchHelper::SetTeam(team_id id)
{
	fTeamID = id;
}


team_id
LaunchHelper::GetTeam(void) const
{
	return fTeamID;
}

	
void
LaunchHelper::MakeEmpty(void)
{
	ArgList::MakeEmpty();
	fRef = entry_ref();
	fType = "";
}


status_t
LaunchHelper::Launch(void)
{
	if (!fRef.name && fType.CountChars() < 1)
		return B_NO_INIT;
	
	const char *args[CountArgs()];
	for (int32 i = 0; i < CountArgs(); i++)
		args[i] = ArgAt(i)->String();
	
	team_id *team = fTeamID > 0 ? &fTeamID : NULL;
	
	status_t status;
	if (fRef.name)
		status = be_roster->Launch(&fRef,CountArgs(),args,team);
	else
		status = be_roster->Launch(fType.String(),CountArgs(),(char **)args,team);
	return status;
}


BString
LaunchHelper::AsString(void)
{
	BString out;
	
	if (fRef.name)
	{
		BPath path(&fRef);
		out << path.Path();
	}
	else if (fType.CountChars() > 0)
		out << fType.String();
	else
		out << "(null file and type)";
	
	out << " " << ArgList::AsString();
	
	return out;
}


ShellHelper::ShellHelper(void)
	:	fCallback(NULL)
{
}


ShellHelper &
ShellHelper::operator =(const ShellHelper &from)
{
	ArgList::operator =(from);
	fCallback = from.fCallback;
	
	return *this;
}


ShellHelper &
ShellHelper::AddEscapedArg(const char *string)
{
	if (string && strlen(string) > 0)
	{
		BString str(string);
		str.CharacterEscape("'", '\\');
		str.Prepend("'");
		str.Append("'");
		AddArg(str.String());
	}
	return *this;
}


void
ShellHelper::SetUpdateCallback(ShellHelperCallback cb)
{
	fCallback = cb;
}


ShellHelperCallback
ShellHelper::GetUpdateCallback(void) const
{
	return fCallback;
}


int
ShellHelper::Run(void)
{
	return system(AsString().String());
}


status_t
ShellHelper::RunInPipe(BString &out, bool redirectStdErr)
{
	BString in(AsString());
	
	if (in.CountChars() < 1)
		return -1;
	
#ifdef HAIKU_PIPE_HACK

	BString tmpfilename("/tmp/ShellHelper.build.tmp.");
	tmpfilename << real_time_clock_usecs();
	
	in << " > " << tmpfilename;
	
	if (redirectStdErr)
		in << " 2>&1";
	system(in.String());
	
	BFile file(tmpfilename.String(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	off_t size;
	file.GetSize(&size);
	
	char *buffer = out.LockBuffer(size + 1);
	if (buffer)
	{
		file.Read(buffer, size);
		out.UnlockBuffer();
	}
	else
		fprintf(stderr, "Couldn't create buffer for command ShellHelper::RunInPipe\n");
	
	file.Unset();
	BEntry(tmpfilename.String()).Remove();

#else

	FILE *fd = popen(in.String(),"r");
	
	if (!fd)
		return -2;
	
	out = "";
	char buffer[1024];
	while (fgets(buffer,1024,fd))
	{
		if (fCallback)
			fCallback(buffer);
		out += buffer;
	}
	
	pclose(fd);

#endif	
	
	return B_OK;
}
