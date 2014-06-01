#include "ClickableStringView.h"


ClickableStringView::ClickableStringView(BRect frame, const char* name,
	const char* text, uint32 resizingMode, uint32 flags)
	:
	BStringView(frame, name, text, resizingMode, flags)
{
}


ClickableStringView::ClickableStringView(const char* name, const char* text,
	uint32 flags)
	:
	BStringView(name, text, flags)
{
}


void
ClickableStringView::MouseUp(BPoint where)
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
