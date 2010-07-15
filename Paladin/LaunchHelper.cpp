#include "LaunchHelper.h"

#include <Roster.h>
#include <stdio.h>

LaunchHelper::LaunchHelper(void)
	:	fArgList(20,true),
		fTeamID(-1)
{
}


LaunchHelper::LaunchHelper(const char *type)
	:	fType(type),
		fArgList(20,true),
		fTeamID(-1)
{
}


LaunchHelper::LaunchHelper(entry_ref &ref)
	:	fRef(ref),
		fArgList(20,true),
		fTeamID(-1)
{
}


LaunchHelper &
LaunchHelper::operator <<(const char *string)
{
	AddArg(string);
	return *this;
}


LaunchHelper &
LaunchHelper::operator <<(const BString &string)
{
	AddArg(string.String());
	return *this;
}


LaunchHelper &
LaunchHelper::operator <<(const long &value)
{
	BString string;
	string << value;
	AddArg(string.String());
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
LaunchHelper::AddArg(const char *string)
{
	if (string && strlen(string) > 0)
	{
		BString *str = new BString(string);
		fArgList.AddItem(str);
	}
}


BString *
LaunchHelper::ArgAt(int32 index)
{
	return fArgList.ItemAt(index);
}


BString *
LaunchHelper::RemoveArg(int32 index)
{
	BString *item = fArgList.RemoveItemAt(index);
	return item;
}


void
LaunchHelper::RemoveArg(BString *string)
{
	fArgList.RemoveItem(string);
}


int32
LaunchHelper::CountArgs(void) const
{
	return fArgList.CountItems();
}


void
LaunchHelper::ParseToArgs(const char *string)
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


void
LaunchHelper::MakeEmpty(void)
{
	fArgList.MakeEmpty();
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
	
	for (int32 i = 0; i < CountArgs(); i++)
		out << " " << ArgAt(i)->String();
	
	return out;
}


void
LaunchHelper::PrintToStream(void)
{
	printf("%s\n",AsString().String());
}


