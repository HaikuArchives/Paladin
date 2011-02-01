#ifndef PLABEL_H
#define PLABEL_H

#include <StringView.h>
#include "PView.h"

/*
	PLabel Properties:
		All PView Properties
		Alignment
		Text
*/

class PLabel : public PView
{
public:
							PLabel(void);
							PLabel(BMessage *msg);
							PLabel(const char *name);
							PLabel(const PLabel &from);
							~PLabel(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(void);

private:
	void					InitProperties(void);
};

#endif
