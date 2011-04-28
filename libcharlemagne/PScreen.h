#ifndef PSCREEN_H
#define PSCREEN_H

#include "PObject.h"

class PScreenBackend;
class BScreen;

class PScreen : public PObject
{
public:
							PScreen(void);
							PScreen(BMessage *msg);
							PScreen(const char *name);
							PScreen(const PScreen &from);
							~PScreen(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BScreen *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

			PScreenBackend *fBackend;
};

#endif

