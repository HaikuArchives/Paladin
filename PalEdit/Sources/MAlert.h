//	MAlert.h
//	© Copyright 1995 Metrowerks Corporation. All rights reserved.
//	Jon Wätte
//
//	This handy set of utility classes can be used to simplify BAlerts.
//	It will make sure that the buttons are numbered in the RIGHT order,
//	which is from right to left, and that the escape key will select
//	button 2 (if there is one)
//
//	Also, buttons are numbered starting at one (1) and you can construct
//	an alert with no button argument, making for a default "OK" button.
//
//	You can also "run" the alert by comparing it to an integer, and you
//	can test its value by repeatedly calling Go() or operator int().
//
//	Each subclass creates an alert with a different icon.

#ifndef MALERT_H
#define MALERT_H

#include "HLibHekkel.h"

#include <Alert.h>


IMPEXP_LIBHEKKEL enum AlertReplies {
	kOKButton = 1,
	kCancelButton = 2,
	kSaveButton = 1,
	kDontSaveButton = 3
};

class IMPEXP_LIBHEKKEL MAlert
{
		BAlert *				fAlert;
		long					fResult;
		long					fNumButtons;
static	bool					sAlertIsNoisy;
protected:
		void					MakeAlert(
									const char * message,
									const char * button1,
									const char * button2,
									const char * button3,
									alert_type type);
								MAlert();
public:
								MAlert(
									const char * message,
									const char * button1 = NULL,
									const char * button2 = NULL,
									const char * button3 = NULL);
								~MAlert();
		long					Go();
		void					SetShortcut(
									long button,
									char key);
								operator int();

static	bool					GetAlertIsNoisy();
static	void					SetAlertIsNoisy(
									bool noisy);
};

typedef MAlert MInfoAlert;


class IMPEXP_LIBHEKKEL MBlankAlert :
	public MAlert
{
public:
								MBlankAlert(
									const char * message,
									const char * button1 = NULL,
									const char * button2 = NULL,
									const char * button3 = NULL);
};


class IMPEXP_LIBHEKKEL MIdeaAlert :
	public MAlert
{
public:
								MIdeaAlert(
									const char * message,
									const char * button1 = NULL,
									const char * button2 = NULL,
									const char * button3 = NULL);
};


class IMPEXP_LIBHEKKEL MWarningAlert :
	public MAlert
{
public:
								MWarningAlert(
									const char * message,
									const char * button1 = NULL,
									const char * button2 = NULL,
									const char * button3 = NULL);
};


class IMPEXP_LIBHEKKEL MStopAlert :
	public MAlert
{
public:
								MStopAlert(
									const char * message,
									const char * button1 = NULL,
									const char * button2 = NULL,
									const char * button3 = NULL);
};

#endif
