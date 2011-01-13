#include "PCheckBox.h"

#include "PArgs.h"

#include <Application.h>
#include <stdio.h>

class PCheckBoxBackend : public BCheckBox
{
public:
				PCheckBoxBackend(PObject *owner);
	void		AttachedToWindow(void);
	void		AllAttached(void);
	void		DetachedFromWindow(void);
	void		AllDetached(void);
	
	void		MakeFocus(bool value);
	
	void		FrameMoved(BPoint pt);
	void		FrameResized(float w, float h);
	
	void		KeyDown(const char *bytes, int32 count);
	void		KeyUp(const char *bytes, int32 count);
	
	void		MouseDown(BPoint pt);
	void		MouseUp(BPoint pt);
	void		MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	
	void		WindowActivated(bool active);
	
	void		Draw(BRect update);
	void		DrawAfterChildren(BRect update);
	status_t	Invoke(BMessage *msg = NULL);
	void		MessageReceived(BMessage *msg);
	
private:
	PObject		*fOwner;
};

PCheckBox::PCheckBox(void)
	:	PControl()
{
	fType = "PCheckBox";
	fFriendlyType = "Checkbox";
	AddInterface("PCheckBox");
	
	InitBackend();
}


PCheckBox::PCheckBox(BMessage *msg)
	:	PControl(msg)
{
	fType = "PCheckBox";
	fFriendlyType = "Checkbox";
	AddInterface("PCheckBox");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BCheckBox::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PCheckBox::PCheckBox(const char *name)
	:	PControl(name)
{
	fType = "PCheckBox";
	fFriendlyType = "Checkbox";
	AddInterface("PCheckBox");
	InitBackend();
}


PCheckBox::PCheckBox(const PCheckBox &from)
	:	PControl(from)
{
	fType = "PCheckBox";
	fFriendlyType = "Checkbox";
	AddInterface("PCheckBox");
	InitBackend();
}


PCheckBox::~PCheckBox(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PCheckBox::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PCheckBox"))
		return new PCheckBox(data);

	return NULL;
}


PObject *
PCheckBox::Create(void)
{
	return new PCheckBox();
}


PObject *
PCheckBox::Duplicate(void) const
{
	return new PCheckBox(*this);
}

void
PCheckBox::InitBackend(BView *view)
{
	fView = (view == NULL) ? new PCheckBoxBackend(this) : view;
	StringValue sv("A check box. It sends a message when its value changes.");
	SetProperty("Description",&sv);
}


PCheckBoxBackend::PCheckBoxBackend(PObject *owner)
	:	BCheckBox(BRect(0,0,1,1),"","",new BMessage),
		fOwner(owner)
{
	
}


void
PCheckBoxBackend::AttachedToWindow(void)
{
	fOwner->SetColorProperty("BackColor",Parent()->ViewColor());
	
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("active", value);
	fOwner->RunEvent("FocusChanged", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::KeyDown(bytes, count);
}


void
PCheckBoxBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::KeyUp(bytes, count);
}


void
PCheckBoxBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BCheckBox::Draw(update);
	
	if (IsFocus())
	{
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
	
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("Draw", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


status_t
PCheckBoxBackend::Invoke(BMessage *msg)
{
	EventData *data = fOwner->FindEvent("Invoke");
	if (!data->hook)
		return BCheckBox::Invoke(msg);
	
	PArgs in, out;
	return fOwner->RunEvent("Invoke", in.ListRef(), out.ListRef());
}


void
PCheckBoxBackend::MessageReceived(BMessage *msg)
{
	PCheckBox *view = dynamic_cast<PCheckBox*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BCheckBox::MessageReceived(msg);
}
