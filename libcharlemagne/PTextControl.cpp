#include "PTextControl.h"

#include "PArgs.h"

#include <Application.h>
#include <stdio.h>
#include <TextControl.h>
#include <Window.h>

int32_t	PTextControlSetPreferredDivider(void *object, PArgList *in, PArgList *out);

class PTextControlBackend : public AutoTextControl
{
public:
			PTextControlBackend(PObject *owner);
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
	void	MouseUp(BPoint pt);
	void	MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);

private:
	PObject 	*fOwner;
	
};

PTextControl::PTextControl(void)
	:	PControl()
{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	// Unlike the other constructors, this one needs to init the properties
	InitProperties();
	InitBackend();
}


PTextControl::PTextControl(BMessage *msg)
	:	PControl(msg)
{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)AutoTextControl::Instantiate(&viewmsg);
	
	InitBackend();
}


PTextControl::PTextControl(const char *name)
	:	PControl(name)
{
	fType = "PTextControl";
	fFriendlyType = "Checkbox";
	AddInterface("PTextControl");
	InitBackend();
}


PTextControl::PTextControl(const PTextControl &from)
	:	PControl(from)
{
	fType = "PTextControl";
	fFriendlyType = "Checkbox";
	AddInterface("PTextControl");
	InitBackend();
}


PTextControl::~PTextControl(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PTextControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTextControl"))
		return new PTextControl(data);

	return NULL;
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
	
	if (fView->Window())
		fView->Window()->Lock();
	
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
		
	if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(control->Text());
	else if (str.ICompare("TextAlignment") == 0)
	{
		alignment label, text;
		control->GetAlignment(&label,&text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		alignment label, text;
		control->GetAlignment(&label,&text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("Divider") == 0)
		((FloatProperty*)prop)->SetValue(control->Divider());
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
	
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
	
	if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		control->SetText(sv.value->String());
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		prop->GetValue(&iv);
		
		alignment label,text;
		control->GetAlignment(&label,&text);
		label = (alignment)*iv.value;
		control->SetAlignment(label,text);
	}
	else if (str.ICompare("TextAlignment") == 0)
	{
		prop->GetValue(&iv);
		
		alignment label,text;
		control->GetAlignment(&label,&text);
		text = (alignment)*iv.value;
		control->SetAlignment(label,text);
	}
	else if (str.ICompare("Divider") == 0)
	{
		prop->GetValue(&fv);
		control->SetDivider(*fv.value);
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
PTextControl::Create(void)
{
	return new PTextControl();
}


PObject *
PTextControl::Duplicate(void) const
{
	return new PTextControl(*this);
}


void
PTextControl::InitBackend(void)
{
	if (!fView)
		fView = new AutoTextControl(BRect(0,0,1,1),"", "", "", new BMessage);
	StringValue sv("A single line text box. It sends a message when its value changes.");
	SetProperty("Description",&sv);
}


void
PTextControl::InitProperties(void)
{
	AddProperty(new StringProperty("Text",""));
	AddProperty(new FloatProperty("Divider",0.0));
	AddProperty(new IntProperty("LabelAlignment",B_ALIGN_LEFT));
	AddProperty(new IntProperty("TextAlignment",B_ALIGN_LEFT));
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
	
	AddMethod(new PMethod("SetPreferredDivider", PTextControlSetPreferredDivider,
							NULL, METHOD_SHOW_IN_EDITOR));
}


status_t
PTextControl::DoSetPreferredDivider(void)
{
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
	if (!fView)
		return B_ERROR;
	
	if (strlen(control->Label()) > 0)
		control->SetDivider(control->StringWidth(control->Label()));
	else
		control->SetDivider(0.0);
	
	return B_OK;
}


PTextControlBackend::PTextControlBackend(PObject *owner)
	:	AutoTextControl(BRect(0,0,1,1),"","","",new BMessage()),
		fOwner(owner)
{
}

void
PTextControlBackend::AttachedToWindow(void)
{
	SetDivider(0.0);
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BTextControl::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PTextControlBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::AllAttached();
}


void
PTextControlBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::DetachedFromWindow();
}


void
PTextControlBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::AllDetached();
}


void
PTextControlBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::MakeFocus(value);
}


void
PTextControlBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::FrameMoved(pt);
}


void
PTextControlBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::FrameResized(w, h);
}


void
PTextControlBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::KeyDown(bytes, count);
}


void
PTextControlBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::KeyUp(bytes, count);
}


void
PTextControlBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");

	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::MouseDown(pt);
}


void
PTextControlBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::MouseUp(pt);
}


void
PTextControlBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::MouseMoved(pt, buttons, msg);
}


void
PTextControlBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::WindowActivated(active);
}


void
PTextControlBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook == NULL)
		BTextControl::Draw(update);
	
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
PTextControlBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextControl::DrawAfterChildren(update);
}


void
PTextControlBackend::MessageReceived(BMessage *msg)
{
	PTextControl *view = dynamic_cast<PTextControl*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BTextControl::MessageReceived(msg);
}


int32_t
PTextControlSetPreferredDivider(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PTextControl *pcontrol = static_cast<PTextControl*>(pobject);
	PObject *object = static_cast<PObject*>(pobject);
	
	if (!object->UsesInterface("PTextControl") || !pcontrol)
		return B_BAD_TYPE;
	
	BTextControl *control = dynamic_cast<BTextControl*>(pcontrol->GetView());
	if (!control)
		return B_BAD_TYPE;
	
	if (control->Window())
		control->Window()->Lock();
	
	if (strlen(control->Label()) > 0)
		control->SetDivider(control->StringWidth(control->Label()));
	else
		control->SetDivider(0.0);
	
	if (control->Window())
		control->Window()->Unlock();
	
	return B_OK;
}
