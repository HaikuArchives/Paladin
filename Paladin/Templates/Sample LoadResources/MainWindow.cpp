#include "MainWindow.h"

#include <Application.h>
#include <View.h>

#include "PictureView.h"

MainWindow::MainWindow(void)
	:	BWindow(BRect(300,300,450,450),"Emo",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	// Once again, we will make a background view to make things look nice
	BView *top = new BView(Bounds(),"top",B_FOLLOW_ALL,B_WILL_DRAW);
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(top);
	
	// This is our own BView subclass which draws one of several pictures
	PictureView *picview = new PictureView();
	
	// Note that we have the background view add our picture view as a child.
	// Unpredictable drawing results will result when views overlap each other. You
	// can also run into problems where a view which is drawn on the screen doesn't
	// receive mouse events and other bizarre situations.
	top->AddChild(picview);
	
	// Move our picture view to the center of the window. This code right here is
	// very common, so get used to seeing and/or using it.
	picview->MoveTo((Bounds().Width() - picview->Bounds().Width()) / 2.0,
					(Bounds().Height() - picview->Bounds().Height()) / 2.0);
}


// QuitRequested() is a BWindow hook function which is called when the window is asked
// to close. When it returns true, the window is given "permission" to quit and it does.
// If QuitRequested() returns false, the window just keeps on running as if nothing
// happened. Implementing this hook to send a quit message to the global application
// instance is the old way of making an app quit when the main window is closed. It is
// included here because legacy apps do it this way instead of using the 
// B_QUIT_ON_WINDOW_CLOSE flag in the constructor. This function also has its uses for
// situations where you need to ask the user "Would you like to save your changes?" and
// giving the user a chance to cancel the window closing altogether.
bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
