#include "GetTextWindow.h"

#include <Catalog.h>
#include <Font.h>
#include <Locale.h>
#include <Region.h>
#include <Screen.h>
#include <ScrollView.h>
#include <String.h>

#include "EscapeCancelFilter.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "GetTextWindow"

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
	AddShortcut(B_ENTER, B_COMMAND_KEY, new BMessage(M_SEND_TEXT));
	
	BView *top = GetBackgroundView();
	
	BRect r(10, 10, 360, 210);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	fText = new BTextView(r, "labelview", r.OffsetToCopy(0,0).InsetByCopy(5,5),
							B_FOLLOW_ALL);
	fText->SetText(text);
	fText->SetTextRect(fText->Bounds().InsetByCopy(5.0, 5.0));
	
	BScrollView *sv = new BScrollView("scroll", fText, B_FOLLOW_ALL, 0,
										false, true);
	
	fOK = new BButton(BRect(0,0,1,1),"ok",B_TRANSLATE("OK"),new BMessage(M_SEND_TEXT));
	fOK->ResizeToPreferred();
	
	fOK->MoveTo(sv->Frame().right - fOK->Bounds().Width(),
					sv->Frame().bottom + 10);
	
	fCancel = new BButton(BRect(0,0,1,1),"fCancel",B_TRANSLATE("Cancel"),
						new BMessage(B_QUIT_REQUESTED));
	fCancel->ResizeToPreferred();
	
	fCancel->MoveTo(fOK->Frame().left - 10 - fCancel->Bounds().Width(),
					fOK->Frame().top);
	
	ResizeTo(sv->Frame().right + 10, fOK->Frame().bottom + 10);
	
	top->AddChild(sv);
	top->AddChild(fOK);
	top->AddChild(fCancel);
	
	MakeCenteredOnShow(true);
	
	fText->MakeFocus(true);
	
	if (fText->Text() && strlen(fText->Text()) > 0)
		fText->SelectAll();
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


BTextView *
GetTextWindow::GetTextView(void)
{
	return fText;
}

