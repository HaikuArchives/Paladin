#include "TemplateWindow.h"

#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Font.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>

#include "Globals.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"
#include "Settings.h"

enum
{
	M_NAME_CHANGED = 'nmch',
	M_TARGET_CHANGED = 'tgch',
	M_TEMPLATE_SELECTED = 'tmsl',
	M_CHOOSE_LIBS = 'chlb',
	M_CUSTOM_APP = 'csap',
	M_CUSTOM_WIN = 'cswn'
};

TemplateWindow::TemplateWindow(const BRect &frame)
	:	DWindow(frame,TR("Choose a Project Type"),B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS |
				B_NOT_V_RESIZABLE)
{
	RegisterWindow();
	
	MakeCenteredOnShow(true);
	
	CheckTemplates();
	
	DPath templatePath(gAppPath.GetFolder());
	templatePath << TR("Templates");
	fTempList.ScanFolder(templatePath.GetFullPath());
	
	BView *top = GetBackgroundView();
	
	if (Bounds().Height() < 100)
		ResizeTo(Bounds().Width(),100);
	if (Bounds().Width() < 100)
		ResizeTo(100,Bounds().Height());
	
	BRect r(Bounds());
	BRect bounds(r);
	
	BMenu *menu = new BMenu("Project Type");
	for (int32 i = 0; i < fTempList.CountTemplates(); i++)
	{
		ProjectTemplate *ptemp = fTempList.TemplateAt(i);
		entry_ref ref = ptemp->GetRef();
		menu->AddItem(new BMenuItem(ref.name,new BMessage(M_TEMPLATE_SELECTED)));
	}
	
	menu->SetRadioMode(true);
	menu->SetLabelFromMarked(true);
	menu->ItemAt(0L)->SetMarked(true);
	
	font_height fh;
	be_plain_font->GetHeight(&fh);
	float fontHeight = fh.ascent + fh.descent + fh.leading;
	r.bottom = MAX(fontHeight,20.0);
	
	r.OffsetBy(10,10);
	fTemplateField = new BMenuField(r,"templatefield",TR("Project Type: "), menu);
	fTemplateField->SetDivider(be_plain_font->StringWidth(TR("Project Type:")) + 5);
	top->AddChild(fTemplateField);
	
	// controls for the options for all project types
	r.OffsetBy(0,r.IntegerHeight() + 10);
	fNameBox = new AutoTextControl(r,"namebox",TR("Project Name:"),NULL,
									new BMessage(M_NAME_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fNameBox);
	fNameBox->ResizeToPreferred();
	r.bottom = r.top + fNameBox->Bounds().Height();
	fNameBox->ResizeTo(bounds.Width() - 20.0,r.Height());
	fNameBox->SetDivider(be_plain_font->StringWidth(TR("Project Name:")) + 5.0);
	
	r.OffsetBy(0,r.IntegerHeight() + 10);
	fTargetBox = new AutoTextControl(r,"targetbox",TR("Target Name:"),"BeApp",
									new BMessage(M_TARGET_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fTargetBox);
	fTargetBox->ResizeToPreferred();
	r.bottom = r.top + fTargetBox->Bounds().Height();
	fTargetBox->ResizeTo(bounds.Width() - 20.0,r.Height());
	fTargetBox->SetDivider(be_plain_font->StringWidth(TR("Target Name:")) + 5.0);
	
	if (!BEntry(PROJECT_PATH).Exists())
		create_directory(PROJECT_PATH,0777);
	
	r.OffsetBy(0,r.Height() + 10);
	fPathBox = new PathBox(r,"pathbox",gProjectPath.GetFullPath(),TR("Location:"),
							B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fPathBox);
	fPathBox->ResizeToPreferred();
	r.bottom = r.top + fPathBox->Bounds().Height();
	fPathBox->ResizeTo(bounds.Width() - 20.0,r.Height());
	fPathBox->SetDivider(be_plain_font->StringWidth(TR("Target Name:")) + 5.0);
	
	r.OffsetBy(0,r.Height() + 5.0);
	fCreateFolder = new BCheckBox(r,"createfolder",TR("Create Project Folder"),NULL);
	fCreateFolder->MoveTo(fPathBox->Divider() + 10.0, r.top);
	fCreateFolder->SetValue(B_CONTROL_ON);
	top->AddChild(fCreateFolder);
	
	r.OffsetBy(0,r.Height() + 5.0);
	fOK = new BButton(r,"ok",TR("Create Project…"), new BMessage(M_CREATE_PROJECT));
	fOK->ResizeToPreferred();
	top->AddChild(fOK);
	fOK->SetEnabled(false);
	fOK->MakeDefault(true);
	
	float offset = fPathBox->Divider();
	fOK->MoveBy(offset,0);
	
	float minwidth = Bounds().Width();
	float minheight = fOK->Frame().bottom + 10.0;
	SetSizeLimits(minwidth,30000,minheight,30000);
	
	gSettings.Lock();
	BRect savedframe;
	if (gSettings.FindRect("template_frame",&savedframe) == B_OK)
		ResizeTo(savedframe.Width(),savedframe.Height());
	else
		ResizeTo(minwidth,minheight);
	gSettings.Unlock();
		
	fNameBox->MakeFocus(true);
}


TemplateWindow::~TemplateWindow(void)
{
	gSettings.Lock();
	if (gSettings.ReplaceRect("template_frame",Frame()) != B_OK)
		gSettings.AddRect("template_frame",Frame());
	gSettings.Unlock();
}


bool
TemplateWindow::QuitRequested(void)
{
	DeregisterWindow();
	return true;
}

void
TemplateWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_NAME_CHANGED:
		case M_TARGET_CHANGED:
		{
			if (fNameBox->Text() && strlen(fNameBox->Text()) > 0 &&
				fTargetBox->Text() && strlen(fTargetBox->Text()) > 0)
				fOK->SetEnabled(true);
			else
				fOK->SetEnabled(false);
			break;
		}
		case M_CREATE_PROJECT:
		{
			BMenu *menu = fTemplateField->Menu();
			BMenuItem *item = menu->FindMarked();
			if (!item)
				break;
			
			int32 selection = menu->IndexOf(item);
			ProjectTemplate *ptemp = fTempList.TemplateAt(selection);
			
			BMessage projmsg(M_CREATE_PROJECT),reply;
			projmsg.AddString("name",fNameBox->Text());
			projmsg.AddString("target",fTargetBox->Text());
			projmsg.AddInt32("type",ptemp->TargetType());
			projmsg.AddString("path",fPathBox->Path());
			
			// TODO: This will later one be an option
			projmsg.AddInt32("scmtype",SCM_HG);
			
			for (int32 i = 0; i < ptemp->CountFiles(); i++)
				projmsg.AddRef("refs",ptemp->FileAt(i));
			
			for (int32 i = 0; i < ptemp->CountLibs(); i++)
				projmsg.AddRef("libs",ptemp->LibAt(i));
			
			projmsg.AddBool("createfolder",(fCreateFolder->Value() == B_CONTROL_ON));
			be_app_messenger.SendMessage(&projmsg,&reply);
			PostMessage(B_QUIT_REQUESTED);
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
	
	if (!needInit)
	{
		BDirectory dir(templatePath.GetFullPath());
		if (dir.CountEntries() == 0)
			needInit = true;
	}
	
	if (needInit)
	{
		create_directory(templatePath.GetFullPath(),0777);
		
		DPath tpath(templatePath);
		BFile file;
		BString filedata;
		
		tpath.Append("Addon");
		create_directory(tpath.GetFullPath(),0777);
		tpath.Append("TEMPLATEINFO");
		file.SetTo(tpath.GetFullPath(), B_READ_WRITE | B_CREATE_FILE);
		filedata = "TYPE=Shared\n";
		file.Write(filedata.String(),filedata.Length());
		
		tpath = templatePath;
		tpath.Append("Empty Application");
		create_directory(tpath.GetFullPath(),0777);
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

