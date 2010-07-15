//	MAlert.cpp
//	Utility class for sane-ifying Be alerts
//	© Copyright 1995 Metrowerks Corporation. All rights reserved.
//	Jon Wätte

#include "MAlert.h"

#include <Beep.h>



bool MAlert::sAlertIsNoisy = FALSE;

bool
MAlert::GetAlertIsNoisy()
{
	return sAlertIsNoisy;
}


void
MAlert::SetAlertIsNoisy(
	bool noisy)
{
	sAlertIsNoisy = noisy;
}


void
MAlert::MakeAlert(
	const char * message,
	const char * button1,
	const char * button2,
	const char * button3,
	alert_type type)
{
	const char * a1, *a2, *a3;
	if (button3)
	{
		fNumButtons = 3;
		a1 = button3;
		a2 = button2;
		a3 = button1;
	}
	else if (button2)
	{
		fNumButtons = 2;
		a1 = button2;
		a2 = button1;
		a3 = NULL;
	}
	else if (button1)
	{
		fNumButtons = 1;
		a1 = button1;
		a2 = NULL;
		a3 = NULL;
	}
	else
	{
		fNumButtons = 1;
		a1 = "OK";
		a2 = NULL;
		a3 = NULL;
	}
	fAlert = new BAlert("Alert", message, a1, a2, a3,
		B_WIDTH_AS_USUAL, type);

	// The Escape key shall trigger the second button from the right
	// (usually "Cancel"). If there is only one button, map Escape to that
	// one.
	if (fNumButtons > 1)
		SetShortcut(2, B_ESCAPE);
	else
		SetShortcut(1, B_ESCAPE);
}


MAlert::MAlert()
{
	fAlert = NULL;
	fResult = 0;
	fNumButtons = 0;
}


MAlert::MAlert(
	const char * message,
	const char * button1,
	const char * button2,
	const char * button3)
{
	MakeAlert(
		message, button1, button2, button3,
		B_INFO_ALERT);
}


MAlert::~MAlert()
{
	delete fAlert;
}


long
MAlert::Go()
{
	if (fAlert)
	{
		if (sAlertIsNoisy)
			beep();
		fResult = fNumButtons-fAlert->Go();
		fAlert = NULL;
	}
	return fResult;
}


void
MAlert::SetShortcut(
	long button,
	char key)
{
	if (fAlert && button > 0 && button <= fNumButtons)
		fAlert->SetShortcut(fNumButtons - button, key);
}


MAlert::operator int()
{
	return Go();
}


MBlankAlert::MBlankAlert(
	const char * message,
	const char * button1,
	const char * button2,
	const char * button3)
{
	MakeAlert(
		message, button1, button2, button3,
		B_EMPTY_ALERT);
}


MIdeaAlert::MIdeaAlert(
	const char * message,
	const char * button1,
	const char * button2,
	const char * button3)
{
	MakeAlert(
		message, button1, button2, button3,
		B_IDEA_ALERT);
}


MWarningAlert::MWarningAlert(
	const char * message,
	const char * button1,
	const char * button2,
	const char * button3)
{
	MakeAlert(
		message, button1, button2, button3,
		B_WARNING_ALERT);
}


MStopAlert::MStopAlert(
	const char * message,
	const char * button1,
	const char * button2,
	const char * button3)
{
	MakeAlert(
		message, button1, button2, button3,
		B_STOP_ALERT);
}

