#ifndef PAPPLICATION_H
#define PAPPLICATION_H

#include <Application.h>
#include <Roster.h>

#include "PObject.h"
#include "PHandler.h"
#include "PComponents.h"

/*
	PApplication
	
	Properties
		CursorVisible
		Flags (read-only)
		IsLaunching (read-only)
		Port (read-only)
		PulseRate
		Signature
		Team (read-only)
		Thread (read-only)
		WindowCount (read-only)
	
	Methods
		ObscureCursor
		Run
		Quit
		WindowAt
	
	Events
		AboutRequested
		AppActivated
		ArgvReceived
		Pulse
		QuitRequested
		ReadyToRun
		RefsReceived
		SetupApp
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
	
	virtual	int32			Run(const char *signature);
	
	static	BArchivable *	Instantiate(BMessage *data);
		
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	
	virtual	status_t		SendMessage(BMessage *msg);
	
private:
	void					InitProperties(void);
	void					InitBackend(void);
	
	app_info				fAppInfo;
	bigtime_t				fPulseRate;
};

#endif
