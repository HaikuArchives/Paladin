/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Copyright 2018 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include "Paladin.h"

#include <Alert.h>
#include <Catalog.h>
#include <Locale.h>
#include <Locker.h>
#include <getopt.h>
#include <Message.h>
#include <Messenger.h>
#include <Mime.h>
#include <Node.h>
#include <Path.h>
#include <Roster.h>
#include <stdlib.h>
#include <String.h>
#include <TranslationUtils.h>

#include "AboutWindow.h"
#include "DebugTools.h"
#include "DPath.h"
#include "ErrorParser.h"
#include "FileUtils.h"
#include "Globals.h"
#include "LaunchHelper.h"
#include "Makemake.h"
#include "MsgDefs.h"
#include "ObjectList.h"
#include "Project.h"
#include "ProjectBuilder.h"
#include "ProjectWindow.h"
#include "SCMManager.h"
#include "Settings.h"
#include "SourceFile.h"
#include "StartWindow.h"
#include "TemplateWindow.h"
#include "PaladinFileFilter.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Paladin"

BPoint gProjectWindowPoint;
static int sReturnCode = 0;

static int32 sWindowCount = 0;
static BLocker sWindowLocker;
int32 gQuitOnZeroWindows = 1;


void
RegisterWindow(void)
{
	sWindowLocker.Lock();
	sWindowCount++;
	sWindowLocker.Unlock();
}


void
DeregisterWindow(void)
{
	bool quit = false;
	sWindowLocker.Lock();
	sWindowCount--;
	
	if (sWindowCount == 0)
		quit = true;
	
	sWindowLocker.Unlock();
	if (quit)
	{
		if (gQuitOnZeroWindows)
			be_app->PostMessage(B_QUIT_REQUESTED);
		else
			gQuitOnZeroWindows = 1;
	}
}


int32
CountRegisteredWindows(void)
{
	int32 count;
	sWindowLocker.Lock();
	count = sWindowCount;
	sWindowLocker.Unlock();
	return count;
}


void
PrintUsage(void)
{
	#ifdef USE_TRACE_TOOLS
	printf(B_TRANSLATE("Usage: Paladin [-b] [-m] [-r] [-s] [-d] [-v] [file1 [file2 ...]]\n"
			"-b, Build the specified project. Only one file can be specified with this switch.\n"
			"-m, Generate a makefile for the specified project.\n"
			"-r, Completely rebuild the project.\n"
			"-s, Use only one thread for building.\n"
			"-d, Print debugging output.\n"
			"-v, Make debugging mode verbose.\n"));
	#else
	printf(B_TRANSLATE("Usage: Paladin [-b] [-m] [-r] [-s] [file1 [file2 ...]]\n"
			"-b, Build the specified project. Only one file can be specified with this switch.\n"
			"-m, Generate a makefile for the specified project.\n"
			"-r, Completely rebuild the project.\n"
			"-s, Use only one thread for building.\n"));
	#endif
}


App::App(void)
	:
	BApplication(APP_SIGNATURE),
	fBuildCleanMode(false),
	fBuilder(NULL)
{
	InitFileTypes();
	InitGlobals();
	EnsureTemplates();
	
	gProjectList = new LockableList<Project>(20,true);
	gProjectWindowPoint.Set(5,24);
	
	fOpenPanel = NULL;
}


App::~App(void)
{
	gSettings.Save();
	
	if (NULL != fBuilder)
		delete fBuilder;
	if (NULL != fOpenPanel)
		delete fOpenPanel;
}


void
App::AboutRequested(void)
{
	AboutWindow *win = new AboutWindow();
	win->Show();
}


void
App::ArgvReceived(int32 argc,char **argv)
{
	bool showUsage = false;
	bool verbose = false;
	int32 i = 1;
	for (i = 1; i < argc; i++)
	{
		int arglen = strlen(argv[i]);
		char *arg = argv[i];
		
		char opt;
		if (arglen == 2 && arg[0] == '-')
			opt = arg[1];
		else
			break;
			
		switch (opt)
		{
			case 'b':
			{
				gBuildMode = true;
				break;
			}
			case 'm':
			{
				gMakeMode = true;
				break;
			}
			case 'r':
			{
				gBuildMode = true;
				fBuildCleanMode = true;
				break;
			}
			case 's':
			{
				gSingleThreadedBuild = true;
				break;
			}
			
			#ifdef USE_TRACE_TOOLS
			case 'v':
			{
				verbose = true;
				break;
			}
			case 'd':
			{
				gPrintDebugMode = 1;
				break;
			}
			#endif
			
			default:
			{
				showUsage = true;
				break;
			}
		}
	}

	if (gPrintDebugMode > 0 && verbose)
		gPrintDebugMode = 2;
	
	if (showUsage)
	{
		PrintUsage();
		
		// A quick hack to not show the start window before
		// the quit request has been processed
		sWindowCount++;
		
		PostMessage(B_QUIT_REQUESTED);
		return;
	}
	
	if (gPrintDebugMode == 1)
		printf(B_TRANSLATE("Printing debug output\n"));
	else if (gPrintDebugMode == 2)
		printf(B_TRANSLATE("Printing debug output with extra detail\n"));
	
	if (gSingleThreadedBuild)
		STRACE(1,("Disabling multithreaded project building\n"));

		
	BMessage refmsg;
	int32 refcount = 0;

	// No initialization -- keep the value from the previous loop
	BEntry projfolder(gProjectPath.GetFullPath());
	entry_ref projref;
	projfolder.GetRef(&projref);
	for (; i < argc; i++)
	{
		// See if the project specified lacks an extension and append it
		BString projPath(argv[i]);
		if (projPath.FindLast(".pld") != projPath.CountChars() - 4)
		{
			projPath << ".pld";
			printf(B_TRANSLATE("Attempting to open %s\n"), projPath.String());
		}
		
		BEntry entry(projPath.String());
		
		entry_ref ref;
		
		if (!entry.Exists())
		{
			ref = FindProject(projref,projPath.String());
			if (!ref.name)
			{
				printf(B_TRANSLATE("Can't find file %s\n"),argv[i]);
				continue;
			}
		}
		else
			entry.GetRef(&ref);
		
		refmsg.AddRef("refs",&ref);
		refcount++;
		optind++;
		
		if (refcount == 1 && (gBuildMode || gMakeMode))
			break;
	}
	
	if (refcount > 0)
		RefsReceived(&refmsg);
	else if (gBuildMode || gMakeMode)
		Quit();
}


void
App::RefsReceived(BMessage *msg)
{
	entry_ref ref;
	int32 i = 0;
	while (msg->FindRef("refs",i,&ref) == B_OK)
	{
		bool isPaladin = Project::IsProject(ref);
		bool isBeIDE = IsBeIDEProject(ref);

		if (gBuildMode && isPaladin)
			BuildProject(ref);
		else
		if (gMakeMode && isPaladin)
			GenerateMakefile(ref);
		else
		if (isPaladin || isBeIDE)
			LoadProject(ref);
		else
			OpenFile(ref);
		i++;
	}
}


bool
App::QuitRequested(void)
{
	gSettings.SetString("lastprojectpath",gLastProjectPath.GetFullPath());
	return true;
}


void
App::ReadyToRun(void)
{
	if (CountRegisteredWindows() < 1 && !gBuildMode)
	{
		StartWindow *win = new StartWindow();
		win->Show();
	}
}


void
App::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_MAKE_PROJECT:
		case M_RUN_PROJECT:
		case M_RUN_IN_TERMINAL:
		case M_RUN_IN_DEBUGGER:
		case M_RUN_WITH_ARGS:
		case M_FORCE_REBUILD:
		case M_SHOW_ADD_NEW_PANEL:
		case M_SHOW_FIND_AND_OPEN_PANEL:
		case M_SHOW_FIND_IN_PROJECT_FILES:
		case M_SHOW_ERROR_WINDOW:
		case M_TOGGLE_ERROR_WINDOW:
		{
			entry_ref ref;
			if (msg->FindRef("refs",&ref) == B_OK)
				PostToProjectWindow(msg,&ref);
			else
				PostToProjectWindow(msg,NULL);
			
			break;
		}

		case M_OPEN_PARTNER:
		{
			entry_ref ref;
			if (msg->FindRef("refs",&ref) == B_OK)
				OpenPartner(ref);
			break;
		}

		case M_NEW_PROJECT:
		{
			TemplateWindow *win = new TemplateWindow(BRect(100, 100, 400, 300));
			win->Show();
			break;
		}

		case M_SHOW_OPEN_PROJECT:
		{
			CheckCreateOpenPanel();
			fOpenPanel->Show();
			break;
		}

		case M_CREATE_PROJECT:
		{
			CreateNewProject(*msg);
			break;
		}

		case M_QUICK_IMPORT:
		{
			entry_ref ref;
			if (msg->FindRef("refs",&ref) != B_OK || !QuickImportProject(DPath(ref)))
			{
				StartWindow *startwin = new StartWindow();
				startwin->Show();
				break;
			}
			break;
		}
		
		// These are for quit determination. We have to use our own counter variable
		// (sWindowCount) because BFilePanels throw the count off. Using a variable
		// is much preferable to subclassing just for this reason.
		case M_REGISTER_WINDOW:
		{
			sWindowCount++;
			break;
		}

		case M_DEREGISTER_WINDOW:
		{
			sWindowCount--;
			if (sWindowCount <= 1)
				PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case EDIT_OPEN_FILE:
		{
			int32 index = 0;
			entry_ref ref;
			while (msg->FindRef("refs",index,&ref) == B_OK)
			{
				int32 line;
				if (msg->FindInt32("line",index,&line) != B_OK)
					line = -1;
				int32 column;
				if (msg->FindInt32("column",index,&column) != B_OK)
					column = -1;
				
				OpenFile(ref,line,column);
				
				index++;
			}
			
			CheckCreateOpenPanel();
			fOpenPanel->GetPanelDirectory(&ref);
			gLastProjectPath.SetTo(ref);
			fOpenPanel->Window();
			break;
		}

		case M_FIND_AND_OPEN_FILE:
		{
			FindAndOpenFile(msg);
			break;
		}

		case M_BUILDING_FILE:
		{
			SourceFile *file;
			if (msg->FindPointer("sourcefile",(void**)&file) == B_OK)
				printf(B_TRANSLATE("Building %s\n"),file->GetPath().GetFileName());
			else
				printf(B_TRANSLATE("NULL pointer in M_BUILDING_FILE\n"));
			break;
		}

		case M_LINKING_PROJECT:
		{
			printf(B_TRANSLATE("Linking\n"));
			break;
		}

		case M_UPDATING_RESOURCES:
		{
			printf(B_TRANSLATE("Updating resources\n"));
			break;
		}

		case M_BUILD_FAILURE:
		{
			BString errstr;
			if (msg->FindString("errstr",&errstr) == B_OK)
				printf("%s\n",errstr.String());
			else
			{
				ErrorList errors;
				errors.Unflatten(*msg);
				printf(B_TRANSLATE("Build failure\n%s"), errors.AsString().String());
			}
			sReturnCode = -1;
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case M_BUILD_WARNINGS:
		{
			BString errstr;
			if (msg->FindString("errstr",&errstr) == B_OK)
				printf("%s\n",errstr.String());
			break;
		}

		case M_BUILD_SUCCESS:
		{
			printf(B_TRANSLATE("Success\n"));
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		default:
			BApplication::MessageReceived(msg);
	}
}

void
App::CheckCreateOpenPanel()
{
	if (NULL == fOpenPanel)
	{
		BMessenger msgr(this);
		BEntry entry(gLastProjectPath.GetFullPath());
		entry_ref ref;
		entry.GetRef(&ref);
		fOpenPanel = new BFilePanel(B_OPEN_PANEL, &msgr, &ref, B_FILE_NODE, false,
			new BMessage(B_REFS_RECEIVED), new PaladinFileFilter() );
		fOpenPanel->Window()->SetTitle(B_TRANSLATE("Paladin: Open project"));
	}
}

void
App::OpenFile(entry_ref ref, int32 line, int32 column)
{
	if (!ref.name)
		return;
	
	BNode node(&ref);
	BString type;
	if (node.ReadAttrString("BEOS:TYPE",&type) != B_OK)
	{
		update_mime_info(BPath(&ref).Path(),0,0,1);
		node.ReadAttrString("BEOS:TYPE",&type);
	}
	
	if (type.CountChars() > 0)
	{
		if (type == PROJECT_MIME_TYPE || type.FindFirst("text/") != 0)
		{
			STRACE(2,("Launching text or project file in roster...\n"));
			be_roster->Launch(&ref);
			return;
		}
	}
	else
	{
		BString extension = BPath(&ref).Path();
		int32 pos = extension.FindLast(".");
		if (pos >= 0)
		{
			extension = extension.String() + pos;
			if (extension.ICompare(".cpp") != 0 &&
				extension.ICompare(".h") != 0 &&
				extension.ICompare(".hpp") != 0 &&
				extension.ICompare(".c") != 0)
			{
				return;
			}
		}
	}
	
	
	BMessage* msg = new BMessage(B_REFS_RECEIVED);
	msg->AddRef("refs",&ref);
	if (line >= 0)
		msg->AddInt32("be:line",line);
	if (column >= 0)
		msg->AddInt32("be:column",column);
	STRACE(2,("Paladin Launching File Ref: %s:%i:%i\n",ref.name,line,column));
	
	/*
	  ANTI-PATTERN - this code left here for future developers' reference
	  Using the below method launches the app, and then be_roster sends
	  ANOTHER B_REFS_REVEIVED of its own, causing the file to be opened twice,
	  or once at the wrong location (i.e. without a line number).
	be_roster->Launch(&ref,msg);
	*/
	
	/*
	entry_ref appRef;
	be_roster->FindApp(&ref,&appRef);
	app_info appInfo;
	be_roster->GetAppInfo(&appRef,&appInfo);
	BMessenger messenger(appInfo.signature);
	messenger.SendMessage(msg,(BHandler*)NULL,B_INFINITE_TIMEOUT);
	// The above results in nothing happening unless the app is ALREADY running
	*/
	
	// Final effort - use the text_search tracker messenger mechanism
	BMessenger target("application/x-vnd.Be-TRAK");
	target.SendMessage(msg);
}


void
App::OpenPartner(entry_ref ref)
{
	entry_ref partnerRef = GetPartnerRef(ref);
	DPath refpath(BPath(&ref).Path());
	
	if (partnerRef.name)
	{
		OpenFile(partnerRef);
		return;
	}
	
	BString errmsg = B_TRANSLATE(
		"Couldn't find a partner file for %refname% in %reffolder%/.");
	errmsg.ReplaceFirst("%refname%", ref.name);
	errmsg.ReplaceFirst("%reffolder%", refpath.GetFolder());
	ShowAlert(errmsg.String());
}


Project *
App::CreateNewProject(const BMessage &settings)
{
	Project *proj = NULL;
	
	BString projectName, targetName, projectPath, templateName, pldName;
	int32 projectType, scmType;
	bool createFolder, populateProject = true;
	
	settings.FindString("name",&projectName);
	settings.FindString("target",&targetName);
	settings.FindInt32("type",&projectType);
	settings.FindString("path",&projectPath);
	settings.FindInt32("scmtype", &scmType);
	settings.FindBool("createfolder",&createFolder);
	settings.FindString("template", &templateName);
	settings.FindString("pldfile", &pldName);

	if (templateName.CountChars() > 0)
	{
		// Templates are now a directory with a TEMPLATEINFO file. All files in the
		// directory are copies, allowing for much greater flexibility than before.
		
		BString projectFileName(projectName);
		projectFileName << ".pld";
		
		DPath templatePath(gAppPath.GetFolder());
		templatePath << "Templates" << templateName;
		
		// Copy the contents of the chosen template folder to the project path
		DPath sourcePath(templatePath);
		DPath destPath(gProjectPath);
		
		if (createFolder)
		{
			destPath << projectName;
			create_directory(destPath.GetFullPath(), 0700);
		}
		
		BString wildcard("'");
		wildcard << sourcePath.GetFullPath() << "'/*";
		ShellHelper shell("cp --no-preserve=mode,ownership ");
		shell << wildcard;
		shell.AddQuotedArg(destPath.GetFullPath());
		shell.Run();
		
		// The copy command copies *everything*, so we have to delete the
		// TEMPLATEINFO file.
		DPath templateInfo(destPath);
		templateInfo << "TEMPLATEINFO";
		BEntry infoEntry(templateInfo.GetFullPath());
		infoEntry.Remove();
		infoEntry.Unset();
		
		DPath finalPath;
		
		// Load project and set info or create one, if needed.
		
		// If the settings contain the name of a .pld project file, we'll search
		// for that first. Assuming that it exists, we'll rename that file to the
		// project name specified. If it doesn't exist or the .pld name is empty,
		// we'll create a new project with the appropriate name.
		
		// The pldname field comes from the TEMPLATEINFO file, which can designate
		// the main project file in a template. This allows a template to have
		// multiple project files, such as for the Tracker Add-on development framework
		// which has both a project file for generating the actual addon and another
		// one which is the testing framework.
		bool createProjFile = true;
		if (pldName.CountChars() > 0)
		{
			// If a .pld project file was specified in TEMPLATEINFO, check to see if
			// the file exists and rename it. If it doesn't exist, we'll create a new
			// file, and if a .pld file already exists with the intended name, we won't
			// do anything except tell the user what's happened.
			DPath oldPldNamePath(destPath);
			oldPldNamePath << pldName;
			BEntry oldPldNameEntry(oldPldNamePath.GetFullPath());
			
			DPath newPldNamePath(destPath);
			newPldNamePath << projectFileName;
			
			BEntry newPldNameEntry(newPldNamePath.GetFullPath());
			if (newPldNameEntry.Exists())
			{
				// createProjFile is false here only if there is a .pld file with the
				// user's chosen project name. If that is the case, we keep both files and
				// let the user sort it out.
				BString errMsg = B_TRANSLATE(
					"Project file '%projectname%.pld' already exists. The "
					"original file for this template is '%pldname%'. You'll need "
					"to open the project folder and figure out which one you wish to keep.");
				errMsg.ReplaceFirst("%projectname%", projectName);
				errMsg.ReplaceFirst("%pldname%", pldName);
				ShowAlert(errMsg);
				populateProject = createProjFile = false;
				
				finalPath = newPldNamePath;
			}
			else
			if (oldPldNameEntry.Exists())
			{
				oldPldNameEntry.Rename(projectFileName.String());
				populateProject = createProjFile = false;
				
				finalPath = newPldNamePath;
			}
		}
		
		if (createProjFile)
		{
			proj = Project::CreateProject(projectName.String(), targetName.String(),
									projectType, projectPath.String(), createFolder);
			if (proj)
				finalPath = proj->GetPath();
		}
		else
		{
			proj = new Project();
			if (proj->Load(finalPath.GetFullPath()) != B_OK)
			{
				delete proj;
				return NULL;
			}
		}
	}
	else
	{
		// This case is for stuff like the Quick Import feature
		proj = Project::CreateProject(projectName.String(), targetName.String(),
									projectType, projectPath.String(), createFolder);
	}
	
	if (!proj)
		return NULL;
	
	scm_t detectedSCM = DetectSCM(projectPath);
	proj->SetSourceControl(detectedSCM == SCM_NONE ? (scm_t)scmType : detectedSCM);
	
	gCurrentProject = proj;
	gProjectList->Lock();
	gProjectList->AddItem(proj);
	gProjectList->Unlock();
	
	BRect r(0,0,200,300);
	/*
	r.OffsetTo(gProjectWindowPoint);
	gProjectWindowPoint.x += 25;
	gProjectWindowPoint.y += 25;
	if (gProjectWindowPoint.x < 0)
		gProjectWindowPoint.x = 0;
	if (gProjectWindowPoint.y < 0)
		gProjectWindowPoint.y - 0;
		*/
	ProjectWindow *projwin = new ProjectWindow(r,gCurrentProject);
	projwin->Show();
	
	BEntry entry(gCurrentProject->GetPath().GetFullPath());
	if (entry.InitCheck() == B_OK)
	{
		entry_ref newprojref;
		entry.GetRef(&newprojref);
		UpdateRecentItems(newprojref);
	}
	
	if (populateProject)
	{
		entry_ref addRef;
		int32 i = 0;
		while (settings.FindRef("libs",i++,&addRef) == B_OK)
		{
			if (BEntry(&addRef).Exists())
				proj->AddLibrary(DPath(addRef).GetFullPath());
		}
		
		i = 0;
		BMessage addMsg(M_IMPORT_REFS);
		while (settings.FindRef("refs",i++,&addRef) == B_OK)
			addMsg.AddRef("refs",&addRef);
		PostToProjectWindow(&addMsg,NULL);
	}
	
	return proj;
}


bool
App::QuickImportProject(DPath folder)
{
	// Quickly makes a project in a folder by importing all resource files and C++ sources.
	if (!BEntry(folder.GetFullPath()).Exists())
		return false;

	BMessage settings;
	settings.AddString("name", folder.GetFileName());
	settings.AddString("target", folder.GetFileName());
	// skipping templatename field on purpose
	settings.AddInt32("type", PROJECT_GUI);
	settings.AddString("path", folder.GetFullPath());
	settings.AddInt32("scmtype", gDefaultSCM);
	settings.AddBool("createfolder", false);
	
	DPath path(folder);
	BEntry entry(path.GetFullPath());
	Project *proj = CreateNewProject(settings);
	if (!proj)
		return false;
	
	entry.SetTo(folder.GetFullPath());
	entry_ref addref;
	entry.GetRef(&addref);
	BMessage addmsg(M_ADD_FILES);
	addmsg.AddRef("refs",&addref);
	
	PostToProjectWindow(&addmsg,NULL);
	return true;
}


void
App::BuildProject(const entry_ref &ref)
{
	BPath path(&ref);
	Project *proj = new Project;
	
	if (proj->Load(path.Path()) != B_OK)
	{
		BMessage msg(M_BUILD_FAILURE);
		PostMessage(&msg);
		
		delete proj;
		return;
	}
	
	if (proj->IsReadOnly())
	{
		BString err = B_TRANSLATE(
			"%path% is on a read-only disk. Please copy the project to another disk "
			"or remount the disk with write support to be able to build it.\n");
		err.ReplaceFirst("%path", path.Path());
		BMessage msg(M_BUILD_FAILURE);
		msg.AddString("errstr",err);
		PostMessage(&msg);
		
		delete proj;
		return;
	}
	
	gProjectList->Lock();
	gProjectList->AddItem(proj);
	gProjectList->Unlock();
	
	gCurrentProject = proj;
	if (!fBuilder)
		fBuilder = new ProjectBuilder(BMessenger(this));
	
	if (fBuildCleanMode)
		proj->ForceRebuild();
	
	fBuilder->BuildProject(proj, POSTBUILD_NOTHING);
}

void
App::GenerateMakefile(const entry_ref &ref)
{
	BPath path(&ref);
	Project *proj = new Project;
	
	if (proj->Load(path.Path()) != B_OK)
	{
		BMessage msg(M_BUILD_FAILURE);
		PostMessage(&msg);
		
		delete proj;
		return;
	}
	
	if (proj->IsReadOnly())
	{
		BString err = B_TRANSLATE(
			"%path% is on a read-only disk. Please copy the project to another disk "
			"or remount the disk with write support to be able to build it.\n");
		err.ReplaceFirst("%path%", path.Path());
		BMessage msg(M_BUILD_FAILURE);
		msg.AddString("errstr",err);
		PostMessage(&msg);
		
		delete proj;
		return;
	}
	
	gProjectList->Lock();
	gProjectList->AddItem(proj);
	gProjectList->Unlock();
	
	gCurrentProject = proj;
	DPath out(proj->GetPath().GetFolder());
	out.Append("Makefile");
	if (MakeMake(proj, out) == B_OK) {
		BEntry entry(out.GetFullPath());
		entry_ref new_ref;
		if (entry.InitCheck() == B_OK) {
			entry.GetRef(&new_ref);
			BMessage refMessage(B_REFS_RECEIVED);
			refMessage.AddRef("refs",&new_ref);
			be_app->PostMessage(&refMessage);
		}
	}
}

void
App::LoadProject(const entry_ref &givenRef)
{
	if (!givenRef.name)
		return;
	
	entry_ref ref(givenRef);
	
	bool isBeIDE = IsBeIDEProject(ref);
	if (isBeIDE)
	{
		// We were given a BeIDE project, so we will see if it has already
		// been converted to a Paladin project. If it has, open the Paladin
		// project. If not, convert it and open it, if successful.
		DPath beidePath(ref);
		BString palPath = beidePath.GetFolder();
		palPath << "/" << beidePath.GetBaseName() << ".pld";
		BEntry entry(palPath.String());
		if (entry.Exists())
			entry.GetRef(&ref);
		else
		{
			if (BeIDE2Paladin(beidePath.GetFullPath(), palPath) == B_OK)
			{
				entry.SetTo(palPath.String());
				entry.GetRef(&ref);
			}
			else
				return;
		}
	}
		
	for (int32 i = 0; i < CountWindows(); i++)
	{
		ProjectWindow *win = dynamic_cast<ProjectWindow*>(WindowAt(i));
		if (!win)
			continue;
		Project *winProject = win->GetProject();
		if (!winProject)
			continue;
		BEntry entry(winProject->GetPath().GetFullPath());
		if (entry.InitCheck() != B_OK)
			continue;
		entry_ref projRef;
		entry.GetRef(&projRef);
		if (ref == projRef)
			return;
	}
	
	BPath path(&ref);
	Project *proj = new Project;
	
	if (proj->Load(path.Path()) != B_OK)
	{
		delete proj;
		return;
	}
	
	// This will be true only if the project file lacks an entry.
	if (proj->SourceControl() == SCM_INIT && !gBuildMode &&
		!proj->IsReadOnly())
	{
		// No given in the project. Attempt to detect one and if there isn't
		// any, see if the user would like to use the default SCM. At the same
		// time, if the user doesn't *want* to use source control, we won't
		// bother him.
		scm_t scm = DetectSCM(proj->GetPath().GetFolder());
		if (scm == SCM_NONE && gDefaultSCM != SCM_NONE)
		{
			BString scmMsg = B_TRANSLATE(
				"This project is not under source control. Would you "
				"like to use %sourcecontrol% for this project?\n"
				"You will only be asked this one time.");
			scmMsg.ReplaceFirst("%sourcecontrol%", SCM2LongName(gDefaultSCM));
			int32 result = ShowAlert(scmMsg.String(), B_TRANSLATE("No"), B_TRANSLATE("Yes"));
			if (result == 1)
				proj->SetSourceControl(gDefaultSCM);
		}
	}
	
	UpdateRecentItems(ref);
	
	gCurrentProject = proj;
	gProjectList->Lock();
	gProjectList->AddItem(proj);
	gProjectList->Unlock();
	
	BRect r(0,0,200,300);
	/*
	r.OffsetTo(gProjectWindowPoint);
	gProjectWindowPoint.x += 25;
	gProjectWindowPoint.y += 25;
	*/
	ProjectWindow *projwin = new ProjectWindow(r,gCurrentProject);
	projwin->Show();
	
	if (proj->IsReadOnly())
	{
		BString errmsg = B_TRANSLATE(
			"This project is on a read-only disk. You will not be able "
			"to build it, but you can still view its files and do anything "
			"else that does not require saving to the disk.");
		ShowAlert(errmsg.String());
	}
}


void
App::UpdateRecentItems(const entry_ref &newref)
{
	gSettings.Lock();
	
	entry_ref ref;
	int32 index = 0;
	while (gSettings.FindRef("recentitems",index++,&ref) == B_OK)
	{
		
		if (ref == newref)
		{
			gSettings.RemoveData("recentitems",index - 1);
			break;
		}
	}
	gSettings.AddRef("recentitems",&newref);
	
	gSettings.Unlock();
}


void
App::PostToProjectWindow(BMessage *msg, entry_ref *file)
{
	if (!msg)
		return;
	
	BPath path(file);
	
	for (int32 i = 0; i < CountWindows(); i++)
	{
		ProjectWindow *win = dynamic_cast<ProjectWindow*>(WindowAt(i));
		if (!win)
			continue;
		if ( (file && win->GetProject()->HasFile(path.Path())) ||
				win->GetProject() == gCurrentProject)
		{
			if (win->AreMenusLocked() && win->RequiresMenuLock(msg->what))
				break;
			win->Activate(true);
			win->PostMessage(msg);
			break;
		}
	}
}


BWindow *
WindowForProject(Project *proj)
{
	if (!proj)
		return NULL;
	
	
	for (int32 i = 0; i < be_app->CountWindows(); i++)
	{
		ProjectWindow *win = dynamic_cast<ProjectWindow*>(be_app->WindowAt(i));
		if (!win)
			continue;
		
		if (win->GetProject() == proj)
			return win;
	}
	return NULL;
}


int
main(int argc, char **argv)
{
	if (argc > 1 && (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0) )
	{
		PrintUsage();
		return 0;
	}
	else
	{
		// Initialize localization under Haiku
//		#ifdef __HAIKU__
//		BCatalog cat;
//		be_locale->GetAppCatalog(&cat);
//		#endif
		
		App().Run();
	}
	
	return sReturnCode;
}
