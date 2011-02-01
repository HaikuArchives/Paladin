#ifndef PPROGRESSBAR_H
#define PPROGRESSBAR_H

#include "PView.h"
#include <StatusBar.h>

/*
	PProgressBar Properties:
			All PView Properties
			BarColor
			BarHeight
			Label
			CurrentValue
			MaxValue
			Text
			TrailingLabel
			TrailingText
*/

class PProgressBar : public PView
{
public:
							PProgressBar(void);
							PProgressBar(BMessage *msg);
							PProgressBar(const char *name);
							PProgressBar(const PProgressBar &from);
							~PProgressBar(void);
			
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
