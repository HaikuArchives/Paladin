#ifndef PAPPLICATION_H
#define PAPPLICATION_H

#include "PObject.h"
#include "PHandler.h"
#include "PComponents.h"

/*
	PApplication
	
	Properties
	
	Methods
	
	Events
		AboutRequested
		AppActivated
		ArgvReceived
		Pulse
		QuitRequested
		ReadyToRun
		RefsReceived
*/

class PAppBackend;

class PApplication : public PHandler
{
public:
							PApplication(void);
							PApplication(BMessage *msg);
							PApplication(const char *name);
							PApplication(const PApplication &from);
							~PApplication(void);
	
	virtual	void			Run(const char *signature);
	
	static	BArchivable *	Instantiate(BMessage *data);
		
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	
	virtual	status_t		SendMessage(BMessage *msg);
	
private:
	void					InitProperties(void);
	void					InitBackend(void);
	
	PAppBackend				*fApp;
};

#endif
