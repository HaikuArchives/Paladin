#ifndef GITSOURCECONTROL_H
#define GITSOURCECONTROL_H

#include "SourceControl.h"


class GitSourceControl : public SourceControl
{
public:
							GitSourceControl(void);
							GitSourceControl(const entry_ref &workingDir);
	virtual					~GitSourceControl(void);
	
	virtual	status_t		CreateRepository(const char *path);
	virtual	status_t		CloneRepository(const char *url, const char *dest);
	
	virtual	bool			NeedsInit(const char *topDir);
	
	virtual	status_t		AddToRepository(const char *path);
	virtual	status_t		RemoveFromRepository(const char *path);
	
	virtual	status_t		Commit(const char *msg);
	virtual	status_t		Merge(const char *rev);
			
	virtual	status_t		Push(const char *url = NULL);
	virtual	status_t		Pull(const char *url = NULL);
	
	virtual	status_t		Revert(const char *relPath);
	
	virtual	status_t		Rename(const char *oldname, const char *newname);
	
	virtual	status_t		GetHistory(BString &out, const char *file);
	virtual	status_t		GetChangeStatus(BString &out);

};


#endif
