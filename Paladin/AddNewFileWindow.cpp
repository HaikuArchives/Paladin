#include "AddNewFileWindow.h"

#include <Button.h>
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Message.h>
#include <Messenger.h>
#include <Path.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "Project.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "AddNewFileWindow"

#define	M_ADD_FILE 'adfl'


AddNewFileWindow::AddNewFileWindow(const BMessage &message,
	const BMessenger &messenger, bool renameMode)
	:
	DWindow(BRect(0,0,400,200),B_TRANSLATE("Add new file"),B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE), fMessage(message),
		fMessenger(messenger)
{
	BString namelabel;
	BString checklabel;
	if (renameMode) {
		SetTitle(B_TRANSLATE("Rename file"));
		namelabel = B_TRANSLATE("New name:");
		checklabel = B_TRANSLATE("Rename partner file");
	}
	else
	{
		namelabel = B_TRANSLATE("New file name:");
		checklabel = B_TRANSLATE("Create both a header and source file");
	}

	AddCommonFilter(new EscapeCancelFilter());

	MakeCenteredOnShow(true);

	fNameText = new AutoTextControl("nametext",
		namelabel.String(), NULL, new BMessage);
	fNameText->SetDivider(fNameText->StringWidth(namelabel.String()) + 5);
	fNameText->DisallowCharacters("/,@\"\\");

	fBothBox = new BCheckBox("partnerbox",checklabel.String(),
		new BMessage);

	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(B_QUIT_REQUESTED));
	BButton* open = new BButton("create", B_TRANSLATE("Create"), new BMessage(M_ADD_FILE));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.AddGroup(B_VERTICAL)
			.Add(fNameText)
			.Add(fBothBox)
			.AddGlue()
			.AddGroup(B_HORIZONTAL)
				.AddGlue()
				.Add(cancel)
				.Add(open)
				.End()
			.End()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.End();

	ResizeTo(400, fNameText->Bounds().Height() * 4  + 4 * 10);

	BRect r = Frame();	
	BRect screen(BScreen().Frame());
	MoveTo((screen.Width() - r.Width()) / 2.0, (screen.Height() - r.Height()) / 2.0);

	open->MakeDefault(true);
	fNameText->MakeFocus(true);
}


void
AddNewFileWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
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
			DWindow::MessageReceived(message);
	}
}
