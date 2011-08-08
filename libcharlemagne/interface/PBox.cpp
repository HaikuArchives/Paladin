#include "PBox.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

class PBoxBackend : public BBox
{
public:
			PBoxBackend(PObject *owner);

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


PBox::PBox(void)
	:	PView(true)

{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
	InitBackend();
	InitMethods();
}


PBox::PBox(BMessage *msg)
	:	PView(msg, true)

{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PBoxBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PBox::PBox(const char *name)
	:	PView(name, true)

{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
	InitMethods();
	InitBackend();
}


PBox::PBox(const PBox &from)
	:	PView(from, true)

{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
	InitMethods();
	InitBackend();
}


PBox::~PBox(void)
{
}


BArchivable *
PBox::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PBox"))
		return new PBox(data);

	return NULL;
}


PObject *
PBox::Create(void)
{
	return new PBox();
}


PObject *
PBox::Duplicate(void) const
{
	return new PBox(*this);
}


status_t
PBox::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BBox *backend = (BBox*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("BorderStyle") == 0)
		((EnumProperty*)prop)->SetValue(backend->Border());
	else
	{
		if (backend->Window())
			backend->Window()->Unlock();

		return PView::GetProperty(name, value, index);
	}

	if (backend->Window())
		backend->Window()->Unlock();

	return prop->GetValue(value);
}


status_t
PBox::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BBox *backend = (BBox*)fView;
	
	BoolValue boolval;
	CharValue charval;
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

	if (str.ICompare("BorderStyle") == 0)
	{
		prop->GetValue(&intval);
		backend->SetBorder((border_style)*intval.value);
	}
	else
	{
		if (backend->Window())
			backend->Window()->Unlock();

		return PView::SetProperty(name, value, index);
	}

	if (backend->Window())
		backend->Window()->Unlock();

	return prop->GetValue(value);
}


void
PBox::InitProperties(void)
{
	SetStringProperty("Description", "A group box");


	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("BorderStyle");
	prop->SetValue((int32)B_FANCY_BORDER);
	prop->SetDescription(" The style of the box's border");
	prop->AddValuePair("Plain", B_PLAIN_BORDER);
	prop->AddValuePair("Fancy", B_FANCY_BORDER);
	prop->AddValuePair("None", B_NO_BORDER);
	AddProperty(prop);

}


void
PBox::InitBackend(void)
{
	if (!fView)
		fView = new PBoxBackend(this);
	StringValue sv("A group box");
	SetProperty("Description", &sv);
}


void
PBox::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PBoxBackend::PBoxBackend(PObject *owner)
	:	BBox(BRect(0, 0, 1, 1)),
		fOwner(owner)
{
}


void
PBoxBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::FrameMoved(param1);
}


void
PBoxBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::FrameResized(param1, param2);
}


void
PBoxBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::KeyUp(bytes, count);
}


void
PBoxBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::MouseMoved(param1, param2, param3);
}


void
PBoxBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::Draw(param1);
}


void
PBoxBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::AttachedToWindow();
}


void
PBoxBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::Pulse();
}


void
PBoxBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::MouseDown(param1);
}


void
PBoxBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::AllAttached();
}


void
PBoxBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::MakeFocus(param1);
}


void
PBoxBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::AllDetached();
}


void
PBoxBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::MouseUp(param1);
}


void
PBoxBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::WindowActivated(param1);
}


void
PBoxBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::DrawAfterChildren(param1);
}


void
PBoxBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::DetachedFromWindow();
}


void
PBoxBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BBox::KeyDown(bytes, count);
}


