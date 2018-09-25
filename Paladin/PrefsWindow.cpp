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
	M_SET_TAB_1	= 'stb1'
};


PrefsWindow::PrefsWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE("Program settings"), B_TITLED_WINDOW,
		B_NOT_V_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	AddShortcut('1', B_COMMAND_KEY, new BMessage(M_SET_TAB_0));
	AddShortcut('2', B_COMMAND_KEY, new BMessage(M_SET_TAB_1));

	// general

	fProjectFolder = new PathBox("projectfolder", gProjectPath.GetFullPath(), "");
	fProjectFolder->MakeValidating(true);
	SetToolTip(fProjectFolder, B_TRANSLATE("The default path for new projects."));

	fShowProjectFolder = new BCheckBox("showfolder",
		B_TRANSLATE("Show project folder on open"), new BMessage);
	SetToolTip(fShowProjectFolder, B_TRANSLATE("When checked, a project's folder is "
		"shown in Tracker when it is opened."));
	if (gShowFolderOnOpen)
		fShowProjectFolder->SetValue(B_CONTROL_ON);

	fDontAddHeaders = new BCheckBox("dontaddheaders",
		B_TRANSLATE("Omit header files from projects"), NULL);
	SetToolTip(fDontAddHeaders, B_TRANSLATE("If checked, header files are not automatically "
		"added to projects."));
	if (gDontManageHeaders)
		fDontAddHeaders->SetValue(B_CONTROL_ON);

	fSlowBuilds = new BCheckBox("slowbuilds", B_TRANSLATE("Use single thread"), NULL);
	SetToolTip(fSlowBuilds, B_TRANSLATE("Build with just one thread instead of one thread "
		"per processor"));
	if (gSingleThreadedBuild)
		fSlowBuilds->SetValue(B_CONTROL_ON);

	fCCache = new BCheckBox("ccache", B_TRANSLATE("Use ccache to build faster"), NULL);
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

	fFastDep = new BCheckBox("fastdep", B_TRANSLATE("Use fastdep dependency checker"), NULL);
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
		B_TRANSLATE("Automatically synchronize modules"), NULL);
	SetToolTip(fAutoSyncModules, B_TRANSLATE("Automatically synchronize modules in your "
		"projects with the those in the code library"));
	if (gAutoSyncModules)
		fAutoSyncModules->SetValue(B_CONTROL_ON);

	fBackupFolder = new PathBox("backupfolder", gBackupPath.GetFullPath(), "");
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
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Mercurial"), NULL));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Git"), NULL));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Subversion"), NULL));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("None"), NULL));

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

	fSVNRepoFolder = new PathBox("svnrepofolder", gSVNRepoPath.GetFullPath(), "");
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
				B_TRANSLATE("Subversion repository folder:")), 0, 1)
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
	gProjectPath = fProjectFolder->Path();
	gSettings.SetString("projectpath", fProjectFolder->Path());

	gBackupPath = fBackupFolder->Path();
	gSettings.SetString("backuppath", fBackupFolder->Path());

	gSVNRepoPath = fSVNRepoFolder->Path();
	gSettings.SetString("svnrepopath", fSVNRepoFolder->Path());

	gShowFolderOnOpen = (fShowProjectFolder->Value() == B_CONTROL_ON);
	gSettings.SetBool("showfolderonopen", gShowFolderOnOpen);

	gDontManageHeaders = (fDontAddHeaders->Value() == B_CONTROL_ON);
	gSettings.SetBool("dontmanageheaders", gDontManageHeaders);

	gSingleThreadedBuild = (fSlowBuilds->Value() == B_CONTROL_ON);
	gSettings.SetBool("singlethreaded", gSingleThreadedBuild);

	gUseCCache = (fCCache->Value() == B_CONTROL_ON);
	gSettings.SetBool("ccache", gUseCCache);

	gUseFastDep = (fFastDep->Value() == B_CONTROL_ON);
	gSettings.SetBool("fastdep", gUseFastDep);

#ifdef BUILD_CODE_LIBRARY
	gAutoSyncModules = (fAutoSyncModules->Value() == B_CONTROL_ON);
	gSettings.SetBool("autosyncmodules", gAutoSyncModules);
#endif

	gDefaultSCM = (scm_t)fSCMChooser->Menu()->IndexOf(fSCMChooser->Menu()->FindMarked());
	gSettings.SetInt32("defaultSCM", gDefaultSCM);

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

		default:
			BWindow::MessageReceived(message);
	}
}
