/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef TEMPLATE_WINDOW_H
#define TEMPLATE_WINDOW_H


#include <Window.h>

#include "TemplateManager.h"


class AutoTextControl;
class BButton;
class BCheckBox;
class BMenuField;
class PathBox;

class TemplateWindow : public BWindow {
public:
								TemplateWindow(const BRect& frame);
	virtual						~TemplateWindow(void);

	virtual	bool				QuitRequested(void);
	virtual	void				MessageReceived(BMessage* message);

private:
			void				CheckTemplates(void);
			BMenuField*			fTemplateField;

			AutoTextControl*	fNameBox;
			AutoTextControl*	fTargetBox;
			PathBox*			fPathBox;
			BCheckBox*			fCreateFolder;
		
			BButton*			fCreateProjectButton;
			TemplateManager		fTempList;
			BMenuField*			fSCMChooser;
};


#endif	// TEMPLATE_WINDOW_H
