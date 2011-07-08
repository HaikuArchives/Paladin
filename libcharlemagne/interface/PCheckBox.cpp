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
	void		MouseMoved(BPoint pt, uint32 transit, const BMessage *msg);
	
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
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BCheckBox::Instantiate(&viewmsg);
	
	InitBackend();
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
PCheckBox::InitBackend(void)
{
	if (!fView)
		fView = new PCheckBoxBackend(this);
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
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PCheckBoxBackend::AttachedToWindow();
}


void
PCheckBoxBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PCheckBoxBackend::AllAttached();
}


void
PCheckBoxBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PCheckBoxBackend::DetachedFromWindow();
}


void
PCheckBoxBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PCheckBoxBackend::AllDetached();
}


void
PCheckBoxBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::MakeFocus(value);
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
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::MouseDown(pt);
}


void
PCheckBoxBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::MouseUp(pt);
}


void
PCheckBoxBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::MouseMoved(pt, transit, msg);
}


void
PCheckBoxBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::WindowActivated(active);
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
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BCheckBox::DrawAfterChildren(update);
}


status_t
PCheckBoxBackend::Invoke(BMessage *msg)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Invoke");
	if (data->hook)
		return fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		return BCheckBox::Invoke(msg);
	
	// Just to hush the compiler
	return B_OK;
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
