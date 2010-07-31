#include "SCMOutputWindow.h"

#include <Application.h>
#include <ScrollView.h>
#include <stdio.h>
#include <String.h>

#define M_APPEND_TO_LOG 'matl'

SCMOutputWindow::SCMOutputWindow(const char *title)
  :	DWindow(BRect(0,0,300,300), title)
{
	SetFlags(B_NOT_CLOSABLE);
	MakeCenteredOnShow(true);
	
	BView *top = GetBackgroundView();
	
	fClose = new BButton(BRect(0,0,1,1), "close", "Close",
						new BMessage(B_QUIT_REQUESTED),
						B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	fClose->ResizeToPreferred();
	fClose->MoveTo(Bounds().right - 10.0 - fClose->Bounds().Width(),
					Bounds().bottom - 10.0 - fClose->Bounds().Height());
	
	BRect logFrame(10.0, 10.0, Bounds().right - 10.0 - B_V_SCROLL_BAR_WIDTH,
					fClose->Frame().top - 10.0);
	fLog = new BTextView(logFrame, "log",
						logFrame.OffsetToCopy(0.0, 0.0).InsetByCopy(5.0, 5.0),
						B_FOLLOW_ALL);
	BScrollView *sv = new BScrollView("scrollview", fLog, B_FOLLOW_ALL, 0,
									false, true);
	top->AddChild(sv);
	top->AddChild(fClose);
	fClose->MakeDefault(true);
	
}


void
SCMOutputWindow::FrameResized(float w, float h)
{
	BRect r = fLog->Frame();
	r.OffsetTo(0.0, 0.0);
	r.InsetBy(5.0, 5.0);
	fLog->SetTextRect(r);
}


void
SCMOutputWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_APPEND_TO_LOG:
		{
			BString text;
			if (msg->FindString("text", &text) == B_OK)
				fLog->Insert(text.String());
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}


void
SCMOutputCallback(const char *text)
{
	int32 count = be_app->CountWindows();
	
	SCMOutputWindow *win = NULL;
	for (int32 i = 0; i < count; i++)
	{
		SCMOutputWindow *temp = dynamic_cast<SCMOutputWindow*>(be_app->WindowAt(i));
		if (temp)
		{
			win = temp;
			break;
		}
	}
	
	if (!win)
		return;
	
	printf("%s",text);
	
	BMessage logmsg(M_APPEND_TO_LOG);
	logmsg.AddString("text", text);
	win->PostMessage(&logmsg);
}

