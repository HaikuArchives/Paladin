#include "PSlider.h"

#include <Application.h>
#include <Slider.h>
#include <Window.h>
#include <stdio.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PSliderBackend : public BSlider
{
public:
			PSliderBackend(PObject *owner);

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


PSlider::PSlider(void)
	:	PControl()
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PSlider::PSlider(BMessage *msg)
	:	PControl(msg)
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PSliderBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PSlider::PSlider(const char *name)
	:	PControl(name)
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	
	InitMethods();
	InitBackend();
}


PSlider::PSlider(const PSlider &from)
	:	PControl(from)
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	
	InitMethods();
	InitBackend();
}


PSlider::~PSlider(void)
{
}


BArchivable *
PSlider::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PSlider"))
		return new PSlider(data);

	return NULL;
}


PObject *
PSlider::Create(void)
{
	return new PSlider();
}


PObject *
PSlider::Duplicate(void) const
{
	return new PSlider(*this);
}
status_t
PSlider::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BSlider *backend = (BSlider*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("BarColor") == 0)
		((ColorProperty*)prop)->SetValue(backend->BarColor());
	else if (str.ICompare("BarThickness") == 0)
		((FloatProperty*)prop)->SetValue(backend->BarThickness());
	else if (str.ICompare("HashMarkCount") == 0)
		((IntProperty*)prop)->SetValue(backend->HashMarkCount());
	else if (str.ICompare("HashMarkLocation") == 0)
		((IntProperty*)prop)->SetValue(backend->HashMarks());
	else if (str.ICompare("MinLimitLabel") == 0)
	{
	((StringProperty*)prop)->SetValue(backend->MinLimitLabel());
	}
	else if (str.ICompare("MaxLimitLabel") == 0)
	{
	((StringProperty*)prop)->SetValue(backend->MaxLimitLabel());
	}
	else if (str.ICompare("Position") == 0)
		((FloatProperty*)prop)->SetValue(backend->Position());
	else if (str.ICompare("ThumbStyle") == 0)
		((EnumProperty*)prop)->SetValue(backend->Style());
	else if (str.ICompare("Orientation") == 0)
		((EnumProperty*)prop)->SetValue(backend->Orientation());
	else if (str.ICompare("FillColor") == 0)
	{
		rgb_color fill;
		backend->FillColor(&fill);
		((ColorProperty*)prop)->SetValue(fill);
	}
	else if (str.ICompare("MinLimit") == 0)
	{
		int32 min, max;
		backend->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(min);
	}
	else if (str.ICompare("MaxLimit") == 0)
	{
		int32 min, max;
		backend->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(max);
	}
	else if (str.ICompare("UsingFillColor") == 0)
	{
		rgb_color dummy;
		((BoolProperty*)prop)->SetValue(backend->FillColor(&dummy));
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
PSlider::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BSlider *backend = (BSlider*)fView;
	
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

	if (str.ICompare("BarColor") == 0)
	{
		prop->GetValue(&colorval);
		backend->SetBarColor(*colorval.value);
	}
	else if (str.ICompare("BarThickness") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetBarThickness(*floatval.value);
	}
	else if (str.ICompare("HashMarkCount") == 0)
	{
		prop->GetValue(&intval);
		backend->SetHashMarkCount(*intval.value);
	}
	else if (str.ICompare("HashMarkLocation") == 0)
	{
		prop->GetValue(&intval);
		backend->SetHashMarks((hash_mark_location)*intval.value);
	}
	else if (str.ICompare("MinLimitLabel") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetLimitLabels(*stringval.value, backend->MaxLimitLabel());
	}
	else if (str.ICompare("MaxLimitLabel") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetLimitLabels(backend->MinLimitLabel(), *stringval.value);
	}
	else if (str.ICompare("Position") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetPosition(*floatval.value);
	}
	else if (str.ICompare("ThumbStyle") == 0)
	{
		prop->GetValue(&intval);
		backend->SetStyle((thumb_style)*intval.value);
	}
	else if (str.ICompare("Orientation") == 0)
	{
		prop->GetValue(&intval);
		backend->SetOrientation((orientation)*intval.value);
	}
	else if (str.ICompare("FillColor") == 0)
	{
		rgb_color c;
		bool usingColor = backend->FillColor(&c);
		 
		prop->GetValue(&colorval);
		backend->UseFillColor(usingColor, &c);
	}
	else if (str.ICompare("MinLimit") == 0)
	{
		int32 min, max;
		backend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		backend->SetLimits(*intval.value, max);
	}
	else if (str.ICompare("MaxLimit") == 0)
	{
		int32 min, max;
		backend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		backend->SetLimits(*intval.value, max);
	}
	else if (str.ICompare("UsingFillColor") == 0)
	{
		prop->GetValue(&boolval);
		backend->UseFillColor(*boolval.value);
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
PSlider::InitBackend(void)
{
	if (!fView)
		fView = new PSliderBackend(this);
	StringValue sv("A slider");
	SetProperty("Description", &sv);
}


void
PSlider::InitProperties(void)
{
	SetStringProperty("Description", "A slider");

	AddProperty(new ColorProperty("BarColor", tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_4_TINT), "The bar's color"));
	AddProperty(new FloatProperty("BarThickness", 6.0));
	AddProperty(new IntProperty("HashMarkCount", 0));
	AddProperty(new IntProperty("HashMarkLocation", (int32)B_HASH_MARKS_NONE));
	AddProperty(new StringProperty("MinLimitLabel", NULL));
	AddProperty(new StringProperty("MaxLimitLabel", NULL));
	AddProperty(new FloatProperty("Position", 0.0));

	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("ThumbStyle");
	prop->SetValue((int32)B_BLOCK_THUMB);
	prop->AddValuePair("Block", B_BLOCK_THUMB);
	prop->AddValuePair("Triangle", B_TRIANGLE_THUMB);
	AddProperty(prop);

	prop = new EnumProperty();
	prop->SetName("Orientation");
	prop->SetValue((int32)B_HORIZONTAL);
	prop->AddValuePair("Horizontal", B_HORIZONTAL);
	prop->AddValuePair("Vertical", B_VERTICAL);
	AddProperty(prop);

	AddProperty(new ColorProperty("FillColor", rgb_color(), "The fill color, if it is being used"));
	AddProperty(new FloatProperty("MinLimit", 0.0));
	AddProperty(new FloatProperty("MaxLimit", 100.0));
	AddProperty(new BoolProperty("UsingFillColor", false));
}


void
PSlider::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PSliderBackend::PSliderBackend(PObject *owner)
	:	BSlider(BRect(0, 0, 1, 1), "", "", new BMessage, 0.0, 100.0),
		fOwner(owner)
{
}


void
PSliderBackend::AttachedToWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::AttachedToWindow();
}


void
PSliderBackend::DetachedFromWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::DetachedFromWindow();
}


void
PSliderBackend::AllAttached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::AllAttached();
}


void
PSliderBackend::AllDetached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::AllDetached();
}


void
PSliderBackend::Pulse()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::Pulse();
}


void
PSliderBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::MakeFocus(param1);
}


void
PSliderBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::FrameMoved(param1);
}


void
PSliderBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::FrameResized(param1, param2);
}


void
PSliderBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::MouseDown(param1);
}


void
PSliderBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::MouseUp(param1);
}


void
PSliderBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::MouseMoved(param1, param2, param3);
}


void
PSliderBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::WindowActivated(param1);
}


void
PSliderBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::Draw(param1);
}


void
PSliderBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::DrawAfterChildren(param1);
}


void
PSliderBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::KeyDown(bytes, count);
}


void
PSliderBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BSlider::KeyUp(bytes, count);
}


