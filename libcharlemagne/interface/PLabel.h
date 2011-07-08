#ifndef PLABEL_H
#define PLABEL_H

#include "PView.h"



class PLabel : public PView
{
public:
							PLabel(void);
							PLabel(BMessage *msg);
							PLabel(const char *name);
							PLabel(const PLabel &from);
							~PLabel(void);

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

