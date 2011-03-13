#include "PButton.h"

#include <Application.h>
#include <Button.h>
#include <stdio.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PButtonBackend : public BButton
{
public:
			PButtonBackend(PObject *owner);

	void	AttachedToWindow();
	void	DetachedFromWindow();
	void	AllAttached();
	void	AllDetached();
	void	Pulse();
	void	MakeFocus(bool param1);
	void	FrameMoved(BPoint param1);
	void	FrameResized(float param1, float param2);
	void	MouseDown(BPoint param1);
	void	MouseUp(BPoint param1);
	void	MouseMoved(BPoint param1, uint32 param2, const BMessage * param3);
	void	WindowActivated(bool param1);
	void	Draw(BRect param1);
	void	DrawAfterChildren(BRect param1);
	void	KeyDown(const char *bytes, int32 count);
	void	KeyUp(const char *bytes, int32 count);

private:
	PObject *fOwner;
};


PButton::PButton(void)
	:	PControl()
{
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	InitBackend();
	InitMethods();
}


PButton::PButton(BMessage *msg)
	:	PControl(msg)
{
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PButtonBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PButton::PButton(const char *name)
	:	PControl(name)
{
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	InitMethods();
	InitBackend();
}


PButton::PButton(const PButton &from)
	:	PControl(from)
{
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	InitMethods();
	InitBackend();
}


PButton::~PButton(void)
{
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
PButton::InitBackend(void)
{
	if (!fView)
		fView = new PButtonBackend(this);
	
	// For buttons, seeing the value is senseless in an editor
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop, PROPERTY_HIDE_IN_EDITOR);
}


void
PButton::InitProperties(void)
{
	SetStringProperty("Description", "A button");

}


void
PButton::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PButtonBackend::PButtonBackend(PObject *owner)
	:	BButton(BRect(0, 0, 1, 1), "", "", new BMessage),
		fOwner(owner)
{
}


void
PButtonBackend::AttachedToWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AttachedToWindow();
}


void
PButtonBackend::DetachedFromWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::DetachedFromWindow();
}


void
PButtonBackend::AllAttached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AllAttached();
}


void
PButtonBackend::AllDetached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AllDetached();
}


void
PButtonBackend::Pulse()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::Pulse();
}


void
PButtonBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MakeFocus(param1);
}


void
PButtonBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::FrameMoved(param1);
}


void
PButtonBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::FrameResized(param1, param2);
}


void
PButtonBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseDown(param1);
}


void
PButtonBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseUp(param1);
}


void
PButtonBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseMoved(param1, param2, param3);
}


void
PButtonBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::WindowActivated(param1);
}


void
PButtonBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::Draw(param1);
}


void
PButtonBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::DrawAfterChildren(param1);
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


