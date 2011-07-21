#include "PrefsWindow.h"

#include <Box.h>
#include <Font.h>
#include <Menu.h>
#include <MenuItem.h>

#include "DPath.h"
#include "Globals.h"
#include "PathBox.h"
#include "PLocale.h"
#include "Settings.h"

enum
{
	M_SET_SCM = 'sscm',
	M_SET_TAB_1 = 'stb1',
	M_SET_TAB_2 = 'stb2'
};

PrefsWindow::PrefsWindow(BRect frame)
	:	DWindow(frame,TR("Program Settings"), B_TITLED_WINDOW, B_NOT_V_RESIZABLE |
																B_NOT_ZOOMABLE)
{
	AddShortcut('1', B_COMMAND_KEY, new BMessage(M_SET_TAB_1));
	AddShortcut('2', B_COMMAND_KEY, new BMessage(M_SET_TAB_2));
	
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	float pw,ph;
	BString label;
	
	BMenu *menu = new BMenu("Settings");
	menu->AddItem(new BMenuItem("General", new BMessage(M_SET_TAB_1)));
	menu->AddItem(new BMenuItem("Source Control", new BMessage(M_SET_TAB_2)));
	
	BRect r(10,10,11,11);
	fViewChooser = new BMenuField(r, "viewchooser", NULL, menu);
	top->AddChild(fViewChooser);
	fViewChooser->ResizeToPreferred();
	r = fViewChooser->Frame();
	fViewChooser->Menu()->SetLabelFromMarked(true);
	fViewChooser->Menu()->SetRadioMode(true);
	fViewChooser->Menu()->ItemAt(0L)->SetMarked(true);
	
	BRect tabBounds(Bounds());
	tabBounds.top = r.bottom + 1.0;
	
	BView *view = new BView(tabBounds, "tab1", B_FOLLOW_ALL, B_WILL_DRAW);
	view->SetViewColor(top->ViewColor());
	top->AddChild(view);
	fViews[0] = view;
	
	fProjectFolder = new PathBox(r,"projfolder",
								gProjectPath.GetFullPath(),
								TR("Default Project Folder:"),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	view->AddChild(fProjectFolder);
	fProjectFolder->GetPreferredSize(&pw,&ph);
	fProjectFolder->SetDivider(fProjectFolder->StringWidth(
								TR("Default Project Folder:")) + 5.0);
	fProjectFolder->ResizeTo(Bounds().Width() - 20.0, ph);
	fProjectFolder->MakeValidating(true);
	SetToolTip(fProjectFolder,TR("The default path for new projects."));
	
	r = fProjectFolder->Frame();
	r.OffsetBy(0,r.Height() + 5);
	fShowProjFolder = new BCheckBox(r,"showfolder",
									TR("Show Project Folder on Open"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	view->AddChild(fShowProjFolder);
	
	if (gShowFolderOnOpen)
		fShowProjFolder->SetValue(B_CONTROL_ON);
	SetToolTip(fShowProjFolder,TR("When checked, a project's folder is shown in "
									"Tracker when it is opened."));
	r.OffsetBy(0, r.Height() + 5);
	fDontAddHeaders = new BCheckBox(r,"dontaddheaders",
									TR("Don't Add Headers to Projects"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	view->AddChild(fDontAddHeaders);
	
	if (gDontManageHeaders)
		fDontAddHeaders->SetValue(B_CONTROL_ON);
	SetToolTip(fDontAddHeaders,TR("If checked, header files are not automatically "
								"added to projects."));
	
	r.OffsetBy(0,r.Height() + 5.0);
	
	BBox *buildBox = new BBox(r, NULL, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	buildBox->SetLabel(TR("Build"));
	view->AddChild(buildBox);
	
	BRect boxr(buildBox->Bounds().InsetByCopy(10.0, 10.0));
	boxr.bottom = boxr.top + fShowProjFolder->Frame().Height();
	
	font_height fheight;
	be_bold_font->GetHeight(&fheight);
	boxr.top = fheight.ascent + fheight.descent + fheight.leading;
	
	fSlowBuilds = new BCheckBox(boxr,"slowbuilds",	TR("Limit Build Threads to 1"),
								new BMessage,
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	buildBox->AddChild(fSlowBuilds);
	if (gSingleThreadedBuild)
		fSlowBuilds->SetValue(B_CONTROL_ON);
	SetToolTip(fSlowBuilds,TR("Paladin will build a project with just one "
							"thread instead of one per processor"));
	
	boxr = fSlowBuilds->Frame();
	boxr.OffsetBy(0,r.Height());
	fCCache = new BCheckBox(boxr,"ccache",TR("Use ccache to build faster"),
							new BMessage, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	buildBox->AddChild(fCCache);
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
	
	boxr = fCCache->Frame();
	boxr.OffsetBy(0,r.Height());
	fFastDep = new BCheckBox(boxr,"fastdep",
									TR("Use fast dependency updates"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	buildBox->AddChild(fFastDep);
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
	
	buildBox->ResizeTo(buildBox->Bounds().Width(), fFastDep->Frame().bottom + 5.0);
	
	r.OffsetTo(10, buildBox->Frame().bottom + 10.0);
	#ifdef BUILD_CODE_LIBRARY
	fAutoSyncModules = new BCheckBox(r,"autosync",
									TR("Automatically Synchronize Modules"),
									new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	view->AddChild(fAutoSyncModules);
	if (gAutoSyncModules)
		fAutoSyncModules->SetValue(B_CONTROL_ON);
	SetToolTip(fAutoSyncModules,TR("Automatically synchronize modules in your "
								"projects with the those in the code library"));
	r.OffsetBy(0,r.Height() + 5.0);
	#endif
	
	r.right = r.left + 1.0;
	r.bottom = r.top + 1.0;
	fBackupFolder = new PathBox(r,"backupfolder", gBackupPath.GetFullPath(),
								TR("Backup Folder:"),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	view->AddChild(fBackupFolder);
	fBackupFolder->MoveTo(r.left,r.top);
	fBackupFolder->GetPreferredSize(&pw,&ph);
	fBackupFolder->ResizeTo(Bounds().Width() - 20.0, ph);
	fBackupFolder->SetDivider(fBackupFolder->StringWidth(
								"Backup Folder:") + 5.0);
	fBackupFolder->MakeValidating(true);
	SetToolTip(fBackupFolder,TR("Sets the location for project backups"));
	
	view = new BView(tabBounds, "tab2", B_FOLLOW_ALL, B_WILL_DRAW);
	view->SetViewColor(top->ViewColor());
	top->AddChild(view);
	fViews[1] = view;
	
	r.OffsetTo(10.0, 10.0);
	menu = new BMenu("SCM Chooser");
	menu->AddItem(new BMenuItem("Mercurial", new BMessage()));
	menu->AddItem(new BMenuItem("Git", new BMessage()));
	menu->AddItem(new BMenuItem("Subversion", new BMessage()));
	menu->AddItem(new BMenuItem("None", new BMessage()));
	
	fSCMChooser = new BMenuField(r, "scmchooser", "Preferred Source Control: ",
								menu);
	fSCMChooser->SetDivider(fSCMChooser->StringWidth("Preferred Source Control: ") + 5.0);
	view->AddChild(fSCMChooser);
	
	#ifdef DISABLE_GIT_SUPPORT
	menu->ItemAt(1)->SetEnabled(false);
	#endif
	
	if (gPlatform == PLATFORM_R5 || gPlatform == PLATFORM_ZETA)
	{
		menu->ItemAt(0)->SetEnabled(false);
		menu->ItemAt(0)->SetLabel("Mercurial Unavailable");
		menu->ItemAt(1)->SetEnabled(false);
		menu->ItemAt(1)->SetLabel("Git Unavailable");
	}
	
	menu->SetLabelFromMarked(true);
	BMenuItem *marked = menu->ItemAt(gDefaultSCM);
	if (marked)
		marked->SetMarked(true);
	else
		menu->ItemAt(menu->CountItems() - 1)->SetMarked(true);

	if (!marked->IsEnabled())
	{
		marked->SetMarked(false);
		for (int32 i = 0; i < menu->CountItems(); i++)
		{
			if (menu->ItemAt(i)->IsEnabled())
			{
				menu->ItemAt(i)->SetMarked(true);
				break;
			}
		}
	}
	
	r = fBackupFolder->Frame();
	r.OffsetTo(10.0, fSCMChooser->Frame().bottom + 10.0);
	fSVNRepoFolder = new PathBox(r,"svnrepofolder", gSVNRepoPath.GetFullPath(),
								TR("Subversion Repository Folder:"),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	view->AddChild(fSVNRepoFolder);
	fSVNRepoFolder->SetDivider(fSVNRepoFolder->StringWidth(
								"Subversion Repository Folder:") + 5.0);
	fSVNRepoFolder->MakeValidating(true);
	SetToolTip(fSVNRepoFolder,TR("Sets the location for the 'server' side of local Subversion repositories."));
	
	fActiveView = fViews[0];
	fViews[1]->Hide();
	
	ResizeTo(Bounds().Width(), fViewChooser->Frame().bottom +
								fBackupFolder->Frame().bottom + 10.0);
	SetSizeLimits(Bounds().Width(), 30000, Bounds().Height(), 30000);
}


bool
PrefsWindow::QuitRequested(void)
{
	gProjectPath = fProjectFolder->Path();
	gSettings.SetString("projectpath",fProjectFolder->Path());
	
	gBackupPath = fBackupFolder->Path();
	gSettings.SetString("backuppath",fBackupFolder->Path());
	
	gSVNRepoPath = fSVNRepoFolder->Path();
	gSettings.SetString("svnrepopath",fSVNRepoFolder->Path());
	
	gShowFolderOnOpen = (fShowProjFolder->Value() == B_CONTROL_ON);
	gSettings.SetBool("showfolderonopen",gShowFolderOnOpen);
	
	gDontManageHeaders = (fDontAddHeaders->Value() == B_CONTROL_ON);
	gSettings.SetBool("dontmanageheaders", gDontManageHeaders);
	
	gSingleThreadedBuild = (fSlowBuilds->Value() == B_CONTROL_ON);
	gSettings.SetBool("singlethreaded",gSingleThreadedBuild);
	
	gUseCCache = (fCCache->Value() == B_CONTROL_ON);
	gSettings.SetBool("ccache",gUseCCache);
	
	gUseFastDep = (fFastDep->Value() == B_CONTROL_ON);
	gSettings.SetBool("fastdep",gUseFastDep);
	
	#ifdef BUILD_CODE_LIBRARY
	gAutoSyncModules = (fAutoSyncModules->Value() == B_CONTROL_ON);
	gSettings.SetBool("autosyncmodules",gAutoSyncModules);
	#endif
	
	gDefaultSCM = (scm_t)fSCMChooser->Menu()->IndexOf(fSCMChooser->Menu()->FindMarked());
	gSettings.SetInt32("defaultSCM", gDefaultSCM);
	
	return true;
}


void
PrefsWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_SET_TAB_1:
		{
			if (!fActiveView->IsHidden())
				fActiveView->Hide();
			fActiveView = fViews[0];
			fActiveView->Show();
			fViewChooser->Menu()->FindMarked()->SetMarked(false);
			fViewChooser->Menu()->ItemAt(0L)->SetMarked(true);
			break;
		}
		case M_SET_TAB_2:
		{
			if (!fActiveView->IsHidden())
				fActiveView->Hide();
			fActiveView = fViews[1];
			fActiveView->Show();
			fViewChooser->Menu()->FindMarked()->SetMarked(false);
			fViewChooser->Menu()->ItemAt(1L)->SetMarked(true);
			break;
		}
		default:
			DWindow::MessageReceived(msg);
	}
}
