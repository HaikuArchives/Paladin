#include "MainWindow.h"

#include <Application.h>
#include <Menu.h>
#include <MenuItem.h>
#include "MainView.h"

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,500,400),"Find",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BRect r(Bounds());
	r.bottom = 20;
	fMenuBar = new BMenuBar(r,"menubar");
	AddChild(fMenuBar);
	
	r = Bounds();
	r.top = 20;
	MainView *view = new MainView(r);
	AddChild(view);
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
