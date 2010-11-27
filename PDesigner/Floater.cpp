#include "Floater.h"
#include <TranslationUtils.h>

Floater::Floater(void)
	:	fAction(FLOATER_NONE)
{
}


Floater::Floater(const int32 &action)
	:	fAction(action)
{
}


Floater::~Floater(void)
{
}


void
Floater::FloaterAttached(BView *handler)
{
}


void
Floater::FloaterDetached(BView *handler)
{
}


void
Floater::UpdateFloaterPosition(const BRect &rect, const int32 &action)
{
	/*
		Action List:
		
		FLOATER_RESIZE: sent when the target view is resized
		FLOATER_MOVE: sent when the target view is moved
	*/
}


void
Floater::SetAction(const int32 &action)
{
	fAction = action;
}


int32
Floater::GetAction(void) const
{
	return fAction;
}


#pragma mark HandleFloater -


HandleFloater::HandleFloater(const char *bitmapName, const int32 &action)
	:	BWindow(BRect(0,0,31,31), "", B_NO_BORDER_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
				B_AVOID_FOCUS),
		Floater(action)
{
	fView = new HandleView(Bounds(), action);
	AddChild(fView);
	
	if (bitmapName)
	{
		BBitmap *bitmap = BTranslationUtils::GetBitmap(bitmapName);
		SetBitmap(bitmap);
	}
}


HandleFloater::~HandleFloater(void)
{
}


void
HandleFloater::SetBitmap(BBitmap *bitmap)
{
	Lock();
	
	delete fView->fBitmap;
	fView->fBitmap = bitmap;
	
	if (bitmap)
		ResizeTo(bitmap->Bounds().Width(), bitmap->Bounds().Height());
	
	Unlock();
}


BBitmap *
HandleFloater::GetBitmap(void)
{
	return fView->fBitmap;
}


void
HandleFloater::FloaterAttached(BView *view)
{
	if (view)
	{
		fView->fTarget = BMessenger(view);
		BRect rect = view->ConvertToScreen(view->Bounds());
		if (GetAction() == FLOATER_MOVE)
			MoveTo(rect.right + 5.0, rect.top);
		else if (GetAction() == FLOATER_RESIZE)
			MoveTo(rect.right - 8.0, rect.bottom - 8.0);
	}
	else
		fView->fTarget = BMessenger();
	Show();
}


void
HandleFloater::FloaterDetached(BView *view)
{
	fView->fTarget = BMessenger();
	Hide();
}


void
HandleFloater::UpdateFloaterPosition(const BRect &rect, const int32 &action)
{
	switch (action)
	{
		case FLOATER_RESIZE:
		{
			if (GetAction() == FLOATER_MOVE)
				MoveTo(rect.right + 5.0, rect.top);
			break;
		}
		case FLOATER_MOVE:
		{
			if (GetAction() == FLOATER_RESIZE)
				MoveTo(rect.right - 8.0, rect.bottom - 8.0);
			break;
		}
		default:
			break;
	}
}


#pragma mark HandleView -


HandleView::HandleView(BRect frame, const int32 &action)
	:	BView(frame, "handleview", B_FOLLOW_ALL, B_WILL_DRAW),
		fBitmap(NULL),
		fAction(action)
{
}


HandleView::~HandleView(void)
{
	delete fBitmap;
}


void
HandleView::Draw(BRect rect)
{
	if (fBitmap)
		DrawBitmap(fBitmap, BPoint(0,0));
	StrokeRect(Bounds());
}


void
HandleView::MouseDown(BPoint pt)
{
	BRect bounds = Bounds();
	uint32 buttons;
	
	BPoint point(ConvertToScreen(pt)), lastPoint(ConvertToScreen(pt));
	
	do
	{
		Window()->UpdateIfNeeded();
		snooze(40000);
		
		GetMouse(&point, &buttons, true);
		ConvertToScreen(&point);
		
		float dx = point.x - lastPoint.x;
		float dy = point.y - lastPoint.y;
		
		Window()->MoveBy(dx, dy);
		lastPoint = point;
		
		BMessage msg(M_FLOATER_ACTION);
		msg.AddInt32("action", fAction);
		msg.AddFloat("dx", dx);
		msg.AddFloat("dy", dy);
		fTarget.SendMessage(&msg);
		
	} while (buttons);
}

