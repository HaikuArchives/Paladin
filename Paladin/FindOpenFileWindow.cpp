#include "FindOpenFileWindow.h"

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <Path.h>
#include <Size.h>
#include <View.h>

#include "AutoTextControl.h"
#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Globals.h"
#include "PLocale.h"
#include "Project.h"


#define	M_FIND_FILE 'fnfl'


FindOpenFileWindow::FindOpenFileWindow(const char* panelText)
	:
	DWindow(BRect(0, 0, 0, 0), TR("Find and open file"), B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	AddCommonFilter(new EscapeCancelFilter());

	fNameTextControl = new AutoTextControl("nameText", TR("Open: "), "",
		new BMessage);
	fNameTextControl->SetExplicitMinSize(
		BSize(fNameTextControl->StringWidth("M") * 20, B_SIZE_UNSET));
	fSystemCheckBox = new BCheckBox("systembox", TR("Search only system folders"),
		new BMessage);

	BButton* cancel = new BButton("cancel", TR("Cancel"),
		new BMessage(B_QUIT_REQUESTED));

	BButton* open = new BButton("open", TR("Open"), new BMessage(M_FIND_FILE));

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
			.Add(fNameTextControl->CreateLabelLayoutItem(), 0, 0)
			.Add(fNameTextControl->CreateTextViewLayoutItem(), 1, 0)
			.Add(fSystemCheckBox, 1, 1)
			.End()
		.AddGlue()
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(cancel)
			.Add(open)
			.End()
		.SetInsets(B_USE_WINDOW_INSETS)
		.End();

	BString text = panelText;
	if (text.CountChars() > 1) {
		fNameTextControl->SetText(text.String());
		int32 position = text.FindLast(".");
		if (position > 0)
			fNameTextControl->TextView()->Select(0, position);
		else
			fNameTextControl->TextView()->SelectAll();
	} else {
		fNameTextControl->SetText(".h");
		fNameTextControl->TextView()->GoToLine(0);
	}

	open->MakeDefault(true);

	fNameTextControl->MakeFocus(true);

	CenterOnScreen();
}


void
FindOpenFileWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_FIND_FILE:
		{
			BMessage findmessage(M_FIND_AND_OPEN_FILE);
			findmessage.AddString("name", fNameTextControl->Text());

			if (fSystemCheckBox->Value() == B_CONTROL_OFF)
				findmessage.AddString("folder", gCurrentProject->GetPath().GetFolder());
			
			#ifdef __HAIKU__
			findmessage.AddString("folder","/boot/system/develop/headers");
			#else
			findmessage.AddString("folder","/boot/develop/headers");
			#endif
			
			DPath path(B_USER_CONFIG_DIRECTORY);
			path << "include";
			findmessage.AddString("folder", path.GetFullPath());

			if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4) {
				path.SetTo(B_USER_NONPACKAGED_DIRECTORY);
				path << "include";
				findmessage.AddString("folder", path.GetFullPath());
			}

			be_app->PostMessage(&findmessage);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		default:
			DWindow::MessageReceived(message);
			break;
	}
}
