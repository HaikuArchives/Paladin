#include "PRadioButton.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

class PRadioButtonBackend : public BRadioButton
{
public:
			PRadioButtonBackend(PObject *owner);

	void	FrameMoved(BPoint param1);
	void	FrameResized(float param1, float param2);
	void	KeyUp(const char *bytes, int32 count);
	void	MouseMoved(BPoint param1, uint32 param2, const BMessage * param3);
	void	Draw(BRect param1);
	void	AttachedToWindow(void);
	void	Pulse(void);
	void	MouseDown(BPoint param1);
	void	AllAttached(void);
	void	MakeFocus(bool param1);
	void	AllDetached(void);
	void	MouseUp(BPoint param1);
	void	WindowActivated(bool param1);
	void	DrawAfterChildren(BRect param1);
	void	DetachedFromWindow(void);
	void	KeyDown(const char *bytes, int32 count);

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
PRadioButton::InitProperties(void)
{
	SetStringProperty("Description", "A radio button");

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
PRadioButtonBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
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
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::FrameResized(param1, param2);
}


void
PRadioButtonBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::KeyUp(bytes, count);
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
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::MouseMoved(param1, param2, param3);
}


void
PRadioButtonBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::Draw(param1);
}


void
PRadioButtonBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::AttachedToWindow();
}


void
PRadioButtonBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::Pulse();
}


void
PRadioButtonBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::MouseDown(param1);
}


void
PRadioButtonBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::AllAttached();
}


void
PRadioButtonBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::MakeFocus(param1);
}


void
PRadioButtonBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::AllDetached();
}


void
PRadioButtonBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::MouseUp(param1);
}


void
PRadioButtonBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::WindowActivated(param1);
}


void
PRadioButtonBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::DrawAfterChildren(param1);
}


void
PRadioButtonBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::DetachedFromWindow();
}


void
PRadioButtonBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BRadioButton::KeyDown(bytes, count);
}


