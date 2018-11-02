/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef PREFS_WINDOW_H
#define PREFS_WINDOW_H


#include <Window.h>


class BCheckBox;
class BMenuField;
class BTabView;
class BView;

class PathBox;

class PrefsWindow : public BWindow {
public:
								PrefsWindow(BRect frame);
	virtual						~PrefsWindow();

	virtual	bool				QuitRequested(void);
	virtual	void				MessageReceived(BMessage* message);

private:

			PathBox*			fProjectFolder;
			BCheckBox*			fShowProjectFolder;
			BCheckBox*			fDontAddHeaders;

			BCheckBox*			fSlowBuilds;
			BCheckBox*			fCCache;
			BCheckBox*			fFastDep;

			BCheckBox*			fAutoSyncModules;

			PathBox*			fBackupFolder;

			BMenuField*			fSCMChooser;
			PathBox*			fSVNRepoFolder;

			BView*				fTabs[2];
			BTabView*			fTabView;
};


#endif	// PREFS_WINDOW_H
