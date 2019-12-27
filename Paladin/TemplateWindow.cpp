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
#include "TemplateWindow.h"

#include <algorithm>

#include <Button.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Font.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <Size.h>
#include <StringView.h>

#include "AutoTextControl.h"
#include "Globals.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PathBox.h"
#include "Project.h"
#include "Settings.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TemplateWindow"

enum {
	M_NAME_CHANGED		= 'nmch',
	M_TARGET_CHANGED	= 'tgch',
	M_TEMPLATE_SELECTED	= 'tmsl',
	M_CHOOSE_LIBS		= 'chlb',
	M_CUSTOM_APP		= 'csap',
	M_CUSTOM_WIN		= 'cswn'
};


TemplateWindow::TemplateWindow(const BRect& frame)
	:
	BWindow(frame, B_TRANSLATE("Choose a project type"), B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	RegisterWindow();
	
	CheckTemplates();
	
	DPath templatePath(gAppPath.GetFolder());
	templatePath << B_TRANSLATE("Templates");
	fTempList.ScanFolder(templatePath.GetFullPath());
	
	// Reuse default english folder if no translated folder is found
	if(fTempList.CountTemplates()==0){
		templatePath = gAppPath.GetFolder();
		templatePath<<"Templates";
		fTempList.ScanFolder(templatePath.GetFullPath());
	}
	
	// #312 allow user specified templates
	fTempList.ScanFolder("/boot/home/config/settings/Paladin/Templates");
	// And allow other packages to install Paladin templates too
	fTempList.ScanFolder("/system/data/Paladin/Templates");
	fTempList.ScanFolder("/system/develop/Paladin/Templates");

	// project type
	BPopUpMenu* projectTypeMenu = new BPopUpMenu(B_TRANSLATE("Project type"));
	for (int32 i = 0; i < fTempList.CountTemplates(); i++) {
		ProjectTemplate* ptemp = fTempList.TemplateAt(i);
		entry_ref ref = ptemp->GetRef();
		projectTypeMenu->AddItem(new BMenuItem(ref.name,
			new BMessage(M_TEMPLATE_SELECTED)));
	}
	projectTypeMenu->ItemAt(0L)->SetMarked(true);

	fTemplateField = new BMenuField("templatefield", B_TRANSLATE("Project type:"),
		projectTypeMenu);
	fTemplateField->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// project name

	fNameBox = new AutoTextControl("namebox", B_TRANSLATE("Project name:"), "",
		new BMessage(M_NAME_CHANGED));
	SetToolTip(fNameBox, B_TRANSLATE("The name of your project. "
		"It can be the same as the Target name, but it does not have to be."));

	// target name

	fTargetBox = new AutoTextControl("targetbox", B_TRANSLATE("Target name:"), "BeApp",
		new BMessage(M_TARGET_CHANGED));
	SetToolTip(fTargetBox, B_TRANSLATE("The name of the compiled application or library"));

	// project path

	fPathBox = new PathBox("pathbox", gProjectPath.GetFullPath(), "");
	fPathBox->SetExplicitMinSize(BSize(be_plain_font->StringWidth("M") * 36,
		B_SIZE_UNSET)),
	SetToolTip(fPathBox, B_TRANSLATE("Set the location for your project."));

	// source control

	BPopUpMenu* scmMenu = new BPopUpMenu("SCM Chooser");
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Mercurial"), new BMessage()));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Git"), new BMessage()));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("Subversion"), new BMessage()));
	scmMenu->AddItem(new BMenuItem(B_TRANSLATE("None"), new BMessage()));

	if (!gHgAvailable) {
		scmMenu->ItemAt(0)->SetEnabled(false);
		scmMenu->ItemAt(0)->SetLabel(B_TRANSLATE("Mercurial unavailable"));
	}
	if (!gGitAvailable) {
		scmMenu->ItemAt(1)->SetEnabled(false);
		scmMenu->ItemAt(1)->SetLabel(B_TRANSLATE("Git unavailable"));
	}
	if (!gSvnAvailable) {
		scmMenu->ItemAt(2)->SetEnabled(false);
		scmMenu->ItemAt(2)->SetLabel(B_TRANSLATE("Subversion unavailable"));
	}

	fSCMChooser = new BMenuField("scmchooser", B_TRANSLATE("Source control:"), scmMenu);
	fSCMChooser->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetToolTip(fSCMChooser,
		B_TRANSLATE("Choose the source control manager for your project, if any."));

	scmMenu->ItemAt(gDefaultSCM)->SetMarked(true);

	BMenuItem* item = scmMenu->FindMarked();
	if (!item->IsEnabled()) {
		item->SetMarked(false);
		for (int32 i = 0; i < scmMenu->CountItems(); i++) {
			if (scmMenu->ItemAt(i)->IsEnabled()) {
				scmMenu->ItemAt(i)->SetMarked(true);
				break;
			}
		}
	}

	// create folder check box

	fCreateFolder = new BCheckBox(B_TRANSLATE("Create project folder"));
	fCreateFolder->SetValue(B_CONTROL_ON);
	SetToolTip(fCreateFolder, B_TRANSLATE("If checked, a folder for your project will be created "
		"in the location entered above."));

	// create project button

	BString createStr(B_TRANSLATE("Create project" B_UTF8_ELLIPSIS));
	fCreateProjectButton = new BButton("ok", createStr,
		new BMessage(M_CREATE_PROJECT));
	fCreateProjectButton->SetEnabled(false);
	fCreateProjectButton->MakeDefault(true);

	// layout

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.Add(fTemplateField->CreateLabelLayoutItem(), 0, 0)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING, 1, 0)
				.Add(fTemplateField->CreateMenuBarLayoutItem())
				.AddGlue()
				.End()

			.Add(fNameBox->CreateLabelLayoutItem(), 0, 1)
			.Add(fNameBox->CreateTextViewLayoutItem(), 1, 1)

			.Add(fTargetBox->CreateLabelLayoutItem(), 0, 2)
			.Add(fTargetBox->CreateTextViewLayoutItem(), 1, 2)

			.Add(new BStringView("location", B_TRANSLATE("Location:")), 0, 3)
			.Add(fPathBox, 1, 3)

			.Add(fSCMChooser->CreateLabelLayoutItem(), 0, 4)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING, 1, 4)
				.Add(fSCMChooser->CreateMenuBarLayoutItem())
				.AddGlue()
				.End()

			.Add(fCreateFolder, 1, 5)
			.End()
		.AddGlue()
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(fCreateProjectButton)
			.End()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.End();

	fNameBox->MakeFocus(true);

	CenterOnScreen();
}


TemplateWindow::~TemplateWindow(void)
{
}


bool
TemplateWindow::QuitRequested(void)
{
	DeregisterWindow();
	return true;
}


void
TemplateWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_NAME_CHANGED:
		case M_TARGET_CHANGED:
		{
			if (fNameBox->Text() && strlen(fNameBox->Text()) > 0 &&
				fTargetBox->Text() && strlen(fTargetBox->Text()) > 0)
				fCreateProjectButton->SetEnabled(true);
			else
				fCreateProjectButton->SetEnabled(false);
			break;
		}

		case M_CREATE_PROJECT:
		{
			BMenu* menu = fTemplateField->Menu();
			BMenuItem* item = menu->FindMarked();
			if (item == NULL)
				break;

			int32 selection = menu->IndexOf(item);
			ProjectTemplate* ptemp = fTempList.TemplateAt(selection);

			BMessage projectMessage(M_CREATE_PROJECT), reply;
			projectMessage.AddString("name", fNameBox->Text());
			projectMessage.AddString("target", fTargetBox->Text());
			projectMessage.AddInt32("type", ptemp->TargetType());
			projectMessage.AddString("path", fPathBox->Path());
			projectMessage.AddString("template", ptemp->GetRef().name);
			projectMessage.AddString("pldfile", ptemp->ProjectFileName());

			BMenu* scmMenu = fSCMChooser->Menu();
			int32 scm = scmMenu->IndexOf(scmMenu->FindMarked());
			projectMessage.AddInt32("scmtype",scm);

			if (strlen(ptemp->ProjectFileName()) == 0) {
				for (int32 i = 0; i < ptemp->CountFiles(); i++)
					projectMessage.AddRef("refs", ptemp->FileAt(i));

				for (int32 i = 0; i < ptemp->CountLibs(); i++)
					projectMessage.AddRef("libs", ptemp->LibAt(i));
			}

			if (!BEntry(PROJECT_PATH).Exists())
				create_directory(PROJECT_PATH, 0755);

			projectMessage.AddBool("createfolder", (fCreateFolder->Value() == B_CONTROL_ON));
			be_app_messenger.SendMessage(&projectMessage, &reply);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		default:
			BWindow::MessageReceived(message);
	}
}


void
TemplateWindow::CheckTemplates(void)
{
	// This checks for the Templates folder in the Paladin application directory
	// and if it doesn't exist or it's empty, we make sure that it exists and
	// at least has empty project templates for each of the four basic types of projects

	DPath templatePath(gAppPath.GetFolder());
	templatePath << "Templates";

	bool needInit = false;
	if (!BEntry(templatePath.GetFullPath()).Exists())
		needInit = true;

	if (!needInit) {
		BDirectory dir(templatePath.GetFullPath());
		if (dir.CountEntries() == 0)
			needInit = true;
	}

	if (needInit) {
		create_directory(templatePath.GetFullPath(), 0777);

		DPath tpath(templatePath);
		BFile file;
		BString filedata;

		tpath.Append("Addon");
		create_directory(tpath.GetFullPath(), 0777);
		tpath.Append("TEMPLATEINFO");
		file.SetTo(tpath.GetFullPath(), B_READ_WRITE | B_CREATE_FILE);
		filedata = "TYPE=Shared\n";
		file.Write(filedata.String(),filedata.Length());

		tpath = templatePath;
		tpath.Append("Empty Application");
		create_directory(tpath.GetFullPath(), 0777);
		tpath.Append("TEMPLATEINFO");
		file.SetTo(tpath.GetFullPath(), B_READ_WRITE | B_CREATE_FILE);
		filedata = "TYPE=Application\n";
		file.Write(filedata.String(),filedata.Length());
		
		tpath = templatePath;
		tpath.Append("Kernel Driver");
		create_directory(tpath.GetFullPath(),0777);
		tpath.Append("TEMPLATEINFO");
		file.SetTo(tpath.GetFullPath(), B_READ_WRITE | B_CREATE_FILE);
		filedata = "TYPE=Driver\n";
		file.Write(filedata.String(),filedata.Length());
		
		tpath = templatePath;
		tpath.Append("Shared Library");
		create_directory(tpath.GetFullPath(),0777);
		tpath.Append("TEMPLATEINFO");
		file.SetTo(tpath.GetFullPath(), B_READ_WRITE | B_CREATE_FILE);
		filedata = "TYPE=Shared\n";
		file.Write(filedata.String(),filedata.Length());
		
		tpath = templatePath;
		tpath.Append("Static Library");
		create_directory(tpath.GetFullPath(),0777);
		tpath.Append("TEMPLATEINFO");
		file.SetTo(tpath.GetFullPath(), B_READ_WRITE | B_CREATE_FILE);
		filedata = "TYPE=Static\n";
		file.Write(filedata.String(),filedata.Length());
		
	}
}
