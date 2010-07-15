#ifndef LAUNCHHELPER_H
#define LAUNCHHELPER_H

#include <Entry.h>
#include <Path.h>
#include <String.h>
#include "ObjectList.h"

class LaunchHelper
{
public:
						LaunchHelper(void);
						LaunchHelper(const char *type);
						LaunchHelper(entry_ref &ref);
	
	LaunchHelper &		operator <<(const char *string);
	LaunchHelper &		operator <<(const BString &string);
	LaunchHelper &		operator <<(const long &value);
	
	void				SetRef(const char *path);
	void				SetRef(entry_ref &ref);
	void				SetType(const char *type);
	
	void				SetTeam(team_id id);
	team_id				GetTeam(void) const;
	
	void				AddArg(const char *string);
	BString *			ArgAt(int32 index);
	BString *			RemoveArg(int32 index);
	void				RemoveArg(BString *string);
	int32				CountArgs(void) const;
	
	// Convert a string into multiple arguments. Basic bash parsing is done. Note that
	// leaving a string open will result in everything after the starting quote being
	// made into one argument. Backslashes or quotes at the end of the input string are 
	// removed
	void				ParseToArgs(const char *string);
	
	void				MakeEmpty(void);
	
	status_t			Launch(void);
	BString				AsString(void);
	void				PrintToStream(void);

private:
	BString					fType;
	entry_ref				fRef;
	BObjectList<BString>	fArgList;
	team_id					fTeamID;
};

#endif
