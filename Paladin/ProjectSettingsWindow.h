/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef PROJECT_SETTINGS_WINDOW_H
#define PROJECT_SETTINGS_WINDOW_H


#include <Autolock.h>
#include <Button.h>
#include <CheckBox.h>
#include <FilePanel.h>
#include <ListView.h>
#include <MenuField.h>
#include <RadioButton.h>
#include <TabView.h>
#include <Window.h>

#include "AutoTextControl.h"


class RefListView;
class Project;
class TypedRefFilter;

/*
	Settings:
	
	target type
	Include paths
	Target name
	
	Debug mode
	Profile mode
	Op for size
	Op level
	extra cc opts
	extra ld opts
*/

class ProjectSettingsWindow : public BWindow {
public:
								ProjectSettingsWindow(BRect frame, Project* project);
	virtual						~ProjectSettingsWindow(void);

	virtual	void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested(void);

private:
			void				AddInclude(const entry_ref &ref);
			
			Project*			fProject;

			BFilePanel*			fFilePanel;
			TypedRefFilter*		fRefFilter;

			BTabView*			fTabView;
			BView*				fBuildView;
			BView*				fGeneralView;

	// General Options
			AutoTextControl*	fTargetText;
			RefListView*		fIncludeList;
			BMenuField*			fTypeField;

	// Build Options
			BCheckBox*			fDebugBox;
			BCheckBox*			fProfileBox;

			BMenuField*			fOpField;
			BCheckBox*			fOpSizeBox;

			AutoTextControl*	fCompileText;
			AutoTextControl*	fLinkText;

			BAutolock*			fAutolock;

			bool				fDirty;
};


#endif	// PROJECT_SETTINGS_WINDOW_H
