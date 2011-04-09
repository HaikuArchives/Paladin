#include "MainWindow.h"

#include <Application.h>
#include <Button.h>
#include <Font.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <StringView.h>

enum
{
	M_FIND = 'find',
	M_REPLACE = 'repl',
	M_REPLACE_FIND = 'rfnd',
	M_REPLACE_ALL = 'rpal'
};

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,500,400), "Find", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BRect r(Bounds());
	r.bottom = 20;
	fMenuBar = new BMenuBar(r,"menubar");
	AddChild(fMenuBar);
	
	r = Bounds();
	r.top = 20;
	BView *top = new BView(r, "top", B_FOLLOW_ALL, B_WILL_DRAW);
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(top);
	
	BStringView *findLabel = new BStringView(BRect(0,0,1,1), "findLabel", "Find:");
	findLabel->ResizeToPreferred();
	top->AddChild(findLabel);
	
	BStringView *replaceLabel = new BStringView(BRect(0,0,1,1), "replaceLabel", "Replace:");
	replaceLabel->ResizeToPreferred();
	top->AddChild(replaceLabel);
	
	findLabel->MoveTo(10.0, 10.0);
	
	fFind = new BButton(BRect(0,0,1,1), "find", "Replace & Find", new BMessage(M_FIND),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fFind->ResizeToPreferred();
	fFind->SetLabel("Find");
	fFind->MoveTo(Bounds().right - fFind->Bounds().Width() - 10.0, 10.0);
	
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
	fReplace = new BButton(r, "replace", "Replace", new BMessage(M_REPLACE),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	top->AddChild(fReplace);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fReplaceFind = new BButton(r, "replacefind", "Replace & Find", new BMessage(M_REPLACE_FIND),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	top->AddChild(fReplaceFind);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fReplaceAll = new BButton(r, "replaceall", "Replace All", new BMessage(M_REPLACE_ALL),
						B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	top->AddChild(fReplaceAll);
	
	BStringView *listLabel = new BStringView(BRect(0,0,1,1), "listLabel", "Files to Search:");
	listLabel->ResizeToPreferred();
	listLabel->MoveTo(10.0, scroll->Frame().bottom + 5.0);
	top->AddChild(listLabel);
	
	// Add the Project dropdown
	// Add the Sources / Headers checkboxes
	
	fFindBox->MakeFocus(true);
}


void
MainWindow::MessageReceived(BMessage *msg)
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


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
