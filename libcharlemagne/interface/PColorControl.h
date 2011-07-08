#ifndef PCOLORCONTROL_H
#define PCOLORCONTROL_H

#include "PControl.h"



class PColorControl : public PControl
{
public:
							PColorControl(void);
							PColorControl(BMessage *msg);
							PColorControl(const char *name);
							PColorControl(const PColorControl &from);
							~PColorControl(void);

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

