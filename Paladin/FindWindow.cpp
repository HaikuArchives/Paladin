#include "FindWindow.h"

#include <Application.h>
#include <Button.h>
#include <Font.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <StringView.h>

#include "DListView.h"
#include "PLocale.h"

enum
{
	M_FIND = 'find',
	M_REPLACE = 'repl',
	M_REPLACE_FIND = 'rfnd',
	M_REPLACE_ALL = 'rpal',
	
	M_TOGGLE_SOURCES = 'tgsc',
	M_TOGGLE_HEADERS = 'tghd',
	M_TOGGLE_OTHER = 'tgot'
};


FindWindow::FindWindow(void)
	:	DWindow(BRect(100,100,500,400), TR("Find"), B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BView *top = GetBackgroundView();
	
	BRect r(Bounds());
	r.bottom = 20;
	fMenuBar = new BMenuBar(r,"menubar");
	top->AddChild(fMenuBar);
	
	r = Bounds();
	r.top = 20;
	
	BStringView *findLabel = new BStringView(BRect(0,0,1,1), "findLabel", TR("Find:"));
	findLabel->ResizeToPreferred();
	top->AddChild(findLabel);
	
	BStringView *replaceLabel = new BStringView(BRect(0,0,1,1), "replaceLabel", TR("Replace:"));
	replaceLabel->ResizeToPreferred();
	top->AddChild(replaceLabel);
	
	findLabel->MoveTo(10.0, fMenuBar->Bounds().bottom + 5.0);
	
	fFind = new BButton(BRect(0,0,1,1), "find", TR("Replace & Find"), new BMessage(M_FIND),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fFind->ResizeToPreferred();
	fFind->SetLabel(TR("Find"));
	fFind->MoveTo(Bounds().right - fFind->Bounds().Width() - 10.0, findLabel->Frame().top);
	
	r.left = 10.0;
	r.top = findLabel->Frame().bottom + 3.0;
	r.right = fFind->Frame().left - B_V_SCROLL_BAR_WIDTH - 10.0;
	r.bottom = r.top + (findLabel->Bounds().Height() * 2.0) + 10.0;
	BRect textRect = r.OffsetToCopy(0.0, 0.0);
	textRect.InsetBy(5.0, 5.0);
	fFindBox = new BTextView(r, "findbox", textRect, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	BScrollView *scroll = new BScrollView("findscroll", fFindBox,
											B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, 0, false, true);
	top->AddChild(scroll);
	
	replaceLabel->MoveTo(10.0, scroll->Frame().bottom + 5.0);
	r.OffsetTo(10.0, replaceLabel->Frame().bottom + 3.0);
	fReplaceBox = new BTextView(r, "replacebox", textRect, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	scroll = new BScrollView("replacescroll", fReplaceBox, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
							0, false, true);
	top->AddChild(scroll);
	
	top->AddChild(fFind);
	
	r = fFind->Frame();
	r.OffsetBy(0.0, r.Height() + 5.0);
	fReplace = new BButton(r, "replace", TR("Replace"), new BMessage(M_REPLACE),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	top->AddChild(fReplace);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fReplaceFind = new BButton(r, "replacefind", TR("Replace & Find"), new BMessage(M_REPLACE_FIND),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	top->AddChild(fReplaceFind);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fReplaceAll = new BButton(r, "replaceall", TR("Replace All"), new BMessage(M_REPLACE_ALL),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	top->AddChild(fReplaceAll);
	
	BStringView *listLabel = new BStringView(BRect(0,0,1,1), "listLabel", TR("Files to Search:"));
	listLabel->ResizeToPreferred();
	listLabel->MoveTo(10.0, scroll->Frame().bottom + 10.0);
	top->AddChild(listLabel);
	
	// Add the Project dropdown
	fProjectMenu = new BMenu(TR("No Project"));
	
	r = listLabel->Frame();
	r.right += be_plain_font->StringWidth("Project:") + 5.0;
	BMenuField *projectField = new BMenuField(r, "projectfield", TR("Project:"), fProjectMenu);
	top->AddChild(projectField);
	projectField->SetEnabled(false);
	
	listLabel->MoveTo(projectField->Frame().right + 30.0, listLabel->Frame().top);
	
	// Add the Sources / Headers checkboxes
	r.OffsetBy(0.0, r.Height() + 10.0);
	fUseSources = new BCheckBox(r, "usesources", TR("Sources"), new BMessage(M_TOGGLE_SOURCES));
	top->AddChild(fUseSources);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fUseHeaders = new BCheckBox(r, "useheaders", TR("Headers"), new BMessage(M_TOGGLE_HEADERS));
	top->AddChild(fUseHeaders);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fUseOtherText = new BCheckBox(r, "useother", TR("Other Text"), new BMessage(M_TOGGLE_OTHER));
	top->AddChild(fUseOtherText);
	
	r.left = listLabel->Frame().left;
	r.top = listLabel->Frame().bottom + 3.0;
	r.right = top->Bounds().right - 10.0 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = top->Bounds().bottom - 10.0 - B_H_SCROLL_BAR_HEIGHT;
	fFileList = new RefListView(r, "filelist", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL);
	scroll = fFileList->MakeScrollView("listscroll", true, true);
	top->AddChild(scroll);
	
	fFindBox->MakeFocus(true);
}


void
FindWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}
