#include "PCheckBox.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

class PCheckBoxBackend : public BCheckBox
{
public:
			PCheckBoxBackend(PObject *owner);

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


PCheckBox::PCheckBox(void)
	:	PControl()

{
	fType = "PCheckBox";
	fFriendlyType = "CheckBox";
	AddInterface("PCheckBox");
	
	InitBackend();
	InitMethods();
}


PCheckBox::PCheckBox(BMessage *msg)
	:	PControl(msg)

{
	fType = "PCheckBox";
	fFriendlyType = "CheckBox";
	AddInterface("PCheckBox");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PCheckBoxBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PCheckBox::PCheckBox(const char *name)
	:	PControl(name)

{
	fType = "PCheckBox";
	fFriendlyType = "CheckBox";
	AddInterface("PCheckBox");
	
	InitMethods();
	InitBackend();
}


PCheckBox::PCheckBox(const PCheckBox &from)
	:	PControl(from)

{
	fType = "PCheckBox";
	fFriendlyType = "CheckBox";
	AddInterface("PCheckBox");
	
	InitMethods();
	InitBackend();
}


PCheckBox::~PCheckBox(void)
{
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
PCheckBox::InitProperties(void)
{
	SetStringProperty("Description", "A check box");

}


void
PCheckBox::InitBackend(void)
{
	if (!fView)
		fView = new PCheckBoxBackend(this);
	StringValue sv("A check box");
	SetProperty("Description", &sv);
}


void
PCheckBox::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PCheckBoxBackend::PCheckBoxBackend(PObject *owner)
	:	BCheckBox(BRect(0, 0, 1, 1), "", "", new BMessage),
		fOwner(owner)
{
}


void
PCheckBoxBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::FrameMoved(param1);
}


void
PCheckBoxBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::FrameResized(param1, param2);
}


void
PCheckBoxBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::KeyUp(bytes, count);
}


void
PCheckBoxBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::MouseMoved(param1, param2, param3);
}


void
PCheckBoxBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::Draw(param1);
}


void
PCheckBoxBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::AttachedToWindow();
}


void
PCheckBoxBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::Pulse();
}


void
PCheckBoxBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::MouseDown(param1);
}


void
PCheckBoxBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::AllAttached();
}


void
PCheckBoxBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::MakeFocus(param1);
}


void
PCheckBoxBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::AllDetached();
}


void
PCheckBoxBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::MouseUp(param1);
}


void
PCheckBoxBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::WindowActivated(param1);
}


void
PCheckBoxBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::DrawAfterChildren(param1);
}


void
PCheckBoxBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::DetachedFromWindow();
}


void
PCheckBoxBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BCheckBox::KeyDown(bytes, count);
}


