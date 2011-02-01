#include "PSlider.h"

#include <Application.h>
#include <stdio.h>

#include "EnumProperty.h"
#include "PArgs.h"

class PSliderBackend : public BSlider
{
public:
			PSliderBackend(PObject *owner);
	void	AttachedToWindow(void);
	void	AllAttached(void);
	void	DetachedFromWindow(void);
	void	AllDetached(void);
	
	void	MakeFocus(bool value);
	
	void	FrameMoved(BPoint pt);
	void	FrameResized(float w, float h);
	
	void	KeyDown(const char *bytes, int32 count);
	void	KeyUp(const char *bytes, int32 count);
	
	void	MouseDown(BPoint pt);
	void	MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	void	MouseUp(BPoint pt);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};

PSlider::PSlider(void)
	:	PControl()
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	
	InitBackend();
}


PSlider::PSlider(BMessage *msg)
	:	PControl(msg)
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BSlider::Instantiate(&viewmsg);
	
	InitBackend();
}


PSlider::PSlider(const char *name)
	:	PControl(name)
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	InitBackend();
}


PSlider::PSlider(const PSlider &from)
	:	PControl(from)
{
	fType = "PSlider";
	fFriendlyType = "Slider";
	AddInterface("PSlider");
	InitBackend();
}


PSlider::~PSlider(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
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
/*
	PSlider Properties:
		BarColor
		BarThickness
		FillColor
		HashMarkCount
		HashMarkLocation
		MinLimit
		MinLimitLabel
		MaxLimit
		MaxLimitLabel
		Orientation
		Position
		ThumbStyle
		UsingFillColor
*/
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BSlider *fSlider = (BSlider*)fView;
	
	if (str.ICompare("BarColor") == 0)
		((ColorProperty*)prop)->SetValue(fSlider->BarColor());
	else if (str.ICompare("BarThickness") == 0)
		((FloatProperty*)prop)->SetValue(fSlider->BarThickness());
	else if (str.ICompare("FillColor") == 0)
	{
		rgb_color fill;
		fSlider->FillColor(&fill);
		((ColorProperty*)prop)->SetValue(fill);
	}
	else if (str.ICompare("HashMarkCount") == 0)
		((IntProperty*)prop)->SetValue(fSlider->HashMarkCount());
	else if (str.ICompare("HashMarkLocation") == 0)
		((IntProperty*)prop)->SetValue(fSlider->HashMarks());
	else if (str.ICompare("MinLimit") == 0)
	{
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(min);
	}
	else if (str.ICompare("MinLimitLabel") == 0)
		((StringProperty*)prop)->SetValue(fSlider->MinLimitLabel());
	else if (str.ICompare("MaxLimit") == 0)
	{
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(max);
	}
	else if (str.ICompare("MaxLimitLabel") == 0)
		((StringProperty*)prop)->SetValue(fSlider->MaxLimitLabel());
	// Orientation
	else if (str.ICompare("Position") == 0)
		((FloatProperty*)prop)->SetValue(fSlider->Position());
	// ThumbStyle
	else if (str.ICompare("UsingFillColor") == 0)
	{
		rgb_color dummy;
		((BoolProperty*)prop)->SetValue(fSlider->FillColor(&dummy));
	}
	else
		return PObject::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PSlider::SetProperty(const char *name, PValue *value, const int32 &index)
{
/*
	PSlider Properties:
		All PView Properties
		All PControl Properties
		
		BarColor
		BarThickness
		FillColor
		HashMarkCount
		HashMarkLocation
		MinLimit
		MinLimitLabel
		MaxLimit
		MaxLimitLabel
		Orientation
		Position
		ThumbStyle
		UsingFillColor
*/
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BSlider *fSlider = (BSlider*)fView;
	
	IntValue iv;
	ColorValue cv;
	FloatValue fv;
	StringValue sv;
	BoolValue bv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (str.ICompare("BarColor") == 0)
	{
		prop->GetValue(&cv);
		fSlider->SetBarColor(*cv.value);
	}
	else if (str.ICompare("BarThickness") == 0)
	{
		prop->GetValue(&fv);
		fSlider->SetBarThickness(*fv.value);
	}
	else if (str.ICompare("FillColor") == 0)
	{
		rgb_color c;
		bool usingColor = fSlider->FillColor(&c);
		 
		prop->GetValue(&cv);
		fSlider->UseFillColor(usingColor, &c);
	}
	else if (str.ICompare("HashMarkCount") == 0)
	{
		prop->GetValue(&iv);
		fSlider->SetHashMarkCount(*iv.value);
	}
	else if (str.ICompare("HashMarkLocation") == 0)
	{
		prop->GetValue(&iv);
		fSlider->SetHashMarks((hash_mark_location)*iv.value);
	}
	else if (str.ICompare("MinLimit") == 0)
	{
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		
		prop->GetValue(&fv);
		fSlider->SetLimits(*iv.value, max);
	}
	else if (str.ICompare("MinLimitLabel") == 0)
	{
		prop->GetValue(&sv);
		fSlider->SetLimitLabels(*sv.value, fSlider->MaxLimitLabel());
	}
	else if (str.ICompare("MaxLimit") == 0)
	{
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		
		prop->GetValue(&fv);
		fSlider->SetLimits(min, *iv.value);
	}
	else if (str.ICompare("MaxLimitLabel") == 0)
	{
		prop->GetValue(&sv);
		fSlider->SetLimitLabels(fSlider->MinLimitLabel(), *sv.value);
	}
	else if (str.ICompare("Orientation") == 0)
	{
		prop->GetValue(&iv);
		fSlider->SetOrientation((orientation)*iv.value);
	}
	else if (str.ICompare("Position") == 0)
	{
		prop->GetValue(&fv);
		fSlider->SetPosition(*fv.value);
	}
	else if (str.ICompare("ThumbStyle") == 0)
	{
		prop->GetValue(&iv);
		fSlider->SetStyle((thumb_style)*iv.value);
	}
	else if (str.ICompare("UsingFillColor") == 0)
	{
		prop->GetValue(&bv);
		fSlider->UseFillColor(*bv.value);
	}
	else
		return PControl::SetProperty(name,value,index);
	
	return prop->GetValue(value);
}

	
void
PSlider::InitBackend(void)
{
	if (!fView)
		fView = new PSliderBackend(this);
}


void
PSlider::InitProperties(void)
{
/*
	PSlider Properties:
		All PView Properties
		
		BarColor
		BarThickness
		FillColor
		HashMarkCount
		HashMarkLocation
		MinLimit
		MinLimitLabel
		MaxLimit
		MaxLimitLabel
		Orientation
		Position
		ThumbStyle
		UsingFillColor
*/

	StringValue sv("A slider");
	SetProperty("Description",&sv);
	
	EnumProperty *prop = new EnumProperty();
	prop->SetName("Orientation");
	prop->AddValuePair("Vertical", B_VERTICAL);
	prop->AddValuePair("Horizontal", B_HORIZONTAL);
	prop->SetDescription("The slider's direction.");
	prop->SetValue((int32)B_HORIZONTAL);
	AddProperty(prop);
	
	prop = new EnumProperty();
	prop->SetName("ThumbStyle");
	prop->AddValuePair("Block", B_BLOCK_THUMB);
	prop->AddValuePair("Triangle", B_TRIANGLE_THUMB);
	prop->SetDescription("The slider's knob style.");
	prop->SetValue((int32)B_BLOCK_THUMB);
	AddProperty(prop);
	
	AddProperty(new ColorProperty("BarColor", 
								tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
											B_DARKEN_4_TINT),
								"The bar's color."));
	AddProperty(new FloatProperty("BarThickness", 6.0));
	AddProperty(new ColorProperty("FillColor", rgb_color(),
								"The fill color, if it is being used."));
	AddProperty(new IntProperty("HashMarkCount", 0));
	AddProperty(new IntProperty("HashMarkLocation", (int32)B_HASH_MARKS_NONE));
	AddProperty(new FloatProperty("MinLimit", 0.0));
	AddProperty(new StringProperty("MinLimitLabel", NULL));
	AddProperty(new FloatProperty("MaxLimit", 100.0));
	AddProperty(new StringProperty("MaxLimitLabel", NULL));
	AddProperty(new FloatProperty("Position", 0.0));
	AddProperty(new BoolProperty("UsingFillColor", false));

}


PSliderBackend::PSliderBackend(PObject *owner)
	:	BSlider(BRect(0,0,1,1),"", "", new BMessage, 0.0, 100.0),
		fOwner(owner)
{
}
void
PSliderBackend::AttachedToWindow(void)
{
	BSlider::AttachedToWindow();
	
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PSliderBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PSliderBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PSliderBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PSliderBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("active", value);
	fOwner->RunEvent("FocusChanged", in.ListRef(), out.ListRef());
}


void
PSliderBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PSliderBackend::FrameResized(float w, float h)
{
	BSlider::FrameResized(w, h);
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
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


void
PSliderBackend::MouseDown(BPoint pt)
{
	BSlider::MouseDown(pt);
	
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PSliderBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PSliderBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PSliderBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
}


void
PSliderBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BSlider::Draw(update);
	
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("Draw", in.ListRef(), out.ListRef());
	
	if (IsFocus())
	{
		SetPenSize(5.0);
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PSliderBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


void
PSliderBackend::MessageReceived(BMessage *msg)
{
	PSlider *view = dynamic_cast<PSlider*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BSlider::MessageReceived(msg);
}
