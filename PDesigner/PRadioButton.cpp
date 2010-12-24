#include "PRadioButton.h"

#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"
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
	void		MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	
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
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BRadioButton::Instantiate(&viewmsg);
	
	InitBackend(view);
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
PRadioButton::InitBackend(BView *view)
{
	fView = (view == NULL) ? new PRadioButtonBackend(this) : view;
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
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("active", value);
	fOwner->RunEvent("FocusChanged", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
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
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PRadioButtonBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
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
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


status_t
PRadioButtonBackend::Invoke(BMessage *msg)
{
	fOwner->SetIntProperty("Value",Value());
	
	BMessage upmsg(M_UPDATE_PROPERTY_EDITOR);
	upmsg.AddInt64("id",fOwner->GetID());
	upmsg.AddString("name","Value");
	be_app->PostMessage(&upmsg);
	
	if (!IsFocus())
		MakeFocus(true);
	
	return BRadioButton::Invoke(msg);
}


void
PRadioButtonBackend::MessageReceived(BMessage *msg)
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
			BRadioButton::MessageReceived(msg);
			break;
		}
	}
}

