#include <Application.h>
#include <Window.h>
#include <TextView.h>
#include <Screen.h>
#include <String.h>
#include <map>

#include "DToolTip.h"

static DToolTip sToolTipInstance;

enum
{
	M_SET_TIP = 'sttp',
	M_SET_ENABLED = 'sten',
	M_SET_DISABLED = 'stds'
};

class DTipWatcherView : public BView
{
public:
				DTipWatcherView(void);
	void		AttachedToWindow(void);
	void		Pulse(void);
	void		SetTip(BView *view, BString text);
	void		SetEnabled(bool value);

	void		SetTipDelay(bigtime_t delay) { fDelayMax = delay; }
	bigtime_t	GetTipDelay(void) { return fDelayMax; }
	
private:
	typedef map<BView *,BString> TipDataMap;

	BView *		ViewForPoint(BPoint where);
	void		ShowTip(BPoint pt, const char *text);
	void		HideTip(void);
	BPoint		PointForTip(BPoint screenpt);
	BString		GetTip(BView *view);
	
	bool		fEnabled;
	TipDataMap	fDataMap;
	time_t		fDelayCounter;
	time_t		fDelayMax;
	BPoint		fLastPoint;
	
	BTextView	*fTipView;
};

class DTipWindow : public BWindow
{
public:
				DTipWindow(void);
	void		MessageReceived(BMessage *msg);
	void		SetTipDelay(bigtime_t delay);
	bigtime_t	GetTipDelay(void);
	
private:
	DTipWatcherView	*fView;
};


DToolTip::DToolTip(void)
	:	fTipWindow(NULL)
{
}


DToolTip::~DToolTip()
{
	if (fTipWindow)
		fTipWindow->PostMessage(B_QUIT_REQUESTED);
}


void
DToolTip::MakeValid(void)
{
	if (!fTipWindow)
	{
		if (be_app_messenger.IsValid())
		{
			fTipWindow = new DTipWindow();
			fTipWindow->Hide();
			fTipWindow->Show();
		}
	}
}


void
DToolTip::SetTip(BView *view, const char *text)
{
	if (!view)
		return;
	
	MakeValid();
	
	BMessage msg(M_SET_TIP);
	msg.AddPointer("view",view);
	if (text)
		msg.AddString("text",text);
	fTipWindow->PostMessage(&msg);
}


void
DToolTip::EnableTips(bool value)
{
	MakeValid();
	
	fTipWindow->PostMessage(value ? M_SET_ENABLED : M_SET_DISABLED);
}


DToolTip *
DToolTip::GetInstance(void)
{
	return &sToolTipInstance;
}


void
DToolTip::SetTipDelay(bigtime_t delay)
{
	fTipWindow->SetTipDelay(delay);
}


bigtime_t
DToolTip::GetTipDelay(void)
{
	return fTipWindow->GetTipDelay();
}


DTipWindow::DTipWindow(void)
	:	BWindow(BRect(0,0,1,1),"DToolTipWindow",B_BORDERED_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
	                B_NOT_MOVABLE | B_AVOID_FOCUS)
{
	fView = new DTipWatcherView();
	AddChild(fView);
	SetPulseRate(100000);
}


void
DTipWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_SET_TIP:
		{
			BView *view;
			BString text;
			if (msg->FindPointer("view",(void**)&view) != B_OK)
				break;
			
			if (msg->FindString("text",&text) != B_OK)
				text = "";
			
			fView->SetTip(view,text);
			break;
		}
		case M_SET_ENABLED:
		{
			fView->SetEnabled(true);
			break;
		}
		case M_SET_DISABLED:
		{
			fView->SetEnabled(false);
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
DTipWindow::SetTipDelay(bigtime_t delay)
{
	if (delay < PulseRate())
		return;
	
	Lock();
	fView->SetTipDelay(delay);
	Unlock();
}


bigtime_t
DTipWindow::GetTipDelay(void)
{
	bigtime_t temp;
	Lock();
	temp = fView->GetTipDelay();
	Unlock();
	return temp;
}


DTipWatcherView::DTipWatcherView(void)
	:	BView(BRect(0,0,1,1),"DTipWatcherView",B_FOLLOW_ALL,B_PULSE_NEEDED),
		fEnabled(true),
		fDelayCounter(0),
		fDelayMax(750000)
{
	fTipView = new BTextView(Bounds(),"",Bounds().InsetByCopy(2,2),B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	fTipView->MakeEditable(false);
	fTipView->MakeSelectable(false);
	fTipView->SetLowColor(240,240,100);
	fTipView->SetViewColor(240,240,100);
	fTipView->SetHighColor(0,0,0);
	AddChild(fTipView);
}


void
DTipWatcherView::AttachedToWindow(void)
{
	BRect r(Window()->Bounds());
	ResizeTo(r.Width(),r.Height());
}


void
DTipWatcherView::Pulse(void)
{
	// Check to see if there has been any activity. If so, just reset.
	BPoint pt;
	uint32 buttons;
	GetMouse(&pt,&buttons);
	ConvertToScreen(&pt);
	
	if (pt != fLastPoint || buttons)
	{
		fDelayCounter = 0;
		fLastPoint = pt;
		HideTip();
		return;
	}
	fLastPoint = pt;
	
	if (!Window()->IsHidden())
		return;
	
	fDelayCounter += Window()->PulseRate();
	if (fDelayCounter < fDelayMax)
		return;
	
	// We got this far, so it's time to show a tip
	BView *view = ViewForPoint(pt);
	if (!view)
		return;
	
	BString text = GetTip(view);
	if (text.CountChars() < 1)
		return;
	
	ShowTip(pt, text.String());
}


void
DTipWatcherView::SetTip(BView *view, BString text)
{
	if (!view)
		return;
	
	fDataMap[view] = text;
}


void
DTipWatcherView::SetEnabled(bool value)
{
	fEnabled = value;
}


BView *
DTipWatcherView::ViewForPoint(BPoint pt)
{
	int32 winIndex = 0;
	
	BView *view = NULL;
	BWindow *win = be_app->WindowAt(winIndex++);
	while (win)
	{
		// Took a hint from Marco Nelissen's BubbleHelper -- the app may have a window that isn't running
		// lying around
		if (!win->IsHidden() && win->LockWithTimeout(1000000) == B_OK)
		{
			BRect r = win->Frame();
			if (r.Contains(pt))
			{
				BPoint winpt(pt.x - r.left, pt.y - r.top);
				view = win->FindView(winpt);
			}
			win->Unlock();
		}
		
		if (view)
			break;
		win = be_app->WindowAt(winIndex++);
	}
	
	return view;
}


void
DTipWatcherView::ShowTip(BPoint pt, const char *text)
{
	if (!text)
		return;
	
	Window()->SetWorkspaces(B_CURRENT_WORKSPACE);
	
	fTipView->SetText(text);
	
	// Figure the size of the tip. We do have a maximum width of 200 and a maximum height of 400, but
	// we only want to use the maximum size if it's necessary.
	fTipView->ResizeTo(200,400);
	fTipView->SetTextRect(BRect(2,2,198,398));
	
	int32 lineCount = fTipView->CountLines();
	float width = 200.0;
	
	if (lineCount == 1)
		width = fTipView->LineWidth() + 4;
	
	float height = fTipView->TextHeight(0,lineCount);
	height = (height < 400) ? height : 400;
	Window()->ResizeTo(width,height);
	
	fTipView->SetTextRect(BRect(2,2, width - 4, height - 4));
	
	BPoint winpt = PointForTip(pt);
	Window()->MoveTo(winpt.x,winpt.y);
	Window()->Show();
}


void
DTipWatcherView::HideTip(void)
{
	if (!Window()->IsHidden())
		Window()->Hide();
}


BPoint
DTipWatcherView::PointForTip(BPoint pt)
{
	// This function takes the location of the cursor. From there we
	// place the tip above and to the right of the mouse. If there isn't room, place it
	// below and to the left of the mouse cursor.
	BPoint outpt(pt);
	
	BRect s(BScreen().Frame());
	BRect w(Window()->Frame());

	outpt.x += 50;
	outpt.y -= 50 - w.Height();
	
	if (outpt.x + w.Width() > s.Width())
		outpt.x = pt.x - 50 - w.Width();
	
	if (outpt.y < 0)
	{
		outpt.y = pt.y + 50;
	
		if (outpt.y + w.Height() > s.Height())
			outpt.y = s.bottom - w.Height();
	}
	
	return outpt;
}


BString
DTipWatcherView::GetTip(BView *view)
{
	BString text = fDataMap[view];
	while (view && text.CountChars() < 1)
	{
		view = view->Parent();
		text = fDataMap[view];
	}
	return text;
}


