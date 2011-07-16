#include "ProjectWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Deskbar.h>
#include <Font.h>
#include <MenuItem.h>
#include <OS.h>
#include <Roster.h>
#include <Screen.h>
#include <ScrollView.h>
#include <stdio.h>
#include <stdlib.h>
#include <StorageKit.h>
#include <String.h>
#include <time.h>
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

enum
{
	M_NEW_WINDOW = 'nwwn',
	M_SHOW_PROGRAM_SETTINGS = 'sprs',
	M_SHOW_PROJECT_SETTINGS = 'spjs',
	M_SHOW_ASCII_TABLE = 'sast',
	M_SHOW_VREGEX = 'svrx',
	M_SHOW_RUN_ARGS = 'srag',
	M_SHOW_LIBRARIES = 'slbw',
	M_SHOW_PROJECT_FOLDER = 'shpf',
	M_RUN_TOOL = 'rntl',
	M_UPDATE_DEPENDENCIES = 'updp',
	M_BUILD_PROJECT = 'blpj',
	M_DEBUG_PROJECT = 'PRnD',
	M_EDIT_FILE = 'edfl',
	M_ADD_NEW_FILE = 'adnf',
	M_SHOW_ADD_PANEL = 'shap',
	M_RENAME_GROUP = 'rngr',
	M_SHOW_LICENSES = 'shlc',
	M_MAKE_MAKE = 'mkmk',
	M_SHOW_CODE_LIBRARY = 'shcl',
	M_SYNC_MODULES = 'synm',
	
	M_GET_CHECK_IN_MSG = 'gcim',
	M_CHECK_IN_PROJECT = 'prci',
	M_REVERT_PROJECT = 'prrv',
	M_PUSH_PROJECT = 'pshp',
	M_PULL_PROJECT = 'pulp',
	M_DIFF_PROJECT = 'dfpj',
	M_PROJECT_SCM_STATUS = 'pscs',
	
	M_TOGGLE_DEBUG_MENU = 'sdbm',
	M_DEBUG_DUMP_DEPENDENCIES = 'dbdd',
	M_DEBUG_DUMP_INCLUDES = 'dbdi'
};

int compare_source_file_items(const BListItem *item1, const BListItem *item2);

ProjectWindow::ProjectWindow(BRect frame, Project *project)
	:	DWindow(frame, "Paladin", B_DOCUMENT_WINDOW, B_NOT_ZOOMABLE |
													B_WILL_ACCEPT_FIRST_CLICK),
		fErrorWindow(NULL),
		fFilePanel(NULL),
		fProject(project),
		fSourceControl(NULL),
		fShowingLibs(false),
		fMenusLocked(false),
		fBuilder(BMessenger(this))
{
	AddCommonFilter(new AltTabFilter());
	SetSizeLimits(200,30000,200,30000);
	RegisterWindow();
	
	// This is for our in-program debug menu which comes in handy now and then.
	// Paladin -d doesn't always get the job done
	AddShortcut('9', B_COMMAND_KEY | B_SHIFT_KEY | B_CONTROL_KEY,
				new BMessage(M_TOGGLE_DEBUG_MENU));
	
	if (fProject)
	{
		fSourceControl = GetSCM(fProject->SourceControl());
		if (fSourceControl)
		{
			if (gPrintDebugMode > 0)
				fSourceControl->SetDebugMode(true);
			
			fSourceControl->SetUpdateCallback(SCMOutputCallback);
			fSourceControl->SetWorkingDirectory(fProject->GetPath().GetFolder());
			
			if (fSourceControl->NeedsInit(fProject->GetPath().GetFolder()))
				fSourceControl->CreateRepository(fProject->GetPath().GetFolder());
		}
	}
	
	BView *top = GetBackgroundView();
	
	BRect bounds(Bounds());
	BRect r(bounds);
	
	r.bottom = 16;
	fMenuBar = new BMenuBar(r,"documentbar");
	top->AddChild(fMenuBar);
	
	r = bounds;
	r.top = fMenuBar->Frame().bottom + 1;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	
	fProjectList = new ProjectList(fProject, r,"filelist",B_FOLLOW_ALL);
	fProjectList->SetInvocationMessage(new BMessage(M_EDIT_FILE));
	
	BScrollView *scrollView = new BScrollView("scrollView",fProjectList,
											B_FOLLOW_ALL,0,false,true);
	top->AddChild(scrollView);
	fProjectList->SetTarget(this);
	
	r.top = r.bottom + 1;
	r.bottom = Bounds().bottom;
	fStatusBar = new BStringView(r,"statusbar", NULL, B_FOLLOW_LEFT_RIGHT |
														B_FOLLOW_BOTTOM);
	top->AddChild(fStatusBar);
	
	fStatusBar->SetViewColor(235,235,235);
	fStatusBar->SetFontSize(10.0);
	
	SetupMenus();
	
	if (project)
	{
		BString title("Paladin: ");
		title << project->GetName();
		SetTitle(title.String());
		
		for (int32 i = 0; i < project->CountGroups(); i++)
		{
			SourceGroup *group = project->GroupAt(i);
			SourceGroupItem *groupitem = new SourceGroupItem(group);
			fProjectList->AddItem(groupitem);
			groupitem->SetExpanded(group->expanded);
			
			for (int32 j = 0; j < group->filelist.CountItems(); j++)
			{
				SourceFile *file = group->filelist.ItemAt(j);
				SourceFileItem *fileitem = new SourceFileItem(file,1);
				
//				fProjectList->AddUnder(fileitem,groupitem);
				fProjectList->AddItem(fileitem);
				
				BString abspath = file->GetPath().GetFullPath();
				if (abspath[0] != '/')
				{
					abspath.Prepend("/");
					abspath.Prepend(project->GetPath().GetFolder());
				}
				BEntry entry(abspath.String());
				if (entry.Exists())
				{
					if (project->CheckNeedsBuild(file,false))
					{
						fileitem->SetDisplayState(SFITEM_NEEDS_BUILD);
						fProjectList->InvalidateItem(fProjectList->IndexOf(fileitem));
					}
					else
						file->SetBuildFlag(BUILD_NO);
				}
				else
				{
					fileitem->SetDisplayState(SFITEM_MISSING);
					fProjectList->InvalidateItem(fProjectList->IndexOf(fileitem));
				}
			}
		}
	}
	
	BNode node(fProject->GetPath().GetFullPath());
	if (node.ReadAttr("project_frame",B_RECT_TYPE,0,&r,sizeof(BRect)) == sizeof(BRect))
	{
		if (r.Width() < 200)
			r.right = r.left + 200;
		if (r.Height() < 200)
			r.top = r.bottom + 200;
		MoveTo(r.left,r.top);
		ResizeTo(r.Width(),r.Height());
	}
	
	fProjectList->MakeFocus(true);
	
	if (gShowFolderOnOpen)
	{
		// Duplicated code from MessageReceived::M_SHOW_PROJECT_FOLDER. Doing
		// it here in combo with snooze() makes it much more likely that the
		// opened project window is frontmost instead of the project folder's window
		entry_ref ref;
		BEntry(fProject->GetPath().GetFolder()).GetRef(&ref);
		BMessenger msgr("application/x-vnd.Be-TRAK");
		
		BMessage reply;
		BMessage openmsg(B_REFS_RECEIVED);
		openmsg.AddRef("refs",&ref);
		msgr.SendMessage(&openmsg);
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
	BRect r(Frame());
	node.WriteAttr("project_frame",B_RECT_TYPE,0,&r,sizeof(BRect));
	delete fFilePanel;
}


bool
ProjectWindow::QuitRequested()
{
	if (fShowingLibs)
	{
		BString title("Libraries: ");
		title << fProject->GetName();
		for (int32 i = 0; i < be_app->CountWindows(); i++)
		{
			BWindow *win = be_app->WindowAt(i);
			if (title.Compare(win->Title()) == 0)
			{
				win->PostMessage(B_QUIT_REQUESTED);
				break;
			}
		}
	}
	
	fProject->Save();
	
	if (fErrorWindow)
	{
		fErrorWindow->Quit();
		fErrorWindow = NULL;
	}
	
	DeregisterWindow();
	return true;
}


void
ProjectWindow::MessageReceived(BMessage *msg)
{
	status_t status;
	
	if ( (msg->WasDropped() && msg->what == B_SIMPLE_DATA) || msg->what == M_ADD_FILES)
	{
		fAddFileStruct.refmsg = *msg;
		fAddFileStruct.parent = this;
		
		uint32 buttons;
		fProjectList->GetMouse(&fAddFileStruct.droppt,&buttons);
		
		thread_id addThread = spawn_thread(AddFileThread,"file adding thread",
											B_NORMAL_PRIORITY, &fAddFileStruct);
		if (addThread >= 0)
			resume_thread(addThread);
	}
	switch (msg->what)
	{
		case M_IMPORT_REFS:
		{
			fImportStruct.refmsg = *msg;
			fImportStruct.parent = this;
			
			thread_id importThread = spawn_thread(ImportFileThread,"file import thread",
												B_NORMAL_PRIORITY, &fImportStruct);
			if (importThread >= 0)
				resume_thread(importThread);
			break;
		}
		case M_BACKUP_PROJECT:
		{
			thread_id backupThread = spawn_thread(BackupThread,"project backup thread",
												B_NORMAL_PRIORITY, this);
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
			if (!fSourceControl)
			{
				printf("NULL source control\n");
				break;
			}
			
			BString out;
			fSourceControl->GetCheckinHeader(out);
			
			bool select = false;
			if (out.CountChars() > 1)
				out.Prepend("\n\n");
			else
			{
				out = TR("Enter the description for the changes in this revision.");
				select = true;
			}
			
			GetTextWindow *gtw = new GetTextWindow("Paladin", out.String(),
													BMessage(M_CHECK_IN_PROJECT),
													BMessenger(this));
			if (!select)
				gtw->GetTextView()->Select(0,0);
			gtw->Show();
			break;
		}
		case M_CHECK_IN_PROJECT:
		{
			BString commitstr;
			if (msg->FindString("text", &commitstr) == B_OK && fSourceControl)
			{
				SCMOutputWindow *win = new SCMOutputWindow(TR("Commit"));
				win->Show();
				fSourceControl->Commit(commitstr.String());
			}
			break;
		}
		case M_REVERT_PROJECT:
		{
			if (!fSourceControl)
				break;
			
			BAlert *revertAlert = new BAlert("Paladin", TR("This will undo all changes "
															"since the last commit. "
															"Continue?"), "Don't Revert",
															"Revert");
			if (revertAlert->Go() == 1)
			{
				SCMOutputWindow *win = new SCMOutputWindow(TR("Revert"));
				win->Show();
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
			ActOnSelectedFiles(msg->what);
			break;
		}
		case M_DIFF_PROJECT:
		{
			if (fSourceControl)
			{
				SCMOutputWindow *win = new SCMOutputWindow(TR("Differences"));
				win->Show();
				fSourceControl->Diff(NULL);
			}
			break;
		}
		case M_PROJECT_SCM_STATUS:
		{
			if (fSourceControl)
			{
				SCMOutputWindow *win = new SCMOutputWindow(TR("Project Status"));
				BString strstatus;
				fSourceControl->GetChangeStatus(strstatus);
				win->GetTextView()->SetText(strstatus.String());
				win->Show();
			}
			break;
		}
		case M_PUSH_PROJECT:
		{
			if (fSourceControl)
			{
				SCMOutputWindow *win = new SCMOutputWindow(TR("Push"));
				win->Show();
				fSourceControl->Push(NULL);
			}
			break;
		}
		case M_PULL_PROJECT:
		{
			if (fSourceControl)
			{
				SCMOutputWindow *win = new SCMOutputWindow(TR("Pull"));
				win->Show();
				status = fSourceControl->Pull(NULL);
				
				if (!status)
					ShowAlert("Unable to pull from the remote repository. If it "
							"uses a secure connection, please set up the appropriate "
							"SSH keys on the remote server.", "OK");
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
			
			SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(selection));
			if (!item)
				break;
			SpawnFileTypes(item->GetData()->GetPath());
			break;
		}
		case M_OPEN_PARENT_FOLDER:
		{
			BMessage openmsg(B_REFS_RECEIVED);
			int32 selindex = 0;
			int32 selection = fProjectList->FullListCurrentSelection();
			selindex++;
			if (selection >= 0)
			{
				while (selection >= 0)
				{
					SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(selection));
					if (!item)
						break;
					
					SourceFile *file = item->GetData();
					BString abspath = file->GetPath().GetFullPath();
					if (abspath[0] != '/')
					{
						abspath.Prepend("/");
						abspath.Prepend(fProject->GetPath().GetFolder());
					}
					DPath filepath(abspath);
					
					entry_ref ref;
					BEntry(filepath.GetFolder()).GetRef(&ref);
					
					openmsg.AddRef("refs",&ref);
					selection = fProjectList->FullListCurrentSelection(selindex++);
				}
				
				BMessenger msgr("application/x-vnd.Be-TRAK");
				msgr.SendMessage(&openmsg);
			}
			
			break;
		}
		case M_SHOW_PROJECT_FOLDER:
		{
			entry_ref ref;
			BEntry(fProject->GetPath().GetFolder()).GetRef(&ref);
			BMessenger msgr("application/x-vnd.Be-TRAK");
			
			BMessage openmsg(B_REFS_RECEIVED);
			openmsg.AddRef("refs",&ref);
			msgr.SendMessage(&openmsg);
			break;
		}
		case M_SHOW_ASCII_TABLE:
		{
			AsciiWindow *ascwin = new AsciiWindow();
			ascwin->Show();
			break;
		}
		case M_SHOW_VREGEX:
		{
			VRegWindow *vregwin = new VRegWindow();
			vregwin->Show();
			break;
		}
		case M_SHOW_LICENSES:
		{
			LicenseManager *man = new LicenseManager(fProject->GetPath().GetFolder());
			man->Show();
			break;
		}
		case M_RUN_TOOL:
		{
			BString sig;
			if (msg->FindString("signature", &sig) == B_OK)
			{
				LaunchHelper launcher(sig.String());
				launcher.Launch();
			}
			break;
		}
		case M_MAKE_MAKE:
		{
			DPath out(fProject->GetPath().GetFolder());
			out.Append("Makefile");
			if (MakeMake(fProject,out) == B_OK);
			{
				BEntry entry(out.GetFullPath());
				entry_ref ref;
				if (entry.InitCheck() == B_OK)
				{
					entry.GetRef(&ref);
					BMessage refmsg(B_REFS_RECEIVED);
					refmsg.AddRef("refs",&ref);
					be_app->PostMessage(&refmsg);
				}
			}
			break;
		}
		case M_SHOW_CODE_LIBRARY:
		{
			#ifdef BUILD_CODE_LIBRARY
			CodeLibWindow *libwin = CodeLibWindow::GetInstance(BRect(100,100,500,350));
			libwin->Show();
			#endif
			
			break;
		}
		case M_OPEN_PARTNER:
		{
			int32 selection = fProjectList->FullListCurrentSelection();
			if (selection < 0)
				break;
			
			SourceFileItem *item = dynamic_cast<SourceFileItem*>(
									fProjectList->FullListItemAt(selection));
			if (!item)
				break;
			entry_ref ref;
			BEntry(fProject->GetPathForFile(item->GetData()).GetFullPath()).GetRef(&ref);
			BMessage refmsg(M_OPEN_PARTNER);
			refmsg.AddRef("refs",&ref);
			be_app->PostMessage(&refmsg);
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
			SourceGroupItem *groupItem = NULL;
			if (selection < 0)
			{
				// Don't need a selection if there is only one group in the project
				if (fProject->CountGroups() == 1)
					groupItem = fProjectList->ItemForGroup(fProject->GroupAt(0));
			}
			else
			{
				BStringItem *strItem = (BStringItem*)fProjectList->FullListItemAt(selection);
				groupItem = fProjectList->GroupForItem(strItem);
			}
			
			if (!groupItem)
				break;
			
			GroupRenameWindow *grwin = new GroupRenameWindow(groupItem->GetData(),
															BMessage(M_RENAME_GROUP),
															BMessenger(this));
			grwin->Show();
			break;
		}
		case M_RENAME_GROUP:
		{
			SourceGroup *group;
			BString newname;
			if (msg->FindPointer("group",(void**)&group) != B_OK ||
				msg->FindString("newname",&newname) != B_OK)
				break;
			
			group->name = newname;
			SourceGroupItem *groupItem = fProjectList->ItemForGroup(group);
			if (!groupItem)
				break;
			
			groupItem->SetText(newname.String());
			fProjectList->InvalidateItem(fProjectList->IndexOf(groupItem));
			
			fProject->Save();
			break;
		}
		case M_SORT_GROUP:
		{
			int32 selection = fProjectList->FullListCurrentSelection();
			
			SourceGroupItem *groupItem = NULL;
			if (selection < 0)
			{
				// Don't need a selection if there is only one group in the project
				if (fProject->CountGroups() == 1)
					groupItem = fProjectList->ItemForGroup(fProject->GroupAt(0));
			}
			else
			{
				BStringItem *strItem = (BStringItem*)fProjectList->FullListItemAt(selection);
				groupItem = fProjectList->GroupForItem(strItem);
			}
			
			if (!groupItem)
				break;
			
			fProjectList->SortItemsUnder(groupItem,true,compare_source_file_items);
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
			BRect r(0,0,350,300);
			BRect screen(BScreen().Frame());
			
			r.OffsetTo((screen.Width() - r.Width()) / 2.0,
						(screen.Height() - r.Height()) / 2.0);
			
			ProjectSettingsWindow *win = new ProjectSettingsWindow(r,fProject);
			win->Show();
			break;
		}
		case M_SHOW_RUN_ARGS:
		{
			RunArgsWindow *argwin = new RunArgsWindow(fProject);
			argwin->Show();
			break;
		}
		case M_JUMP_TO_MSG:
		{
			entry_ref ref;
			if (msg->FindRef("refs",&ref) == B_OK)
			{
				msg->what = B_REFS_RECEIVED;
				be_app->PostMessage(msg);
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
			be_app->PostMessage(msg);
			break;
		}
		case M_NEW_WINDOW:
		{
			be_app->PostMessage(M_NEW_PROJECT);
			break;
		}
		case M_SHOW_PROGRAM_SETTINGS:
		{
			PrefsWindow *prefwin = new PrefsWindow(BRect(0,0,500,400));
			prefwin->Show();
			break;
		}
		case M_SHOW_FIND_AND_OPEN_PANEL:
		{
			BString text;
			msg->FindString("name",&text);
			
			// Passing a NULL string to this is OK
			FindOpenFileWindow *findwin = new FindOpenFileWindow(text.String());
			findwin->Show();
			break;
		}
		case M_FILE_NEEDS_BUILD:
		{
			SourceFile *file;
			if (msg->FindPointer("file",(void**)&file) == B_OK)
			{
				SourceFileItem *item = fProjectList->ItemForFile(file);
				if (item)
				{
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
			
			BMessage refmsg(B_REFS_RECEIVED);
			while (selection >= 0)
			{
				SourceFileItem *item = dynamic_cast<SourceFileItem*>
										(fProjectList->FullListItemAt(selection));
				if (item && item->GetData())
				{
					BString abspath = item->GetData()->GetPath().GetFullPath();
					if (abspath[0] != '/')
					{
						abspath.Prepend("/");
						abspath.Prepend(fProject->GetPath().GetFolder());
					}
					
					BEntry entry(abspath.String());
					if (entry.InitCheck() == B_OK)
					{
						entry_ref ref;
						entry.GetRef(&ref);
						refmsg.AddRef("refs",&ref);
					}
					else
					{
						if (!entry.Exists())
						{
							BString errmsg = TR("Couldn't find XXXXX. It may have been moved or renamed.");
							errmsg.ReplaceFirst("XXXXX",abspath.String());
							BAlert *alert = new BAlert("Paladin",errmsg.String(),"OK");
							alert->Go();
						}
					}
				}
				else
				{
					SourceGroupItem *groupItem = dynamic_cast<SourceGroupItem*>
											(fProjectList->FullListItemAt(selection));
					if (groupItem)
					{
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
			be_app->PostMessage(&refmsg);
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
			LibraryWindow *libwin = new LibraryWindow(Frame().OffsetByCopy(15,15),
														BMessenger(this), fProject);
			libwin->Show();
			break;
		}
		case M_SHOW_ADD_NEW_PANEL:
		{
			AddNewFileWindow *anfwin = new AddNewFileWindow(BMessage(M_ADD_NEW_FILE),
														BMessenger(this));
			anfwin->Show();
			break;
		}
		case M_SHOW_FIND_IN_PROJECT_FILES:
		{
			FindWindow *findwin = new FindWindow();
			findwin->Show();
			break;
		}
		case M_ADD_NEW_FILE:
		{
			BString name;
			bool makepair;
			if (msg->FindString("name",&name) == B_OK && msg->FindBool("makepair",&makepair) == B_OK)
				AddNewFile(name,makepair);
			break;
		}
		case M_SHOW_ADD_PANEL:
		{
			if (!fFilePanel)
			{
				BMessenger msgr(this);
				BEntry entry(fProject->GetPath().GetFolder());
				entry_ref ref;
				entry.GetRef(&ref);
				fFilePanel = new BFilePanel(B_OPEN_PANEL,&msgr,&ref,B_FILE_NODE,true,
											new BMessage(M_ADD_FILES));
			}
			fFilePanel->Show();
			break;
		}
		case M_REMOVE_FILES:
		{
			bool save = false;
			
			for (int32 i = 0; i < fProjectList->CountItems(); i++)
			{
				SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->ItemAt(i));
				if (item && item->IsSelected())
				{
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
			// We don't do this when forcing a rebuild of the sources because sometimes it
			// can take quite a while
			if (gUseCCache && gCCacheAvailable)
			{
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
			
			for (int32 i = 0; i < fProjectList->FullListCountItems(); i++)
			{
				SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(i));
				if (!item)
					continue;
				
				SourceFile *file = item->GetData();
				if (file->UsesBuild())
				{
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
			if (!fProject->Debug())
			{
				BString errmsg = TR("Your project does not have debugging information compiled ");
				errmsg << TR("in and will need to be rebuilt to debug. Do you wish to rebuild and ")
					<< TR("run the debugger?");
				BAlert *alert = new BAlert("Paladin",
													"Debugging information needs to compiled into "
													"your project. This may take some time for large "
													"projects. Do you wish to rebuild and run "
													"the debugger?",
											"Rebuild","Cancel");
				if (alert->Go() == 1)
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
			SourceFile *file;
			if (msg->FindPointer("file",(void**)&file) == B_OK)
			{
				BString out;
				out << TR("Examining ") << file->GetPath().GetFileName();
				fStatusBar->SetText(out.String());
			}
			break;
		}
		case M_BUILDING_FILE:
		{
			SourceFile *file;
			if (msg->FindPointer("sourcefile",(void**)&file) == B_OK)
			{
				SourceFileItem *item = fProjectList->ItemForFile(file);
				if (item)
				{
					item->SetDisplayState(SFITEM_BUILDING);
					fProjectList->InvalidateItem(fProjectList->IndexOf(item));
					
					BString out;
					
					int32 count,total;
					if (msg->FindInt32("count",&count) == B_OK &&
						msg->FindInt32("total",&total) == B_OK)
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
			SourceFile *file;
			if (msg->FindPointer("sourcefile",(void**)&file) == B_OK)
			{
				SourceFileItem *item = fProjectList->ItemForFile(file);
				if (item)
				{
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
			fStatusBar->SetText(TR("Updating Resources"));
			break;
		}
		case M_DOING_POSTBUILD:
		{
			fStatusBar->SetText(TR("Performing Post-build tasks"));
			break;
		}
		case M_BUILD_FAILURE:
		{
			SetMenuLock(false);
			
			// fall through
		}
		case M_BUILD_MESSAGES:
		case M_BUILD_WARNINGS:
		{
			if (!fErrorWindow)
			{
				BRect screen(BScreen().Frame());
				BRect r(screen);
				r.left = r.right / 4.0;
				r.right *= .75;
				r.top = r.bottom - 200;
				
				BDeskbar deskbar;
				if (deskbar.Location() == B_DESKBAR_BOTTOM)
					r.OffsetBy(0,-deskbar.Frame().Height());
				
				fErrorWindow = new ErrorWindow(r,this);
				fErrorWindow->Show();
			}
			else
			{
				if (!fErrorWindow->IsFront())
					fErrorWindow->Activate();
			}
			fStatusBar->SetText("");
			
			// Should this be an Unflatten or an Append?
			ErrorList *errorList = fProject->GetErrorList();
			errorList->Unflatten(*msg);
			fErrorWindow->PostMessage(msg);
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
			thread_id syncID = spawn_thread(SyncThread,"module update thread",
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
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}


void
ProjectWindow::MenusBeginning(void)
{
	gSettings.Lock();
	int32 index = 0;
	entry_ref ref;
	while (gSettings.FindRef("recentitems",index++,&ref) == B_OK)
	{
		if (!BEntry(&ref).Exists())
		{
			index--;
			gSettings.RemoveData("recentitems",index);
		}
		else
		{
			DPath refpath(ref);
			BMessage *refmsg = new BMessage(B_REFS_RECEIVED);
			refmsg->AddRef("refs",&ref);
			fRecentMenu->AddItem(new BMenuItem(refpath.GetBaseName(),refmsg));
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
ProjectWindow::AddFile(const entry_ref &ref, BPoint *pt)
{
	BPath path(&ref);
	
	if (fProject->HasFileName(path.Path()))
	{
		STRACE(1,("%s is already part of the project\n", path.Path()));
		return;
	}
	
	SourceFile *file = gFileFactory.CreateSourceFileItem(path.Path());
	SourceFileItem *item = new SourceFileItem(file,1);
	
	int32 selection;
	if (pt)
		selection = fProjectList->IndexOf(*pt);
	else
		selection = fProjectList->FullListCurrentSelection();
	
	SourceGroupItem *groupItem = dynamic_cast<SourceGroupItem*>
								(fProjectList->FullListItemAt(selection));
	if (groupItem)
	{
		if (!groupItem->IsExpanded())
			fProjectList->Expand(groupItem);
		fProjectList->AddUnder(item,groupItem);
		fProject->AddFile(file,groupItem->GetData(),0);
	}
	else
	{
		if (selection >= 0)
		{
			fProjectList->AddItem(item,selection + 1);
			groupItem = (SourceGroupItem*)fProjectList->Superitem(item);
			
			int32 index = fProjectList->UnderIndexOf(item);
			
			// if we've added it via drag & drop, add it after the item under the
			// drop point. If, however, we added it via the filepanel, it makes the
			// most sense to add it after the selection
//			if (pt)
//				index++;
			fProject->AddFile(file,groupItem->GetData(),index);
			STRACE(1,("Added file %s to project\n",path.Path()));
		}
		else
		{
			// No selection, so we'll add it to the last group available unless there
			// isn't one, which shouldn't ever be.
			if (fProject->CountGroups())
			{
				groupItem = fProjectList->ItemForGroup(
							fProject->GroupAt(fProject->CountGroups() - 1));
				fProjectList->AddUnder(item,groupItem);
				fProjectList->MoveItem(fProjectList->FullListIndexOf(item),
										fProjectList->FullListCountItems());
				
				fProject->AddFile(file,groupItem->GetData(),-1);
				STRACE(1,("Added file %s to project\n",path.Path()));
			}
			else
			{
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
ProjectWindow::RequiresMenuLock(const int32 &command)
{
	switch (command)
	{
		case M_SHOW_ERROR_WINDOW:
		case B_ABOUT_REQUESTED:
		case M_SHOW_FIND_AND_OPEN_PANEL:
		case M_NEW_WINDOW:
		case M_SHOW_LICENSES:
		case M_SHOW_ASCII_TABLE:
		case M_SHOW_VREGEX:
		case M_SHOW_PROJECT_FOLDER:
		{
			return false;
			break;
		}
		default:
			break;
	}
	return true;
}


void
ProjectWindow::ActOnSelectedFiles(const int32 &command)
{
	SCMOutputWindow *win = NULL;
	switch (command)
	{
		case M_ADD_SELECTION_TO_REPO:
		{
			if (!fSourceControl)
				return;
				
			win = new SCMOutputWindow(TR("Add to Repository"));
			win->Show();
			break;
		}
		case M_REMOVE_SELECTION_FROM_REPO:
		{
			if (!fSourceControl)
				return;
				
			win = new SCMOutputWindow(TR("Remove from Repository"));
			win->Show();
			break;
		}
		case M_REVERT_SELECTION:
		{
			if (!fSourceControl)
				return;
				
			win = new SCMOutputWindow(TR("Revert"));
			win->Show();
			break;
		}
		case M_DIFF_SELECTION:
		{
			if (!fSourceControl)
				return;
				
			win = new SCMOutputWindow(TR("Show Differences"));
			win->Show();
			break;
		}
		default:
			break;
	}
	
	for (int32 i = 0; i < fProjectList->CountItems(); i++)
	{
		SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->ItemAt(i));
		if (item && item->IsSelected())
		{
			SourceFile *file = item->GetData();
			
			BString relPath = file->GetPath().GetFullPath();
			if (relPath.FindFirst(fProject->GetPath().GetFolder()) == 0)
			{
				relPath.RemoveFirst(fProject->GetPath().GetFolder());
				relPath.RemoveFirst("/");
			}
			
			BString relPartnerPath;
			entry_ref partnerRef = GetPartnerRef(file->GetPath().GetRef());
			if (partnerRef.name)
			{
				DPath partnerPath(partnerRef);
				relPartnerPath = partnerPath.GetFullPath();
				if (relPartnerPath.FindFirst(fProject->GetPath().GetFolder()) == 0)
				{
					relPartnerPath.RemoveFirst(fProject->GetPath().GetFolder());
					relPartnerPath.RemoveFirst("/");
				}
			}
			switch (command)
			{
				case M_REBUILD_FILE:
				{
					if (file->UsesBuild())
					{
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
				default:
				{
					return;
				}
			}
		}
	}
}


void
ProjectWindow::SetupMenus(void)
{
	fFileMenu = new BMenu(TR("File"));
	fFileMenu->AddItem(new BMenuItem(TR("New Project…"),new BMessage(M_NEW_WINDOW),
									'N',B_COMMAND_KEY | B_SHIFT_KEY));
	fFileMenu->AddItem(new BMenuItem(TR("Open Project…"),new BMessage(M_SHOW_OPEN_PROJECT),
									'O',B_COMMAND_KEY));
	fRecentMenu = new BMenu(TR("Open Recent Project"));
	fFileMenu->AddItem(fRecentMenu);
	fFileMenu->AddItem(new BMenuItem(TR("Find and Open File…"),new BMessage(M_SHOW_FIND_AND_OPEN_PANEL),
									'D',B_COMMAND_KEY));
	fFileMenu->AddSeparatorItem();
	fFileMenu->AddItem(new BMenuItem(TR("Program Settings…"),new BMessage(M_SHOW_PROGRAM_SETTINGS)));
	fFileMenu->AddItem(new BMenuItem(TR("About Paladin…"),new BMessage(B_ABOUT_REQUESTED)));
	fMenuBar->AddItem(fFileMenu);
	
	
	fSourceMenu = new BMenu(TR("Source Control"));
	fSourceMenu->AddItem(new BMenuItem(TR("Check Project In"),
										new BMessage(M_GET_CHECK_IN_MSG), 'I',
										B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddItem(new BMenuItem(TR("Show Changed Files from Last Check-in"),
										new BMessage(M_PROJECT_SCM_STATUS), 'S',
										B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddItem(new BMenuItem(TR("Show Differences from Last Check-in"),
										new BMessage(M_DIFF_PROJECT), 'D',
										B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddSeparatorItem();
	fSourceMenu->AddItem(new BMenuItem(TR("Revert Project"),
										new BMessage(M_REVERT_PROJECT)));
	fSourceMenu->AddSeparatorItem();
	fSourceMenu->AddItem(new BMenuItem(TR("Add Selected Files to Repository"),
										new BMessage(M_ADD_SELECTION_TO_REPO)));
	fSourceMenu->AddItem(new BMenuItem(TR("Remove Selected Files from Repository"),
										new BMessage(M_REMOVE_SELECTION_FROM_REPO)));
	fSourceMenu->AddItem(new BMenuItem(TR("Revert Selected Files"),
										new BMessage(M_REVERT_SELECTION)));
	fSourceMenu->AddSeparatorItem();
	fSourceMenu->AddItem(new BMenuItem(TR("Push Changes to Remote Repository"),
										new BMessage(M_PUSH_PROJECT), B_UP_ARROW,
										B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	fSourceMenu->AddItem(new BMenuItem(TR("Pull Changes from Remote Repository"),
										new BMessage(M_PULL_PROJECT), B_DOWN_ARROW,
										B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY));
	
	
	fProjectMenu = new BMenu(TR("Project"));
	
	fProjectMenu->AddItem(new BMenuItem(TR("Settings…"),new BMessage(M_SHOW_PROJECT_SETTINGS)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Add New File…"),new BMessage(M_SHOW_ADD_NEW_PANEL),
										'N',B_COMMAND_KEY));
	fProjectMenu->AddItem(new BMenuItem(TR("Add Files…"),new BMessage(M_SHOW_ADD_PANEL)));
	fProjectMenu->AddItem(new BMenuItem(TR("Remove Selected Files"),new BMessage(M_REMOVE_FILES), B_DELETE));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Change System Libraries…"),
										new BMessage(M_SHOW_LIBRARIES)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Find in Project Files" B_UTF8_ELLIPSIS),
											new BMessage(M_SHOW_FIND_IN_PROJECT_FILES), 'F',
														B_COMMAND_KEY | B_SHIFT_KEY));
	
	#ifdef BUILD_CODE_LIBRARY
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Synchronize with Code Library"),
										new BMessage(M_SYNC_MODULES)));
	#endif
	fProjectMenu->AddSeparatorItem();
	
	fProjectMenu->AddItem(fSourceMenu);
	
	fProjectMenu->AddSeparatorItem();
	
	fProjectMenu->AddItem(new BMenuItem(TR("New Group"),new BMessage(M_NEW_GROUP)));
	fProjectMenu->AddItem(new BMenuItem(TR("Rename Group"),new BMessage(M_SHOW_RENAME_GROUP)));
	fProjectMenu->AddItem(new BMenuItem(TR("Sort Group"),new BMessage(M_SORT_GROUP)));
	fProjectMenu->AddSeparatorItem();
	fProjectMenu->AddItem(new BMenuItem(TR("Show Project Folder…"),
							new BMessage(M_SHOW_PROJECT_FOLDER)));
	fMenuBar->AddItem(fProjectMenu);
	
	
	fBuildMenu = new BMenu(TR("Build"));
	
	fBuildMenu->AddItem(new BMenuItem(TR("Make Project"),new BMessage(M_BUILD_PROJECT),'M'));
	fBuildMenu->AddItem(new BMenuItem(TR("Run"),new BMessage(M_RUN_PROJECT),'R'));
	fBuildMenu->AddItem(new BMenuItem(TR("Run Logged…"),new BMessage(M_RUN_IN_TERMINAL),
										'R', B_COMMAND_KEY | B_SHIFT_KEY));
	fBuildMenu->AddItem(new BMenuItem(TR("Debug"),new BMessage(M_DEBUG_PROJECT),'R',
										B_COMMAND_KEY | B_CONTROL_KEY));
	fBuildMenu->AddSeparatorItem();
	fBuildMenu->AddItem(new BMenuItem(TR("Generate Makefile…"),new BMessage(M_MAKE_MAKE)));
	fBuildMenu->AddSeparatorItem();
	fBuildMenu->AddItem(new BMenuItem(TR("Set Run Arguments…"),new BMessage(M_SHOW_RUN_ARGS)));
	fBuildMenu->AddSeparatorItem();
	fBuildMenu->AddItem(new BMenuItem(TR("Update Dependencies"),new BMessage(M_UPDATE_DEPENDENCIES)));
	
	BMenuItem *item = new BMenuItem(TR("Empty Build Cache"),new BMessage(M_EMPTY_CCACHE));
	fBuildMenu->AddItem(item);
	item->SetEnabled(gCCacheAvailable);
	
	fBuildMenu->AddItem(new BMenuItem(TR("Force Project Rebuild"),new BMessage(M_FORCE_REBUILD),'-'));
	fMenuBar->AddItem(fBuildMenu);
	
	fToolsMenu = new BMenu(TR("Tools"));
	
	#ifdef BUILD_CODE_LIBRARY
	fToolsMenu->AddItem(new BMenuItem(TR("Code Library…"),new BMessage(M_SHOW_CODE_LIBRARY),'L'));
	#endif
	fToolsMenu->AddItem(new BMenuItem(TR("Error Window…"),new BMessage(M_TOGGLE_ERROR_WINDOW),'I'));
	fToolsMenu->AddItem(new BMenuItem(TR("ASCII Table…"),new BMessage(M_SHOW_ASCII_TABLE)));
	fToolsMenu->AddItem(new BMenuItem(TR("Regular Expression Tester…"),new BMessage(M_SHOW_VREGEX)));
	
	BMessage *msg = new BMessage(M_RUN_TOOL);
	msg->AddString("signature", "application/x-vnd.dw-SymbolFinder");
	fToolsMenu->AddItem(new BMenuItem(TR("Symbol Finder…"), msg));
	fToolsMenu->AddSeparatorItem();
	fToolsMenu->AddItem(new BMenuItem(TR("Make Project Backup"),new BMessage(M_BACKUP_PROJECT)));
	fToolsMenu->AddSeparatorItem();
	fToolsMenu->AddItem(new BMenuItem(TR("Set Software License…"),new BMessage(M_SHOW_LICENSES)));
	fMenuBar->AddItem(fToolsMenu);
}


void
ProjectWindow::SetMenuLock(bool locked)
{
	if (locked)
	{
		fProjectMenu->SetEnabled(false);
		fBuildMenu->SetEnabled(false);
	}
	else
	{
		fProjectMenu->SetEnabled(true);
		fBuildMenu->SetEnabled(true);
	}
}


void
ProjectWindow::MakeGroup(int32 selection)
{
	if (selection < 1 || 
		dynamic_cast<SourceGroupItem*>(fProjectList->FullListItemAt(selection)) ||
		dynamic_cast<SourceGroupItem*>(fProjectList->FullListItemAt(selection - 1)))
		return;
	
	int32 newgroupindex = -1;
	if (fProject->CountGroups() > 1)
	{
		int32 groupcount = 0;
		for (int32 i = selection - 1; i > 0; i--)
			if (dynamic_cast<SourceGroupItem*>(fProjectList->FullListItemAt(i)))
				groupcount++;
	}

	SourceGroupItem *oldgroupitem = (SourceGroupItem*)fProjectList->Superitem(
													fProjectList->FullListItemAt(selection));
	SourceGroup *oldgroup = oldgroupitem->GetData();
	SourceGroup *newgroup = fProject->AddGroup("New Group",newgroupindex);
	SourceGroupItem *newgroupitem = new SourceGroupItem(newgroup);
	fProjectList->AddItem(newgroupitem,selection);

	int32 index = selection + 1;
	SourceFileItem *fileitem = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(index));
	while (fileitem)
	{
		fProjectList->RemoveItem(fileitem);
		fProjectList->AddItem(fileitem,index);
		oldgroup->filelist.RemoveItem(fileitem->GetData(),false);
		newgroup->filelist.AddItem(fileitem->GetData());
		
		index++;
		fileitem = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(index));
	}
	
	fProjectList->Expand(newgroupitem);
	fProjectList->InvalidateItem(selection);
	fProject->Save();
}


void
ProjectWindow::ToggleErrorWindow(ErrorList *list)
{
	if (fErrorWindow)
		fErrorWindow->PostMessage(B_QUIT_REQUESTED);
	else
		ShowErrorWindow(list);
}


void
ProjectWindow::ShowErrorWindow(ErrorList *list)
{
	if (fErrorWindow && list)
	{
		BMessage msg;
		list->Flatten(msg);
		msg.what = M_BUILD_WARNINGS;
		fErrorWindow->PostMessage(&msg);
	}
	else
	{
		BRect screen(BScreen().Frame());
		BRect r(screen);
		
		r.left = r.right / 4.0;
		r.right *= .75;
		r.top = r.bottom - 200;
		BDeskbar deskbar;
		if (deskbar.Location() == B_DESKBAR_BOTTOM)
			r.OffsetBy(0,-deskbar.Frame().Height());
		
		fErrorWindow = new ErrorWindow(r,this,list);
		fErrorWindow->Show();
	}
	fStatusBar->SetText("");
}


void
ProjectWindow::CullEmptyGroups(void)
{
	for (int32 i = fProjectList->CountItems() - 1; i >= 0; i--)
	{
		SourceGroupItem *groupitem = dynamic_cast<SourceGroupItem*>(fProjectList->ItemAt(i));
		if (groupitem && groupitem->GetData()->filelist.CountItems() == 0 && fProject->CountGroups() > 1)
		{
			fProject->RemoveGroup(groupitem->GetData(),true);
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
	for (int32 i = 0; i < fProjectList->CountItems(); i++)
	{
		SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->FullListItemAt(i));
		if (item)
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
	BMenuItem *debugItem = fMenuBar->FindItem("Debug Paladin");
	if (debugItem)
		fMenuBar->RemoveItem(debugItem);
	else
	{
		BMenu *debug = new BMenu("Debug Paladin");
		debug->AddItem(new BMenuItem("Dump Dependencies",new BMessage(M_DEBUG_DUMP_DEPENDENCIES)));
		debug->AddItem(new BMenuItem("Dump Includes",new BMessage(M_DEBUG_DUMP_INCLUDES)));
		fMenuBar->AddItem(debug);
	}
}


void
ProjectWindow::DoBuild(int32 postbuild)
{
	if (fErrorWindow)
		fErrorWindow->PostMessage(M_CLEAR_ERROR_LIST);
	fProject->GetErrorList()->msglist.MakeEmpty();
	
	// Missing file check
	for (int32 i = 0; i < fProjectList->CountItems(); i++)
	{
		SourceFileItem *item = dynamic_cast<SourceFileItem*>(fProjectList->ItemAt(i));
		if (item && item->GetDisplayState() == SFITEM_MISSING)
		{
			BAlert *alert = new BAlert("Paladin",
					TR("The project cannot be built because some of its files are missing."),"OK");
			alert->Go();
			return;
		}
	}
	
	fStatusBar->SetText("Examining source files");
	UpdateIfNeeded();
	
	SetMenuLock(true);
	fBuilder.BuildProject(fProject,postbuild);
}


void
ProjectWindow::AddNewFile(BString name, bool create_pair)
{
	BMessage msg(M_ADD_FILES);
	
	DPath projfile(fProject->GetPath().GetFolder());
	projfile << name;
	
	entry_ref ref = gFileFactory.CreateSourceFile(projfile.GetFolder(),
												projfile.GetFileName(),
												create_pair ? SOURCEFILE_PAIR : 0);
	if (!ref.name || strlen(ref.name) == 0)
		return;
	
	AddFile(ref);
	
	if (fSourceControl)
		fSourceControl->AddToRepository(projfile.GetFullPath());
	
	msg.AddRef("refs",&ref);
	
	if (create_pair && fSourceControl)
	{
		entry_ref partnerRef = GetPartnerRef(ref);
		DPath partnerPath(partnerRef);
		fSourceControl->AddToRepository(projfile.GetFullPath());
		msg.AddRef("refs",&partnerRef);
		
		if (!gDontManageHeaders)
			AddFile(partnerRef);
		fSourceControl->AddToRepository(partnerPath.GetFullPath());
	}
	
	be_app->PostMessage(&msg);
}


int32
ProjectWindow::AddFileThread(void *data)
{
	add_file_struct *addstruct = (add_file_struct*)data;
	
	int32 i = 0;
	entry_ref addref;
	while (addstruct->refmsg.FindRef("refs",i,&addref) == B_OK)
	{
		BNode node(&addref);
		BString type;
		if (node.ReadAttrString("BEOS:TYPE",&type) == B_OK && type == PROJECT_MIME_TYPE)
		{
			BMessage refmsg(B_REFS_RECEIVED);
			refmsg.AddRef("refs",&addref);
			be_app->PostMessage(&refmsg);
			i++;
			continue;
		}
		
		BEntry entry(&addref);
		DPath path(addref);
		if (entry.IsDirectory())
			addstruct->parent->AddFolder(addref);
		else
		{
			addstruct->parent->Lock();
			addstruct->parent->AddFile(addref,&addstruct->droppt);
			addstruct->parent->Unlock();
		}
		
		i++;
	}
	
	addstruct->parent->Lock();
	addstruct->parent->CullEmptyGroups();
	addstruct->parent->fProject->Save();
	addstruct->parent->Unlock();
	return 0;
}


void
ProjectWindow::AddFolder(entry_ref folderref)
{
	BDirectory dir;
	if (dir.SetTo(&folderref) != B_OK)
		return;
	
	if (strcmp(folderref.name,"CVS") == 0 ||
		strcmp(folderref.name,".svn") == 0 ||
		strcmp(folderref.name,".git") == 0 ||
		strcmp(folderref.name,".hg") == 0)
		return;
	
	dir.Rewind();
	
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		if (BEntry(&ref).IsDirectory())
			AddFolder(ref);
		else
		{
			// Here is where we actually add the file to the project. The name of the folder
			// containing the file will be the name of the file's group. If the group doesn't
			// exist, it will be created at the end of the list, but if it does, we will fake
			// a drop onto the group item to reuse the code path by finding the group and getting
			// its position in the list.
			
			DPath filepath(ref);
			if (filepath.GetExtension() && 
				!( strcmp(filepath.GetExtension(),"cpp") == 0 ||
				strcmp(filepath.GetExtension(),"c") == 0 ||
				strcmp(filepath.GetExtension(),"cc") == 0 ||
				strcmp(filepath.GetExtension(),"cxx") == 0 ||
				strcmp(filepath.GetExtension(),"rdef") == 0 ||
				strcmp(filepath.GetExtension(),"rsrc") == 0) )
				continue;
			
			// Don't bother adding build files from other systems
			if (strcmp(filepath.GetFileName(), "Jamfile") == 0 ||
				strcmp(filepath.GetFileName(), "Makefile") == 0)
				continue;
			
			DPath parent(filepath.GetFolder());
			SourceGroup *group = NULL;
			SourceGroupItem *groupItem = NULL;
			
			fProject->Lock();
			group = fProject->FindGroup(parent.GetFileName());
			
			Lock();
			if (group)
				groupItem = fProjectList->ItemForGroup(group);
			else
			{
				group = fProject->AddGroup(parent.GetFileName());
				groupItem = new SourceGroupItem(group);
				fProjectList->AddItem(groupItem);
				UpdateIfNeeded();
			}
			
			if (groupItem)
			{
				int32 index = fProjectList->IndexOf(groupItem);
				BPoint pt = fProjectList->ItemFrame(index).LeftTop();
				pt.x += 5;
				pt.y += 5;
				
				AddFile(ref,&pt);
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
	
	DPath srcpath(ref);
	DPath destpath(fProject->GetPath().GetFolder());
	command << "copyattr --data '" << srcpath.GetFullPath() << "' '" 
			<< destpath.GetFullPath() << "'";
	system(command.String());
	
	BString ext(srcpath.GetExtension());
	if ((ext.ICompare("h") == 0) || (ext.ICompare("hxx") == 0) ||
			(ext.ICompare("hpp") == 0) || (ext.ICompare("h++") == 0))
		return;
	
	DPath destfile(destpath);
	destfile << ref.name;
	BEntry destEntry(destfile.GetFullPath());
	entry_ref destref;
	destEntry.GetRef(&destref);
	AddFile(destref,NULL);
}


int32
ProjectWindow::ImportFileThread(void *data)
{
	add_file_struct *addstruct = (add_file_struct*)data;
	
	int32 i = 0;
	entry_ref addref;
	while (addstruct->refmsg.FindRef("refs",i,&addref) == B_OK)
	{
		addstruct->parent->Lock();
		addstruct->parent->ImportFile(addref);
		addstruct->parent->Unlock();
		i++;
	}
	
	addstruct->parent->Lock();
	addstruct->parent->CullEmptyGroups();
	addstruct->parent->fProject->Save();
	addstruct->parent->Unlock();
	return 0;
}


int32
ProjectWindow::BackupThread(void *data)
{
	ProjectWindow *parent = (ProjectWindow*)data;
	Project *proj = parent->fProject;
	
	char timestamp[32];
	time_t t = real_time_clock();
	strftime(timestamp,32,"_%Y-%m-%d-%H%M%S",localtime(&t));
	
	BPath folder(proj->GetPath().GetFolder());
	BPath folderparent;
	folder.GetParent(&folderparent);
	
	BString command = "cd '";
	command << folderparent.Path() << "'; ";
	command << "zip -9 -r -y '"
			<< gBackupPath.GetFullPath() << "/"
			<< proj->GetName() << timestamp << "' '"
			<< folder.Leaf() << "' -x *.o";
	
	system(command.String());
	
	parent->Lock();
	parent->fStatusBar->SetText("");
	parent->SetMenuLock(false);
	parent->Unlock();
	
	return 0;
}


int32
ProjectWindow::SyncThread(void *data)
{
	#ifdef BUILD_CODE_LIBRARY
	ProjectWindow *parent = (ProjectWindow*)data;
	
	parent->Lock();
	parent->fStatusBar->SetText(TR("Updating Modules"));
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
compare_source_file_items(const BListItem *item1, const BListItem *item2)
{
	if (!item2)
		return (item1 != NULL) ? -1 : 0;
	else
	if (!item1)
		return 1;
	
	SourceFileItem *one = (SourceFileItem*)item1;
	SourceFileItem *two = (SourceFileItem*)item2;
	
	if (!one->Text())
		return (two->Text() != NULL) ? 1 : 0;
	else
	if (!two->Text())
		return -1;
	
	// Sort based on type.
	int8 type1, type2;
	switch (one->GetData()->GetType())
	{
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
			break;
		}
	}
	
	switch (two->GetData()->GetType())
	{
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
			break;
		}
	}
	
	if (type1 < type2)
		return -1;
	else if (type2 < type1)
		return 1;
	
	return strcmp(one->Text(),two->Text());
}
