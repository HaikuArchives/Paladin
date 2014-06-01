/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H


#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Button.h>
#include <Messenger.h>
#include <Font.h>
#include <StatusBar.h>


enum
{
	ABOUT_STARTUP	= 0,
	ABOUT_OK
};

enum
{
	M_RESET_STATUS	= 'mrst',
	M_SET_STATUS	= 'msst',
	M_UPDATE_STATUS	= 'mups',
	M_SHOW_ABOUT	= 'msha'
};


class AboutWindow : public BWindow {
public:
								AboutWindow(void);
};


#endif	// ABOUT_WINDOW_H
