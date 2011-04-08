#ifndef PCLIPBOARD_H
#define PCLIPBOARD_H

#include "PHandler.h"

class BClipboard;

class PClipboard : public PHandler
{
public:
							PClipboard(void);
							PClipboard(BMessage *msg);
							PClipboard(const char *name);
							PClipboard(const PClipboard &from);
							~PClipboard(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BClipboard *	GetBackend(void) const;
protected:
	virtual	void			InitBackend(void);

private:
	void					InitProperties(void);
	void					InitMethods(void);

	BClipboard *fBackend;
};

#endif

