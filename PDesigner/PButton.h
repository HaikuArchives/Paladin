#ifndef PBUTTON_H
#define PBUTTON_H

#include <Button.h>
#include "PControl.h"

/*
	PButton Properties:
		All PView Properties
		All PControl Properties
*/

class PButton : public PControl
{
public:
							PButton(void);
							PButton(BMessage *msg);
							PButton(const char *name);
							PButton(const PButton &from);
							~PButton(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(BView *view = NULL);
	void					InitMethods(void);
};

#endif
