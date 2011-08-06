#ifndef PMESSENGER_H
#define PMESSENGER_H

#include <Messenger.h>

#include "PObject.h"
#include "PComponents.h"

/*
	PMessenger
	
	Properties
		IsValid
		TargetLocal
		Team
		
	Methods
		LockTarget
		SendMessage
*/


class PMessenger : public PObject
{
public:
							PMessenger(void);
							PMessenger(BMessage *msg);
							PMessenger(const char *name);
							PMessenger(const PMessenger &from);
							~PMessenger(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
		
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	
private:
	void					InitProperties(void);
	void					InitMethods(void);
	
	BMessenger				fMessenger;
};

#endif
