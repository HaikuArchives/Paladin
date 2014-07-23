#include "SCMOutputWindow.h"

#include <Application.h>
#include <LayoutBuilder.h>
#include <ScrollView.h>
#include <stdio.h>
#include <String.h>

#define M_APPEND_TO_LOG 'matl'

SCMOutputWindow::SCMOutputWindow(const char *title)
  :	DWindow(BRect(0,0,400,300), title)
{
	SetFlags(B_NOT_CLOSABLE);
	MakeCenteredOnShow(true);
	
	BView *top = GetBackgroundView();
	
	fClose = new BButton(BRect(0,0,1,1), "close", "Close",
						new BMessage(B_QUIT_REQUESTED));
	fLog = new BTextView("log");
	BScrollView *sv = new BScrollView("scrollview", fLog, 0,
									false, true);
	BLayoutBuilder::Grid<>(top, B_USE_HALF_ITEM_SPACING)
		/* column, row, columnSpan, rowSpan */
		.SetInsets(0)
		.Add(sv, 0, 0, 3, 1)
		.Add(fClose, 1, 1);
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


BTextView *
SCMOutputWindow::GetTextView(void)
{
	return fLog;
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
	
	BString out(text);
	int32 pos = out.FindLast("Source control command return value");
	if (pos >= 0)
		out.Truncate(pos);
	
	printf("%s",out.String());
	
	if (!win)
		return;
	
	BMessage logmsg(M_APPEND_TO_LOG);
	logmsg.AddString("text", out);
	win->PostMessage(&logmsg);
}

