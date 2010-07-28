#ifndef SOURCECONTROL_H
#define SOURCECONTROL_H

#include <Entry.h>
#include <Path.h>
#include <String.h>

enum
{
	SCM_BRANCH		= 0x00000001,
	SCM_DISTRIBUTED	= 0x00000002
};

typedef void (*SourceControlCallback)(const char *newText);

class SourceControl
{
public:
							SourceControl(void);
							SourceControl(const entry_ref &workingDir);
	virtual					~SourceControl(void);
	
			const char *	GetShortName(void) const;
			const char *	GetLongName(void) const;
			int32			GetFlags(void) const;
			
	virtual	status_t		SetWorkingDirectory(const entry_ref &topDir);
	virtual	status_t		SetWorkingDirectory(const char *path);
			const char *	GetWorkingDirectory(void) const;
			void			SetUsername(const char *username);
			
	// Turns a folder into a repository and adds all files in it to the repo.
	// Committing the changes is still necessary
	virtual	status_t		CreateRepository(const char *path);
	
	// Returns true if the specified path contains a folder dedicated to
	// source control. This only checks the specified path. If the path is, for
	// example a subdirectory of a Git repository, the .git folder will not be
	// detected and will return false.
	virtual	bool			DetectRepository(const char *path);
	
	virtual	status_t		CloneRepository(const char *url, const char *dest);
	
	virtual	bool			NeedsInit(const char *topDir);
	
	// Add a file or folder to the repository.
	virtual	status_t		AddToRepository(const char *path);
	virtual	status_t		RemoveFromRepository(const char *path);
	
	virtual	status_t		Commit(const char *msg);
	virtual	status_t		Merge(const char *rev);
			
	virtual	status_t		Push(const char *url = NULL);
	virtual	status_t		Pull(const char *url = NULL);
	
	// Pass NULL to revert the whole working copy.
	virtual	status_t		Revert(const char *relPath);
	
	virtual	status_t		Rename(const char *oldname, const char *newname);
	
	virtual	status_t		GetHistory(BString &out, const char *file);
	virtual	status_t		GetChangeStatus(BString &out);
			
			void			SetURL(const char *url);
			BString			GetURL(void) const;
			
			void			SetUpdateCallback(SourceControlCallback cb);
			SourceControlCallback	GetUpdateCallback(void) const;
			
			void			SetDebugMode(bool value);
			
			void			SetVerboseMode(bool value);
			bool			GetVerboseMode(void) const;
			
			void			RunCustomCommand(const char *command);
protected:
			void			SetShortName(const char *name);
			void			SetLongName(const char *name);
			void			SetFlags(int32 flags);
			
			BString			GetUsername(void) const;
			BString			GetPassword(void) const;
			
			int				RunCommand(const BString &in, BString &out);

private:
	BString					fShortName,
							fLongName;
	
	BString					fURL;
	
	int32					fFlags;
	
	BString					fUsername,
							fPassword;
	
	BString					fWorkingDir;
	bool					fDebug,
							fVerbose;
	SourceControlCallback	fCallback;
};

#endif
