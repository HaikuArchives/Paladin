#include "GetTextWindow.h"

#include <Region.h>
#include <Screen.h>
#include <String.h>

#include "EscapeCancelFilter.h"
#include "PLocale.h"

#define M_TEXT_CHANGED 'txch'
#define M_SEND_TEXT 'sntx'

#include <stdio.h>
GetTextWindow::GetTextWindow(const char *title, const char *text,
									const BMessage &msg, const BMessenger &msgr,
									const int32 &style)
	:	DWindow(BRect(0,0,300,300), title, B_MODAL_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fMessage(msg),
		fMessenger(msgr)
{
	AddCommonFilter(new EscapeCancelFilter());
	
	BView *top = GetBackgroundView();
	
	fLabel = new BTextView(BRect(10, 10, 290, 110), "labelview", BRect(5, 5, 95, 5),
							B_FOLLOW_ALL);
	fLabel->SetText(text);
	fLabel->SetTextRect(fLabel->Bounds().InsetByCopy(5.0, 5.0));
	fLabel->ResizeTo(fLabel->Bounds().Width(), fLabel->CountLines() *
												fLabel->LineHeight());
	fLabel->MakeEditable(false);
	fLabel->SetViewColor(top->ViewColor());
	
	fText = new AutoTextControl(BRect(10,10,11,11),"argtext","", "",
									new BMessage(M_TEXT_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fText);
	fText->ResizeToPreferred();
	fText->ResizeTo(Bounds().Width() - 20, fText->Bounds().Height());
	fText->SetDivider(0);
	fText->MoveTo(10.0, fLabel->Bounds().bottom + 15.0);
	
	fOK = new BButton(BRect(0,0,1,1),"ok",TR("OK"),new BMessage(M_SEND_TEXT));
	fOK->ResizeToPreferred();
	
	fOK->MoveTo(Bounds().right - 10 - fOK->Bounds().Width(),
					fText->Frame().bottom + 10);
	
	fCancel = new BButton(BRect(0,0,1,1),"fCancel",TR("Cancel"),
						new BMessage(B_QUIT_REQUESTED));
	fCancel->ResizeToPreferred();
	
	fCancel->MoveTo(fOK->Frame().left - 10 - fCancel->Bounds().Width(),
					fText->Frame().bottom + 10);
	
	ResizeTo(Bounds().Width(),fOK->Frame().bottom + 10);
	
	top->AddChild(fLabel);
	top->AddChild(fOK);
	top->AddChild(fCancel);
	
	MakeCenteredOnShow(true);
	
	fOK->MakeDefault(true);
	fText->MakeFocus(true);
	
	if (fText->Text() && strlen(fText->Text()) > 0)
		fText->TextView()->SelectAll();
	else
		fOK->SetEnabled(false);
}


void
GetTextWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_TEXT_CHANGED:
		{
			fOK->SetEnabled((strlen(fText->Text()) > 0) ? true : false);
			break;
		}
		case M_SEND_TEXT:
		{
			BString text = fText->Text();
			
			fMessage.AddString("text",text.String());
			fMessenger.SendMessage(&fMessage);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}
