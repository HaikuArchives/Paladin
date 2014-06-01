#include "TerminalWindow.h"

#include <Font.h>
#include <Message.h>
#include <ScrollView.h>
#include <stdio.h>

#include "PLocale.h"

#define M_RUN_COMMAND 'rncm'


TerminalWindow::TerminalWindow(const char *commandline)
	:
	DWindow(BRect(0, 0, 400, 300), TR("Terminal output")),
	fCommand(commandline)
{
	SetSizeLimits(300,30000,200,30000);
	MakeCenteredOnShow(true);
	
	BRect r(Bounds());
	r.InsetBy(10,10);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	BRect textRect(r);
	textRect.InsetBy(5,5);
	
	fTextView = new BTextView(r, "text", textRect, B_FOLLOW_ALL);
	BScrollView *scrollView = new BScrollView("scroller",fTextView,
											B_FOLLOW_ALL, 0, false,true);
	GetBackgroundView()->AddChild(scrollView);
	fTextView->SetFont(be_fixed_font);
}


TerminalWindow::~TerminalWindow(void)
{
}


void
TerminalWindow::MessageReceived(BMessage *msg)
{
	if (msg->what == M_RUN_COMMAND)
	{
		if (!IsHidden())
			Hide();
		
		FILE *fd = popen(fCommand.String(),"r");
		if (fd)
		{
			BString data;
			char buffer[1024];
			while (fgets(buffer,1024,fd))
				data += buffer;
			pclose(fd);
			
			fTextView->SetText(data.String());
		}
		
		Show();
	}
	else
		DWindow::MessageReceived(msg);
}


void
TerminalWindow::RunCommand(void)
{
	PostMessage(M_RUN_COMMAND);
}


void
TerminalWindow::FrameResized(float w, float h)
{
	BRect r = fTextView->Bounds();
	r.InsetBy(5,5);
	fTextView->SetTextRect(r);
	UpdateIfNeeded();
}

