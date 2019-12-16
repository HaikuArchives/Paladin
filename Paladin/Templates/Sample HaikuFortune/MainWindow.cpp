#include "MainWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Button.h>
#include <Screen.h>
#include <ScrollView.h>
#include <View.h>

enum
{
	M_GET_ANOTHER_FORTUNE = 'gafn',
	M_ABOUT_REQUESTED = 'abrq'
};


MainWindow::MainWindow(void)
  :	BWindow(BRect(0,0,300,300), "HaikuFortune", B_DOCUMENT_WINDOW,
  			B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE),
 	fFortune(gFortunePath.String())
{
	// Create all of the controls for our window.
	BView *back = new BView(Bounds(), "background", B_FOLLOW_ALL, B_WILL_DRAW);
	back->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(back);
	
	BButton *close = new BButton(BRect(0,0,1,1), "closebutton", "Close",
								new BMessage(B_QUIT_REQUESTED), B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	close->ResizeToPreferred();
	close->MoveTo(Bounds().right - 15 - close->Frame().Width(), 
				Bounds().bottom - 15 - close->Frame().Height());
	back->AddChild(close);
	close->MakeDefault(true);
	
	BButton *next = new BButton(BRect(0,0,1,1), "nextbutton", "Get Another",
								new BMessage(M_GET_ANOTHER_FORTUNE), B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	next->ResizeToPreferred();
	next->MoveTo(close->Frame().left - 15 - next->Frame().Width(), 
				Bounds().bottom - 15 - next->Frame().Height());
	back->AddChild(next);
	
	BButton *about = new BButton(BRect(0,0,1,1), "aboutbutton", "About…",
								new BMessage(M_ABOUT_REQUESTED), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	about->ResizeToPreferred();
	about->MoveTo(next->Frame().left - 15 - about->Frame().Width(), 
				Bounds().bottom - 15 - about->Frame().Height());
	back->AddChild(about);
	
	
	BRect r(15,15,Bounds().right - B_V_SCROLL_BAR_WIDTH - 15, next->Frame().top - 15);
	fTextView = new BTextView(r, "textview", r.OffsetToCopy(0,0).InsetByCopy(10,10), B_FOLLOW_ALL,
								B_WILL_DRAW | B_PULSE_NEEDED | B_FRAME_EVENTS);
	fTextView->MakeEditable(false);
	
	// BScrollViews are a little weird. You can't create one without having created its target.
	// It also automatically calls AddChild() to attach its target to itself, so all that is
	// necessary is to instantiate it and attach it to the window. It's not even necessary (or
	// possible) to specify the size of the BScrollView because it calculates its size based
	// on that of its target.
	BScrollView *sv = new BScrollView("scrollview", fTextView, B_FOLLOW_ALL, 0, false, true);
	back->AddChild(sv);
	
	BString fortune;
	status_t status = fFortune.GetFortune(fortune);
	if (status == B_OK)
	{
		BString title;
		title.Prepend("Fortune: ");
		SetTitle(title.String());
		
		fTextView->SetText(fortune.String());
	}
	else	
	{
		fTextView->SetText("HaikuFortune had a problem getting a fortune.\n\n"
			"Please make sure that you have installed fortune files to "
			"the folder ");
		fTextView->Insert(gFortunePath.String());
	}
	
	// This line is for working around a problem in Zeta. BButton::MakeDefault doesn't
	// do anything except change the focus. The idea is to be able to press Enter to close
	// HaikuFortune and the space bar to get another fortune. Zeta doesn't let us do this, so
	// we'll leave focus on the Close button.
	if (B_BEOS_VERSION <= B_BEOS_VERSION_5)
		next->MakeFocus(true);
	
	// Calculate a good width for the window and center it
	SetSizeLimits(45 + close->Bounds().Width() + next->Bounds().Width(), 30000, 200, 30000) ;
	r = BScreen().Frame();
	MoveTo((r.Width()-Bounds().Width()) / 2.0, r.Height() / 4.0);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_GET_ANOTHER_FORTUNE:
		{
			BString fortune;
			status_t status = fFortune.GetFortune(fortune);
			if (status == B_OK)
			{
				BString title;
				fFortune.LastFilename(title);
				title.Prepend("Fortune: ");
				SetTitle(title.String());
				
				fTextView->SetText(fortune.String());
			}
			else	
			{
				fTextView->SetText("HaikuFortune had a problem getting a fortune.\n\n"
					"Please make sure that you have installed fortune files to "
					"the folder ");
				fTextView->Insert(gFortunePath.String());
			}
			break;
		}
		case M_ABOUT_REQUESTED:
		{
			// Using a BAlert for the About window is a common occurrence. They take
			// care of all of the layout, so all that needs done is write the text that
			// goes in the alert.
			BAlert *alert = new BAlert("HaikuFortune",
										"A graphical fortune program for Haiku.\n\n",
										"OK");
			alert->Go();
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
MainWindow::FrameResized(float w, float h)
{
	// The BWindow::FrameResized() method is called whenever the window is resized.
	// In this case, we change the size of the text rectangle in the text view used
	// for the fortune. We have to do this because it won't do it itself. Lazy.
	BRect textrect = fTextView->TextRect();
	
	textrect.right = textrect.left + (w - B_V_SCROLL_BAR_WIDTH - 40);
	fTextView->SetTextRect(textrect);
}

