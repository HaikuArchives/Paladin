#include "Floater.h"
#include <TranslatorFormats.h>
#include <TranslationUtils.h>

#include <stdio.h>

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
Floater::FloaterAttached(PView *view)
{
}


void
Floater::FloaterDetached(void)
{
}


void
Floater::Update(PView *view, const int32 &action)
{
	/*
		Action List:
		
		FLOATER_RESIZE: sent when the target view is resized
		FLOATER_MOVE: sent when the target view is moved
		FLOATER_SET_LOCATION: sent when all floaters need new locations
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
	:	Floater(action)
{
	
	fWindow = new HandleWindow(bitmapName, action);
}


HandleFloater::~HandleFloater(void)
{
	fWindow->Lock();
	fWindow->Quit();
}


void
HandleFloater::SetBitmap(BBitmap *bitmap)
{
	fWindow->SetBitmap(bitmap);
}


BBitmap *
HandleFloater::GetBitmap(void)
{
	return fWindow->GetBitmap();
}


void
HandleFloater::FloaterAttached(PView *pview)
{
	fWindow->FloaterAttached(pview);
}


void
HandleFloater::FloaterDetached(void)
{
	fWindow->FloaterDetached();
}


void
HandleFloater::Update(PView *pview, const int32 &action)
{
	fWindow->Update(pview, action);
}


#pragma mark HandleWindow -


HandleWindow::HandleWindow(const char *bitmapName, const int32 &action)
	:	BWindow(BRect(0,0,31,31), "", B_NO_BORDER_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
				B_AVOID_FOCUS)
{
	fView = new HandleView(Bounds(), action);
	AddChild(fView);
	
	if (bitmapName)
	{
		BBitmap *bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, bitmapName);
		SetBitmap(bitmap);
	}
	Hide();
	Show();
}


HandleWindow::~HandleWindow(void)
{
}


void
HandleWindow::SetBitmap(BBitmap *bitmap)
{
	Lock();
	
	delete fView->fBitmap;
	fView->fBitmap = bitmap;
	
	if (bitmap)
		ResizeTo(bitmap->Bounds().Width(), bitmap->Bounds().Height());
	
	Unlock();
}


BBitmap *
HandleWindow::GetBitmap(void)
{
	return fView->fBitmap;
}


void
HandleWindow::FloaterAttached(PView *pview)
{
	if (pview)
	{
		BView *view = pview->GetView();
		
		fView->fTarget = BMessenger(view);
		
		if (view->Window())
			view->Window()->Lock();
		
		BRect rect = view->ConvertToScreen(view->Bounds());
		
		if (view->Window())
			view->Window()->Unlock();
		
		Lock();
		switch (fView->fAction)
		{
			case FLOATER_MOVE:
			{
				MoveTo(rect.right + 5.0, rect.top);
				break;
			}
			case FLOATER_RESIZE:
			{
				MoveTo(rect.right - 8.0, rect.bottom - 8.0);
				break;
			}
			default:
			{
				break;
			}
		}
		Unlock();
		
		if (IsHidden())
			Show();
	}
	else
		fView->fTarget = BMessenger();
}


void
HandleWindow::FloaterDetached(void)
{
	fView->fTarget = BMessenger();
	if (!IsHidden())
		Hide();
}


void
HandleWindow::Update(PView *pview, const int32 &action)
{
	BView *view = pview->GetView();
	BWindow *window = view->Window();
	
	if (!window)
		return;
	
	window->Lock();
	BRect rect = view->ConvertToScreen(view->Bounds());
	window->Unlock();
	
	switch (action)
	{
		case FLOATER_INTERNAL_MOVE:
		case FLOATER_INTERNAL_RESIZE:
		case FLOATER_SET_LOCATION:
		{
			if (fView->fAction == FLOATER_MOVE)
				MoveTo(rect.right + 5.0, rect.top);
			else if (fView->fAction == FLOATER_RESIZE)
				MoveTo(rect.right - 8.0, rect.bottom - 8.0);
			break;
		}
		case FLOATER_RESIZE:
		{
			if (fView->fAction == FLOATER_MOVE)
				MoveTo(rect.right + 5.0, rect.top);
			break;
		}
		case FLOATER_MOVE:
		{
			if (fView->fAction == FLOATER_RESIZE)
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
	else
		StrokeRect(Bounds());
}


void
HandleView::MouseDown(BPoint pt)
{
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

