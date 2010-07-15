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
private:
	PathBox			*fProjectFolder,
					*fBackupFolder;
					
	BCheckBox		*fShowProjFolder,
					*fSlowBuilds,
					*fCCache,
					*fFastDep,
					*fAutoSyncModules;
	
	BMenuField		*fSCMChooser;
};

#endif
