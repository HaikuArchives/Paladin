#include "FindOpenFileWindow.h"

#include <Application.h>
#include <Button.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Globals.h"
#include "PLocale.h"
#include "Project.h"

#define	M_FIND_FILE 'fnfl'


FindOpenFileWindow::FindOpenFileWindow(const char *paneltext)
	:	DWindow(BRect(0,0,500,400),TR("Find and Open File"),B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	AddCommonFilter(new EscapeCancelFilter());
	
	BView *top = GetBackgroundView();
	
	fNameText = new AutoTextControl(BRect(10,10,11,11),"nametext",TR("Open: "),
									"", new BMessage, B_FOLLOW_LEFT_RIGHT |
														B_FOLLOW_TOP);
	top->AddChild(fNameText);
	fNameText->ResizeToPreferred();
	fNameText->ResizeTo(Bounds().Width() - 20,fNameText->Bounds().Height());
	fNameText->SetDivider(fNameText->StringWidth(TR("Open: ")) + 5);
	
	BRect r = fNameText->Frame();
	r.OffsetBy(0,r.Height() + 10.0);
	
	fSystemBox = new BCheckBox(r,"systembox",TR("Search only system folders"),
								new BMessage);
	top->AddChild(fSystemBox);
	fSystemBox->ResizeToPreferred();
	r = fSystemBox->Frame();
	fSystemBox->MoveBy(fNameText->Divider(), 0);
	
	r.OffsetBy(0,r.Height() + 10.0);
	BButton *cancel = new BButton(r,"cancel",TR("Cancel"),
									new BMessage(B_QUIT_REQUESTED));
	cancel->ResizeToPreferred();
	top->AddChild(cancel);
	
	ResizeTo(300, cancel->Frame().bottom + 10);
	cancel->MoveTo( (Bounds().Width() - (cancel->Bounds().Width() * 2) - 10) / 2,
					cancel->Frame().top);
	
	r = cancel->Frame();
	r.OffsetBy(r.Width() + 10,0);
	BButton *open = new BButton(r,"open",TR("Open"), new BMessage(M_FIND_FILE));
	top->AddChild(open);
	
	r = Frame();
	BRect screen(BScreen().Frame());
	MoveTo( (screen.Width() - r.Width()) / 2.0, (screen.Height() - r.Height()) / 2.0);
	
	fNameText->MakeFocus(true);
	
	BString text = paneltext;
	if (text.CountChars() > 1)
	{
		fNameText->SetText(text.String());
		int32 pos = text.FindLast(".");
		if (pos > 0)
			fNameText->TextView()->Select(0,pos);
		else
			fNameText->TextView()->SelectAll();
	}
	else
	{
		fNameText->SetText(".h");
		fNameText->TextView()->GoToLine(0);
	}
	
	open->MakeDefault(true);
}


void
FindOpenFileWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_FIND_FILE:
		{
			BMessage findmsg(M_FIND_AND_OPEN_FILE);
			findmsg.AddString("name",fNameText->Text());
			
			if (fSystemBox->Value() == B_CONTROL_OFF)
				findmsg.AddString("folder",gCurrentProject->GetPath().GetFolder());
			
			findmsg.AddString("folder","/boot/develop/headers");
			
			DPath path(B_USER_CONFIG_DIRECTORY);
			path << "include";
			findmsg.AddString("folder", path.GetFullPath());
			
			if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4)
			{
				path.SetTo(B_COMMON_DIRECTORY);
				path << "include";
				findmsg.AddString("folder", path.GetFullPath());
			}
			
			be_app->PostMessage(&findmsg);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
			DWindow::MessageReceived(msg);
	}
}
