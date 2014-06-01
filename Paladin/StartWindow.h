/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef START_WINDOW_H
#define START_WINDOW_H


#include <Window.h>


class BitmapButton;
class BFilePanel;
class BListView;
class TypedRefFilter;

class StartWindow : public BWindow {
public:
								StartWindow(void);
	virtual						~StartWindow(void);

	virtual	bool				QuitRequested(void);
	virtual	void				MessageReceived(BMessage* message);

private:
			BitmapButton*		fNewButton;
			BitmapButton*		fOpenButton;
			BitmapButton*		fOpenRecentButton;
			BitmapButton*		fQuickImportButton;
			BitmapButton*		fOnlineImportButton;

			BListView*			fRecentProjectsListView;
			BFilePanel*			fOpenPanel;
			BFilePanel*			fImportPanel;
};


#endif	// START_WINDOW_H
