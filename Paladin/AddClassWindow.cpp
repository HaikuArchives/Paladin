#include "AddClassWindow.h"

#include <Button.h>
#include <Message.h>
#include <ScrollView.h>

#define M_ADD_CLASS 'adcl'

AddClassWindow::AddClassWindow(void)
	:	BWindow(BRect(100,100,500,400),"Add Class",
				B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE)
{
	BRect r(Bounds());
	
	BView *top = new BView(r, "top", B_FOLLOW_ALL, B_WILL_DRAW);
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(top);
	
	BButton *add = new BButton(r,"addbutton","Add…",new BMessage(M_ADD_CLASS),
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	add->ResizeToPreferred();
	add->MoveTo(r.right - add->Bounds().Width() - 10.0,
				r.bottom - add->Bounds().Height() - 10.0);
	top->AddChild(add);
	add->MakeDefault(true);
	
	r.InsetBy(10,10);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom = add->Frame().top - 10 - B_H_SCROLL_BAR_HEIGHT;
	
	fClassList = new BListView(r,"classlist",B_SINGLE_SELECTION_LIST,
								B_FOLLOW_ALL);
	BScrollView *scrollView = new BScrollView("scroller",fClassList,
											B_FOLLOW_ALL,0,true,true);
	scrollView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	top->AddChild(scrollView);
	
	PopulateList();
	
	fClassList->MakeFocus(true);
}


void
AddClassWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_ADD_CLASS:
		{
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}


void
AddClassWindow::PopulateList(void)
{
}

