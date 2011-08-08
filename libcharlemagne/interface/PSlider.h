#ifndef PSLIDER_H
#define PSLIDER_H

#include "PControl.h"
#include <Application.h>
#include <Slider.h>
#include <Window.h>
#include <stdio.h>


class PSliderBackend;
class BSlider;

class PSlider : public PControl
{
public:
							PSlider(void);
							PSlider(BMessage *msg);
							PSlider(const char *name);
							PSlider(const PSlider &from);
							~PSlider(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BSlider *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

