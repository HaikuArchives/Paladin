#include "ClickableStringView.h"

ClickableStringView::ClickableStringView(BRect frame, const char *name,
										const char *text,
										uint32 resize, uint32 flags)
	:	BStringView(frame,name,text,resize,flags)
{
}


void
ClickableStringView::MouseUp(BPoint pt)
{
	Invoke();
}


void
ClickableStringView::AttachedToWindow(void)
{
	BStringView::AttachedToWindow();
	SetTarget((BHandler*)Window());
}


void
ClickableStringView::DetachedFromWindow(void)
{
	BStringView::DetachedFromWindow();
	SetTarget(NULL);
}

