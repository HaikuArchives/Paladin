#ifndef LAUNCHHELPER_H
#define LAUNCHHELPER_H

#include <Entry.h>
#include <Path.h>
#include <String.h>
#include "ObjectList.h"

// This class isn't particularly useful on its own. It's used as a base for
// the LaunchHelper and ShellHelper classes to be able to manage the parameter
// list.
class ArgList
{
public:
								ArgList(void);
								ArgList(const char *string);
								ArgList(const BString &string);
								ArgList(const ArgList &from);
	virtual						~ArgList(void);

			ArgList &			operator <<(const char *string);
			ArgList &			operator <<(const BString &string);
			ArgList &			operator <<(const long &value);
			ArgList &			operator =(const char *string);
			ArgList &			operator =(const BString &string);
			ArgList &			operator =(const ArgList &list);

			ArgList &			AddArg(const char *string);
			ArgList &			AddList(const ArgList &list);
			BString *			ArgAt(int32 index) const;
			BString *			RemoveArg(int32 index);
			void				RemoveArg(BString *string);
			int32				CountArgs(void) const;
	virtual	void				MakeEmpty(void);

			// Convert a string into multiple arguments. Basic bash parsing is
			// done. Note that leaving a string open will result in everything
			// after the starting quote being made into one argument.
			//Backslashes or quotes at the end of the input string are removed
			void				ParseToArgs(const char *string);
			
	virtual	BString				AsString(void);
			void				PrintToStream(void);

private:

	BObjectList<BString>	fArgList;
};


// This class is for making it easier to launch an app (with arguments) via the Roster
class LaunchHelper : public ArgList
{
public:
						LaunchHelper(void);
						LaunchHelper(const char *type);
						LaunchHelper(entry_ref &ref);
	
	LaunchHelper &		operator =(const LaunchHelper &list);

	void				SetRef(const char *path);
	void				SetRef(entry_ref &ref);
	void				SetType(const char *type);
	
	void				SetTeam(team_id id);
	team_id				GetTeam(void) const;
	
	void				MakeEmpty(void);
	
	status_t			Launch(void);
	BString				AsString(void);

private:
	BString					fType;
	entry_ref				fRef;
	team_id					fTeamID;
};

typedef void (*ShellHelperCallback)(const char *newText);

// This class is for making it easier to run a command in the shell, possibly
// via a pipe
class ShellHelper : public ArgList
{
public:
						ShellHelper(void);
	
	ShellHelper &		operator =(const ShellHelper &list);

	ShellHelper	&		AddEscapedArg(const char *string);
	
	void				SetUpdateCallback(ShellHelperCallback cb);
	ShellHelperCallback	GetUpdateCallback(void) const;
	
	int					Run(void);
	status_t			RunInPipe(BString &out, bool redirectStdErr);

private:

	ShellHelperCallback	fCallback;
};

#endif
