#include "FloatingResizeHandle.h"
#include <View.h>

class HandleView : public BView
{
public:
			HandleView(const BRect &frame, BView *target);
	void	MouseDown(BPoint pt);
	void	Draw(BRect update);

private:
	BView	*fView;
};

HandleView::HandleView(const BRect &frame,BView *target)
	:	BView(frame,"",B_FOLLOW_ALL, B_WILL_DRAW),
		fView(target)
{
	SetViewColor(255,255,155);
}


void
HandleView::Draw(BRect update)
{
	SetHighColor(128,128,128);
	StrokeRect(Bounds());
}


void
HandleView::MouseDown(BPoint downpt)
{
	uint32 buttons;
	BPoint pt, oldpt;

	GetMouse(&oldpt, &buttons,false);
	ConvertToScreen(&oldpt);
	while (buttons)
	{
		snooze(40000);
		
		GetMouse(&pt, &buttons,false);
		ConvertToScreen(&pt);
		if (pt.x != oldpt.x || pt.y != oldpt.y)
		{
			if (fView)
			{
				if (fView->Window())
					fView->Window()->Lock();
				fView->ResizeBy(pt.x - oldpt.x, pt.y - oldpt.y);
				
				// This mess is to force the view in question to fully redraw itself
				// after being resized. Some controls, such as BCheckBox under Zeta, don't
				// react well to B_FULL_UPDATE_ON_RESIZE. :(
				fView->LockLooper();
				fView->Invalidate();
				fView->UnlockLooper();
				
				fView->Window()->UpdateIfNeeded();
				if (fView->Window())
					fView->Window()->Unlock();
				Window()->MoveBy(pt.x - oldpt.x, pt.y - oldpt.y);
			}
			oldpt = pt;
		}
	}
}


FloatingResizeHandle::FloatingResizeHandle(BView *target, float size)
	:	BWindow(BRect(0,0,size,size),"",B_NO_BORDER_WINDOW_LOOK,B_FLOATING_APP_WINDOW_FEEL,
					B_AVOID_FOCUS)
{
	AddChild(new HandleView(Bounds(),target));
	if (target)
	{
		BPoint pt(target->Bounds().right + 2.0, target->Bounds().bottom + 2.0);
		target->ConvertToScreen(&pt);
		MoveTo(pt);
	}
}
