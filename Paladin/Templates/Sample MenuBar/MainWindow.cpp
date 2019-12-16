#include "MainWindow.h"

#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <View.h>


// Create a list of message types for this application.
// A message type needs to be an int.
// Create a rather unique int by casting a string of characters.
enum
{
	M_SET_COLOR_RED = 'sred',
	M_SET_COLOR_GREEN = 'sgrn',
	M_SET_COLOR_BLUE = 'sblu',
	M_SET_COLOR_BLACK = 'sblk'
};


// http://www.haiku-os.org/legacy-docs/bebook/BWindow.html#BWindow_Constructor
MainWindow::MainWindow(void)
	:
	BWindow(BRect(100,100,500,400),"MenuApp",B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE)
{
	BRect r(Bounds());
	r.bottom = 20;
	
	// The menu bar is created inside a rectangle area "r"
	// http://www.haiku-os.org/legacy-docs/bebook/BMenuBar.html#BMenuBar_Constructor
	BMenuBar *menuBar = new BMenuBar(r,"menubar");
	
	// Add the menu bar to the window
	AddChild(menuBar);
	
	// Create a menu to go in the menu bar
	// http://www.haiku-os.org/legacy-docs/bebook/BMenu.html#BMenu_Constructor
	BMenu *menu = new BMenu("Colors");
	
	// A menu item only needs a text label and
	// a message to send when it's pressed.
	// You can also provide a keyboard shortcut character.
	// http://www.haiku-os.org/legacy-docs/bebook/BMenuItem.html#BMenuItem_Constructor
	menu->AddItem(new BMenuItem("Red",new BMessage(M_SET_COLOR_RED),'R'));
	menu->AddItem(new BMenuItem("Green",new BMessage(M_SET_COLOR_GREEN),'G'));
	menu->AddItem(new BMenuItem("Blue",new BMessage(M_SET_COLOR_BLUE),'B'));
	menu->AddItem(new BMenuItem("Black",new BMessage(M_SET_COLOR_BLACK),'K'));
	menuBar->AddItem(menu);
	
	// Create a "view" to do some drawing commands to.
	// In this example, we will be changing its color.
	// http://www.haiku-os.org/legacy-docs/bebook/BView.html#BView_Constructor
	BView *view = new BView(BRect(100,100,300,200),"colorview",B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(view);
	view->SetViewColor(0,0,160);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	// http://www.haiku-os.org/legacy-docs/bebook/BWindow.html#BWindow_FindView
	BView *view = FindView("colorview");
	
	// Find out what type of message was recieved
	// and act accordingly.
	// http://www.haiku-os.org/legacy-docs/bebook/BMessage.html#BMessage_what
	switch (msg->what)
	{
		case M_SET_COLOR_RED:
		{
			view->SetViewColor(160,0,0);
			
			// An invalidated view will be forced to be redrawn.
			// http://www.haiku-os.org/legacy-docs/bebook/BView.html#BView_Invalidate
			view->Invalidate();
			break;
		}
		case M_SET_COLOR_GREEN:
		{
			view->SetViewColor(0,160,0);
			view->Invalidate();
			break;
		}
		case M_SET_COLOR_BLUE:
		{
			view->SetViewColor(0,0,160);
			view->Invalidate();
			break;
		}
		case M_SET_COLOR_BLACK:
		{
			view->SetViewColor(0,0,0);
			view->Invalidate();
			break;
		}
		default:
		{
			// Some other message was recieved!
			// Just send it to the parent "BWindow" class.
			BWindow::MessageReceived(msg);
			break;
		}
	}
}
