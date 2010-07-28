#ifndef SVNSOURCECONTROL_H
#define SVNSOURCECONTROL_H

#include "SourceControl.h"

class SVNSourceControl : public SourceControl
{
public:
							SVNSourceControl(void);
							SVNSourceControl(const entry_ref &workingDir);
							~SVNSourceControl(void);
	
			status_t		CreateRepository(const char *path);
			bool			DetectRepository(const char *path);
			status_t		CloneRepository(const char *url, const char *dest);
			
			bool			NeedsInit(const char *topDir);
	
			status_t		AddToRepository(const char *path);
			status_t		RemoveFromRepository(const char *path);
			
			status_t		Commit(const char *msg);
			
			status_t		Revert(const char *relPath);
			status_t		GetHistory(BString &out, const char *file);
			status_t		GetChangeStatus(BString &out);
	
	// These are SVN-specific. They are for setting the folder where local
	// repositories are kept.
	static	void			SetRepositoryPath(const char *path);
	static	const char *	GetRepositoryPath(const char *path);
};


#endif
