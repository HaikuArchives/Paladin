#ifndef PBUTTON_H
#define PBUTTON_H

#include "PControl.h"
#include <Application.h>
#include <Button.h>
#include <stdio.h>


class PButtonBackend;
class BButton;

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
	
			BButton *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

