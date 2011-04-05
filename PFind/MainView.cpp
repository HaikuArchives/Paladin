#include "MainView.h"

MainView::MainView(BRect frame)
	:	BView(frame, "mainview", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}


void
MainView::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

