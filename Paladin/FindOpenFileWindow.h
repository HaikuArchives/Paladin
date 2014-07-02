/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _FIND_OPEN_FILE_WINDOW_H
#define _FIND_OPEN_FILE_WINDOW_H


#include "DWindow.h"


class AutoTextControl;
class BCheckBox;

class FindOpenFileWindow : public DWindow {
public:
								FindOpenFileWindow(const char* panelText);
			void				MessageReceived(BMessage* message);

private:
			AutoTextControl*	fNameTextControl;
			BCheckBox*			fSystemCheckBox;
};


#endif // _FIND_OPEN_FILE_WINDOW_H
