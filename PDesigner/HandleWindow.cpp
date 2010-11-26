#include "HandleWindow.h"
#include <Bitmap.h>
#include <View.h>

class HandleView : public BView
{
public:
						HandleView(BRect frame, BMessenger &msgr, int32 what);
	virtual				~HandleView(void);
	virtual	void		Draw(BRect rect);
	
	virtual	void		MouseDown(BPoint pt);
	
			void		SetBitmap(BBitmap *bitmap);
			BBitmap *	GetBitmap(void);

			void		SetMessenger(BMessenger &msgr) { fTarget = msgr; }
			void		SendMessage(BMessage &msg) { fTarget.SendMessage(&msg); }
private:
	BBitmap		*fBitmap;
	BMessenger	fTarget;
	int32		fCommand;
};


HandleView::HandleView(BRect frame, BMessenger &msgr, int32 what)
	:	BView(frame, "handleview", B_FOLLOW_ALL, B_WILL_DRAW),
		fBitmap(NULL),
		fCommand(what)
{
	fTarget = msgr;
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
HandleView::SetBitmap(BBitmap *bitmap)
{
	delete fBitmap;
	fBitmap = bitmap;
	
	if (fBitmap && Window())
	{
		Window()->Lock();
		Window()->ResizeTo(fBitmap->Bounds().Width(), fBitmap->Bounds().Height());
		Window()->Unlock();
	}
}


BBitmap *
HandleView::GetBitmap(void)
{
	return fBitmap;
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
		
		BMessage msg(fCommand);
		msg.AddFloat("dx", dx);
		msg.AddFloat("dy", dy);
		fTarget.SendMessage(&msg);
		
	} while (buttons);
}


HandleWindow::HandleWindow(const BRect &frame, BBitmap *bitmap, BMessenger msgr, int32 what)
	:	BWindow(frame, "", B_NO_BORDER_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL,
				B_AVOID_FOCUS)
{
	HandleView *view = new HandleView(Bounds(), msgr, what);
	AddChild(view);
	
	if (bitmap)
		view->SetBitmap(bitmap);
}

