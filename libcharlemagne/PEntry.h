#ifndef PENTRY_H
#define PENTRY_H

#include "PObject.h"

class BEntry;


class PEntry : public PObject
{
public:
							PEntry(void);
							PEntry(BMessage *msg);
							PEntry(const char *name);
							PEntry(const PEntry &from);
							~PEntry(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BEntry *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

			BEntry		*fBackend;

};

#endif

