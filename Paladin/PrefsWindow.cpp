/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "PrefsWindow.h"

#include <Box.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <Font.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <StringView.h>
#include <TabView.h>
#include <View.h>

#include "DPath.h"
#include "Globals.h"
#include "PathBox.h"
#include "Settings.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PrefsWindow"

enum
{
	M_SET_SCM	= 'sscm',
	M_SET_TAB_0	= 'stb0',
	M_SET_TAB_1	= 'stb1',
	M_SET_PROJECT_FOLDER = 'sprf',
	M_SET_SHOW_PROJECT_FOLDER = 'sspf',
	M_SET_DONT_ADD_HEADERS = 'sdah',
	M_SET_SLOW_BUILDS = 'ssbl',
	M_SET_CCACHE = 'scac',
	M_SET_FASTDEP = 'sfsd',
	M_SET_AUTOSYNC = 'saus',
	M_SET_BACKUP_FOLDER = 'sbuf',
	M_SET_REPO_FOLDER = 'sref'
};


PrefsWindow::PrefsWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE("Paladin settings"), B_TITLED_WINDOW,
		B_NOT_V_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	AddShortcut('1', B_COMMAND_KEY, new BMessage(M_SET_TAB_0));
	AddShortcut('2', B_COMMAND_KEY, new BMessage(M_SET_TAB_1));

	// general

	fProjectFolder = new PathBox("projectfolder", gProjectPath.GetFullPath(), 
		new BMessage(M_SET_PROJECT_FOLDER));
	fProjectFolder->MakeValidating(true);
	SetToolTip(fProjectFolder, B_TRANSLATE("The default path for new projects."));

	fShowProjectFolder = new BCheckBox("showfolder",
		B_TRANSLATE("Show project folder on open"), new BMessage(M_SET_SHOW_PROJECT_FOLDER));
	SetToolTip(fShowProjectFolder, B_TRANSLATE("When checked, a project's folder is "
		"shown in Tracker when it is opened."));
	if (gShowFolderOnOpen)
		fShowProjectFolder->SetValue(B_CONTROL_ON);

	fDontAddHeaders = new BCheckBox("dontaddheaders",
		B_TRANSLATE("Omit header files from projects"), 
		new BMessage(M_SET_DONT_ADD_HEADERS));
	SetToolTip(fDontAddHeaders, B_TRANSLATE("If checked, header files are not automatically "
		"added to projects."));
	if (gDontManageHeaders)
		fDontAddHeaders->SetValue(B_CONTROL_ON);

	fSlowBuilds = new BCheckBox("slowbuilds", B_TRANSLATE("Use single thread"),
		new BMessage(M_SET_SLOW_BUILDS));
	SetToolTip(fSlowBuilds, B_TRANSLATE("Build with just one thread instead of one thread "
		"per processor"));
	if (gSingleThreadedBuild)
		fSlowBuilds->SetValue(B_CONTROL_ON);

	fCCache = new BCheckBox("ccache", B_TRANSLATE("Use ccache to build faster"),
		new BMessage(M_SET_CCACHE));
	SetToolTip(fCCache, B_TRANSLATE("Compiler caching is another way to speed up builds"));
	if (gCCacheAvailable) {
		if (gUseCCache)
			fCCache->SetValue(B_CONTROL_ON);
	} else {
		BString label = fCCache->Label();
		label << " -- " << B_TRANSLATE("unavailable");
		fCCache->SetLabel(label.String());
		fCCache->SetEnabled(false);
	}

	fFastDep = new BCheckBox("fastdep", B_TRANSLATE("Use fastdep dependency checker"),
		new BMessage(M_SET_FASTDEP));
	SetToolTip(fFastDep, B_TRANSLATE("Use the fastdep dependency checker instead of gcc"));
	if (gFastDepAvailable) {
		if (gUseFastDep)
			fFastDep->SetValue(B_CONTROL_ON);
	} else {
		BString label = fFastDep->Label();
		label << " -- " << B_TRANSLATE("unavailable");
		fFastDep->SetLabel(label.String());
		fFastDep->SetEnabled(false);
	}

	BBox* buildBox = new BBox(B_FANCY_BORDER,
		BLayoutBuilder::Group<>(B_VERTICAL, 0)
			.Add(fSlowBuilds)
			.Add(fCCache)
			.Add(fFastDep)
			.SetInsets(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.View());
	buildBox->SetLabel(B_TRANSLATE("Build"));

	fAutoSyncModules = new BCheckBox("autosync",
		B_TRANSLATE("Automatically synchronize modules"),
		new BMessage(M_SET_AUTOSYNC));
	SetToolTip(fAutoSyncModules, B_TRANSLATE("Automatically synchronize modules in your "
		"projects with the those in the code library"));
	if (gAutoSyncModules)
		fAutoSyncModules->SetValue(B_CONTROL_ON);

	fBackupFolder = new PathBox("backupfolder", gBackupPath.GetFullPath(), 
		new BMessage(M_SET_BACKUP_FOLDER));
	fBackupFolder->MakeValidating(true);
	SetToolTip(fBackupFolder, B_TRANSLATE("Sets the location for project backups"));

	fTabs[0] = BLayoutBuilder::Grid<>(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
		.Add(new BStringView("projects folder label", B_TRANSLATE("Projects folder:")), 0, 0)
		.Add(fProjectFolder, 1, 0)

		.AddGroup(B_VERTICAL, 0.0f, 1, 1)
			.Add(fShowProjectFolder)
			.Add(fDontAddHeaders)
			.End()

		.Add(buildBox, 1, 2)

		.Add(fAutoSyncModules, 1, 3)

		.Add(new BStringView("backups folder label", B_TRANSLATE("Backups folder:")), 0, 4)
		.Add(fBackupFolder, 1, 4)

		.SetInsets(B_USE_DEFAULT_SPACING)
		.View();
	fTabs[0]->SetName(B_TRANSLATE("General"));
	fTabs[0]->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

#ifndef BUILD_CODE_LIBRARY
	fAutoSyncModules->Hide();
#endif

	// source control

	BPopUpMenu* scmMenu = new BPopUpMenu("SCM Chooser");
	BMessage* setScmMessage = new BMessage(M_SET_SCM);
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Mercurial"), setScmMessage));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Git"), setScmMessage));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Subversion"), setScmMessage));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("None"), setScmMessage));

	fSCMChooser = new BMenuField("scmchooser", B_TRANSLATE("Preferred source control:"),
		scmMenu);

	BMenuItem* marked = scmMenu->ItemAt(gDefaultSCM);
	if (marked != NULL)
		marked->SetMarked(true);
	else
		scmMenu->ItemAt(0)->SetMarked(true);

	if (!marked->IsEnabled()) {
		// if the default SCM is disabled unmark it and mark the first one that
		// is enabled.
		marked->SetMarked(false);
		for (int32 i = 0; i < scmMenu->CountItems(); i++) {
			if (scmMenu->ItemAt(i)->IsEnabled()) {
				scmMenu->ItemAt(i)->SetMarked(true);
				break;
			}
		}
	}

	fSVNRepoFolder = new PathBox("svnrepofolder", gSVNRepoPath.GetFullPath(), 
		new BMessage(M_SET_REPO_FOLDER));
	fSVNRepoFolder->MakeValidating(true);
	SetToolTip(fSVNRepoFolder, B_TRANSLATE("Sets the location for the 'server' side of "
		"local Subversion repositories."));

	fTabs[1] = BLayoutBuilder::Group<>(B_VERTICAL)
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.Add(fSCMChooser->CreateLabelLayoutItem(), 0, 0)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING, 1, 0)
				.Add(fSCMChooser->CreateMenuBarLayoutItem())
				.AddGlue()
				.End()
	
			.Add(new BStringView("svn repo folder label",
				B_TRANSLATE("Source control repository folder:")), 0, 1)
			.Add(fSVNRepoFolder, 1, 1)
			.End()
		.AddGlue()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.View();
	fTabs[1]->SetName(B_TRANSLATE("Source control"));
	fTabs[1]->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// tab view

	fTabView = new BTabView("tabview", B_WIDTH_FROM_LABEL);
	fTabView->SetBorder(B_NO_BORDER);
	fTabView->AddTab(fTabs[0]);
	fTabView->AddTab(fTabs[1]);
	fTabView->Select(0L);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.AddStrut(B_USE_SMALL_SPACING)
		.Add(fTabView)
		.End();

	fProjectFolder->MakeFocus(true);

	CenterOnScreen();
}


bool
PrefsWindow::QuitRequested(void)
{
	return true;
}


void
PrefsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_SET_TAB_0:
		{
			fTabView->Select(0L);
			break;
		}

		case M_SET_TAB_1:
		{
			fTabView->Select(1L);
			break;
		}
		case M_SET_PROJECT_FOLDER:
		{
			gProjectPath = fProjectFolder->Path();
			gSettings.SetString("projectpath", fProjectFolder->Path());
			gSettings.Save();
			break;
		}
		case M_SET_SHOW_PROJECT_FOLDER:
		{
			gShowFolderOnOpen = (fShowProjectFolder->Value() == B_CONTROL_ON);
			gSettings.SetBool("showfolderonopen", gShowFolderOnOpen);
			gSettings.Save();
			break;
		}
		case M_SET_DONT_ADD_HEADERS:
		{
			gDontManageHeaders = (fDontAddHeaders->Value() == B_CONTROL_ON);
			gSettings.SetBool("dontmanageheaders", gDontManageHeaders);
			gSettings.Save();
			break;
		}
		case M_SET_SLOW_BUILDS:
		{
			gSingleThreadedBuild = (fSlowBuilds->Value() == B_CONTROL_ON);
			gSettings.SetBool("singlethreaded", gSingleThreadedBuild);
			gSettings.Save();
			break;
		}
		case M_SET_CCACHE:
		{
			gUseCCache = (fCCache->Value() == B_CONTROL_ON);
			gSettings.SetBool("ccache", gUseCCache);
			gSettings.Save();
			break;
		}
		case M_SET_FASTDEP:
		{
			gUseFastDep = (fFastDep->Value() == B_CONTROL_ON);
			gSettings.SetBool("fastdep", gUseFastDep);
			gSettings.Save();
			break;
		}
		case M_SET_AUTOSYNC:
		{
#ifdef BUILD_CODE_LIBRARY
			gAutoSyncModules = (fAutoSyncModules->Value() == B_CONTROL_ON);
			gSettings.SetBool("autosyncmodules", gAutoSyncModules);
			gSettings.Save();
#endif
			break;
		}
		case M_SET_BACKUP_FOLDER:
		{
			gBackupPath = fBackupFolder->Path();
			gSettings.SetString("backuppath", fBackupFolder->Path());
			gSettings.Save();
			break;
		}
		case M_SET_SCM:
		{
			gDefaultSCM = (scm_t)fSCMChooser->Menu()->IndexOf(fSCMChooser->Menu()->FindMarked());
			gSettings.SetInt32("defaultSCM", gDefaultSCM);
			gSettings.Save();
			break;
		}
		case M_SET_REPO_FOLDER:
		{
			gSVNRepoPath = fSVNRepoFolder->Path();
			gSettings.SetString("svnrepopath", fSVNRepoFolder->Path());
			gSettings.Save();
			break;
		}

		default:
			BWindow::MessageReceived(message);
	}
}
