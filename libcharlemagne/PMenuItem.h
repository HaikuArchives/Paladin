#ifndef PMENUITEM_H
#define PMENUITEM_H

#include "PObject.h"

class BMenuItem;


class PMenuItem : public PObject
{
public:
							PMenuItem(void);
							PMenuItem(BMessage *msg);
							PMenuItem(const char *name);
							PMenuItem(const PMenuItem &from);
							~PMenuItem(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BMenuItem *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

			BMenuItem		*fBackend;

};

#endif

