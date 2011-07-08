#include "PRadioButton.h"

#include <Application.h>
#include <RadioButton.h>
#include <stdio.h>
#include <Window.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PRadioButtonBackend : public BRadioButton
{
public:
			PRadioButtonBackend(PObject *owner);

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


PRadioButton::PRadioButton(void)
	:	PControl()
{
	fType = "PRadioButton";
	fFriendlyType = "RadioButton";
	AddInterface("PRadioButton");
	
	InitBackend();
	InitMethods();
}


PRadioButton::PRadioButton(BMessage *msg)
	:	PControl(msg)
{
	fType = "PRadioButton";
	fFriendlyType = "RadioButton";
	AddInterface("PRadioButton");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PRadioButtonBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PRadioButton::PRadioButton(const char *name)
	:	PControl(name)
{
	fType = "PRadioButton";
	fFriendlyType = "RadioButton";
	AddInterface("PRadioButton");
	
	InitMethods();
	InitBackend();
}


PRadioButton::PRadioButton(const PRadioButton &from)
	:	PControl(from)
{
	fType = "PRadioButton";
	fFriendlyType = "RadioButton";
	AddInterface("PRadioButton");
	
	InitMethods();
	InitBackend();
}


PRadioButton::~PRadioButton(void)
{
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
	StringValue sv("A radio button");
	SetProperty("Description", &sv);
}


void
PRadioButton::InitProperties(void)
{
	SetStringProperty("Description", "A radio button");

}


void
PRadioButton::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PRadioButtonBackend::PRadioButtonBackend(PObject *owner)
	:	BRadioButton(BRect(0, 0, 1, 1), "", "", new BMessage),
		fOwner(owner)
{
}


void
PRadioButtonBackend::AttachedToWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::AttachedToWindow();
}


void
PRadioButtonBackend::DetachedFromWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::DetachedFromWindow();
}


void
PRadioButtonBackend::AllAttached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::AllAttached();
}


void
PRadioButtonBackend::AllDetached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::AllDetached();
}


void
PRadioButtonBackend::Pulse()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::Pulse();
}


void
PRadioButtonBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MakeFocus(param1);
}


void
PRadioButtonBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::FrameMoved(param1);
}


void
PRadioButtonBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::FrameResized(param1, param2);
}


void
PRadioButtonBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MouseDown(param1);
}


void
PRadioButtonBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MouseUp(param1);
}


void
PRadioButtonBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::MouseMoved(param1, param2, param3);
}


void
PRadioButtonBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::WindowActivated(param1);
}


void
PRadioButtonBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::Draw(param1);
}


void
PRadioButtonBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BRadioButton::DrawAfterChildren(param1);
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


