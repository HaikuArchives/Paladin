#ifndef PTEXTCONTROL_H
#define PTEXTCONTROL_H

#include "PControl.h"



class PTextControl : public PControl
{
public:
							PTextControl(void);
							PTextControl(BMessage *msg);
							PTextControl(const char *name);
							PTextControl(const PTextControl &from);
							~PTextControl(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);


protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

