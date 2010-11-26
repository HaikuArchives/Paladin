#ifndef PRADIOBUTTON_H
#define PRADIOBUTTON_H

#include <RadioButton.h>
#include "PControl.h"

/*
	PRadioButton Properties:
		All PView Properties
		All PControl Properties
*/

class PRadioButton : public PControl
{
public:
							PRadioButton(void);
							PRadioButton(BMessage *msg);
							PRadioButton(const char *name);
							PRadioButton(const PRadioButton &from);
							~PRadioButton(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(BView *view = NULL);
};

#endif
