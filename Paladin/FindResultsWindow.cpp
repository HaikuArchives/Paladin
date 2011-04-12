#include "FindResultsWindow.h"

FindResultsWindow::FindResultsWindow(void)
	:	DWindow(BRect(100,100,600,500), "Find Results")
{
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	
}


void
FindResultsWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}

