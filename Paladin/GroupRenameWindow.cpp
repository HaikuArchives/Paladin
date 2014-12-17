#include "GroupRenameWindow.h"

#include <LayoutBuilder.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "PLocale.h"
#include "SourceFile.h"

#define M_NAME_CHANGED 'nmch'

GroupRenameWindow::GroupRenameWindow(SourceGroup *group, const BMessage &msg,
									const BMessenger &msgr)
	:	DWindow(BRect(0,0,300,300),TR("Rename group"), B_MODAL_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fGroup(group),
		fMessage(msg),
		fMessenger(msgr)
{
	AddCommonFilter(new EscapeCancelFilter());

	BView *top = new BView("topview", B_WILL_DRAW);
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	fGroupText = new AutoTextControl(BRect(),"argtext",TR("New group name: "),
									fGroup->name.String(), new BMessage(M_NAME_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fGroupText->SetDivider(fGroupText->StringWidth(TR("New group name: ")) + 5);

	fRename = new BButton(BRect(0,0,1,1),"rename",TR("Rename"),new BMessage(B_QUIT_REQUESTED));	
	BButton *cancel = new BButton(BRect(0,0,1,1),"cancel",TR("Cancel"),new BMessage(B_QUIT_REQUESTED));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.AddGroup(B_VERTICAL)
			.Add(fGroupText)
			.AddGlue()
			.AddGroup(B_HORIZONTAL)
				.AddGlue()
				.Add(fRename)
				.Add(cancel)
				.End()
			.End()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.End();

	ResizeTo(400, fGroupText->Bounds().Height() * 3  + 4 * 10);

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

