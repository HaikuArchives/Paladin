#include "PTemplate.h"
#include <Application.h>
#include <View.h>
#include <Window.h>

#include "CInterface.h"
#include "EnumProperty.h"
#include "PArgs.h"


class PTemplateBackend : public BView
{
public:
			PTemplateBackend(PObject *owner);
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

PTemplate::PTemplate(void)
	:	PView(true)
{
	fType = "PTemplate";
	fFriendlyType = "Template";
	AddInterface("PTemplate");
	
	InitProperties();
	InitMethods();
	InitBackend();
}


PTemplate::PTemplate(BMessage *msg)
	:	PView(msg, true)
{
	fType = "PTemplate";
	fFriendlyType = "Template";
	AddInterface("PTemplate");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BView::Instantiate(&viewmsg);
	
	InitBackend();
}


PTemplate::PTemplate(const char *name)
	:	PView(name, true)
{
	fType = "PTemplate";
	fFriendlyType = "Template";
	AddInterface("PTemplate");
	InitMethods();
	InitBackend();
}


PTemplate::PTemplate(const PTemplate &from)
	:	PView(from, true)
{
	fType = "PTemplate";
	fFriendlyType = "Template";
	AddInterface("PTemplate");
	InitMethods();
	InitBackend();
}


PTemplate::~PTemplate(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PTemplate::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTemplate"))
		return new PTemplate(data);

	return NULL;
}


status_t
PTemplate::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BView *view = dynamic_cast<BView*>(fView);
		
	if (str.ICompare("PropertyOne") == 0)
	{
		// call backend method here
		view->CountChildren();
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PView::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PTemplate::SetProperty(const char *name, PValue *value, const int32 &index)
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
	
	BView *view = dynamic_cast<BView*>(fView);
	
	if (str.ICompare("PropertyOne") == 0)
	{
		// prop->GetValue(&sv);
		// Invoke backend method here
		view->CountChildren();
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PView::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


PObject *
PTemplate::Create(void)
{
	return new PTemplate();
}


PObject *
PTemplate::Duplicate(void) const
{
	return new PTemplate(*this);
}

void
PTemplate::InitBackend(void)
{
	if (!fView)
		fView = new PTemplateBackend(this);
	StringValue sv("A generalized View-based control template.");
	SetProperty("Description",&sv);
}


void
PTemplate::InitProperties(void)
{
	// Add properties here
}


void
PTemplate::InitMethods(void)
{
	// Add methods and non-PView events here
}


PTemplateBackend::PTemplateBackend(PObject *owner)
	:	BView(BRect(0,0,1,1), "Template", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW),
		fOwner(owner)
{
	
}


void
PTemplateBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BView::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PTemplateBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::AllAttached();
}


void
PTemplateBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::DetachedFromWindow();
}


void
PTemplateBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::AllDetached();
}


void
PTemplateBackend::MakeFocus(bool value)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::MakeFocus(value);
}


void
PTemplateBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::FrameMoved(pt);
}


void
PTemplateBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::FrameResized(w, h);
}


void
PTemplateBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::KeyDown(bytes, count);
}


void
PTemplateBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::KeyUp(bytes, count);
}


void
PTemplateBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::MouseDown(pt);
}


void
PTemplateBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::MouseUp(pt);
}


void
PTemplateBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::MouseMoved(pt, buttons, msg);
}


void
PTemplateBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::WindowActivated(active);
}


void
PTemplateBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BView::Draw(update);
	
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
PTemplateBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BView::DrawAfterChildren(update);
}


void
PTemplateBackend::MessageReceived(BMessage *msg)
{
	PTemplate *view = dynamic_cast<PTemplate*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BView::MessageReceived(msg);
}


int32_t
PTemplateAttachedToWindow(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BView *fView = (BView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	fView->BView::AttachedToWindow();
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTemplateDraw(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BView *fView = (BView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgs args(in);
	BRect r;
	args.FindRect("update", &r);
	
	fView->BView::Draw(r);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTemplateFrameResized(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BView *fView = (BView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w,h;
	find_parg_float(in, "width", &w);
	find_parg_float(in, "height", &h);
	
	fView->BView::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}

