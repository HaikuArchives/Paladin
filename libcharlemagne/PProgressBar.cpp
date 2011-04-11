#include "PProgressBar.h"

#include <Application.h>
#include <StatusBar.h>
#include <Window.h>
#include <stdio.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PProgressBarBackend : public BStatusBar
{
public:
			PProgressBarBackend(PObject *owner);

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


PProgressBar::PProgressBar(void)
	:	PView(true)
{
	fType = "PProgressBar";
	fFriendlyType = "Progress Bar";
	AddInterface("PProgressBar");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PProgressBar::PProgressBar(BMessage *msg)
	:	PView(msg, true)
{
	fType = "PProgressBar";
	fFriendlyType = "Progress Bar";
	AddInterface("PProgressBar");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PProgressBarBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PProgressBar::PProgressBar(const char *name)
	:	PView(name, true)
{
	fType = "PProgressBar";
	fFriendlyType = "Progress Bar";
	AddInterface("PProgressBar");
	
	InitMethods();
	InitBackend();
}


PProgressBar::PProgressBar(const PProgressBar &from)
	:	PView(from, true)
{
	fType = "PProgressBar";
	fFriendlyType = "Progress Bar";
	AddInterface("PProgressBar");
	
	InitMethods();
	InitBackend();
}


PProgressBar::~PProgressBar(void)
{
}


BArchivable *
PProgressBar::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PProgressBar"))
		return new PProgressBar(data);

	return NULL;
}


PObject *
PProgressBar::Create(void)
{
	return new PProgressBar();
}


PObject *
PProgressBar::Duplicate(void) const
{
	return new PProgressBar(*this);
}
status_t
PProgressBar::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BStatusBar *backend = (BStatusBar*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("BarColor") == 0)
		((ColorProperty*)prop)->SetValue(backend->BarColor());
	else if (str.ICompare("BarHeight") == 0)
		((FloatProperty*)prop)->SetValue(backend->BarHeight());
	else if (str.ICompare("Label") == 0)
		((StringProperty*)prop)->SetValue(backend->Label());
	else if (str.ICompare("CurrentValue") == 0)
	{
		((FloatProperty*)prop)->SetValue(backend->CurrentValue());
	}
	else if (str.ICompare("MaxValue") == 0)
		((FloatProperty*)prop)->SetValue(backend->MaxValue());
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(backend->Text());
	else if (str.ICompare("TrailingLabel") == 0)
		((StringProperty*)prop)->SetValue(backend->TrailingLabel());
	else if (str.ICompare("TrailingText") == 0)
		((StringProperty*)prop)->SetValue(backend->TrailingText());
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
PProgressBar::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BStatusBar *backend = (BStatusBar*)fView;
	
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
	else if (str.ICompare("BarHeight") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetBarHeight(*floatval.value);
	}
	else if (str.ICompare("CurrentValue") == 0)
	{
		prop->GetValue(&floatval);
		float current = backend->CurrentValue();
		backend->Update((*floatval.value) - current);
	}
	else if (str.ICompare("MaxValue") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetMaxValue(*floatval.value);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetText(*stringval.value);
	}
	else if (str.ICompare("TrailingText") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetTrailingText(*stringval.value);
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
PProgressBar::InitBackend(void)
{
	if (!fView)
		fView = new PProgressBarBackend(this);
	StringValue sv("A progress indicator");
	SetProperty("Description", &sv);
}


void
PProgressBar::InitProperties(void)
{
	SetStringProperty("Description", "A progress indicator");

	AddProperty(new ColorProperty("BarColor", 50, 150, 255, "The bar's color"));
	AddProperty(new FloatProperty("BarHeight", 10.0));
	AddProperty(new StringProperty("Label", NULL));
	AddProperty(new FloatProperty("CurrentValue", 0.0));
	AddProperty(new FloatProperty("MaxValue", 100.0));
	AddProperty(new StringProperty("Text", ""));
	AddProperty(new StringProperty("TrailingLabel", ""));
	AddProperty(new StringProperty("TrailingText", ""));
}


void
PProgressBar::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PProgressBarBackend::PProgressBarBackend(PObject *owner)
	:	BStatusBar(BRect(0,0,1,1), "ProgressBarBackend"),
		fOwner(owner)
{
}


void
PProgressBarBackend::AttachedToWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::AttachedToWindow();
}


void
PProgressBarBackend::DetachedFromWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::DetachedFromWindow();
}


void
PProgressBarBackend::AllAttached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::AllAttached();
}


void
PProgressBarBackend::AllDetached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::AllDetached();
}


void
PProgressBarBackend::Pulse()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::Pulse();
}


void
PProgressBarBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MakeFocus(param1);
}


void
PProgressBarBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::FrameMoved(param1);
}


void
PProgressBarBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::FrameResized(param1, param2);
}


void
PProgressBarBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MouseDown(param1);
}


void
PProgressBarBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MouseUp(param1);
}


void
PProgressBarBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MouseMoved(param1, param2, param3);
}


void
PProgressBarBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::WindowActivated(param1);
}


void
PProgressBarBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::Draw(param1);
}


void
PProgressBarBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::DrawAfterChildren(param1);
}


void
PProgressBarBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::KeyDown(bytes, count);
}


void
PProgressBarBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::KeyUp(bytes, count);
}


