#ifndef PPROGRESSBAR_H
#define PPROGRESSBAR_H

#include "PView.h"



class PProgressBar : public PView
{
public:
							PProgressBar(void);
							PProgressBar(BMessage *msg);
							PProgressBar(const char *name);
							PProgressBar(const PProgressBar &from);
							~PProgressBar(void);

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

