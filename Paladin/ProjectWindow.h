/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H


#include <Entry.h>
#include <FilePanel.h>
#include <MenuBar.h>
#include <Menu.h>
#include <Message.h>
#include <StringView.h>
#include <Window.h>

#include "ProjectBuilder.h"
#include "ProjectStatus.h"
#include "ProjectSettingsWindow.h"

class ErrorWindow;
class ProjectList;
class Project;
class ProjectWindow;
class ProjectStatus;
class SourceControl;
class SourceFile;
class PrefsWindow;


typedef struct {
	BMessage		refMessage;
	ProjectWindow*	parent;
	BPoint			dropPoint;
} add_file_struct;


class ProjectWindow : public BWindow  {
public:
								ProjectWindow(BRect frame, Project* project = NULL);
	virtual						~ProjectWindow();

	virtual	bool		    	QuitRequested();
	virtual	void				MessageReceived(BMessage *message);
	virtual	void				MenusBeginning(void);
	virtual	void				MenusEnded(void);
	virtual	void				AddFile(const entry_ref &ref, BPoint* where = NULL);
			Project*			GetProject(void) const { return fProject; }

			bool				AreMenusLocked(void) const { return fMenusLocked; }
			bool				RequiresMenuLock(const int32 &command);

private:
			void				ActOnSelectedFiles(const int32 &command);
			void				CreateMenuBar();
			void				SetMenuLock(bool locked);
			void				MakeGroup(int32 selection);
			void				ToggleErrorWindow(ErrorList* list);
			void				ShowErrorWindow(ErrorList* list);
			void				CullEmptyGroups(void);
			void				SortGroup(int32 selection);
			void				UpdateProjectList(void);
			void				UpdateDependencies(void);
			void				ToggleDebugMenu(void);

			void				DoBuild(int32 postbuild);
			void				AddNewFile(BString name, bool createPair);
	static	int32				AddFileThread(void* data);
			void				AddFolder(entry_ref folderref);
			void				ImportFile(entry_ref ref);
	static	int32				ImportFileThread(void* data);
	static	int32				BackupThread(void* data);
	static	int32				SyncThread(void* data);
	
			void				SetStatus(const char* msg);

			ErrorWindow*		fErrorWindow;

			BMenuBar*			fMenuBar;
			BMenu*				fFileMenu;
			BMenu*				fProjectMenu;
			BMenu*				fBuildMenu;
			BMenu*				fToolsMenu;
			BMenu*				fSourceMenu;
			BMenu*				fRecentMenu;

			ProjectList*		fProjectList;
			ProjectStatus*		fStatusBar;
			BFilePanel*			fFilePanel;
			Project*			fProject;
			SourceControl*		fSourceControl;
			ProjectSettingsWindow*	fProjectSettingsWindow;

			bool				fShowingLibs;
			bool				fMenusLocked;

			add_file_struct		fAddFileStruct;
			add_file_struct		fImportStruct;
			ProjectBuilder		fBuilder;
			int32				fBuildingFile;
			
			PrefsWindow*		fPrefsWindow;
};


#endif	// PROJECT_WINDOW_H
