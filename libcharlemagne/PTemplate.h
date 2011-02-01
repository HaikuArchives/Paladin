#ifndef PTEMPLATE_H
#define PTEMPLATE_H

#include "PView.h"

/*
	PTemplate Properties:
		All PView Properties
*/

class PTemplate : public PView
{
public:
							PTemplate(void);
							PTemplate(BMessage *msg);
							PTemplate(const char *name);
							PTemplate(const PTemplate &from);
							~PTemplate(void);
			
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
