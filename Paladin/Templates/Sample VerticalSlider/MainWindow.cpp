#include "MainWindow.h"
#include <Slider.h>

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
	M_BUTTON_CLICKED = 'btcl',
	M_SLIDE = 'slde'
};


MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,300,300),"ClickMe",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | 
																	B_QUIT_ON_WINDOW_CLOSE),
		fCount(0)
{
	// Create a slider. It has min and max value, a name, a label, and a message sent on
	// mouse clicks. It can be vertical or horizontal, and have a block or triangle thumb
	slider = new BSlider(BRect(10,10,11,110),"slider","Slide me!",
		new BMessage(M_BUTTON_CLICKED), 0.0f, 100.0f, B_VERTICAL, B_BLOCK_THUMB, B_FOLLOW_LEFT|B_FOLLOW_TOP);
		
	// This message is sent when the slider values changes. We will handle it in
	// Messagereceived.
	slider->SetModificationMessage(new BMessage(M_SLIDE));
	
	slider->ResizeToPreferred();
	
	// Add our button to the window
	AddChild(slider);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	// The way that BMessages are identified is by the public property 'what'.
	switch (msg->what)
	{
		// If the message was the one sent to the window by the slider
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
		case M_SLIDE:
		{
			// Change the slider label to its current value. Notice how this happens as
			// you slide the slider, while the window title is changed only when you
			// release it.
			BString labelString;
			int32 val;
			msg->FindInt32("be:value", 0, &val);
			
			labelString << val;
			slider->SetLabel(labelString);
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
