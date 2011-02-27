#include "PBox.h"
#include <Application.h>
#include <Box.h>
#include <Window.h>

#include "CInterface.h"
#include "EnumProperty.h"
#include "PArgs.h"

int32_t BBoxSetBorder(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxDraw(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxAttachedToWindow(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxDetachedFromWindow(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxAllAttached(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxAllDetached(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxFrameResized(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxMouseUp(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxMouseDown(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxMouseMoved(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxFrameMoved(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxResizeToPreferred(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxGetPreferredSize(void *pobj, PArgList *in, PArgList *out);
int32_t BBoxMakeFocus(void *pobj, PArgList *in, PArgList *out);

class PBoxBackend : public BBox
{
public:
			PBoxBackend(PObject *owner);
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

PBox::PBox(void)
	:	PView()
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
	InitProperties();
	InitMethods();
	InitBackend();
}


PBox::PBox(BMessage *msg)
	:	PView(msg)
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BBox::Instantiate(&viewmsg);
	
	InitBackend();
}


PBox::PBox(const char *name)
	:	PView(name)
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	InitMethods();
	InitBackend();
}


PBox::PBox(const PBox &from)
	:	PView(from)
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	InitMethods();
	InitBackend();
}


PBox::~PBox(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PBox::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PBox"))
		return new PBox(data);

	return NULL;
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
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BBox *box = dynamic_cast<BBox*>(fView);
		
	if (str.ICompare("Label") == 0)
		((StringProperty*)prop)->SetValue(box->Label());
	else if (str.ICompare("BorderStyle") == 0)
		((IntProperty*)prop)->SetValue(box->Border());
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
	
	BBox *box = dynamic_cast<BBox*>(fView);
	
	if (str.ICompare("Label") == 0)
	{
		prop->GetValue(&sv);
		box->SetLabel(sv.value->String());
	}
	else if (str.ICompare("BorderStyle") == 0)
	{
		prop->GetValue(&iv);
		box->SetBorder((border_style)*iv.value);
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
PBox::Create(void)
{
	return new PBox();
}


PObject *
PBox::Duplicate(void) const
{
	return new PBox(*this);
}

void
PBox::InitBackend(void)
{
	if (!fView)
		fView = new PBoxBackend(this);
	StringValue sv("A box for grouping together associated controls.");
	SetProperty("Description",&sv);
}


void
PBox::InitProperties(void)
{
	AddProperty(new StringProperty("Label",""));
	
	EnumProperty *eprop = new EnumProperty();
	eprop->SetName("BorderStyle");
	eprop->AddValuePair("Fancy Border",B_FANCY_BORDER);
	eprop->AddValuePair("Plain Border",B_PLAIN_BORDER);
	eprop->AddValuePair("No Border",B_NO_BORDER);
	eprop->SetValue(B_FANCY_BORDER);
	AddProperty(eprop);
	
}


void
PBox::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("border", PARG_INT32, "The border style constant");
	AddInheritedMethod(new PMethod("BBox::SetBorder", BBoxSetBorder, &pmi));
	
	pmi.SetArg(0, "update", PARG_RECT, "The area to update");
	AddInheritedMethod(new PMethod("BBox::Draw", BBoxDraw, &pmi));
	
	pmi.MakeEmpty();
	AddInheritedMethod(new PMethod("BBox::AttachedToWindow", BBoxAttachedToWindow));
	AddInheritedMethod(new PMethod("BBox::DetachedFromWindow", BBoxAttachedToWindow));
	AddInheritedMethod(new PMethod("BBox::AllAttached", BBoxAllAttached));
	AddInheritedMethod(new PMethod("BBox::AllDetached", BBoxAllDetached));
	
	pmi.AddArg("width", PARG_FLOAT, "The new width");
	pmi.AddArg("height", PARG_FLOAT, "The new height");
	AddInheritedMethod(new PMethod("BBox::FrameResized", BBoxFrameResized, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("where", PARG_POINT, "The location of the mouse up event");
	AddInheritedMethod(new PMethod("BBox::MouseUp", BBoxMouseUp, &pmi));

	pmi.SetArg(0, "where", PARG_POINT, "The location of the mouse down event");
	AddInheritedMethod(new PMethod("BBox::MouseDown", BBoxMouseDown, &pmi));
	
	pmi.SetArg(0, "where", PARG_POINT, "The current location of the pointer");
	pmi.AddArg("transit", PARG_INT32, "The transit flag. See BView::MouseDown.");
	pmi.AddArg("buttons", PARG_INT32, "The current mouse button state");
	pmi.AddArg("message", PARG_POINTER, "The drag message. NULL if empty");
	AddInheritedMethod(new PMethod("BBox::MouseMoved", BBoxMouseMoved, &pmi));
	pmi.MakeEmpty();
	
	AddInheritedMethod(new PMethod("BBox::ResizeToPreferred", BBoxResizeToPreferred));
	
	pmi.AddArg("where", PARG_POINT, "The new top left corner of the frame.");
	AddInheritedMethod(new PMethod("BBox::FrameMoved", BBoxFrameMoved, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddReturnValue("width", PARG_FLOAT, "The preferred width");
	pmi.AddReturnValue("height", PARG_FLOAT, "The preferred height");
	AddInheritedMethod(new PMethod("BBox::GetPreferredSize", BBoxGetPreferredSize, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("focus", PARG_BOOL, "The current focus value - active or inactive.");
	AddInheritedMethod(new PMethod("BBox::MakeFocus", BBoxMakeFocus, &pmi));
	pmi.MakeEmpty();
}


PBoxBackend::PBoxBackend(PObject *owner)
	:	BBox(BRect(0,0,1,1),"Label"),
		fOwner(owner)
{
	
}


void
PBoxBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BBox::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PBoxBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::AllAttached();
}


void
PBoxBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::DetachedFromWindow();
}


void
PBoxBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::AllDetached();
}


void
PBoxBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::MakeFocus(value);
}


void
PBoxBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::FrameMoved(pt);
}


void
PBoxBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::FrameResized(w, h);
}


void
PBoxBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::KeyDown(bytes, count);
}


void
PBoxBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::KeyUp(bytes, count);
}


void
PBoxBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::MouseDown(pt);
}


void
PBoxBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::MouseUp(pt);
}


void
PBoxBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::MouseMoved(pt, buttons, msg);
}


void
PBoxBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::WindowActivated(active);
}


void
PBoxBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BBox::Draw(update);
	
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
PBoxBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BBox::DrawAfterChildren(update);
}


void
PBoxBackend::MessageReceived(BMessage *msg)
{
	PBox *view = dynamic_cast<PBox*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BBox::MessageReceived(msg);
}


int32_t
PBoxAttachedToWindow(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	fView->BBox::AttachedToWindow();
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PBoxDraw(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgs args(in);
	BRect r;
	int32_t outCode = args.FindRect("update", &r);
	if (outCode != B_OK)
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return outCode;
	}
	
	fView->BBox::Draw(r);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PBoxFrameResized(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w,h;
	int32_t outCode;
	outCode = find_parg_float(in, "width", &w);
	if (outCode != B_OK)
		return outCode;
	
	outCode = find_parg_float(in, "height", &h);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxSetBorder(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	int32_t b;
	int32_t outCode = find_parg_int32(in, "border", &b);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::SetBorder((border_style)b);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxDraw(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float left, top, right, bottom;
	int32_t outCode = find_parg_rect(in, "update", &left, &top, &right, &bottom);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::Draw(BRect(left, top, right, bottom));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxAttachedToWindow(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BBox::AttachedToWindow();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxDetachedFromWindow(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BBox::DetachedFromWindow();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxAllAttached(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BBox::AllAttached();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxAllDetached(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BBox::AllDetached();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxFrameResized(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w,h;
	int32 outCode;
	outCode = find_parg_float(in, "width", &w);
	if (outCode != B_OK)
		return outCode;
	
	outCode = find_parg_float(in, "height", &h);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxMouseUp(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32 outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::MouseUp(BPoint(x,y));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxMouseDown(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32 outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::MouseDown(BPoint(x,y));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxMouseMoved(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32_t outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;

	int32_t transit;
	outCode = find_parg_int32(in, "transit", &transit);
	if (outCode != B_OK)
		return outCode;
	
	void *msg;
	outCode = find_parg_pointer(in, "message", &msg);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::MouseMoved(BPoint(x,y), transit, (BMessage*)msg);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxFrameMoved(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32 outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::FrameMoved(BPoint(x,y));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxResizeToPreferred(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BBox::ResizeToPreferred();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxGetPreferredSize(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w, h;
	fView->BBox::GetPreferredSize(&w, &h);
	
	PArgs outArgs(out);
	outArgs.MakeEmpty();
	outArgs.AddFloat("width", w);
	outArgs.AddFloat("heigh", h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BBoxMakeFocus(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PBox *parent = static_cast<PBox*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BBox *fView = (BBox*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	bool b;
	int32_t outCode;
	outCode = find_parg_bool(in, "focus", &b);
	if (outCode != B_OK)
		return outCode;
	
	fView->BBox::MakeFocus(b);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}

