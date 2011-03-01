#include "PColorControl.h"
#include <Application.h>
#include <Window.h>
#include <ColorControl.h>

#include "CInterface.h"
#include "EnumProperty.h"
#include "PArgs.h"


class PColorControlBackend : public BColorControl
{
public:
			PColorControlBackend(PObject *owner);
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
	void	MouseMoved(BPoint pt, uint32 transit, const BMessage *msg);
	void	MouseUp(BPoint pt);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};


PColorControl::PColorControl(void)
	:	PControl()
{
	fType = "PColorControl";
	fFriendlyType = "Color Control";
	AddInterface("PColorControl");
	
	InitProperties();
	InitMethods();
	InitBackend();
}


PColorControl::PColorControl(BMessage *msg)
	:	PControl(msg)
{
	fType = "PColorControl";
	fFriendlyType = "Color Control";
	AddInterface("PColorControl");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BView::Instantiate(&viewmsg);
	
	InitBackend();
}


PColorControl::PColorControl(const char *name)
	:	PControl(name)
{
	fType = "PColorControl";
	fFriendlyType = "Color Control";
	AddInterface("PColorControl");
	InitMethods();
	InitBackend();
}


PColorControl::PColorControl(const PColorControl &from)
	:	PControl(from)
{
	fType = "PColorControl";
	fFriendlyType = "Color Control";
	AddInterface("PColorControl");
	InitMethods();
	InitBackend();
}


PColorControl::~PColorControl(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PColorControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PColorControl"))
		return new PColorControl(data);

	return NULL;
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
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BColorControl *view = dynamic_cast<BColorControl*>(fView);
		
	if (str.ICompare("CellSize") == 0)
		((FloatProperty*)prop)->SetValue(view->CellSize());
	else if (str.ICompare("Layout") == 0)
		((EnumProperty*)prop)->SetValue(view->Layout());
	else if (str.ICompare("ValueAsColor") == 0)
		((ColorProperty*)prop)->SetValue(view->ValueAsColor());
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PControl::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
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
	
	BoolValue bv;
	ColorValue cv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BColorControl *view = dynamic_cast<BColorControl*>(fView);
	
	if (str.ICompare("CellSize") == 0)
	{
		prop->GetValue(&fv);
		view->SetCellSize(*fv.value);
	}
	else if (str.ICompare("Layout") == 0)
	{
		prop->GetValue(&iv);
		view->SetLayout((color_control_layout)(*iv.value));
	}
	else if (str.ICompare("ValueAsColor") == 0)
	{
		prop->GetValue(&cv);
		view->SetValue(*cv.value);
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PControl::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
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

void
PColorControl::InitBackend(void)
{
	if (!fView)
		fView = new PColorControlBackend(this);
	StringValue sv("A color picker control");
	SetProperty("Description",&sv);
}


void
PColorControl::InitProperties(void)
{
	// Add properties here
	AddProperty(new FloatProperty("CellSize", 10.0));
	
	EnumProperty *prop = new EnumProperty();
	prop->SetName("Layout");
	prop->AddValuePair("B_CELLS_4x64", B_CELLS_4x64);
	prop->AddValuePair("B_CELLS_8x32", B_CELLS_8x32);
	prop->AddValuePair("B_CELLS_16x16", B_CELLS_16x16);
	prop->AddValuePair("B_CELLS_32x8", B_CELLS_32x8);
	prop->AddValuePair("B_CELLS_64x4", B_CELLS_64x4);
	AddProperty(prop);
	
	AddProperty(new ColorProperty("ValueAsColor", 0,0,0));
}


void
PColorControl::InitMethods(void)
{
	// Add methods and non-PView events here
}


PColorControlBackend::PColorControlBackend(PObject *owner)
	:	BColorControl(BPoint(0,0), B_CELLS_16x16, 10.0, "ColorControl"),
		fOwner(owner)
{
}


void
PColorControlBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BColorControl::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PColorControlBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::AllAttached();
}


void
PColorControlBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::DetachedFromWindow();
}


void
PColorControlBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::AllDetached();
}


void
PColorControlBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MakeFocus(value);
}


void
PColorControlBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::FrameMoved(pt);
}


void
PColorControlBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::FrameResized(w, h);
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


void
PColorControlBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MouseDown(pt);
}


void
PColorControlBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MouseUp(pt);
}


void
PColorControlBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::MouseMoved(pt, transit, msg);
}


void
PColorControlBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::WindowActivated(active);
}


void
PColorControlBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BColorControl::Draw(update);
	
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
PColorControlBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BColorControl::DrawAfterChildren(update);
}


void
PColorControlBackend::MessageReceived(BMessage *msg)
{
	PColorControl *view = dynamic_cast<PColorControl*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BColorControl::MessageReceived(msg);
}


int32_t
PColorControlAttachedToWindow(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BColorControl *fView = (BColorControl*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	fView->BColorControl::AttachedToWindow();
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PColorControlDraw(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BColorControl *fView = (BColorControl*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgs args(in);
	BRect r;
	args.FindRect("update", &r);
	
	fView->BColorControl::Draw(r);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PColorControlFrameResized(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BColorControl *fView = (BColorControl*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w,h;
	find_parg_float(in, "width", &w);
	find_parg_float(in, "height", &h);
	
	fView->BColorControl::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}

