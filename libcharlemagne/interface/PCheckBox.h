#ifndef PCHECKBOX_H
#define PCHECKBOX_H

#include "PControl.h"
#include <Application.h>
#include <CheckBox.h>
#include <stdio.h>
#include <Window.h>


class PCheckBoxBackend;
class BCheckBox;

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
	
			BCheckBox *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

