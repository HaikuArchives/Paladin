#include "PButton.h"

#include <Application.h>
#include <Window.h>
#include <stdio.h>

#include "DebugTools.h"
#include "PArgs.h"

int32_t BButtonDraw(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonAttachedToWindow(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonDetachedFromWindow(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonAllAttached(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonAllDetached(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonFrameResized(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonMouseUp(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonMouseDown(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonMouseMoved(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonFrameMoved(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonResizeToPreferred(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonGetPreferredSize(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonMakeFocus(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonKeyDown(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonMakeDefault(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonSetLabel(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonWindowActivated(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonSetValue(void *pobj, PArgList *in, PArgList *out);
int32_t BButtonInvoke(void *pobj, PArgList *in, PArgList *out);


class PButtonBackend : public BButton
{
public:
			PButtonBackend(PObject *owner);
			~PButtonBackend(void);
			
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

PButton::PButton(void)
	:	PControl()
{
	STRACE(("new PButton(void)\n"), TRACE_CREATE);
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	InitMethods();
	InitBackend();
}


PButton::PButton(BMessage *msg)
	:	PControl(msg)
{
	STRACE(("new PButton(msg)\n"), TRACE_CREATE);
	
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BButton::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PButton::PButton(const char *name)
	:	PControl(name)
{
	STRACE(("new PButton(%s)\n",name), TRACE_CREATE);
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	InitMethods();
	InitBackend();
}


PButton::PButton(const PButton &from)
	:	PControl(from)
{
	STRACE(("new PButton(copy)\n"), TRACE_CREATE);
	fType = "PButton";
	fFriendlyType = "Button";
	AddInterface("PButton");
	InitMethods();
	InitBackend();
}


PButton::~PButton(void)
{
	STRACE(("Delete PButton()\n"), TRACE_DESTROY);
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PButton::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PButton"))
		return new PButton(data);

	return NULL;
}


PObject *
PButton::Create(void)
{
	return new PButton();
}


PObject *
PButton::Duplicate(void) const
{
	return new PButton(*this);
}


void
PButton::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("update", PARG_RECT, "The area to update");
	AddInheritedMethod(new PMethod("BButton::Draw", BButtonDraw, &pmi));
	
	pmi.MakeEmpty();
	AddInheritedMethod(new PMethod("BButton::AttachedToWindow", BButtonAttachedToWindow));
	AddInheritedMethod(new PMethod("BButton::DetachedFromWindow", BButtonAttachedToWindow));
	AddInheritedMethod(new PMethod("BButton::AllAttached", BButtonAllAttached));
	AddInheritedMethod(new PMethod("BButton::AllDetached", BButtonAllDetached));
	
	pmi.AddArg("width", PARG_FLOAT, "The new width");
	pmi.AddArg("height", PARG_FLOAT, "The new height");
	AddInheritedMethod(new PMethod("BButton::FrameResized", BButtonFrameResized, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("where", PARG_POINT, "The location of the mouse up event");
	AddInheritedMethod(new PMethod("BButton::MouseUp", BButtonMouseUp, &pmi));

	pmi.SetArg(0, "where", PARG_POINT, "The location of the mouse down event");
	AddInheritedMethod(new PMethod("BButton::MouseDown", BButtonMouseDown, &pmi));
	
	pmi.SetArg(0, "where", PARG_POINT, "The current location of the pointer");
	pmi.AddArg("transit", PARG_INT32, "The mouse transition state");
	pmi.AddArg("message", PARG_POINTER, "The drag message. NULL if empty");
	AddInheritedMethod(new PMethod("BButton::MouseMoved", BButtonMouseMoved, &pmi));
	pmi.MakeEmpty();
	
	AddInheritedMethod(new PMethod("BButton::ResizeToPreferred", BButtonResizeToPreferred));
	
	pmi.AddArg("where", PARG_POINT, "The new top left corner of the frame.");
	AddInheritedMethod(new PMethod("BButton::FrameMoved", BButtonFrameMoved, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddReturnValue("width", PARG_FLOAT, "The preferred width");
	pmi.AddReturnValue("height", PARG_FLOAT, "The preferred height");
	AddInheritedMethod(new PMethod("BButton::GetPreferredSize", BButtonGetPreferredSize, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("focus", PARG_BOOL, "The current focus value - active or inactive.");
	AddInheritedMethod(new PMethod("BButton::MakeFocus", BButtonMakeFocus, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("count", PARG_INT32, "Size of the key down array, in bytes");
	pmi.AddArg("bytes", PARG_RAW, "Character array containing the key pressed.");
	AddInheritedMethod(new PMethod("BButton::KeyDown", BButtonKeyDown, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("value", PARG_BOOL, "Whether or not the button should be the default");
	AddInheritedMethod(new PMethod("BButton::MakeDefault", BButtonMakeDefault, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("label", PARG_STRING, "The label for the button.");
	AddInheritedMethod(new PMethod("BButton::SetLabel", BButtonSetLabel, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("active", PARG_BOOL, "Whether or not the window is active");
	AddInheritedMethod(new PMethod("BButton::WindowActivated", BButtonWindowActivated, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("value", PARG_INT32, "The on/off value for the button");
	AddInheritedMethod(new PMethod("BButton::SetValue", BButtonSetValue, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("message", PARG_POINTER, "Pointer to the message to send. May be NULL.");
	AddInheritedMethod(new PMethod("BButton::Invoke", BButtonInvoke, &pmi));
	pmi.MakeEmpty();
	
}


void
PButton::InitBackend(BView *view)
{
	if (!fView)
		fView = new PButtonBackend(this);
	StringValue sv("A basic button object. It sends a message when clicked.");
	SetProperty("Description",&sv);
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
}


PButtonBackend::PButtonBackend(PObject *owner)
	:	BButton(BRect(0,0,1,1),"", "", new BMessage),
		fOwner(owner)
{
	STRACE(("new PButtonBackend\n"), TRACE_CREATE);
}


PButtonBackend::~PButtonBackend(void)
{
	STRACE(("Delete PButtonBackend\n"), TRACE_DESTROY);
}


void
PButtonBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BButton::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PButtonBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AllAttached();
}


void
PButtonBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::DetachedFromWindow();
}


void
PButtonBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::AllDetached();
}


void
PButtonBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MakeFocus(value);
}


void
PButtonBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::FrameMoved(pt);
}


void
PButtonBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::FrameResized(w, h);
}


void
PButtonBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::KeyDown(bytes, count);
}


void
PButtonBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::KeyUp(bytes, count);
}


void
PButtonBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");

	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseDown(pt);
}


void
PButtonBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseUp(pt);
}


void
PButtonBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::MouseMoved(pt, transit, msg);
}


void
PButtonBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::WindowActivated(active);
}


void
PButtonBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook == NULL)
		BButton::Draw(update);
	
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
PButtonBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BButton::DrawAfterChildren(update);
}


void
PButtonBackend::MessageReceived(BMessage *msg)
{
	PButton *view = dynamic_cast<PButton*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BButton::MessageReceived(msg);
}


int32_t
BButtonDraw(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float left, top, right, bottom;
	int32_t outCode = find_parg_rect(in, "update", &left, &top, &right, &bottom);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::Draw(BRect(left, top, right, bottom));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonAttachedToWindow(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BButton::AttachedToWindow();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonDetachedFromWindow(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BButton::DetachedFromWindow();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonAllAttached(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BButton::AllAttached();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonAllDetached(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BButton::AllDetached();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonFrameResized(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
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
	
	fView->BButton::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonMouseUp(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32 outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::MouseUp(BPoint(x,y));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonMouseDown(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32 outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::MouseDown(BPoint(x,y));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonMouseMoved(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
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
	
	fView->BButton::MouseMoved(BPoint(x,y), transit, (BMessage*)msg);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonFrameMoved(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float x, y;
	int32 outCode;
	outCode = find_parg_point(in, "where", &x, &y);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::FrameMoved(BPoint(x,y));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonResizeToPreferred(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->BButton::ResizeToPreferred();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonGetPreferredSize(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w, h;
	fView->BButton::GetPreferredSize(&w, &h);
	
	PArgs outArgs(out);
	outArgs.MakeEmpty();
	outArgs.AddFloat("width", w);
	outArgs.AddFloat("heigh", h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonMakeFocus(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	bool b;
	int32_t outCode;
	outCode = find_parg_bool(in, "focus", &b);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::MakeFocus(b);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonKeyDown(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgListItem *item = find_parg(in, "bytes", NULL);
	if (!item != B_OK)
		return B_NAME_NOT_FOUND;
	
	char bytes[item->datasize];
	
	fView->BButton::KeyDown(bytes, item->datasize);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonMakeDefault(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	bool b;
	int32_t outCode;
	outCode = find_parg_bool(in, "value", &b);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::MakeDefault(b);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonSetLabel(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgs inArgs(in);
	BString label;
	int32_t outCode;
	outCode = inArgs.FindString("label", &label);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::SetLabel(label.String());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonWindowActivated(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	bool b;
	int32_t outCode;
	outCode = find_parg_bool(in, "active", &b);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::WindowActivated(b);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonSetValue(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	int32_t value;
	int32_t outCode;
	outCode = find_parg_int32(in, "value", &value);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::SetValue(value);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
BButtonInvoke(void *pobj, PArgList *in, PArgList *out)
{
	if (!pobj || !in || !out)
		return B_ERROR;
	
	PButton *parent = static_cast<PButton*>(pobj);
	if (!parent)
		return B_BAD_TYPE;
	
	BButton *fView = (BButton*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BMessage *msg;
	int32_t outCode;
	outCode = find_parg_pointer(in, "message", (void**)&msg);
	if (outCode != B_OK)
		return outCode;
	
	fView->BButton::Invoke(msg);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


