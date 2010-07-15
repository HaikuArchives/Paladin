#include "AddNewFileWindow.h"

#include <Button.h>
#include <Path.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"

#define	M_ADD_FILE 'adfl'


AddNewFileWindow::AddNewFileWindow(const BMessage &msg, const BMessenger &msgr,
									bool rename_mode)
	:	DWindow(BRect(0,0,500,400),TR("Add New File"),B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
				fMessage(msg),
				fMessenger(msgr)
{
	BString namelabel, checklabel;
	if (rename_mode)
	{
		SetTitle("Rename File");
		namelabel = TR("New Name: ");
		checklabel = TR("Rename partner file");
	}
	else
	{
		namelabel = TR("New File Name: ");
		checklabel = TR("Create both a header and source file");
	}
	
	AddCommonFilter(new EscapeCancelFilter());
	
	MakeCenteredOnShow(true);
	
	BView *top = GetBackgroundView();
	
	fNameText = new AutoTextControl(BRect(10,10,11,11),"nametext",namelabel.String(),
									NULL, new BMessage, B_FOLLOW_LEFT_RIGHT |
														B_FOLLOW_TOP);
	top->AddChild(fNameText);
	fNameText->ResizeToPreferred();
	fNameText->ResizeTo(Bounds().Width() - 20,fNameText->Bounds().Height());
	fNameText->SetDivider(fNameText->StringWidth(namelabel.String()) + 5);
	
	BRect r = fNameText->Frame();
	r.OffsetBy(0,r.Height() + 10.0);
	
	fBothBox = new BCheckBox(r,"partnerbox",checklabel.String(),
							new BMessage);
	top->AddChild(fBothBox);
	fBothBox->ResizeToPreferred();
	r = fBothBox->Frame();
	
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
	BButton *open = new BButton(r,"create",TR("Create"), new BMessage(M_ADD_FILE));
	top->AddChild(open);
	
	r = Frame();
	BRect screen(BScreen().Frame());
	MoveTo( (screen.Width() - r.Width()) / 2.0, (screen.Height() - r.Height()) / 2.0);
	
	open->MakeDefault(true);
	fNameText->MakeFocus(true);
}


void
AddNewFileWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_ADD_FILE:
		{
			fMessage.MakeEmpty();
			fMessage.AddString("name",fNameText->Text());
			fMessage.AddBool("makepair", (fBothBox->Value() == B_CONTROL_ON));
			fMessenger.SendMessage(&fMessage);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
			DWindow::MessageReceived(msg);
	}
}
