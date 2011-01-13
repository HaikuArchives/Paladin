#ifndef PTEXTCONTROL_H
#define PTEXTCONTROL_H

#include "AutoTextControl.h"
#include "PControl.h"

/*
	PTextControl Properties:
		All PView Properties
		All PControl Properties
		Divider
		LabelAlignment
		TextAlignment
		Text
*/

class PTextControl : public PControl
{
public:
							PTextControl(void);
							PTextControl(BMessage *msg);
							PTextControl(const char *name);
							PTextControl(const PTextControl &from);
							~PTextControl(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(BView *view = NULL);

private:
	void					InitProperties(void);
	status_t				DoSetPreferredDivider(void);
};

#endif
