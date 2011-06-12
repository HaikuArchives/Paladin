#ifndef PREFS_WINDOW_H
#define PREFS_WINDOW_H

#include <MenuField.h>

#include "DWindow.h"
#include "CheckBox.h"

class PathBox;

class PrefsWindow : public DWindow
{
public:
						PrefsWindow(BRect frame);
				bool	QuitRequested(void);
				void	MessageReceived(BMessage *msg);
private:
	PathBox			*fProjectFolder,
					*fBackupFolder;
					
	BCheckBox		*fShowProjFolder,
					*fDontAddHeaders,
					*fSlowBuilds,
					*fCCache,
					*fFastDep,
					*fAutoSyncModules;
	
	BMenuField		*fSCMChooser,
					*fViewChooser;
	
	BView			*fViews[2],
					*fActiveView;
};

#endif
