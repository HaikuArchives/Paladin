#include "PButton.h"

#include <Application.h>
#include <stdio.h>

#include "DebugTools.h"
#include "PArgs.h"

class PButtonBackend : public BButton
{
public:
			PButtonBackend(PObject *owner);
			~PButtonBackend(void);
			
	void	AttachedToWindow(void);
	void	AllAttached(void);
	void	DetachedFromWindow(void);
	void	AllDetached(void);
	
	void	MakeFocus(bool value);
	
	void	FrameMoved(BPoint pt);
	void	FrameResized(float w, float h);
	
	void	KeyDown(const char *bytes, int32 count);
	void	KeyUp(const char *bytes, int32 count);
	
	void	MouseDown(BPoint pt);
	void	MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	void	MouseUp(BPoint pt);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};

PButton::PButton(void)
	:	PControl()
{
	STRACE(("new PButton(void)\n"), TRACE_CREATE);
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	InitMethods();
	InitBackend();
}


PButton::PButton(BMessage *msg)
	:	PControl(msg)
{
	STRACE(("new PButton(msg)\n"), TRACE_CREATE);
	
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BButton::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PButton::PButton(const char *name)
	:	PControl(name)
{
	STRACE(("new PButton(%s)\n",name), TRACE_CREATE);
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	InitMethods();
	InitBackend();
}


PButton::PButton(const PButton &from)
	:	PControl(from)
{
	STRACE(("new PButton(copy)\n"), TRACE_CREATE);
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	InitMethods();
	InitBackend();
}


PButton::~PButton(void)
{
	STRACE(("Delete PButton()\n"), TRACE_DESTROY);
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PButton::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PButton"))
		return new PButton(data);

	return NULL;
}


PObject *
PButton::Create(void)
{
	return new PButton();
}


PObject *
PButton::Duplicate(void) const
{
	return new PButton(*this);
}


void
PButton::InitMethods(void)
{
}


void
PButton::InitBackend(BView *view)
{
	if (!fView)
		fView = new PButtonBackend(this);
	StringValue sv("A basic button object. It sends a message when clicked.");
	SetProperty("Description",&sv);
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
}


PButtonBackend::PButtonBackend(PObject *owner)
	:	BButton(BRect(0,0,1,1),"", "", new BMessage),
		fOwner(owner)
{
	STRACE(("new PButtonBackend\n"), TRACE_CREATE);
}


PButtonBackend::~PButtonBackend(void)
{
	STRACE(("Delete PButtonBackend\n"), TRACE_DESTROY);
}


void
PButtonBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BButton::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PButtonBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AllAttached();
}


void
PButtonBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::DetachedFromWindow();
}


void
PButtonBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AllDetached();
}


void
PButtonBackend::MakeFocus(bool value)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MakeFocus(value);
}


void
PButtonBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::FrameMoved(pt);
}


void
PButtonBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::FrameResized(w, h);
}


void
PButtonBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::KeyDown(bytes, count);
}


void
PButtonBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::KeyUp(bytes, count);
}


void
PButtonBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");

	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseDown(pt);
}


void
PButtonBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseUp(pt);
}


void
PButtonBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseMoved(pt, buttons, msg);
}


void
PButtonBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::WindowActivated(active);
}


void
PButtonBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook == NULL)
		BButton::Draw(update);
	
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
PButtonBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::DrawAfterChildren(update);
}


void
PButtonBackend::MessageReceived(BMessage *msg)
{
	PButton *view = dynamic_cast<PButton*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BButton::MessageReceived(msg);
}
