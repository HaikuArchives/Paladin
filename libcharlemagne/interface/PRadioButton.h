#ifndef PRADIOBUTTON_H
#define PRADIOBUTTON_H

#include "PControl.h"
#include <Application.h>
#include <RadioButton.h>
#include <stdio.h>
#include <Window.h>


class PRadioButtonBackend;
class BRadioButton;

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
	
			BRadioButton *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

