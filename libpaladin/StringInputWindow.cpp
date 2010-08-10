#include "StringInputWindow.h"

#include <Application.h>
#include <Button.h>
#include <Path.h>
#include <Screen.h>

#define M_INVOKE 'invk'

StringInputWindow::StringInputWindow(const char *title, const char *text, BMessage msg,
									BMessenger target)
	:	DWindow(BRect(0,0,300,200),title,B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_V_RESIZABLE),
		fMessage(msg),
		fMessenger(target)
{
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	BRect r = Bounds().InsetByCopy(10,10);
	r.bottom = r.top + 10;
	BRect textRect = r.OffsetToCopy(0,0);
	textRect.InsetBy(10,10);
	fTextView = new BTextView(r,"paneltext",textRect,B_FOLLOW_LEFT | B_FOLLOW_TOP);
	top->AddChild(fTextView);
	fTextView->MakeEditable(false);
	fTextView->SetText(text);
	fTextView->ResizeTo(r.Width(), 20.0 + (fTextView->CountLines() * 
									fTextView->TextHeight(0,fTextView->TextLength())));
	fTextView->SetViewColor(top->ViewColor());
	
	fText = new BTextControl(BRect(10,10,11,11),"nametext","", "", new BMessage);
	top->AddChild(fText);
	fText->ResizeToPreferred();
	fText->ResizeTo(Bounds().Width() - 20,fText->Bounds().Height());
	fText->SetDivider(0.0);
	fText->MoveTo(10,fTextView->Frame().bottom + 10.0);
	
	r = fText->Frame();
	r.OffsetBy(0,r.Height() + 10.0);
	BButton *cancel = new BButton(r,"cancel","Cancel",
									new BMessage(B_QUIT_REQUESTED));
	cancel->ResizeToPreferred();
	top->AddChild(cancel);
	
	ResizeTo(300, cancel->Frame().bottom + 10);
	cancel->MoveTo( Bounds().Width() - (cancel->Bounds().Width() * 2) - 20,
					cancel->Frame().top);
	
	r = cancel->Frame();
	r.OffsetBy(r.Width() + 10,0);
	BButton *open = new BButton(r,"ok","OK", new BMessage(M_INVOKE));
	top->AddChild(open);
	open->MakeDefault(true);
	fText->MakeFocus(true);
	
	open->MakeDefault(true);
}


void
StringInputWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_INVOKE:
		{
			BMessage findmsg(fMessage);
			findmsg.AddString("string",fText->Text());
			fMessenger.SendMessage(&findmsg);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
			DWindow::MessageReceived(msg);
	}
}


BTextControl *
StringInputWindow::TextControl(void)
{
	return fText;
}


BTextView *
StringInputWindow::TextLabel(void)
{
	return fTextView;
}

