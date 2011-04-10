#include "PColorControl.h"

#include <Application.h>
#include <ColorControl.h>
#include <stdio.h>
#include <Window.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PColorControlBackend : public BColorControl
{
public:
			PColorControlBackend(PObject *owner);

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


PColorControl::PColorControl(void)
	:	PControl()
{
	fType = "PColorControl";
	fFriendlyType = "ColorControl";
	AddInterface("PColorControl");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PColorControl::PColorControl(BMessage *msg)
	:	PControl(msg)
{
	fType = "PColorControl";
	fFriendlyType = "ColorControl";
	AddInterface("PColorControl");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PColorControlBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PColorControl::PColorControl(const char *name)
	:	PControl(name)
{
	fType = "PColorControl";
	fFriendlyType = "ColorControl";
	AddInterface("PColorControl");
	
	InitMethods();
	InitBackend();
}


PColorControl::PColorControl(const PColorControl &from)
	:	PControl(from)
{
	fType = "PColorControl";
	fFriendlyType = "ColorControl";
	AddInterface("PColorControl");
	
	InitMethods();
	InitBackend();
}


PColorControl::~PColorControl(void)
{
}


BArchivable *
PColorControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PColorControl"))
		return new PColorControl(data);

	return NULL;
}


PObject *
PColorControl::Create(void)
{
	return new PColorControl();
}


PObject *
PColorControl::Duplicate(void) const
{
	return new PColorControl(*this);
}
status_t
PColorControl::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BColorControl *backend = (BColorControl*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("CellSize") == 0)
		((FloatProperty*)prop)->SetValue(backend->CellSize());
	else if (str.ICompare("Layout") == 0)
		((EnumProperty*)prop)->SetValue(backend->Layout());
	else if (str.ICompare("ValueAsColor") == 0)
		((ColorProperty*)prop)->SetValue(backend->ValueAsColor());
	else
	{
		if (backend->Window())
			backend->Window()->Unlock();

		return PControl::GetProperty(name, value, index);
	}

	if (backend->Window())
		backend->Window()->Unlock();

	return prop->GetValue(value);
}


status_t
PColorControl::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BColorControl *backend = (BColorControl*)fView;
	
	BoolValue boolval;
	ColorValue colorval;
	FloatValue floatval;
	IntValue intval;
	PointValue pointval;
	RectValue rectval;
	StringValue stringval;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("CellSize") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetCellSize(*floatval.value);
	}
	else if (str.ICompare("Layout") == 0)
	{
		prop->GetValue(&intval);
		backend->SetLayout((color_control_layout)*intval.value);
	}
	else if (str.ICompare("ValueAsColor") == 0)
	{
		prop->GetValue(&colorval);
		backend->SetValue(*colorval.value);
	}
	else
	{
		if (backend->Window())
			backend->Window()->Unlock();

		return PControl::SetProperty(name, value, index);
	}

	if (backend->Window())
		backend->Window()->Unlock();

	return prop->GetValue(value);
}


void
PColorControl::InitBackend(void)
{
}


void
PColorControl::InitProperties(void)
{
	SetStringProperty("Description", "A button");

	AddProperty(new FloatProperty("CellSize", 10.0));

	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("Layout");
	prop->SetValue((int32)B_CELLS_16x16);
	prop->AddValuePair("4 x 64", B_CELLS_4x64);
	prop->AddValuePair("8 x 32", B_CELLS_8x32);
	prop->AddValuePair("16 x 16", B_CELLS_16x16);
	prop->AddValuePair("32 x 8", B_CELLS_32x8);
	prop->AddValuePair("64 x 4", B_CELLS_64x4);
	AddProperty(prop);

	AddProperty(new ColorProperty("ValueAsColor", 0,0,0));
}


void
PColorControl::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PColorControlBackend::PColorControlBackend(PObject *owner)
	:	BColorControl(BPoint(0,0), B_CELLS_16x16, 10.0, "ColorControl"),
		fOwner(owner)
{
}


void
PColorControlBackend::AttachedToWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::AttachedToWindow();
}


void
PColorControlBackend::DetachedFromWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::DetachedFromWindow();
}


void
PColorControlBackend::AllAttached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::AllAttached();
}


void
PColorControlBackend::AllDetached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::AllDetached();
}


void
PColorControlBackend::Pulse()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::Pulse();
}


void
PColorControlBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MakeFocus(param1);
}


void
PColorControlBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::FrameMoved(param1);
}


void
PColorControlBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::FrameResized(param1, param2);
}


void
PColorControlBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MouseDown(param1);
}


void
PColorControlBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MouseUp(param1);
}


void
PColorControlBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MouseMoved(param1, param2, param3);
}


void
PColorControlBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::WindowActivated(param1);
}


void
PColorControlBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::Draw(param1);
}


void
PColorControlBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::DrawAfterChildren(param1);
}


void
PColorControlBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::KeyDown(bytes, count);
}


void
PColorControlBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::KeyUp(bytes, count);
}


