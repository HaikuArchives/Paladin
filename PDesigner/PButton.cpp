#include "PButton.h"

#include <Application.h>
#include <stdio.h>

#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"
#include "PArgs.h"

class PButtonBackend : public BButton
{
public:
			PButtonBackend(PObject *owner);
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
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	InitBackend();
}


PButton::PButton(BMessage *msg)
	:	PControl(msg)
{
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
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	InitBackend();
}


PButton::PButton(const PButton &from)
	:	PControl(from)
{
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	InitBackend();
}


PButton::~PButton(void)
{
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
PButton::InitBackend(BView *view)
{
	fView = (view == NULL) ? new PButtonBackend(this) : view;
	StringValue sv("A basic button object. It sends a message when clicked.");
	SetProperty("Description",&sv);
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
}


PButtonBackend::PButtonBackend(PObject *owner)
	:	BButton(BRect(0,0,1,1),"", "", new BMessage),
		fOwner(owner)
{
}
void
PButtonBackend::AttachedToWindow(void)
{
	BButton::AttachedToWindow();
	
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PButtonBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PButtonBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PButtonBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PButtonBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("active", value);
	fOwner->RunEvent("FocusChanged", in.ListRef(), out.ListRef());
}


void
PButtonBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PButtonBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
}


void
PButtonBackend::KeyDown(const char *bytes, int32 count)
{
	BButton::KeyDown(bytes, count);
	
	PArgs in, out;
	in.AddString("bytes", bytes);
	in.AddInt32("count", count);
	fOwner->RunEvent("KeyDown", in.ListRef(), out.ListRef());
}


void
PButtonBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddString("bytes", bytes);
	in.AddInt32("count", count);
	fOwner->RunEvent("KeyUp", in.ListRef(), out.ListRef());
}


void
PButtonBackend::MouseDown(BPoint pt)
{
	BButton::MouseDown(pt);
	
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PButtonBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PButtonBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PButtonBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
}


void
PButtonBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook == NullPMethod)
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
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


void
PButtonBackend::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_FLOATER_ACTION:
		{
			int32 action;
			if (msg->FindInt32("action", &action) != B_OK)
				break;
			
			float dx, dy;
			msg->FindFloat("dx", &dx);
			msg->FindFloat("dy", &dy);
			
			FloaterBroker *broker = FloaterBroker::GetInstance();
			
			switch (action)
			{
				case FLOATER_MOVE:
				{
					MoveBy(dx, dy);
					broker->NotifyFloaters((PView*)fOwner, FLOATER_MOVE);
					break;
				}
				case FLOATER_RESIZE:
				{
					ResizeBy(dx, dy);
					broker->NotifyFloaters((PView*)fOwner, FLOATER_RESIZE);
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
		{
			BButton::MessageReceived(msg);
			break;
		}
	}
}
