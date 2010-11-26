#include "PRadioButton.h"
#include "MsgDefs.h"

#include <Application.h>
#include <stdio.h>

class PRadioButtonBackend : public BRadioButton
{
public:
				PRadioButtonBackend(PObject *owner);
	void		AttachedToWindow(void);
	void		Draw(BRect update);
	void		MakeFocus(bool value);
	void		MouseUp(BPoint pt);
	status_t	Invoke(BMessage *msg = NULL);

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
}


void
PRadioButtonBackend::Draw(BRect update)
{
	BRadioButton::Draw(update);
	if (IsFocus())
	{
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PRadioButtonBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	Invalidate();
	
	BRadioButton::MakeFocus(value);
}


void
PRadioButtonBackend::MouseUp(BPoint pt)
{
	MakeFocus(true);
	
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BRadioButton::MouseUp(pt);
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
