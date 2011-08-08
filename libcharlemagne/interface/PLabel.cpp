#include "PLabel.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

class PLabelBackend : public BStringView
{
public:
			PLabelBackend(PObject *owner);

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


PLabel::PLabel(void)
	:	PView(true)

{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitBackend();
	InitMethods();
}


PLabel::PLabel(BMessage *msg)
	:	PView(msg, true)

{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PLabelBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PLabel::PLabel(const char *name)
	:	PView(name, true)

{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitMethods();
	InitBackend();
}


PLabel::PLabel(const PLabel &from)
	:	PView(from, true)

{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitMethods();
	InitBackend();
}


PLabel::~PLabel(void)
{
}


BArchivable *
PLabel::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PLabel"))
		return new PLabel(data);

	return NULL;
}


PObject *
PLabel::Create(void)
{
	return new PLabel();
}


PObject *
PLabel::Duplicate(void) const
{
	return new PLabel(*this);
}


status_t
PLabel::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BStringView *backend = (BStringView*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("Alignment") == 0)
		((EnumProperty*)prop)->SetValue(backend->Alignment());
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(backend->Text());
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
PLabel::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BStringView *backend = (BStringView*)fView;
	
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

	if (str.ICompare("Alignment") == 0)
	{
		prop->GetValue(&intval);
		backend->SetAlignment((alignment)*intval.value);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetText(*stringval.value);
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
PLabel::InitProperties(void)
{
	SetStringProperty("Description", "A text label");


	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("Alignment");
	prop->SetValue((int32)B_ALIGN_LEFT);
	prop->AddValuePair("Left", B_ALIGN_LEFT);
	prop->AddValuePair("Center", B_ALIGN_CENTER);
	prop->AddValuePair("Right", B_ALIGN_RIGHT);
	AddProperty(prop);

	AddProperty(new StringProperty("Text", NULL, " The label's text"));
}


void
PLabel::InitBackend(void)
{
	if (!fView)
		fView = new PLabelBackend(this);
	StringValue sv("A text label");
	SetProperty("Description", &sv);
}


void
PLabel::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PLabelBackend::PLabelBackend(PObject *owner)
	:	BStringView(BRect(0,0,1,1),"", ""),
		fOwner(owner)
{
}


void
PLabelBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::FrameMoved(param1);
}


void
PLabelBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::FrameResized(param1, param2);
}


void
PLabelBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::KeyUp(bytes, count);
}


void
PLabelBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::MouseMoved(param1, param2, param3);
}


void
PLabelBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::Draw(param1);
}


void
PLabelBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::AttachedToWindow();
}


void
PLabelBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::Pulse();
}


void
PLabelBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::MouseDown(param1);
}


void
PLabelBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::AllAttached();
}


void
PLabelBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::MakeFocus(param1);
}


void
PLabelBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::AllDetached();
}


void
PLabelBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::MouseUp(param1);
}


void
PLabelBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::WindowActivated(param1);
}


void
PLabelBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::DrawAfterChildren(param1);
}


void
PLabelBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::DetachedFromWindow();
}


void
PLabelBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BStringView::KeyDown(bytes, count);
}


