/*
 * Copyright 2011 David Couzelis. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <Catalog.h>
#include <StringView.h>

#include "FallLeaves.h"
#include "FLConfigView.h"


// For language translations
#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FallLeaves"


enum {
	MSG_SET_AMOUNT		= 'amnt',
	MSG_SET_SPEED		= 'sped'
};


FLConfigView::FLConfigView(BRect frame, FallLeaves* saver,
	int32 amount, int32 speed)
	:
	BView(frame, B_EMPTY_STRING, B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
	fSaver(saver),
	fAmountSlider(NULL),
	fSpeedSlider(NULL)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// Screensaver name
	BRect rect(10, 10, 20, 20);
	BStringView* stringView = new BStringView(rect, B_EMPTY_STRING,
		B_TRANSLATE("Fall Leaves"));
	stringView->SetFont(be_bold_font);
	stringView->ResizeToPreferred();
	AddChild(stringView);

	// Developer name
	rect.OffsetBy(0, stringView->Bounds().Height() + 4);
	stringView = new BStringView(rect, B_EMPTY_STRING,
		B_TRANSLATE("by David Couzelis"));
	stringView->ResizeToPreferred();
	AddChild(stringView);
	
	BRect bounds = Bounds();
	bounds.InsetBy(10, 10);
	BRect internalFrame(0, 0, bounds.Width(), 20);

	// Speed setting slider
	fSpeedSlider = new BSlider(internalFrame, B_EMPTY_STRING,
		B_TRANSLATE("Speed:"), new BMessage(MSG_SET_SPEED),
		kMinSpeed, kMaxSpeed, B_BLOCK_THUMB,
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	fSpeedSlider->SetValue(speed);
	fSpeedSlider->ResizeToPreferred();
	bounds.bottom -= fSpeedSlider->Bounds().Height() * 1.5;
	fSpeedSlider->MoveTo(bounds.LeftBottom());
	AddChild(fSpeedSlider);

	// Amount setting slider
	fAmountSlider = new BSlider(internalFrame, B_EMPTY_STRING,
		B_TRANSLATE("Amount:"), new BMessage(MSG_SET_AMOUNT),
		kMinAmount, kMaxAmount, B_BLOCK_THUMB,
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	fAmountSlider->SetValue(amount);
	fAmountSlider->ResizeToPreferred();
	bounds.bottom -= fAmountSlider->Bounds().Height() * 1.5;
	fAmountSlider->MoveTo(bounds.LeftBottom());
	AddChild(fAmountSlider);
};


void
FLConfigView::AttachedToWindow()
{
	fAmountSlider->SetTarget(this);
	fSpeedSlider->SetTarget(this);
}


void
FLConfigView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_SET_AMOUNT:
			fSaver->SetAmount(fAmountSlider->Value());
			break;
		case MSG_SET_SPEED:
			fSaver->SetSpeed(fSpeedSlider->Value());
			break;
		default:
			BHandler::MessageReceived(message);
	}
}
