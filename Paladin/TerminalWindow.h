/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _TERMINAL_WINDOW_H
#define _TERMINAL_WINDOW_H


#include "DWindow.h"

#include <String.h>
#include <TextView.h>


class TerminalWindow : public DWindow {
public:
							TerminalWindow(const char* commandLine);
	virtual					~TerminalWindow(void);

			void			MessageReceived(BMessage* message);
			void			RunCommand(void);
			void			FrameResized(float, float);

private:
			BTextView*		fTextView;
			BString			fCommand;
};


#endif // _TERMINAL_WINDOW_H
