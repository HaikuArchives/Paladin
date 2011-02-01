#ifndef PSLIDER_H
#define PSLIDER_H

#include <Slider.h>
#include "PControl.h"

/*
	PSlider Properties:
		All PView Properties
		All PControl Properties
		
		BarColor
		BarThickness
		FillColor
		HashMarkCount
		HashMarkLocation
		MinLimit
		MinLimitLabel
		MaxLimit
		MaxLimitLabel
		Orientation
		Position
		ThumbStyle
		UsingFillColor
*/

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
	
protected:
	virtual void			InitBackend(void);

private:
	void					InitProperties(void);
};

#endif
