#include "AddNewFileWindow.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <Messenger.h>
#include <Path.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"

#define	M_ADD_FILE 'adfl'


AddNewFileWindow::AddNewFileWindow(const BMessage &message,
	const BMessenger &messenger, bool renameMode)
	:
	DWindow(BRect(0,0,400,200),TR("Add new file"),B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE), fMessage(message),
		fMessenger(messenger)
{
	BString namelabel;
	BString checklabel;
	if (renameMode) {
		SetTitle("Rename file");
		namelabel = TR("New name: ");
		checklabel = TR("Rename partner file");
	}
	else
	{
		namelabel = TR("New file name: ");
		checklabel = TR("Create both a header and source file");
	}

	AddCommonFilter(new EscapeCancelFilter());

	MakeCenteredOnShow(true);

	fNameText = new AutoTextControl("nametext",
		namelabel.String(), NULL, new BMessage);
	fNameText->SetDivider(fNameText->StringWidth(namelabel.String()) + 5);
	fNameText->DisallowCharacters("/,@\"\\");

	fBothBox = new BCheckBox("partnerbox",checklabel.String(),
		new BMessage);

	BButton* cancel = new BButton("cancel", TR("Cancel"),
		new BMessage(B_QUIT_REQUESTED));
	BButton* open = new BButton("create", TR("Create"), new BMessage(M_ADD_FILE));

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
