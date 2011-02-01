#ifndef PBOX_H
#define PBOX_H

#include <Box.h>
#include "PView.h"

/*
	PBox Properties:
		All PView Properties
		Label
		Border Style
*/

class PBox : public PView
{
public:
							PBox(void);
							PBox(BMessage *msg);
							PBox(const char *name);
							PBox(const PBox &from);
							~PBox(void);
			
	static	BArchivable *	Instantiate(BMessage *data);

	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(void);

private:
	void					InitProperties(void);
	void					InitMethods(void);
};

#endif
