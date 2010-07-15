#include "PrefsWindow.h"

#include "DPath.h"
#include "Globals.h"
#include "PathBox.h"
#include "PLocale.h"
#include "Settings.h"


PrefsWindow::PrefsWindow(BRect frame)
	:	DWindow(frame,TR("Program Settings"))
{
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	float pw,ph;
	BString label;
	
	fProjectFolder = new PathBox(BRect(10,10,11,11),"projfolder",
								gProjectPath.GetFullPath(),
								TR("Default Project Folder:"),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fProjectFolder);
	fProjectFolder->GetPreferredSize(&pw,&ph);
	fProjectFolder->SetDivider(fProjectFolder->StringWidth(
								TR("Default Project Folder:")) + 5.0);
	fProjectFolder->ResizeTo(Bounds().Width() - 20.0, ph);
	fProjectFolder->MakeValidating(true);
	SetToolTip(fProjectFolder,TR("The default path for new projects."));
	
	BRect r(fProjectFolder->Frame());
	r.OffsetBy(0,r.Height() + 5);
	fShowProjFolder = new BCheckBox(r,"showfolder",
									TR("Show Project Folder on Open"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fShowProjFolder);
	
	fShowProjFolder->GetPreferredSize(&pw,&ph);
	fShowProjFolder->ResizeTo(r.Width(), ph);
	if (gShowFolderOnOpen)
		fShowProjFolder->SetValue(B_CONTROL_ON);
	SetToolTip(fShowProjFolder,TR("If checked, when a project is opened, "
								"its parent folder is opened in Tracker"));
	
	r = fShowProjFolder->Frame();
	r.OffsetBy(0,r.Height());
	fSlowBuilds = new BCheckBox(r,"slowbuilds",	TR("Limit Build Threads to 1"),
								new BMessage,
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fSlowBuilds);
	if (gSingleThreadedBuild)
		fSlowBuilds->SetValue(B_CONTROL_ON);
	SetToolTip(fSlowBuilds,TR("Paladin will build a project with just one "
							"thread instead of one per processor"));
	
	r = fSlowBuilds->Frame();
	r.OffsetBy(0,r.Height());
	fCCache = new BCheckBox(r,"ccache",TR("Use ccache to build faster"),
							new BMessage, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fCCache);
	if (gCCacheAvailable)
	{
		if (gUseCCache)
			fCCache->SetValue(B_CONTROL_ON);
	}
	else
	{
		label = fCCache->Label();
		label << " -- " << TR("unavailable");
		fCCache->SetLabel(label.String());
		fCCache->SetEnabled(false);
	}
	SetToolTip(fCCache,TR("Compiler caching is another way to speed up builds."));
	
	r = fCCache->Frame();
	r.OffsetBy(0,r.Height());
	fFastDep = new BCheckBox(r,"fastdep",
									TR("Use fast dependency updates"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fFastDep);
	if (gFastDepAvailable)
	{
		if (gUseFastDep)
			fFastDep->SetValue(B_CONTROL_ON);
	}
	else
	{
		label = fFastDep->Label();
		label << " -- " << TR("unavailable");
		fFastDep->SetLabel(label.String());
		fFastDep->SetEnabled(false);
	}
	SetToolTip(fFastDep,TR("Use the fastdep dependency checker instead of gcc."));
	
	r.OffsetBy(0,r.Height());
	fAutoSyncModules = new BCheckBox(r,"autosync",
									TR("Automatically Synchronize Modules"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fAutoSyncModules);
	if (gAutoSyncModules)
		fAutoSyncModules->SetValue(B_CONTROL_ON);
	SetToolTip(fAutoSyncModules,TR("Automatically synchronize modules in your "
								"projects with the those in the code library"));
	
	r.OffsetBy(0,r.Height() + 5.0);
	r.right = r.left + 1.0;
	r.bottom = r.top + 1.0;
	fBackupFolder = new PathBox(r,"backupfolder", gBackupPath.GetFullPath(),
								TR("Backup Folder:"),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fBackupFolder);
	fBackupFolder->MoveTo(r.left,r.top);
	fBackupFolder->GetPreferredSize(&pw,&ph);
	fBackupFolder->ResizeTo(Bounds().Width() - 20.0, ph);
	fBackupFolder->SetDivider(fBackupFolder->StringWidth(
								"Backup Folder:") + 5.0);
	fBackupFolder->MakeValidating(true);
	SetToolTip(fBackupFolder,TR("Sets the location for project backups"));
	
	ResizeTo(Bounds().Width(), fBackupFolder->Frame().bottom + 10.0);
}


bool
PrefsWindow::QuitRequested(void)
{
	gProjectPath = fProjectFolder->Path();
	gSettings.SetString("projectpath",fProjectFolder->Path());
	
	gBackupPath = fBackupFolder->Path();
	gSettings.SetString("backuppath",fBackupFolder->Path());
	
	gShowFolderOnOpen = (fShowProjFolder->Value() == B_CONTROL_ON);
	gSettings.SetBool("showfolderonopen",gShowFolderOnOpen);
	
	gSingleThreadedBuild = (fSlowBuilds->Value() == B_CONTROL_ON);
	gSettings.SetBool("singlethreaded",gSingleThreadedBuild);
	
	gUseCCache = (fCCache->Value() == B_CONTROL_ON);
	gSettings.SetBool("ccache",gUseCCache);
	
	gUseFastDep = (fFastDep->Value() == B_CONTROL_ON);
	gSettings.SetBool("fastdep",gUseFastDep);
	
	gAutoSyncModules = (fAutoSyncModules->Value() == B_CONTROL_ON);
	gSettings.SetBool("autosyncmodules",gAutoSyncModules);
	
	return true;
}

