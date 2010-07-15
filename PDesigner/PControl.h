#ifndef PCONTROL_H
#define PCONTROL_H

#include "PView.h"
#include <Control.h>

/*
	Control Properties:
		Enabled
		Label
		Message
		Value
*/

class PControl : public PView
{
public:
							PControl(void);
							PControl(BMessage *msg);
							PControl(const char *name);
							PControl(const PControl &from);
	virtual					~PControl(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
protected:
	virtual void			InitBackend(BView *view = NULL);
	
private:
	void					InitPControl(void);
	void					InitProperties(void);
};

#endif
