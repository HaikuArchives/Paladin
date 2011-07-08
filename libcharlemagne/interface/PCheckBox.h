#ifndef PCHECKBOX_H
#define PCHECKBOX_H

#include <CheckBox.h>
#include "PControl.h"

/*
	PCheckBox Properties:
		All PView Properties
		All PControl Properties
*/

class PCheckBox : public PControl
{
public:
							PCheckBox(void);
							PCheckBox(BMessage *msg);
							PCheckBox(const char *name);
							PCheckBox(const PCheckBox &from);
							~PCheckBox(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend();
};

#endif
