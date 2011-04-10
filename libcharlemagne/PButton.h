#ifndef PBUTTON_H
#define PBUTTON_H

#include "PControl.h"



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
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

