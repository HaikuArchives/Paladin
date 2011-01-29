#ifndef PCLIPBOARD_H
#define PCLIPBOARD_H

#include "PHandler.h"
#include <Clipboard.h>

/*
	PClipboard
		Properties:
			LocalCount (read-only)
			Locked
			Name (read-only)
			SystemCount (read-only)
		
		Methods:
			Clear
			Commit
			Data
			Revert
*/

class PClipboard : public PHandler
{
public:
							PClipboard(void);
							PClipboard(BMessage *msg);
							PClipboard(const char *name);
							PClipboard(const PClipboard &from);
							~PClipboard(void);
			
	static	BArchivable *	Instantiate(BMessage *data);

	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	BClipboard *			GetBackend(void);
	
private:
	
	void					InitBackend(void);
	void					InitProperties(void);
	void					InitMethods(void);
	
	BClipboard				*fClipboard;
};

#endif
