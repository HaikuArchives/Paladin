#include "PCheckBox.h"

#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"

#include <Application.h>
#include <stdio.h>

class PCheckBoxBackend : public BCheckBox
{
public:
				PCheckBoxBackend(PObject *owner);
	void		AttachedToWindow(void);
	void		Draw(BRect update);
	void		MakeFocus(bool value);
	void		MouseUp(BPoint pt);
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
}


void
PCheckBoxBackend::Draw(BRect update)
{
	BCheckBox::Draw(update);
	if (IsFocus())
	{
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PCheckBoxBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	Invalidate();
	
	BCheckBox::MakeFocus(value);
}


void
PCheckBoxBackend::MouseUp(BPoint pt)
{
	MakeFocus(true);
	
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BCheckBox::MouseUp(pt);
}


status_t
PCheckBoxBackend::Invoke(BMessage *msg)
{
	fOwner->SetIntProperty("Value",Value());
	
	BMessage upmsg(M_UPDATE_PROPERTY_EDITOR);
	upmsg.AddInt64("id",fOwner->GetID());
	upmsg.AddString("name","Value");
	be_app->PostMessage(&upmsg);
	
	if (!IsFocus())
		MakeFocus(true);
	
	return BCheckBox::Invoke(msg);
}


void
PCheckBoxBackend::MessageReceived(BMessage *msg)
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
			BCheckBox::MessageReceived(msg);
			break;
		}
	}
}
