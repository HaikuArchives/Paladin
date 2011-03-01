#include "PRadioButton.h"

#include "PArgs.h"

#include <Application.h>
#include <stdio.h>

class PRadioButtonBackend : public BRadioButton
{
public:
				PRadioButtonBackend(PObject *owner);
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

PRadioButton::PRadioButton(void)
	:	PControl()
{
	fType = "PRadioButton";
	fFriendlyType = "Radio Button";
	AddInterface("PRadioButton");
	
	InitBackend();
}


PRadioButton::PRadioButton(BMessage *msg)
	:	PControl(msg)
{
	fType = "PRadioButton";
	fFriendlyType = "Radio Button";
	AddInterface("PRadioButton");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BRadioButton::Instantiate(&viewmsg);
	
	InitBackend();
}


PRadioButton::PRadioButton(const char *name)
	:	PControl(name)
{
	fType = "PRadioButton";
	fFriendlyType = "Radio Button";
	AddInterface("PRadioButton");
	InitBackend();
}


PRadioButton::PRadioButton(const PRadioButton &from)
	:	PControl(from)
{
	fType = "PRadioButton";
	fFriendlyType = "Radio Button";
	AddInterface("PRadioButton");
	InitBackend();
}


PRadioButton::~PRadioButton(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PRadioButton::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PRadioButton"))
		return new PRadioButton(data);

	return NULL;
}


PObject *
PRadioButton::Create(void)
{
	return new PRadioButton();
}


PObject *
PRadioButton::Duplicate(void) const
{
	return new PRadioButton(*this);
}

void
PRadioButton::InitBackend(void)
{
	if (!fView)
		fView = new PRadioButtonBackend(this);
	StringValue sv("A one-from-many decisionmaking control.");
	SetProperty("Description",&sv);
}


PRadioButtonBackend::PRadioButtonBackend(PObject *owner)
	:	BRadioButton(BRect(0,0,1,1),"","",new BMessage),
		fOwner(owner)
{
	
}


void
PRadioButtonBackend::AttachedToWindow(void)
{
	fOwner->SetColorProperty("BackColor",Parent()->ViewColor());
	
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::AttachedToWindow();
}


void
PRadioButtonBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::AllAttached();
}


void
PRadioButtonBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::DetachedFromWindow();
}


void
PRadioButtonBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::AllDetached();
}


void
PRadioButtonBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MakeFocus(value);
}


void
PRadioButtonBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::FrameMoved(pt);
}


void
PRadioButtonBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::FrameResized(w, h);
}


void
PRadioButtonBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::KeyDown(bytes, count);
}


void
PRadioButtonBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::KeyUp(bytes, count);
}


void
PRadioButtonBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MouseDown(pt);
}


void
PRadioButtonBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MouseUp(pt);
}


void
PRadioButtonBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MouseMoved(pt, transit, msg);
}


void
PRadioButtonBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::WindowActivated(active);
}


void
PRadioButtonBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BRadioButton::Draw(update);
	
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("Draw", in.ListRef(), out.ListRef());
	
	if (IsFocus())
	{
		SetPenSize(5.0);
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PRadioButtonBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::DrawAfterChildren(update);
}


status_t
PRadioButtonBackend::Invoke(BMessage *msg)
{
	PArgs in, out;
	in.AddPointer("message", msg);
	EventData *data = fOwner->FindEvent("Invoke");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::Invoke(msg);
	
	return B_OK;
}


void
PRadioButtonBackend::MessageReceived(BMessage *msg)
{
	PRadioButton *view = dynamic_cast<PRadioButton*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BRadioButton::MessageReceived(msg);
}

