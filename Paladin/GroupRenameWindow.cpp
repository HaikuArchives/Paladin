#include "GroupRenameWindow.h"

#include <Catalog.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "SourceFile.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "GroupRenameWindow"

#define M_NAME_CHANGED 'nmch'

GroupRenameWindow::GroupRenameWindow(SourceGroup *group, const BMessage &msg,
									const BMessenger &msgr)
	:	DWindow(BRect(0,0,300,300),B_TRANSLATE("Rename group"), B_MODAL_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fGroup(group),
		fMessage(msg),
		fMessenger(msgr)
{
	AddCommonFilter(new EscapeCancelFilter());

	fGroupText = new AutoTextControl("argtext",B_TRANSLATE("New group name: "),
									fGroup->name.String(), new BMessage(M_NAME_CHANGED));
	fGroupText->SetDivider(fGroupText->StringWidth(B_TRANSLATE("New group name: ")) + 5);

	fRename = new BButton("rename",B_TRANSLATE("Rename"),new BMessage(B_QUIT_REQUESTED));	
	BButton *cancel = new BButton("cancel",B_TRANSLATE("Cancel"),new BMessage(B_QUIT_REQUESTED));

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

