#include "PButton.h"

#include <Application.h>
#include <stdio.h>

#include "MsgDefs.h"

class PButtonBackend : public BButton
{
public:
			PButtonBackend(PObject *owner);
	void	MakeFocus(bool value);
	void	MouseUp(BPoint pt);

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
PButtonBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BButton::MakeFocus(value);
}


void
PButtonBackend::MouseUp(BPoint pt)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BButton::MouseUp(pt);
}

