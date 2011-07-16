#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <Entry.h>
#include <FilePanel.h>
#include <MenuBar.h>
#include <Menu.h>
#include <Message.h>
#include <StringView.h>

#include "DWindow.h"

class ErrorWindow;
class ProjectList;
class Project;
class SourceControl;
class SourceFile;

#include "ProjectBuilder.h"

class ProjectWindow;

typedef struct
{
	BMessage refmsg;
	ProjectWindow *parent;
	BPoint droppt;
} add_file_struct;

class ProjectWindow : public DWindow 
{
public:
						ProjectWindow(BRect frame, Project *proj = NULL);
						~ProjectWindow();
			bool    	QuitRequested();
			void		MessageReceived(BMessage *msg);
			void		MenusBeginning(void);
			void		MenusEnded(void);
			void		AddFile(const entry_ref &ref, BPoint *point = NULL);
			Project *	GetProject(void) const { return fProject; }
			
			bool		AreMenusLocked(void) const { return fMenusLocked; }
			bool		RequiresMenuLock(const int32 &command);
private:
			void		ActOnSelectedFiles(const int32 &command);
			void		SetupMenus(void);
			void		SetMenuLock(bool locked);
			void		MakeGroup(int32 selection);
			void		ToggleErrorWindow(ErrorList *list);
			void		ShowErrorWindow(ErrorList *list);
			void		CullEmptyGroups(void);
			void		UpdateDependencies(void);
			void		ToggleDebugMenu(void);
			
			void		DoBuild(int32 postbuild);
			void		AddNewFile(BString name, bool create_pair);
	static	int32		AddFileThread(void *data);
			void		AddFolder(entry_ref folderref);
			void		ImportFile(entry_ref ref);
	static	int32		ImportFileThread(void *data);
	static	int32		BackupThread(void *data);
	static	int32		SyncThread(void *data);
	
	
	ErrorWindow			*fErrorWindow;
	
	BMenuBar			*fMenuBar;
	BMenu				*fFileMenu,
						*fProjectMenu,
						*fBuildMenu,
						*fToolsMenu,
						*fSourceMenu,
						*fRecentMenu;
						
	ProjectList			*fProjectList;
	BStringView			*fStatusBar;
	BFilePanel			*fFilePanel;
	Project				*fProject;
	SourceControl		*fSourceControl;
	bool				fShowingLibs;
	bool				fMenusLocked;
	
	add_file_struct		fAddFileStruct;
	add_file_struct		fImportStruct;
	ProjectBuilder		fBuilder;
	int32				fBuildingFile;
};


#endif
