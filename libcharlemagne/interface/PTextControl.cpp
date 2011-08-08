#include "PTextControl.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

int32_t PTextControlSetPreferredDivider(void *pobject, void *in, void *out, void *ptr = NULL);

class PTextControlBackend : public AutoTextControl
{
public:
			PTextControlBackend(PObject *owner);

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


PTextControl::PTextControl(void)
	:	PControl()

{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	InitBackend();
	InitMethods();
}


PTextControl::PTextControl(BMessage *msg)
	:	PControl(msg)

{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PTextControlBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PTextControl::PTextControl(const char *name)
	:	PControl(name)

{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	InitMethods();
	InitBackend();
}


PTextControl::PTextControl(const PTextControl &from)
	:	PControl(from)

{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	InitMethods();
	InitBackend();
}


PTextControl::~PTextControl(void)
{
}


BArchivable *
PTextControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTextControl"))
		return new PTextControl(data);

	return NULL;
}


PObject *
PTextControl::Create(void)
{
	return new PTextControl();
}


PObject *
PTextControl::Duplicate(void) const
{
	return new PTextControl(*this);
}


status_t
PTextControl::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	AutoTextControl *backend = (AutoTextControl*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("TextAlignment") == 0)
	{
		alignment label, text;
		backend->GetAlignment(&label, &text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(backend->Text());
	else if (str.ICompare("Divider") == 0)
		((FloatProperty*)prop)->SetValue(backend->Divider());
	else if (str.ICompare("LabelAlignment") == 0)
	{
		alignment label, text;
		backend->GetAlignment(&label, &text);
		((IntProperty*)prop)->SetValue(text);
	}
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
PTextControl::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	AutoTextControl *backend = (AutoTextControl*)fView;
	
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

	if (str.ICompare("TextAlignment") == 0)
	{
		prop->GetValue(&intval);
		
		alignment label,text;
		backend->GetAlignment(&label,&text);
		text = (alignment)*intval.value;
		backend->SetAlignment(label,text);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetText(*stringval.value);
	}
	else if (str.ICompare("Divider") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetDivider(*floatval.value);
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		prop->GetValue(&intval);
		
		alignment label, text;
		backend->GetAlignment(&label, &text);
		label = (alignment)*intval.value;
		backend->SetAlignment(label, text);
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
PTextControl::InitProperties(void)
{
	SetStringProperty("Description", "A single-line text editing control");


	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("TextAlignment");
	prop->SetValue((int32)B_ALIGN_LEFT);
	prop->AddValuePair("Left", B_ALIGN_LEFT);
	prop->AddValuePair("Center", B_ALIGN_CENTER);
	prop->AddValuePair("Right", B_ALIGN_RIGHT);
	AddProperty(prop);

	AddProperty(new StringProperty("Text", NULL, " The control's text"));
	AddProperty(new FloatProperty("Divider", 0.0));
	prop = new EnumProperty();
	prop->SetName("LabelAlignment");
	prop->SetValue((int32)B_ALIGN_LEFT);
	prop->AddValuePair("Left", B_ALIGN_LEFT);
	prop->AddValuePair("Center", B_ALIGN_CENTER);
	prop->AddValuePair("Right", B_ALIGN_RIGHT);
	AddProperty(prop);

}


void
PTextControl::InitBackend(void)
{
	if (!fView)
		fView = new PTextControlBackend(this);
	StringValue sv("A single-line text editing control");
	SetProperty("Description", &sv);
}


void
PTextControl::InitMethods(void)
{
	PMethodInterface pmi;
	
	AddMethod(new PMethod("SetPreferredDivider", PTextControlSetPreferredDivider, &pmi));
	pmi.MakeEmpty();

}


int32_t
PTextControlSetPreferredDivider(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PTextControl *pcontrol = static_cast<PTextControl*>(pobject);
	PObject *object = static_cast<PObject*>(pobject);
	
	if (!object->UsesInterface("PTextControl") || !pcontrol)
		return B_BAD_TYPE;
	
	BTextControl *backend = dynamic_cast<BTextControl*>(pcontrol->GetView());
	if (!backend)
		return B_BAD_TYPE;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	if (strlen(backend->Label()) > 0)
		backend->SetDivider(backend->StringWidth(backend->Label()));
	else
		backend->SetDivider(0.0);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


PTextControlBackend::PTextControlBackend(PObject *owner)
	:	AutoTextControl(BRect(0, 0, 1, 1), "", "", "",new BMessage()),
		fOwner(owner)
{
}


void
PTextControlBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::FrameMoved(param1);
}


void
PTextControlBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::FrameResized(param1, param2);
}


void
PTextControlBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::KeyUp(bytes, count);
}


void
PTextControlBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::MouseMoved(param1, param2, param3);
}


void
PTextControlBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::Draw(param1);
}


void
PTextControlBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::AttachedToWindow();
}


void
PTextControlBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::Pulse();
}


void
PTextControlBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::MouseDown(param1);
}


void
PTextControlBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::AllAttached();
}


void
PTextControlBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::MakeFocus(param1);
}


void
PTextControlBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::AllDetached();
}


void
PTextControlBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::MouseUp(param1);
}


void
PTextControlBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::WindowActivated(param1);
}


void
PTextControlBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::DrawAfterChildren(param1);
}


void
PTextControlBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::DetachedFromWindow();
}


void
PTextControlBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		AutoTextControl::KeyDown(bytes, count);
}


