/*
 * Copyright 2011 David Couzelis. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _FLCONFIGVIEW_H_
#define _FLCONFIGVIEW_H_


#include <Slider.h>
#include <View.h>


typedef class FallLeaves;


class FLConfigView : public BView
{
public:
					FLConfigView(BRect frame, FallLeaves* saver,
						int32 amount, int32 speed);
	
	void			AttachedToWindow();
	void			MessageReceived(BMessage* message);
private:
	FallLeaves*		fSaver;
	
	BSlider*		fAmountSlider;
	BSlider*		fSpeedSlider;
};


#endif
