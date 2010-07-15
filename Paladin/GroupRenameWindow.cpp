#include "GroupRenameWindow.h"

#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "PLocale.h"
#include "SourceFile.h"

#define M_NAME_CHANGED 'nmch'

GroupRenameWindow::GroupRenameWindow(SourceGroup *group, const BMessage &msg,
									const BMessenger &msgr)
	:	DWindow(BRect(0,0,300,300),TR("Rename Group"), B_MODAL_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fGroup(group),
		fMessage(msg),
		fMessenger(msgr)
{
	AddCommonFilter(new EscapeCancelFilter());
	
	BView *top = GetBackgroundView();
	
	fGroupText = new AutoTextControl(BRect(10,10,11,11),"argtext",TR("New Group Name: "),
									fGroup->name.String(), new BMessage(M_NAME_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fGroupText);
	fGroupText->ResizeToPreferred();
	fGroupText->ResizeTo(Bounds().Width() - 20,fGroupText->Bounds().Height());
	fGroupText->SetDivider(fGroupText->StringWidth(TR("New Group Name: ")) + 5);
	
	fRename = new BButton(BRect(0,0,1,1),"rename",TR("Rename"),new BMessage(B_QUIT_REQUESTED));
	fRename->ResizeToPreferred();
	top->AddChild(fRename);
	
	fRename->MoveTo(Bounds().right - 10 - fRename->Bounds().Width(),
					fGroupText->Frame().bottom + 10);
	
	BButton *cancel = new BButton(BRect(0,0,1,1),"cancel",TR("Cancel"),new BMessage(B_QUIT_REQUESTED));
	cancel->ResizeToPreferred();
	top->AddChild(cancel);
	
	cancel->MoveTo(fRename->Frame().left - 10 - cancel->Bounds().Width(),
					fGroupText->Frame().bottom + 10);
	
	ResizeTo(Bounds().Width(),fRename->Frame().bottom + 10);
	
	MakeCenteredOnShow(true);
	
	fRename->MakeDefault(true);
	fGroupText->MakeFocus(true);
	fGroupText->TextView()->SelectAll();
}


void
GroupRenameWindow::MessageReceived(BMessage *msg)
{
	if (msg->what == M_NAME_CHANGED)
		fRename->SetEnabled((strlen(fGroupText->Text()) > 0) ? true : false);
	else
		DWindow::MessageReceived(msg);
}


bool
GroupRenameWindow::QuitRequested(void)
{
	if (strlen(fGroupText->Text()) > 0 &&
		fGroup->name.Compare(fGroupText->Text()) != 0)
	{
		fMessage.AddPointer("group",fGroup);
		fMessage.AddString("newname",fGroupText->Text());
		fMessenger.SendMessage(&fMessage);
	}
	return true;
}

