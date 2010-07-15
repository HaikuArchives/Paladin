#ifndef CODELIB_WIN_H
#define CODELIB_WIN_H

#include "DWindow.h"

#include <Button.h>
#include <FilePanel.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>

#include "AutoTextControl.h"
#include "CodeLib.h"

class AddFileView;
class DListView;
class RefListView;
class Project;

class CodeLibWindow : public DWindow
{
public:
	static	CodeLibWindow *	GetInstance(BRect frame);
							~CodeLibWindow(void);
			void			MessageReceived(BMessage *msg);
	
private:
							CodeLibWindow(BRect frame);
						
			void			SetupMenus(void);
			void			SetupContextMenus(void);
			void			ScanModules(void);
			void			ShowModule(const char *name);
			void			ExportModule(void);
		
		BMenuBar		*fBar;
		BMenu			*fProjectMenu;
		DListView		*fModList;
		RefListView		*fFileList;
		CodeModule		*fCurrentModule;
		Project			*fCurrentProject;
					
		AutoTextControl	*fDescription;
		BView			*fMainView;
		AddFileView		*fAddView;
		BFilePanel		*fFilePanel;
};

#endif
