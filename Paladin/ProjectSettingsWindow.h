#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "DWindow.h"
#include <Autolock.h>
#include <Button.h>
#include <CheckBox.h>
#include <FilePanel.h>
#include <ListView.h>
#include <MenuField.h>
#include <RadioButton.h>
#include <TabView.h>

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

class ProjectSettingsWindow : public DWindow
{
public:
						ProjectSettingsWindow(BRect frame, Project *proj);
						~ProjectSettingsWindow(void);
				void	MessageReceived(BMessage *msg);
				bool	QuitRequested(void);
private:
				void	AddInclude(const entry_ref &ref);
				
	Project			*fProject;
	
	BFilePanel		*fFilePanel;
	TypedRefFilter	*fRefFilter;
	
	BTabView		*fTabView;
	BView			*fBuildView;
	BView			*fGeneralView;
	
	// General Options
	AutoTextControl	*fTargetText;
	RefListView		*fIncludeList;
	BMenuField		*fTypeField;
	
	// Build Options
	BCheckBox		*fDebugBox,
					*fProfileBox;
	
	BMenuField		*fOpField;
	BCheckBox		*fOpSizeBox;
	
	AutoTextControl	*fCompileText,
					*fLinkText;
	
	BAutolock		*fAutolock;
	
	bool			fDirty;
};

#endif
