#include "MainWindow.h"

// Button.h adds the class definiton for the BButton control
#include <Button.h>

// The BString class is a phenomenally useful class which eliminates
// almost all hassle associated with manipulating strings.
#include <String.h>

// The BView class is the generic class used for creating controls and
// drawing things inside a window.
#include <View.h>

// This defines the identifier for the message that our button will send. The
// letters inside the single quotes are translated into an integer. The value
// for M_BUTTON_CLICKED is arbitrary, so as long as it's unique, it's not too
// important what it is.
enum
{
	M_BUTTON_CLICKED = 'btcl'
};


MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,300,200),"ClickMe",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | 
																	B_QUIT_ON_WINDOW_CLOSE),
		fCount(0)
{
	// Create a button in pretty much the same way that we did the label in
	// the last lesson. The BRect() call inside the BButton constructor is a
	// quick shortcut that eliminates creating a variable.
	BButton *button = new BButton(BRect(10,10,11,11),"button","Click Me!",
									new BMessage(M_BUTTON_CLICKED));
	
	// Like with last lesson's label, make the button choose how big it should be.
	button->ResizeToPreferred();
	
	// Add our button to the window
	AddChild(button);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	// The way that BMessages are identified is by the public property 'what'.
	switch (msg->what)
	{
		// If the message was the one sent to the window by the button
		case M_BUTTON_CLICKED:
		{
			fCount++;
			
			BString labelString("Clicks: ");
			
			// This adds the value of fCount to the end of labelString. More on this later.
			labelString << fCount;
			
			// Set the window's title to the new string we've made
			SetTitle(labelString.String());
			break;
		}
		default:
		{
			// If the message doesn't match one of the ones we explicitly define, it must
			// be some sort of system message, so we will call the version of MessageReceived()
			// created for BWindow so that it can handle them. THIS IS REQUIRED if you want
			// your window to act the way that you expect it to.
			BWindow::MessageReceived(msg);
			break;
		}
	}
}
