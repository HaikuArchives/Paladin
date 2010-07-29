#ifndef HGSOURCECONTROL_H
#define HGSOURCECONTROL_H

#include "SourceControl.h"


class HgSourceControl : public SourceControl
{
public:
							HgSourceControl(void);
							HgSourceControl(const entry_ref &workingDir);
	virtual					~HgSourceControl(void);
	
	virtual	status_t		CreateRepository(const char *path);
	virtual	bool			DetectRepository(const char *path);
	virtual	status_t		CloneRepository(const char *url, const char *dest);
	
	virtual	bool			NeedsInit(const char *topDir);
	
	virtual	status_t		AddToRepository(const char *path);
	virtual	status_t		RemoveFromRepository(const char *path);
	
	virtual	status_t		Commit(const char *msg);
	virtual	status_t		Merge(const char *rev = NULL);
			
	virtual	status_t		Push(const char *url = NULL);
	virtual	status_t		Pull(const char *url = NULL);
	
	// Recovers a failed commit. This is specific to Mercurial
			void			Recover(void);

	virtual	status_t		Revert(const char *relPath);
	
	virtual	status_t		Rename(const char *oldname, const char *newname);
	
	virtual	status_t		GetHistory(BString &out, const char *file);
	virtual	status_t		GetChangeStatus(BString &out);

};


#endif
