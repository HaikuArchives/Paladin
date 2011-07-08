#ifndef PRADIOBUTTON_H
#define PRADIOBUTTON_H

#include "PControl.h"



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
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

