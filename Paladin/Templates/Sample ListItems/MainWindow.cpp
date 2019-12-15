#include "MainWindow.h"

#include <Button.h>
#include <ListItem.h>
#include <ScrollView.h>

enum
{
	M_RESET_WINDOW = 'rswn',
	M_SET_TITLE = 'sttl'
};

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,500,400),"The Weird World of Sports",B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE)
{
	// Here we will make a BView that covers the white area so that we can set the
	// "background color"
	BRect r(Bounds());
	BView *top = new BView(r,"topview",B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(top);
	
	// ui_color() returns a system color, such as the window tab color, menu text color,
	// and so forth.
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// Create a button and place it at the bottom right corner of the window. The empty BRect
	// that we use for the BButton's frame is because we're going to have it resize itself and
	// then move it to the corner based on the actual size of the button
	BButton *reset = new BButton(BRect(),"resetbutton","Reset", new BMessage(M_RESET_WINDOW),
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	top->AddChild(reset);
	reset->ResizeToPreferred();
	
	// Bottom right corner of the window with 10 pixels of padding between the button and the
	// window edge. 10 pixels is kind of a de facto standard for control padding.
	reset->MoveTo(Bounds().right - reset->Bounds().Width() - 10.0,
					Bounds().bottom - reset->Bounds().Height() - 10.0);
	
	r = Bounds();
	r.InsetBy(10.0,10.0);
	
	// When working with BScrollViews, you must compensate for the width/height of the scrollbars.
	// B_V_SCROLL_BAR_WIDTH is a defined constant for the width of the vertical scroll bar.
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	// Frame() works like Bounds() except that it returns the size and location of the control
	// in the coordinate space of the parent view. This will make fListView's bottom stop 10
	// pixels above the button.
	r.bottom = reset->Frame().top - 10.0 - B_H_SCROLL_BAR_HEIGHT;
	
	// Most of these parameters are exactly the same as for BView except that we can also
	// specify whether the user is able to select just 1 item in the list or multiple items by
	// clicking on items while holding a modifier key on the keyboard.
	fListView = new BListView(r,"sportlist",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL);
	
	// We didn't call AddChild on fListView because our BScrollView will do that for us. When
	// created, it creates scrollbars and targets the specified view for any scrolling they do.
	// When the BScrollView is attached to the window, it calls AddChild on fListView for us.
	
	// If we call AddChild on fListView before we create this scrollview, our program will drop
	// to the debugger when we call AddChild on the BScrollView -- a BView can have only one parent.
	BScrollView *scrollView = new BScrollView("scrollview",fListView, B_FOLLOW_ALL, 0,true,true);
	top->AddChild(scrollView);
	
	// A BListView's selection message is sent to the window any time that the list's selection
	// changes.
	fListView->SetSelectionMessage(new BMessage(M_SET_TITLE));
	
	fListView->AddItem(new BStringItem("Toe Wrestling"));
	fListView->AddItem(new BStringItem("Electric Toilet Racing"));
	fListView->AddItem(new BStringItem("Bog Snorkeling"));
	fListView->AddItem(new BStringItem("Chess Boxing"));
	fListView->AddItem(new BStringItem("Cheese Rolling"));
	fListView->AddItem(new BStringItem("Unicycle Polo"));
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_RESET_WINDOW:
		{
			fListView->DeselectAll();
			break;
		}
		case M_SET_TITLE:
		{
			int32 selection = fListView->CurrentSelection();
			
			if (selection < 0)
			{
				// This code is here because when we press the Reset button, the selection
				// changes and an M_SET_TITLE message is sent, but because nothing is
				// selected, CurrentSelection() returns -1.
				SetTitle("The Weird World of Sports");
				break;
			}
			
			BStringItem *item = dynamic_cast<BStringItem*>(fListView->ItemAt(selection));
			if (item)
				SetTitle(item->Text());
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}
