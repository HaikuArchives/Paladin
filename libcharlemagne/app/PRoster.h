#ifndef PROSTER_H
#define PROSTER_H

#include <Roster.h>
#include "PObject.h"

/*
	PRoster
		Properties:
			
		Methods:
			ActivateApp
			AddToRecentDocuments
			AddToRecentFolders
			Broadcast
			FindApp
			GetActiveAppInfo
			GetAppInfo
			GetAppList
			GetRecentApps
			GetRecentDocuments
			GetRecentFolders
			GetRunningAppInfo
			IsRunning
			Launch
			StartWatching
			StopWatching
			TeamFor
*/

class PRoster : public PObject
{
public:
							PRoster(void);
							PRoster(BMessage *msg);
							PRoster(const char *name);
							PRoster(const PRoster &from);
							~PRoster(void);
			
	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
private:
	
	void					InitBackend(void);
	void					InitProperties(void);
	void					InitMethods(void);
};

#endif
