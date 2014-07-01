/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _V_REG_WINDOW_H
#define _V_REG_WINDOW_H


#include <Window.h>


class AutoTextControl;
class BTextView;

class VRegWindow : public BWindow {
public:
								VRegWindow();

	virtual	void				MessageReceived(BMessage* message);
			
private:
			void				RunSearch(void);

			AutoTextControl*	fRegexTextControl;
			BTextView*			fSourceView;
			BTextView*			fMatchView;
};


#endif // _V_REG_WINDOW_H
