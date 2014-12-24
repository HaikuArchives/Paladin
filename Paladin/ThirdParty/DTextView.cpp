#include "DTextView.h"

DTextView::DTextView(const BRect &frame, const char *name, const int32 &resize)
	:	BTextView(frame, name, frame, resize),
		fNotifyChanges(false),
		fLayout(false)
{
	SetFlags(Flags() | B_FRAME_EVENTS);
	UpdateTextRect();
	
	SetMessage(new BMessage(M_TEXT_CHANGED));
}

DTextView::DTextView(const char *name)
	:	BTextView(name),
		fNotifyChanges(false),
		fLayout(true)
{
	SetFlags(Flags() | B_FRAME_EVENTS);
	
	SetMessage(new BMessage(M_TEXT_CHANGED));
}
	
void
DTextView::FrameResized(float w, float h)
{
	if (!fLayout)
		UpdateTextRect();
}


void
DTextView::UpdateTextRect(void)
{
	BRect r(Bounds());
	r.InsetBy(5.0, 5.0);
	
	SetTextRect(r);
}


void
DTextView::InsertText(const char *inText, int32 inLength, int32 inOffset,
					const text_run_array *inRuns)
{
	BTextView::InsertText(inText, inLength, inOffset, inRuns);
	
	if (IsNotifyingChanges())
		Invoke();
}


void
DTextView::DeleteText(int32 fromOffset, int32 toOffset)
{
	BTextView::DeleteText(fromOffset, toOffset);
	
	if (IsNotifyingChanges())
		Invoke();
}


void
DTextView::SetChangeNotifications(bool value)
{
	fNotifyChanges = value;
}


bool
DTextView::IsNotifyingChanges(void) const
{
	return fNotifyChanges;
}


BScrollView *
DTextView::MakeScrollView(const char *name, bool horizontal, bool vertical)
{
	if (Parent())
		RemoveSelf();
	
	BScrollView* sv;
	if (!fLayout)
		sv = new BScrollView(name, this, ResizingMode(), 0, horizontal, vertical);
	else
		sv = new BScrollView(name, this, 0, horizontal, vertical);
	sv->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	return sv;
}
