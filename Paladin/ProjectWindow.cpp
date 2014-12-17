/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "ProjectWindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <Alert.h>
#include <Application.h>
#include <Deskbar.h>
#include <Entry.h>
#include <FilePanel.h>
#include <Font.h>
#include <InterfaceDefs.h>
#include <LayoutBuilder.h>
#include <MenuItem.h>
#include <Node.h>
#include <OS.h>
#include <Roster.h>
#include <Screen.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <TypeConstants.h>
#include <View.h>

#include "AddNewFileWindow.h"
#include "AltTabFilter.h"
#include "AppDebug.h"
#include "AsciiWindow.h"
#include "CodeLibWindow.h"
#include "DebugTools.h"
#include "ErrorParser.h"
#include "ErrorWindow.h"
#include "FileActions.h"
#include "FileFactory.h"
#include "FindOpenFileWindow.h"
#include "FindWindow.h"
#include "GetTextWindow.h"
#include "Globals.h"
#include "GroupRenameWindow.h"
#include "LaunchHelper.h"
#include "LibWindow.h"
#include "LicenseManager.h"
#include "Makemake.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "PrefsWindow.h"
#include "ProjectBuilder.h"
#include "ProjectList.h"
#include "ProjectSettingsWindow.h"
#include "Project.h"
#include "RunArgsWindow.h"
#include "SCMManager.h"
#include "SCMOutputWindow.h"
#include "Settings.h"
#include "SourceFile.h"
#include "VRegWindow.h"


enum {
	M_NEW_WINDOW				= 'nwwn',
	M_SHOW_PROGRAM_SETTINGS		= 'sprs',
	M_SHOW_PROJECT_SETTINGS		= 'spjs',
	M_SHOW_ASCII_TABLE			= 'sast',
	M_SHOW_VREGEX				= 'svrx',
	M_SHOW_RUN_ARGS				= 'srag',
	M_SHOW_LIBRARIES			= 'slbw',
	M_SHOW_PROJECT_FOLDER		= 'shpf',
	M_RUN_TOOL					= 'rntl',
	M_UPDATE_DEPENDENCIES		= 'updp',
	M_BUILD_PROJECT				= 'blpj',
	M_DEBUG_PROJECT				= 'PRnD',
	M_EDIT_FILE					= 'edfl',
	M_ADD_NEW_FILE				= 'adnf',
	M_SHOW_ADD_PANEL			= 'shap',
	M_RENAME_GROUP				= 'rngr',
	M_SHOW_LICENSES				= 'shlc',
	M_MAKE_MAKE					= 'mkmk',
	M_SHOW_CODE_LIBRARY			= 'shcl',
	M_SYNC_MODULES				= 'synm',

	M_GET_CHECK_IN_MSG			= 'gcim',
	M_CHECK_IN_PROJECT			= 'prci',
	M_REVERT_PROJECT			= 'prrv',
	M_PUSH_PROJECT				= 'pshp',
	M_PULL_PROJECT				= 'pulp',
	M_DIFF_PROJECT				= 'dfpj',
	M_PROJECT_SCM_STATUS		= 'pscs',

	M_TOGGLE_DEBUG_MENU			= 'sdbm',
	M_DEBUG_DUMP_DEPENDENCIES	= 'dbdd',
	M_DEBUG_DUMP_INCLUDES		= 'dbdi'
};


static int
compare_source_file_items(const BListItem* item1, const BListItem* item2);


ProjectWindow::ProjectWindow(BRect frame, Project* project)
	:
	BWindow(frame, "Paladin", B_DOCUMENT_WINDOW,
		B_NOT_ZOOMABLE),
	fErrorWindow(NULL),
	fFilePanel(NULL),
	fProject(project),
	fSourceControl(NULL),
	fShowingLibs(false),
	fMenusLocked(false),
	fBuilder(BMessenger(this))
{
	AddCommonFilter(new AltTabFilter());
	SetSizeLimits(200, 30000, 200, 30000);
	RegisterWindow();

	// This is for our in-program debug menu which comes in handy now and then.
	// Paladin -d doesn't always get the job done
	AddShortcut('9', B_COMMAND_KEY | B_SHIFT_KEY | B_CONTROL_KEY,
		new BMessage(M_TOGGLE_DEBUG_MENU));

	if (fProject != NULL) {
		fSourceControl = GetSCM(fProject->SourceControl());
		if (fSourceControl != NULL) {
			if (gPrintDebugMode > 0)
				fSourceControl->SetDebugMode(true);

			fSourceControl->SetUpdateCallback(SCMOutputCallback);
			fSourceControl->SetWorkingDirectory(fProject->GetPath().GetFolder());

			if (fSourceControl->NeedsInit(fProject->GetPath().GetFolder()))
				fSourceControl->CreateRepository(fProject->GetPath().GetFolder());
		}
	}

	CreateMenuBar();

	fProjectList = new ProjectList(fProject, "filelist", B_WILL_DRAW);
	fProjectList->SetInvocationMessage(new BMessage(M_EDIT_FILE));

	BScrollView* fileListScrollView = new BScrollView("filelistscrollview",
		fProjectList, 0, false, true, B_NO_BORDER);

	fStatusBar = new BStringView("statusbar", NULL);
	fStatusBar->SetFontSize(10.0f);
	fStatusBar->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_1_TINT));
	fStatusBar->SetExplicitMinSize(BSize(B_H_SCROLL_BAR_HEIGHT + 1, B_SIZE_UNSET));

	if (project != NULL) {
		BString title("Paladin: ");
		title << project->GetName();
		SetTitle(title.String());

		for (int32 i = 0; i < project->CountGroups(); i++) {
			SourceGroup* group = project->GroupAt(i);
			SourceGroupItem* groupitem = new SourceGroupItem(group);
			fProjectList->AddItem(groupitem);
			groupitem->SetExpanded(group->expanded);

			for (int32 j = 0; j < group->filelist.CountItems(); j++) {
				SourceFile* file = group->filelist.ItemAt(j);
				SourceFileItem* fileitem = new SourceFileItem(file,1);

//				fProjectList->AddUnder(fileitem, groupitem);
				fProjectList->AddItem(fileitem);

				BString abspath = file->GetPath().GetFullPath();
				if (abspath[0] != '/') {
					abspath.Prepend("/");
					abspath.Prepend(project->GetPath().GetFolder());
				}
				BEntry entry(abspath.String());
				if (entry.Exists()) {
					if (project->CheckNeedsBuild(file,false)) {
						fileitem->SetDisplayState(SFITEM_NEEDS_BUILD);
						fProjectList->InvalidateItem(fProjectList->IndexOf(fileitem));
					} else
						file->SetBuildFlag(BUILD_NO);
				} else {
					fileitem->SetDisplayState(SFITEM_MISSING);
					fProjectList->InvalidateItem(fProjectList->IndexOf(fileitem));
				}
			}
		}
	}

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(fMenuBar)
		.Add(fileListScrollView)
		.Add(fStatusBar)
		.SetInsets(0, 0, -1, 0)
			// hack to get scroll view frame to line up
		.End();

	BNode node(fProject->GetPath().GetFullPath());
	if (node.ReadAttr("project_frame", B_RECT_TYPE, 0, &frame, sizeof(BRect))) {
		if (frame.Width() < 200)
			frame.right = frame.left + 200;

		if (frame.Height() < 200)
			frame.top = frame.bottom + 200;

		MoveTo(frame.LeftTop());
		ResizeTo(frame.Width(), frame.Height());
	}

	fProjectList->SetTarget(this);
	fProjectList->MakeFocus(true);

	if (gShowFolderOnOpen) {
		// Duplicated code from MessageReceived::M_SHOW_PROJECT_FOLDER. Doing
		// it here in combo with snooze() makes it much more likely that the
		// opened project window is frontmost instead of the project folder's
		// window.
		entry_ref ref;
		BEntry(fProject->GetPath().GetFolder()).GetRef(&ref);
		BMessenger messenger("application/x-vnd.Be-TRAK");

		BMessage reply;
		BMessage openMessage(B_REFS_RECEIVED);
		openMessage.AddRef("refs", &ref);
		messenger.SendMessage(&openMessage);
		snooze(50000);
	}

	if (gAutoSyncModules)
		PostMessage(M_SYNC_MODULES);
}


ProjectWindow::~ProjectWindow()
{
	if (gAutoSyncModules)
		ProjectWindow::SyncThread(this);

	gProjectList->Lock();

	int32 index = gProjectList->IndexOf(fProject);
	gProjectList->RemoveItemAt(index);
	gProjectList->Unlock();

	BNode node(fProject->GetPath().GetFullPath());
	BRect frame(Frame());
	node.WriteAttr("project_frame", B_RECT_TYPE, 0, &frame, sizeof(BRect));

	delete fFilePanel;
}


bool
ProjectWindow::QuitRequested()
{
	if (fShowingLibs) {
		BString title("Libraries: ");
		title << fProject->GetName();
		for (int32 i = 0; i < be_app->CountWindows(); i++) {
			BWindow* window = be_app->WindowAt(i);
			if (title.Compare(window->Title()) == 0) {
				window->PostMessage(B_QUIT_REQUESTED);
				break;
			}
		}
	}

	fProject->Save();

	if (fErrorWindow != NULL) {
		fErrorWindow->Quit();
		fErrorWindow = NULL;
	}

	DeregisterWindow();

	return true;
}


void
ProjectWindow::MessageReceived(BMessage* message)
{
	status_t status;

	if ((message->WasDropped() && message->what == B_SIMPLE_DATA)
		|| message->what == M_ADD_FILES) {
		fAddFileStruct.refMessage = *message;
		fAddFileStruct.parent = this;

		uint32 buttons;
		fProjectList->GetMouse(&fAddFileStruct.dropPoint, &buttons);

		thread_id addThread = spawn_thread(AddFileThread, "file adding thread",
			B_NORMAL_PRIORITY, &fAddFileStruct);
		if (addThread >= 0)
			resume_thread(addThread);
	}

	switch (message->what) {
		case M_IMPORT_REFS:
		{
			fImportStruct.refMessage = *message;
			fImportStruct.parent = this;

			thread_id importThread = spawn_thread(ImportFileThread,
				"file import thread", B_NORMAL_PRIORITY, &fImportStruct);
			if (importThread >= 0)
				resume_thread(importThread);

			break;
		}

		case M_BACKUP_PROJECT:
		{
			thread_id backupThread = spawn_thread(BackupThread,
				"project backup thread", B_NORMAL_PRIORITY, this);
			if (backupThread >= 0)
			{
				fStatusBar->SetText(TR("Backing up project"));
				UpdateIfNeeded();
				
				SetMenuLock(true);
				resume_thread(backupThread);
			}
			break;
		}

		case M_GET_CHECK_IN_MSG:
		{
			if (!fSourceControl) {
				printf("NULL source control\n");
				break;
			}

			BString out;
			fSourceControl->GetCheckinHeader(out);

			bool select = false;
			if (out.CountChars() > 1)
				out.Prepend("\n\n");
			else {
				out = TR("Enter the description for the changes in this revision.");
				select = true;
			}

			GetTextWindow* getTextWindow = new GetTextWindow("Paladin", out.String(),
				BMessage(M_CHECK_IN_PROJECT), BMessenger(this));
			if (!select)
				getTextWindow->GetTextView()->Select(0, 0);

			getTextWindow->Show();
			break;
		}

		case M_CHECK_IN_PROJECT:
		{
			BString commitMessage;
			if (fSourceControl
				&& message->FindString("text", &commitMessage) == B_OK) {
				SCMOutputWindow* window = new SCMOutputWindow(TR("Commit"));
				window->Show();
				fSourceControl->Commit(commitMessage.String());
			}
			break;
		}

		case M_REVERT_PROJECT:
		{
			if (!fSourceControl)
				break;
			
			int32 result = ShowAlert(
				TR("This will undo all changes since the last commit. "
					"Continue?"), TR("Don't revert"), TR("Revert"));
			if (result == 1) {
				SCMOutputWindow* window = new SCMOutputWindow(TR("Revert"));
				window->Show();
				fSourceControl->Revert(NULL);
			}
			break;
		}

		case M_REBUILD_FILE:
		case M_ADD_SELECTION_TO_REPO:
		case M_REMOVE_SELECTION_FROM_REPO:
		case M_REVERT_SELECTION:
		case M_DIFF_SELECTION:
		{
			ActOnSelectedFiles(message->what);
			break;
		}

		case M_DIFF_PROJECT:
		{
			if (fSourceControl) {
				SCMOutputWindow* window = new SCMOutputWindow(TR("Differences"));
				window->Show();
				fSourceControl->Diff(NULL);
			}
			break;
		}

		case M_PROJECT_SCM_STATUS:
		{
			if (fSourceControl) {
				SCMOutputWindow* window = new SCMOutputWindow(TR("Project status"));
				BString strstatus;
				fSourceControl->GetChangeStatus(strstatus);
				window->GetTextView()->SetText(strstatus.String());
				window->Show();
			}
			break;
		}

		case M_PUSH_PROJECT:
		{
			if (fSourceControl) {
				SCMOutputWindow* window = new SCMOutputWindow(TR("Push"));
				window->Show();
				fSourceControl->Push(NULL);
			}
			break;
		}

		case M_PULL_PROJECT:
		{
			if (fSourceControl) {
				SCMOutputWindow* window = new SCMOutputWindow(TR("Pull"));
				window->Show();
				status = fSourceControl->Pull(NULL);

				if (status != B_OK) {
					ShowAlert("Unable to pull from the remote repository. If it "
						"uses a secure connection, please set up the appropriate "
						"SSH keys on the remote server.", "OK");
				}
			}
			break;
		}

		case M_CULL_EMPTY_GROUPS:
		{
			CullEmptyGroups();
			break;
		}

		case M_RUN_FILE_TYPES:
		{
			int32 selection = fProjectList->FullListCurrentSelection();
			if (selection < 0)
				break;

			SourceFileItem* item = dynamic_cast<SourceFileItem*>(
				fProjectList->FullListItemAt(selection));
			if (item == NULL)
				break;

			SpawnFileTypes(item->GetData()->GetPath());
			break;
		}

		case M_OPEN_PARENT_FOLDER:
		{
			BMessage openMessage(B_REFS_RECEIVED);
			int32 selectionIndex = 0;
			int32 selection = fProjectList->FullListCurrentSelection();
			selectionIndex++;
			if (selection >= 0) {
				while (selection >= 0) {
					SourceFileItem* item = dynamic_cast<SourceFileItem*>(
						fProjectList->FullListItemAt(selection));
					if (item == NULL)
						break;

					SourceFile* file = item->GetData();
					BString abspath = file->GetPath().GetFullPath();
					if (abspath[0] != '/') {
						abspath.Prepend("/");
						abspath.Prepend(fProject->GetPath().GetFolder());
					}
					DPath filepath(abspath);

					entry_ref ref;
					BEntry(filepath.GetFolder()).GetRef(&ref);

					openMessage.AddRef("refs", &ref);
					selection = fProjectList->FullListCurrentSelection(
						selectionIndex++);
				}

				BMessenger messenger("application/x-vnd.Be-TRAK");
				messenger.SendMessage(&openMessage);
			}
			break;
		}

		case M_SHOW_PROJECT_FOLDER:
		{
			entry_ref ref;
			BEntry(fProject->GetPath().GetFolder()).GetRef(&ref);
			BMessenger messenger("application/x-vnd.Be-TRAK");

			BMessage openMessage(B_REFS_RECEIVED);
			openMessage.AddRef("refs",&ref);
			messenger.SendMessage(&openMessage);
			break;
		}

		case M_SHOW_ASCII_TABLE:
		{
			AsciiWindow* window = new AsciiWindow();
			window->Show();
			break;
		}

		case M_SHOW_VREGEX:
		{
			VRegWindow* window = new VRegWindow();
			window->Show();
			break;
		}

		case M_SHOW_LICENSES:
		{
			LicenseManager* manager
				= new LicenseManager(fProject->GetPath().GetFolder());
			manager->Show();
			break;
		}

		case M_RUN_TOOL:
		{
			BString signature;
			if (message->FindString("signature", &signature) == B_OK) {
				LaunchHelper launcher(signature.String());
				launcher.Launch();
			}
			break;
		}

		case M_MAKE_MAKE:
		{
			DPath out(fProject->GetPath().GetFolder());
			out.Append("Makefile");
			if (MakeMake(fProject, out) == B_OK); {
				BEntry entry(out.GetFullPath());
				entry_ref ref;
				if (entry.InitCheck() == B_OK) {
					entry.GetRef(&ref);
					BMessage refMessage(B_REFS_RECEIVED);
					refMessage.AddRef("refs",&ref);
					be_app->PostMessage(&refMessage);
				}
			}
			break;
		}

		case M_SHOW_CODE_LIBRARY:
		{
#ifdef BUILD_CODE_LIBRARY
			CodeLibWindow* window
				= CodeLibWindow::GetInstance(BRect(100, 100, 500, 350));
			window->Show();
#endif
			break;
		}

		case M_OPEN_PARTNER:
		{
			int32 selection = fProjectList->FullListCurrentSelection();
			if (selection < 0)
				break;

			SourceFileItem* item = dynamic_cast<SourceFileItem*>(
				fProjectList->FullListItemAt(selection));
			if (item == NULL)
				break;

			entry_ref ref;
			BEntry(fProject->GetPathForFile(
				item->GetData()).GetFullPath()).GetRef(&ref);
			BMessage refMessage(M_OPEN_PARTNER);
			refMessage.AddRef("refs", &ref);
			be_app->PostMessage(&refMessage);
			break;
		}

		case M_NEW_GROUP:
		{
			MakeGroup(fProjectList->FullListCurrentSelection());
			PostMessage(M_SHOW_RENAME_GROUP);
			break;
		}

		case M_SHOW_RENAME_GROUP:
		{
			int32 selection = fProjectList->FullListCurrentSelection();
			SourceGroupItem* groupItem = NULL;
			if (selection < 0) {
				// Don't need a selection if there is only one group in the project
				if (fProject->CountGroups() == 1)
					groupItem = fProjectList->ItemForGroup(fProject->GroupAt(0));
			} else {
				BStringItem *stringItem
					= (BStringItem*)fProjectList->FullListItemAt(selection);
				groupItem = fProjectList->GroupForItem(stringItem);
			}
			if (groupItem == NULL)
				break;

			GroupRenameWindow *window = new GroupRenameWindow(groupItem->GetData(),
				BMessage(M_RENAME_GROUP), BMessenger(this));
			window->Show();
			break;
		}

		case M_RENAME_GROUP:
		{
			SourceGroup* group;
			BString newName;
			if (message->FindPointer("group", (void**)&group) != B_OK
				|| message->FindString("newname", &newName) != B_OK) {
				break;
			}

			group->name = newName;
			SourceGroupItem* groupItem = fProjectList->ItemForGroup(group);
			if (groupItem == NULL)
				break;

			groupItem->SetText(newName.String());
			fProjectList->InvalidateItem(fProjectList->IndexOf(groupItem));

			fProject->Save();
			break;
		}

		case M_SORT_GROUP:
		{
			int32 selection = fProjectList->FullListCurrentSelection();
			
			SourceGroupItem* groupItem = NULL;
			if (selection < 0) {
				// Don't need a selection if there is only one group in the project
				if (fProject->CountGroups() == 1)
					groupItem = fProjectList->ItemForGroup(fProject->GroupAt(0));
			} else {
				BStringItem* stringItem
					= (BStringItem*)fProjectList->FullListItemAt(selection);
				groupItem = fProjectList->GroupForItem(stringItem);
			}
			if (groupItem == NULL)
				break;

			fProjectList->SortItemsUnder(groupItem, true, compare_source_file_items);
			groupItem->GetData()->Sort();
			fProject->Save();

			break;
		}

		case M_TOGGLE_ERROR_WINDOW:
		{
			ToggleErrorWindow(fProject->GetErrorList());
			break;
		}

		case M_SHOW_ERROR_WINDOW:
		{
			ShowErrorWindow(fProject->GetErrorList());
			break;
		}

		case M_SHOW_PROJECT_SETTINGS:
		{
			BRect r(0, 0, 300, 200);
			BRect screen(BScreen().Frame());

			r.OffsetTo((screen.Width() - r.Width()) / 2.0,
				(screen.Height() - r.Height()) / 2.0);
			
			ProjectSettingsWindow* window
				= new ProjectSettingsWindow(r, fProject);
			window->Show();
			break;
		}

		case M_SHOW_RUN_ARGS:
		{
			RunArgsWindow* window = new RunArgsWindow(fProject);
			window->Show();
			break;
		}

		case M_JUMP_TO_MSG:
		{
			entry_ref ref;
			if (message->FindRef("refs", &ref) == B_OK) {
				message->what = B_REFS_RECEIVED;
				be_app->PostMessage(message);
			}
			break;
		}

		case B_ABOUT_REQUESTED:
		{
			be_app->PostMessage(B_ABOUT_REQUESTED);
			break;
		}

		case M_SHOW_OPEN_PROJECT:
		{
			be_app->PostMessage(message);
			break;
		}

		case M_NEW_WINDOW:
		{
			be_app->PostMessage(M_NEW_PROJECT);
			break;
		}

		case M_SHOW_PROGRAM_SETTINGS:
		{
			PrefsWindow* window = new PrefsWindow(BRect(0, 0, 500, 400));
			window->Show();
			break;
		}

		case M_SHOW_FIND_AND_OPEN_PANEL:
		{
			BString text;
			message->FindString("name",&text);
			
			// Passing a NULL string to this is OK
			FindOpenFileWindow* window = new FindOpenFileWindow(text.String());
			window->Show();
			break;
		}

		case M_FILE_NEEDS_BUILD:
		{
			SourceFile* file;
			if (message->FindPointer("file", (void**)&file) == B_OK) {
				SourceFileItem* item = fProjectList->ItemForFile(file);
				if (item != NULL) {
					item->SetDisplayState(SFITEM_NEEDS_BUILD);
					fProjectList->InvalidateItem(fProjectList->IndexOf(item));
				}
			}
			break;
		}

		case M_EDIT_FILE:
		{
			int32 i = 0;
			int32 selection = fProjectList->FullListCurrentSelection(i);
			i++;

			BMessage refMessage(B_REFS_RECEIVED);
			while (selection >= 0) {
				SourceFileItem* item = dynamic_cast<SourceFileItem*>(
					fProjectList->FullListItemAt(selection));
				if (item != NULL && item->GetData()) {
					BString abspath = item->GetData()->GetPath().GetFullPath();
					if (abspath[0] != '/') {
						abspath.Prepend("/");
						abspath.Prepend(fProject->GetPath().GetFolder());
					}

					BEntry entry(abspath.String());
					if (entry.InitCheck() == B_OK) {
						entry_ref ref;
						entry.GetRef(&ref);
						refMessage.AddRef("refs",&ref);
					} else {
						if (!entry.Exists()) {
							BString errorMessage = TR("Couldn't find XXXXX. "
								"It may have been moved or renamed.");
							errorMessage.ReplaceFirst("XXXXX", abspath.String());
							ShowAlert(errorMessage.String());
						}
					}
				} else {
					SourceGroupItem* groupItem = dynamic_cast<SourceGroupItem*>(
						fProjectList->FullListItemAt(selection));
					if (groupItem) {
						if (groupItem->IsExpanded())
							fProjectList->Collapse(groupItem);
						else
							fProjectList->Expand(groupItem);

						groupItem->GetData()->expanded = groupItem->IsExpanded();
					}
				}

				selection = fProjectList->CurrentSelection(i);
				i++;
			}
			be_app->PostMessage(&refMessage);
			break;
		}

		case M_LIBWIN_CLOSED:
		{
			fShowingLibs = false;
			break;
		}

		case M_SHOW_LIBRARIES:
		{
			fShowingLibs = true;
			LibraryWindow* window = new LibraryWindow(Frame().OffsetByCopy(15, 15),
				BMessenger(this), fProject);
			window->Show();
			break;
		}

		case M_SHOW_ADD_NEW_PANEL:
		{
			AddNewFileWindow* window = new AddNewFileWindow(BMessage(M_ADD_NEW_FILE),
				BMessenger(this));
			window->Show();
			break;
		}

		case M_SHOW_FIND_IN_PROJECT_FILES:
		{
			if (!gLuaAvailable) {
				ShowAlert(TR("Paladin's multi-file Find window depends on Lua. "
					"It will need to be installed if you wish to use "
					"this feature."), "OK", NULL, NULL, B_STOP_ALERT);
				break;
			}

			FindWindow* window = new FindWindow();
			window->Show();
			break;
		}

		case M_ADD_NEW_FILE:
		{
			BString name;
			bool makepair;
			if (message->FindString("name", &name) == B_OK
				&& message->FindBool("makepair",&makepair) == B_OK) {
				AddNewFile(name,makepair);
			}
			break;
		}

		case M_SHOW_ADD_PANEL:
		{
			if (fFilePanel == NULL) {
				BMessenger messenger(this);
				BEntry entry(fProject->GetPath().GetFolder());
				entry_ref ref;
				entry.GetRef(&ref);
				fFilePanel = new BFilePanel(B_OPEN_PANEL, &messenger, &ref,
					B_FILE_NODE, true, new BMessage(M_ADD_FILES));
			}
			fFilePanel->Show();
			break;
		}

		case M_REMOVE_FILES:
		{
			bool save = false;
			
			for (int32 i = 0; i < fProjectList->CountItems(); i++) {
				SourceFileItem* item = dynamic_cast<SourceFileItem*>(
					fProjectList->ItemAt(i));
				if (item != NULL && item->IsSelected()) {
					fProjectList->RemoveItem(item);
					fProject->RemoveFile(item->GetData());
					delete item;
					save = true;
					i--;
				}
			}
			CullEmptyGroups();
			if (save)
				fProject->Save();

			break;
		}

		case M_EMPTY_CCACHE:
		{
			// We don't do this when forcing a rebuild of the sources because
			// sometimes it can take quite a while
			if (gUseCCache && gCCacheAvailable) {
				fStatusBar->SetText(TR("Emptying build cache"));
				UpdateIfNeeded();
				system("ccache -c > /dev/null");
				fStatusBar->SetText("");
				UpdateIfNeeded();
			}
			break;
		}

		case M_FORCE_REBUILD:
		{
			fProject->ForceRebuild();
			
			for (int32 i = 0; i < fProjectList->FullListCountItems(); i++) {
				SourceFileItem* item = dynamic_cast<SourceFileItem*>(
					fProjectList->FullListItemAt(i));
				if (item == NULL)
					continue;

				SourceFile* file = item->GetData();
				if (file->UsesBuild()) {
					item->SetDisplayState(SFITEM_NEEDS_BUILD);
					fProjectList->InvalidateItem(i);
				}
			}

			// This is necessary because InvalidateItem() uses indices from ItemAt(),
			// not FullListItemAt
			fProjectList->Invalidate();
			break;
		}

		case M_UPDATE_DEPENDENCIES:
		{
			UpdateDependencies();
			break;
		}

		case M_MAKE_PROJECT:
		case M_BUILD_PROJECT:
		{
			fBuildingFile = 0;
			DoBuild(POSTBUILD_NOTHING);
			break;
		}

		case M_RUN_PROJECT:
		{
			DoBuild(POSTBUILD_RUN);
			break;
		}

		case M_RUN_IN_TERMINAL:
		{
			DoBuild(POSTBUILD_RUN_IN_TERMINAL);
			break;
		}

		case M_DEBUG_PROJECT:
		{
			if (!fProject->Debug()) {
				BString errorMessage
					= TR("Your project does not have debugging information compiled "
					"in and will need to be rebuilt to debug. "
					"Do you wish to rebuild and run the debugger?");
				int32 result = ShowAlert(
					"Debugging information needs to compiled into "
					"your project. This may take some time for large "
					"projects. Do you wish to rebuild and run "
					"the debugger?", "Rebuild", "Cancel");
				if (result == 1)
					break;

				fProject->SetDebug(true);
				fProject->Save();
				fProject->ForceRebuild();
			}

			DoBuild(POSTBUILD_DEBUG);
			break;
		}

		case M_EXAMINING_FILE:
		{
			SourceFile* file;
			if (message->FindPointer("file",(void**)&file) == B_OK) {
				BString out;
				out << TR("Examining ") << file->GetPath().GetFileName();
				fStatusBar->SetText(out.String());
			}
			break;
		}

		case M_BUILDING_FILE:
		{
			SourceFile* file;
			if (message->FindPointer("sourcefile",(void**)&file) == B_OK) {
				SourceFileItem *item = fProjectList->ItemForFile(file);
				if (item != NULL) {
					item->SetDisplayState(SFITEM_BUILDING);
					fProjectList->InvalidateItem(fProjectList->IndexOf(item));

					BString out;
					int32 count;
					int32 total;
					if (message->FindInt32("count", &count) == B_OK
						&& message->FindInt32("total", &total) == B_OK)
					{
						fBuildingFile = MAX(fBuildingFile, count);
						out << "(" << fBuildingFile << "/" << total << ") ";
					}

					out << TR("Building ") << item->Text();
					fStatusBar->SetText(out.String());
				}
			}
			break;
		}

		case M_BUILDING_DONE:
		{
			SourceFile* file;
			if (message->FindPointer("sourcefile", (void**)&file) == B_OK)
			{
				SourceFileItem* item = fProjectList->ItemForFile(file);
				if (item != NULL) {
					item->SetDisplayState(SFITEM_NORMAL);
					fProjectList->InvalidateItem(fProjectList->IndexOf(item));
				}
			}
			break;
		}

		case M_LINKING_PROJECT:
		{
			fStatusBar->SetText(TR("Linking"));
			break;
		}

		case M_UPDATING_RESOURCES:
		{
			fStatusBar->SetText(TR("Updating resources"));
			break;
		}

		case M_DOING_POSTBUILD:
		{
			fStatusBar->SetText(TR("Performing post-build tasks"));
			break;
		}

		case M_BUILD_FAILURE:
			SetMenuLock(false);
			// fall-through
		case M_BUILD_MESSAGES:
		case M_BUILD_WARNINGS:
		{
			if (fErrorWindow == NULL) {
				BRect screen(BScreen().Frame());
				BRect r(screen);
				r.left = r.right / 4.0;
				r.right *= .75;
				r.top = r.bottom - 200;

				BDeskbar deskbar;
				if (deskbar.Location() == B_DESKBAR_BOTTOM)
					r.OffsetBy(0,-deskbar.Frame().Height());

				fErrorWindow = new ErrorWindow(r, this);
				fErrorWindow->Show();
			} else {
				if (!fErrorWindow->IsFront())
					fErrorWindow->Activate();
			}
			fStatusBar->SetText("");

			// Should this be an Unflatten or an Append?
			ErrorList* errorList = fProject->GetErrorList();
			errorList->Unflatten(*message);
			fErrorWindow->PostMessage(message);
			break;
		}

		case M_BUILD_SUCCESS:
		{
			SetMenuLock(false);
			fStatusBar->SetText("");
			break;
		}

		case M_ERRORWIN_CLOSED:
		{
			fErrorWindow = NULL;
			break;
		}

		case M_SYNC_MODULES:
		{
#ifdef BUILD_CODE_LIBRARY
			thread_id syncID = spawn_thread(SyncThread, "module update thread",
				B_NORMAL_PRIORITY, this);
			if (syncID >= 0)
				resume_thread(syncID);
#endif
			break;
		}

		case M_TOGGLE_DEBUG_MENU:
		{
			ToggleDebugMenu();
			break;
		}

		case M_DEBUG_DUMP_DEPENDENCIES:
		{
			DumpDependencies(fProject);
			break;
		}

		case M_DEBUG_DUMP_INCLUDES:
		{
			DumpIncludes(fProject);
			break;
		}

		default:
			BWindow::MessageReceived(message);
	}
}


void
ProjectWindow::MenusBeginning(void)
{
	int32 index = 0;
	entry_ref ref;

	gSettings.Lock();

	while (gSettings.FindRef("recentitems", index++, &ref) == B_OK) {
		if (!BEntry(&ref).Exists()) {
			index--;
			gSettings.RemoveData("recentitems",index);
		} else {
			DPath refpath(ref);
			BMessage* refMessage = new BMessage(B_REFS_RECEIVED);
			refMessage->AddRef("refs", &ref);
			fRecentMenu->AddItem(new BMenuItem(refpath.GetBaseName(), refMessage));
		}
	}

	gSettings.Unlock();

	fRecentMenu->SetTargetForItems(be_app);
}


void
ProjectWindow::MenusEnded(void)
{
	while (fRecentMenu->ItemAt(0L))
		delete fRecentMenu->RemoveItem(0L);
}


void
ProjectWindow::AddFile(const entry_ref& ref, BPoint* where)
{
	BPath path(&ref);

	if (fProject->HasFileName(path.Path())) {
		STRACE(1, ("%s is already part of the project\n", path.Path()));
		return;
	}

	SourceFile* file = gFileFactory.CreateSourceFileItem(path.Path());
	SourceFileItem* item = new SourceFileItem(file,1);

	int32 selection;
	if (where != NULL)
		selection = fProjectList->IndexOf(*where);
	else
		selection = fProjectList->FullListCurrentSelection();

	SourceGroupItem* groupItem = dynamic_cast<SourceGroupItem*>(
		fProjectList->FullListItemAt(selection));
	if (groupItem != NULL) {
		if (!groupItem->IsExpanded())
			fProjectList->Expand(groupItem);

		fProjectList->AddUnder(item,groupItem);
		fProject->AddFile(file,groupItem->GetData(),0);
	} else {
		if (selection >= 0) {
			fProjectList->AddItem(item, selection + 1);
			groupItem = (SourceGroupItem*)fProjectList->Superitem(item);

			int32 index = fProjectList->UnderIndexOf(item);

			// if we've added it via drag & drop, add it after the item under the
			// drop point. If, however, we added it via the filepanel, it makes the
			// most sense to add it after the selection
			//if (where)
			//	index++;
			fProject->AddFile(file, groupItem->GetData(), index);
			STRACE(1,( "Added file %s to project\n", path.Path()));
		} else {
			// No selection, so we'll add it to the last group available unless
			// there isn't one, which shouldn't ever be.
			if (fProject->CountGroups()) {
				groupItem = fProjectList->ItemForGroup(
					fProject->GroupAt(fProject->CountGroups() - 1));
				fProjectList->AddUnder(item,groupItem);
				fProjectList->MoveItem(fProjectList->FullListIndexOf(item),
					fProjectList->FullListCountItems());

				fProject->AddFile(file,groupItem->GetData(),-1);
				STRACE(1,("Added file %s to project\n",path.Path()));
			} else {
				debugger("BUG: No groups exist");
				delete file;

				return;
			}
		}
	}

	if (file->UsesBuild())
		item->SetDisplayState(SFITEM_NEEDS_BUILD);
	else
		item->SetDisplayState(SFITEM_NORMAL);

	fProjectList->InvalidateItem(fProjectList->IndexOf(item));
	if (groupItem)
		fProjectList->InvalidateItem(fProjectList->IndexOf(groupItem));
}


bool
ProjectWindow::RequiresMenuLock(const int32& command)
{
	switch (command) {
		case M_SHOW_ERROR_WINDOW:
		case B_ABOUT_REQUESTED:
		case M_SHOW_FIND_AND_OPEN_PANEL:
		case M_NEW_WINDOW:
		case M_SHOW_LICENSES:
		case M_SHOW_ASCII_TABLE:
		case M_SHOW_VREGEX:
		case M_SHOW_PROJECT_FOLDER:
			return false;
	}

	return true;
}


void
ProjectWindow::ActOnSelectedFiles(const int32& command)
{
	SCMOutputWindow* window = NULL;
	switch (command) {
		case M_ADD_SELECTION_TO_REPO:
		{
			if (!fSourceControl)
				return;

			window = new SCMOutputWindow(TR("Add to repository"));
			window->Show();
			break;
		}

		case M_REMOVE_SELECTION_FROM_REPO:
		{
			if (!fSourceControl)
				return;
				
			window = new SCMOutputWindow(TR("Remove from repository"));
			window->Show();
			break;
		}

		case M_REVERT_SELECTION:
		{
			if (!fSourceControl)
				return;
				
			window = new SCMOutputWindow(TR("Revert"));
			window->Show();
			break;
		}

		case M_DIFF_SELECTION:
		{
			if (!fSourceControl)
				return;
				
			window = new SCMOutputWindow(TR("Show differences"));
			window->Show();
			break;
		}
	}

	for (int32 i = 0; i < fProjectList->CountItems(); i++) {
		SourceFileItem* item = dynamic_cast<SourceFileItem*>(
			fProjectList->ItemAt(i));
		if (item != NULL && item->IsSelected()) {
			SourceFile* file = item->GetData();

			BString relPath = file->GetPath().GetFullPath();
			if (relPath.FindFirst(fProject->GetPath().GetFolder()) == 0) {
				relPath.RemoveFirst(fProject->GetPath().GetFolder());
				relPath.RemoveFirst("/");
			}

			BString relPartnerPath;
			entry_ref partnerRef = GetPartnerRef(file->GetPath().GetRef());
			if (partnerRef.name) {
				DPath partnerPath(partnerRef);
				relPartnerPath = partnerPath.GetFullPath();
				if (relPartnerPath.FindFirst(fProject->GetPath().GetFolder()) == 0) {
					relPartnerPath.RemoveFirst(fProject->GetPath().GetFolder());
					relPartnerPath.RemoveFirst("/");
				}
			}

			switch (command) {
				case M_REBUILD_FILE:
				{
					if (file->UsesBuild()) {
						file->RemoveObjects(*fProject->GetBuildInfo());
						item->SetDisplayState(SFITEM_NEEDS_BUILD);
						fProjectList->InvalidateItem(fProjectList->IndexOf(item));
					}
					break;
				}

				case M_ADD_SELECTION_TO_REPO:
				{
					fSourceControl->AddToRepository(relPath.String());
					if (relPartnerPath.CountChars() > 0)
						fSourceControl->AddToRepository(relPartnerPath.String());

					break;
				}

				case M_REMOVE_SELECTION_FROM_REPO:
				{
					fSourceControl->RemoveFromRepository(relPath.String());
					if (relPartnerPath.CountChars() > 0)
						fSourceControl->RemoveFromRepository(relPartnerPath.String());

					break;
				}

				case M_REVERT_SELECTION:
				{
					fSourceControl->Revert(relPath.String());
					break;
				}

				case M_DIFF_SELECTION:
				{
					fSourceControl->Diff(relPath.String());
					break;
				}
			}
		}
	}
}


void
ProjectWindow::CreateMenuBar(void)
{
	fMenuBar = new BMenuBar("documentbar");

	// File menu

	fFileMenu = new BMenu(TR("File"));
	fFileMenu->AddItem(new BMenuItem(TR("New project") B_UTF8_ELLIPSIS,
		new BMessage(M_NEW_WINDOW), 'N', B_COMMAND_KEY | B_SHIFT_KEY));
	fFileMenu->AddItem(new BMenuItem(TR("Open project") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_OPEN_PROJECT), 'O', B_COMMAND_KEY));
	fRecentMenu = new BMenu(TR("Open recent project"));
	fFileMenu->AddItem(fRecentMenu);
	fFileMenu->AddItem(new BMenuItem(TR("Find and open file") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_FIND_AND_OPEN_PANEL), 'D', B_COMMAND_KEY));
	fFileMenu->AddSeparatorItem();
	fFileMenu->AddItem(new BMenuItem(TR("Program settings") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_PROGRAM_SETTINGS)));
	fFileMenu->AddItem(new BMenuItem(TR("About Paladin") B_UTF8_ELLIPSIS,
		new BMessage(B_ABOUT_REQUESTED)));
	fMenuBar->AddItem(fFileMenu);

	// Source menu

	fSourceMenu = new BMenu(TR("Source control"));
	fSourceMenu->AddItem(new BMenuItem(TR("Check project in"),
		new BMessage(M_GET_CHECK_IN_MSG), 'I',
		B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddItem(new BMenuItem(TR("Show changed files from last check-in"),
		new BMessage(M_PROJECT_SCM_STATUS), 'S',
		B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddItem(new BMenuItem(TR("Show differences from last check-in"),
		new BMessage(M_DIFF_PROJECT), 'D',
		B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddSeparatorItem();
	fSourceMenu->AddItem(new BMenuItem(TR("Revert project"),
		new BMessage(M_REVERT_PROJECT)));
	fSourceMenu->AddSeparatorItem();
	fSourceMenu->AddItem(new BMenuItem(TR("Add selected files to repository"),
		new BMessage(M_ADD_SELECTION_TO_REPO)));
	fSourceMenu->AddItem(new BMenuItem(TR("Remove Selected files from repository"),
		new BMessage(M_REMOVE_SELECTION_FROM_REPO)));
	fSourceMenu->AddItem(new BMenuItem(TR("Revert selected files"),
		new BMessage(M_REVERT_SELECTION)));
	fSourceMenu->AddSeparatorItem();
	fSourceMenu->AddItem(new BMenuItem(TR("Push changes to remote repository"),
		new BMessage(M_PUSH_PROJECT), B_UP_ARROW,
		B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddItem(new BMenuItem(TR("Pull changes from remote repository"),
		new BMessage(M_PULL_PROJECT), B_DOWN_ARROW,
		B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));

	// Project menu

	fProjectMenu = new BMenu(TR("Project"));
	
	fProjectMenu->AddItem(new BMenuItem(TR("Settings") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_PROJECT_SETTINGS)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Add new file") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_ADD_NEW_PANEL), 'N', B_COMMAND_KEY));
	fProjectMenu->AddItem(new BMenuItem(TR("Add files") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_ADD_PANEL)));
	fProjectMenu->AddItem(new BMenuItem(TR("Remove selected files"),
		new BMessage(M_REMOVE_FILES)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Change system libraries") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_LIBRARIES)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Find in project files") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_FIND_IN_PROJECT_FILES), 'F',
		B_COMMAND_KEY | B_SHIFT_KEY));

#ifdef BUILD_CODE_LIBRARY
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Synchronize with code library"),
		new BMessage(M_SYNC_MODULES)));
#endif
	fProjectMenu->AddSeparatorItem();

	fProjectMenu->AddItem(fSourceMenu);

	fProjectMenu->AddSeparatorItem();

	fProjectMenu->AddItem(new BMenuItem(TR("New group"),
		new BMessage(M_NEW_GROUP)));
	fProjectMenu->AddItem(new BMenuItem(TR("Rename group"),
		new BMessage(M_SHOW_RENAME_GROUP)));
	fProjectMenu->AddItem(new BMenuItem(TR("Sort group"),
		new BMessage(M_SORT_GROUP)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Show project folder") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_PROJECT_FOLDER)));
	fMenuBar->AddItem(fProjectMenu);

	// Build menu

	fBuildMenu = new BMenu(TR("Build"));

	fBuildMenu->AddItem(new BMenuItem(TR("Make project"),
		new BMessage(M_BUILD_PROJECT), 'M'));
	fBuildMenu->AddItem(new BMenuItem(TR("Run"),
		new BMessage(M_RUN_PROJECT), 'R'));
	fBuildMenu->AddItem(new BMenuItem(TR("Run logged") B_UTF8_ELLIPSIS,
		new BMessage(M_RUN_IN_TERMINAL), 'R', B_COMMAND_KEY | B_SHIFT_KEY));
	fBuildMenu->AddItem(new BMenuItem(TR("Debug"), new BMessage(M_DEBUG_PROJECT),
		'R', B_COMMAND_KEY | B_CONTROL_KEY));
	fBuildMenu->AddSeparatorItem();
	fBuildMenu->AddItem(new BMenuItem(TR("Generate makefile") B_UTF8_ELLIPSIS,
		new BMessage(M_MAKE_MAKE)));
	fBuildMenu->AddSeparatorItem();
	fBuildMenu->AddItem(new BMenuItem(TR("Set run arguments") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_RUN_ARGS)));
	fBuildMenu->AddSeparatorItem();
	fBuildMenu->AddItem(new BMenuItem(TR("Update dependencies"),
		new BMessage(M_UPDATE_DEPENDENCIES)));

	BMenuItem* item = new BMenuItem(TR("Empty build cache"),
		new BMessage(M_EMPTY_CCACHE));
	fBuildMenu->AddItem(item);
	item->SetEnabled(gCCacheAvailable);

	fBuildMenu->AddItem(new BMenuItem(TR("Force project to rebuild"),
		new BMessage(M_FORCE_REBUILD), '-'));
	fMenuBar->AddItem(fBuildMenu);

	// Tools menu

	fToolsMenu = new BMenu(TR("Tools"));
	
#ifdef BUILD_CODE_LIBRARY
	fToolsMenu->AddItem(new BMenuItem(TR("Code library") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_CODE_LIBRARY), 'L'));
#endif
	fToolsMenu->AddItem(new BMenuItem(TR("Error window") B_UTF8_ELLIPSIS,
		new BMessage(M_TOGGLE_ERROR_WINDOW), 'I'));
	fToolsMenu->AddItem(new BMenuItem(TR("ASCII table") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_ASCII_TABLE)));
	fToolsMenu->AddItem(new BMenuItem(TR("Regular expression tester") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_VREGEX)));

	BMessage* message = new BMessage(M_RUN_TOOL);
	message->AddString("signature", "application/x-vnd.dw-SymbolFinder");
	fToolsMenu->AddItem(new BMenuItem(TR("Symbol finder") B_UTF8_ELLIPSIS, message));
	fToolsMenu->AddSeparatorItem();
	fToolsMenu->AddItem(new BMenuItem(TR("Make project backup"),
		new BMessage(M_BACKUP_PROJECT)));
	fToolsMenu->AddSeparatorItem();
	fToolsMenu->AddItem(new BMenuItem(TR("Set software license") B_UTF8_ELLIPSIS,
		new BMessage(M_SHOW_LICENSES)));
	fMenuBar->AddItem(fToolsMenu);
}

void
ProjectWindow::SetMenuLock(bool locked)
{
	if (locked) {
		fProjectMenu->SetEnabled(false);
		fBuildMenu->SetEnabled(false);
	} else {
		fProjectMenu->SetEnabled(true);
		fBuildMenu->SetEnabled(true);
	}
}


void
ProjectWindow::MakeGroup(int32 selection)
{
	if (selection < 1
		|| dynamic_cast<SourceGroupItem*>(
			fProjectList->FullListItemAt(selection))
		|| dynamic_cast<SourceGroupItem*>(
			fProjectList->FullListItemAt(selection - 1))) {
		return;
	}

	int32 newGroupIndex = -1;
	if (fProject->CountGroups() > 1) {
		int32 groupcount = 0;
		for (int32 i = selection - 1; i > 0; i--) {
			if (dynamic_cast<SourceGroupItem*>(fProjectList->FullListItemAt(i)))
				groupcount++;
		}
	}

	SourceGroupItem* oldgroupitem = (SourceGroupItem*)fProjectList->Superitem(
		fProjectList->FullListItemAt(selection));
	SourceGroup* oldgroup = oldgroupitem->GetData();
	SourceGroup* newgroup = fProject->AddGroup("New group", newGroupIndex);
	SourceGroupItem* newGroupItem = new SourceGroupItem(newgroup);
	fProjectList->AddItem(newGroupItem, selection);

	int32 index = selection + 1;
	SourceFileItem* fileitem = dynamic_cast<SourceFileItem*>(
		fProjectList->FullListItemAt(index));
	while (fileitem != NULL) {
		fProjectList->RemoveItem(fileitem);
		fProjectList->AddItem(fileitem, index);
		oldgroup->filelist.RemoveItem(fileitem->GetData(), false);
		newgroup->filelist.AddItem(fileitem->GetData());

		index++;
		fileitem = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(index));
	}

	fProjectList->Expand(newGroupItem);
	fProjectList->InvalidateItem(selection);
	fProject->Save();
}


void
ProjectWindow::ToggleErrorWindow(ErrorList* list)
{
	if (fErrorWindow != NULL)
		fErrorWindow->PostMessage(B_QUIT_REQUESTED);
	else
		ShowErrorWindow(list);
}


void
ProjectWindow::ShowErrorWindow(ErrorList* list)
{
	if (fErrorWindow != NULL && list != NULL) {
		BMessage message;
		list->Flatten(message);
		message.what = M_BUILD_WARNINGS;
		fErrorWindow->PostMessage(&message);
	} else {
		BRect screen(BScreen().Frame());
		BRect r(screen);
				
		BDeskbar deskbar;
		if (deskbar.Location() == B_DESKBAR_BOTTOM)
			r.OffsetBy(0,-deskbar.Frame().Height());

		fErrorWindow = new ErrorWindow(r, this, list);
		fErrorWindow->Show();
	}

	fStatusBar->SetText("");
}


void
ProjectWindow::CullEmptyGroups(void)
{
	for (int32 i = fProjectList->CountItems() - 1; i >= 0; i--) {
		SourceGroupItem* groupitem = dynamic_cast<SourceGroupItem*>(
			fProjectList->ItemAt(i));
		if (groupitem && groupitem->GetData()->filelist.CountItems() == 0
			&& fProject->CountGroups() > 1) {
			fProject->RemoveGroup(groupitem->GetData(), true);
			fProjectList->RemoveItem(groupitem);
		}
	}

	fProject->Save();
}


void
ProjectWindow::UpdateDependencies(void)
{
	bool toggleHack = false;
	if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4)
		toggleHack = true;

	if (toggleHack)
		gUsePipeHack = false;
	
	fStatusBar->SetText("Updating dependencies");
	SetMenuLock(true);
	for (int32 i = 0; i < fProjectList->CountItems(); i++) {
		SourceFileItem* item = dynamic_cast<SourceFileItem*>(
			fProjectList->FullListItemAt(i));
		if (item != NULL)
			item->GetData()->UpdateDependencies(*fProject->GetBuildInfo());

		UpdateIfNeeded();
	}
	SetMenuLock(false);
	fStatusBar->SetText("");

	if (toggleHack)
		gUsePipeHack = true;
}


void
ProjectWindow::ToggleDebugMenu(void)
{
	BMenuItem* debugItem = fMenuBar->FindItem("Debug Paladin");
	if (debugItem != NULL)
		fMenuBar->RemoveItem(debugItem);
	else {
		BMenu* debug = new BMenu("Debug Paladin");
		debug->AddItem(new BMenuItem("Dump dependencies",
			new BMessage(M_DEBUG_DUMP_DEPENDENCIES)));
		debug->AddItem(new BMenuItem("Dump includes",
			new BMessage(M_DEBUG_DUMP_INCLUDES)));
		fMenuBar->AddItem(debug);
	}
}


void
ProjectWindow::DoBuild(int32 postbuild)
{
	if (fErrorWindow != NULL)
		fErrorWindow->PostMessage(M_CLEAR_ERROR_LIST);

	fProject->GetErrorList()->msglist.MakeEmpty();

	// Missing file check
	for (int32 i = 0; i < fProjectList->CountItems(); i++) {
		SourceFileItem* item = dynamic_cast<SourceFileItem*>(
			fProjectList->ItemAt(i));
		if (item != NULL && item->GetDisplayState() == SFITEM_MISSING) {
			ShowAlert(TR("The project cannot be built because some of its "
				"files are missing."));
			return;
		}
	}

	fStatusBar->SetText("Examining source files");
	UpdateIfNeeded();

	SetMenuLock(true);
	fBuilder.BuildProject(fProject,postbuild);
}


void
ProjectWindow::AddNewFile(BString name, bool createPair)
{
	BMessage message(M_ADD_FILES);

	DPath projectFile(fProject->GetPath().GetFolder());
	projectFile << name;

	entry_ref ref = gFileFactory.CreateSourceFile(projectFile.GetFolder(),
		projectFile.GetFileName(), createPair ? SOURCEFILE_PAIR : 0);
	if (ref.name == NULL || *ref.name == '\0')
		return;

	AddFile(ref);

	if (fSourceControl)
		fSourceControl->AddToRepository(projectFile.GetFullPath());

	message.AddRef("refs", &ref);

	if (createPair && fSourceControl) {
		entry_ref partnerRef = GetPartnerRef(ref);
		DPath partnerPath(partnerRef);
		fSourceControl->AddToRepository(projectFile.GetFullPath());
		message.AddRef("refs", &partnerRef);

		if (!gDontManageHeaders)
			AddFile(partnerRef);

		fSourceControl->AddToRepository(partnerPath.GetFullPath());
	}

	be_app->PostMessage(&message);
}


int32
ProjectWindow::AddFileThread(void* data)
{
	add_file_struct* addFileStruct = (add_file_struct*)data;

	int32 i = 0;
	entry_ref addref;
	while (addFileStruct->refMessage.FindRef("refs", i, &addref) == B_OK) {
		BNode node(&addref);
		BString type;
		if (node.ReadAttrString("BEOS:TYPE", &type) == B_OK
			&& type == PROJECT_MIME_TYPE) {
			BMessage refMessage(B_REFS_RECEIVED);
			refMessage.AddRef("refs", &addref);
			be_app->PostMessage(&refMessage);
			i++;
			continue;
		}

		BEntry entry(&addref);
		DPath path(addref);
		if (entry.IsDirectory())
			addFileStruct->parent->AddFolder(addref);
		else {
			addFileStruct->parent->Lock();
			addFileStruct->parent->AddFile(addref,&addFileStruct->dropPoint);
			addFileStruct->parent->Unlock();
		}

		i++;
	}

	addFileStruct->parent->Lock();
	addFileStruct->parent->CullEmptyGroups();
	addFileStruct->parent->fProject->Save();
	addFileStruct->parent->Unlock();

	return 0;
}


void
ProjectWindow::AddFolder(entry_ref folderRef)
{
	BDirectory directory;
	if (directory.SetTo(&folderRef) != B_OK)
		return;

	if (strcmp(folderRef.name,"CVS") == 0
		|| strcmp(folderRef.name,".svn") == 0
		|| strcmp(folderRef.name,".git") == 0
		|| strcmp(folderRef.name,".hg") == 0) {
		return;
	}

	directory.Rewind();

	entry_ref ref;
	while (directory.GetNextRef(&ref) == B_OK) {
		if (BEntry(&ref).IsDirectory())
			AddFolder(ref);
		else {
			// Here is where we actually add the file to the project.
			//
			// The name of the folder containing the file will be the name
			// of the file's group. If the group doesn't exist, it will be
			// created at the end of the list, but if it does, we will fake
			// a drop onto the group item to reuse the code path by finding
			// the group and getting its position in the list.
			DPath filepath(ref);
			if (filepath.GetExtension()
				&& !(strcmp(filepath.GetExtension(),"cpp") == 0
					|| strcmp(filepath.GetExtension(),"c") == 0
					|| strcmp(filepath.GetExtension(),"cc") == 0
					|| strcmp(filepath.GetExtension(),"cxx") == 0
					|| strcmp(filepath.GetExtension(),"rdef") == 0
					|| strcmp(filepath.GetExtension(),"rsrc") == 0)) {
				continue;
			}

			// don't bother adding build files from other systems
			if (strcmp(filepath.GetFileName(), "Jamfile") == 0
				|| strcmp(filepath.GetFileName(), "Makefile") == 0) {
				continue;
			}

			DPath parent(filepath.GetFolder());
			SourceGroup* group = NULL;
			SourceGroupItem* groupItem = NULL;

			fProject->Lock();
			group = fProject->FindGroup(parent.GetFileName());

			Lock();

			if (group != NULL)
				groupItem = fProjectList->ItemForGroup(group);
			else {
				group = fProject->AddGroup(parent.GetFileName());
				groupItem = new SourceGroupItem(group);
				fProjectList->AddItem(groupItem);
				UpdateIfNeeded();
			}

			if (groupItem != NULL) {
				int32 index = fProjectList->IndexOf(groupItem);
				BPoint where = fProjectList->ItemFrame(index).LeftTop();
				where.x += 5;
				where.y += 5;

				AddFile(ref,&where);
			}

			Unlock();

			fProject->Unlock();
		}
	}
}


void
ProjectWindow::ImportFile(entry_ref ref)
{
	BString command;

	DPath sourcePath(ref);
	DPath destinationPath(fProject->GetPath().GetFolder());
	command << "copyattr --data '" << sourcePath.GetFullPath() << "' '" 
		<< destinationPath.GetFullPath() << "'";
	system(command.String());

	BString extension(sourcePath.GetExtension());
	if ((extension.ICompare("h") == 0)
		|| (extension.ICompare("hxx") == 0)
		|| (extension.ICompare("hpp") == 0)
		|| (extension.ICompare("h++") == 0)) {
		return;
	}

	DPath destinationFile(destinationPath);
	destinationFile << ref.name;
	BEntry destEntry(destinationFile.GetFullPath());
	entry_ref destinationRef;
	destEntry.GetRef(&destinationRef);
	AddFile(destinationRef, NULL);
}


int32
ProjectWindow::ImportFileThread(void* data)
{
	add_file_struct* addFileStruct = (add_file_struct*)data;

	int32 i = 0;
	entry_ref addref;
	while (addFileStruct->refMessage.FindRef("refs", i, &addref) == B_OK) {
		addFileStruct->parent->Lock();
		addFileStruct->parent->ImportFile(addref);
		addFileStruct->parent->Unlock();
		i++;
	}

	addFileStruct->parent->Lock();
	addFileStruct->parent->CullEmptyGroups();
	addFileStruct->parent->fProject->Save();
	addFileStruct->parent->Unlock();

	return 0;
}


int32
ProjectWindow::BackupThread(void* data)
{
	ProjectWindow* parent = (ProjectWindow*)data;
	Project* project = parent->fProject;

	char timestamp[32];
	time_t t = real_time_clock();
	strftime(timestamp, 32, "_%Y-%m-%d-%H%M%S", localtime(&t));

	BPath folder(project->GetPath().GetFolder());
	BPath folderparent;
	folder.GetParent(&folderparent);

	BString command = "cd '";
	command << folderparent.Path() << "'; zip -9 -r -y '"
		<< gBackupPath.GetFullPath() << "/"
		<< project->GetName() << timestamp << "' '"
		<< folder.Leaf() << "' -x *.o";

	system(command.String());

	parent->Lock();
	parent->fStatusBar->SetText("");
	parent->SetMenuLock(false);
	parent->Unlock();

	return 0;
}


int32
ProjectWindow::SyncThread(void* data)
{
#ifdef BUILD_CODE_LIBRARY
	ProjectWindow* parent = (ProjectWindow*)data;

	parent->Lock();
	parent->fStatusBar->SetText(TR("Updating modules"));
	parent->SetMenuLock(true);
	parent->Unlock();

	SyncProjectModules(gCodeLib, parent->fProject);

	parent->Lock();
	parent->fStatusBar->SetText("");
	parent->SetMenuLock(false);
	parent->Unlock();
#endif
	
	return 0;
}


int
compare_source_file_items(const BListItem* item1, const BListItem* item2)
{
	if (item2 == NULL)
		return (item1 != NULL) ? -1 : 0;
	else if (item1 == NULL)
		return 1;

	SourceFileItem* one = (SourceFileItem*)item1;
	SourceFileItem* two = (SourceFileItem*)item2;

	if (one->Text() == NULL)
		return (two->Text() != NULL) ? 1 : 0;
	else if (two->Text() == NULL)
		return -1;

	// sort based on type
	int8 type1;
	int8 type2;
	switch (one->GetData()->GetType()) {
		case TYPE_C:
		{
			type1 = 0;
			break;
		}

		case TYPE_LEX:
		case TYPE_YACC:
		{
			type1 = 1;
			break;
		}

		case TYPE_RESOURCE:
		{
			type1 = 2;
			break;
		}

		case TYPE_LIB:
		{
			type1 = 3;
			break;
		}

		default:
		{
			type1 = 10;
		}
	}

	switch (two->GetData()->GetType()) {
		case TYPE_C:
		{
			type2 = 0;
			break;
		}

		case TYPE_LEX:
		case TYPE_YACC:
		{
			type2 = 1;
			break;
		}

		case TYPE_RESOURCE:
		{
			type2 = 2;
			break;
		}

		case TYPE_LIB:
		{
			type2 = 3;
			break;
		}

		default:
		{
			type2 = 10;
		}
	}

	if (type1 < type2)
		return -1;
	else if (type2 < type1)
		return 1;

	return strcmp(one->Text(),two->Text());
}
