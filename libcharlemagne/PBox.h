#ifndef PBOX_H
#define PBOX_H

#include "PView.h"



class PBox : public PView
{
public:
							PBox(void);
							PBox(BMessage *msg);
							PBox(const char *name);
							PBox(const PBox &from);
							~PBox(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

protected:
	virtual	void			InitBackend(void);

private:
	void					InitProperties(void);
	void					InitMethods(void);

};

#endif

