#include "PScrollBar.h"

#include <Application.h>
#include <ScrollBar.h>
#include <Window.h>
#include <stdio.h>

#include "EnumProperty.h"
#include "PArgs.h"
#include "PObjectBroker.h"

/*
	ScrollBar Properties:
		LargeStep
		Min
		Max
		Orientation
		Proportion
		SmallStep
		Target
		Value
*/

class PScrollBarBackend : public BScrollBar
{
public:
			PScrollBarBackend(PObject *owner, orientation posture);
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
	void	MouseMoved(BPoint pt, uint32 transit, const BMessage *msg);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);

private:
	PObject	*fOwner;
};

PScrollBar::PScrollBar(void)
	:	PView(),
		fPViewTarget(NULL)
{
	fType = "PScrollBar";
	fFriendlyType = "ScrollBar";
	AddInterface("PScrollBar");

	InitProperties();
	InitBackend();
}


PScrollBar::PScrollBar(BMessage *msg)
	:	PView(msg),
		fPViewTarget(NULL)
{
	fType = "PScrollBar";
	fFriendlyType = "ScrollBar";
	AddInterface("PScrollBar");

	
	InitBackend();
}


PScrollBar::PScrollBar(const char *name)
	:	PView(name),
		fPViewTarget(NULL)
{
	fType = "PScrollBar";
	fFriendlyType = "ScrollBar";
	AddInterface("PScrollBar");

	InitBackend();
}


PScrollBar::PScrollBar(const PScrollBar &from)
	:	PView(from),
		fPViewTarget(from.fPViewTarget)
{
	fType = "PScrollBar";
	fFriendlyType = "ScrollBar";
	AddInterface("PScrollBar");

	InitBackend();
}


PScrollBar::~PScrollBar(void)
{
}


BArchivable *
PScrollBar::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PScrollBar"))
		return new PScrollBar(data);

	return NULL;
}


PObject *
PScrollBar::Create(void)
{
	return new PScrollBar();
}


PObject *
PScrollBar::Duplicate(void) const
{
	return new PScrollBar(*this);
}


status_t
PScrollBar::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BScrollBar *viewAsScrollBar = (BScrollBar*)fView;
	
	if (str.ICompare("Value") == 0)
		((FloatProperty*)prop)->SetValue(viewAsScrollBar->Value());
	else if (str.ICompare("Target") == 0)
	{
		if (fPViewTarget)
			((IntProperty*)prop)->SetValue(fPViewTarget->GetID());
		else
		{
			// 0 is just as much an invalid object handle as it is an
			// officially invalid pointer value.
			((IntProperty*)prop)->SetValue(0LL);
		}
	}
	else if (str.ICompare("Min") == 0)
	{
		float min, max;
		viewAsScrollBar->GetRange(&min, &max);
		
		((FloatProperty*)prop)->SetValue(min);
	}
	else if (str.ICompare("Max") == 0)
	{
		float min, max;
		viewAsScrollBar->GetRange(&min, &max);
		
		((FloatProperty*)prop)->SetValue(max);
	}
	else if (str.ICompare("LargeStep") == 0)
	{
		float big, small;
		viewAsScrollBar->GetSteps(&small, &big);
		
		((FloatProperty*)prop)->SetValue(big);
	}
	else if (str.ICompare("SmallStep") == 0)
	{
		float big, small;
		viewAsScrollBar->GetSteps(&small, &big);
		
		((FloatProperty*)prop)->SetValue(small);
	}
	else if (str.ICompare("Orientation") == 0)
		((IntProperty*)prop)->SetValue(viewAsScrollBar->Orientation());
	else if (str.ICompare("Proportion") == 0)
		((FloatProperty*)prop)->SetValue(viewAsScrollBar->Proportion());
	else if (str.ICompare("PreferredWidth") == 0)
	{
		float pwidth,pheight;
		viewAsScrollBar->GetPreferredSize(&pwidth,&pheight);
		if (pwidth < 5)
			pwidth = 100;
		((FloatProperty*)prop)->SetValue(pwidth);
	}
	else if (str.ICompare("PreferredHeight") == 0)
	{
		float pwidth,pheight;
		viewAsScrollBar->GetPreferredSize(&pwidth,&pheight);
		if (pheight < 5)
			pheight = 100;
		((FloatProperty*)prop)->SetValue(pheight);
	}
	else
		return PView::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PScrollBar::SetProperty(const char *name, PValue *value, const int32 &index)
{
	// Orientation property is missing because it is read-only
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	BScrollBar *viewAsScrollBar = (BScrollBar*)fView;
	
	IntValue iv;
	FloatValue fv;
	
	if (viewAsScrollBar->Window())
		viewAsScrollBar->Window()->Lock();
	
/*
	ScrollBar Properties:
		LargeStep
		Min
		Max
		Orientation (read-only)
		Proportion
		SmallStep
		Target
		Value
*/
	if (str.ICompare("LargeStep") == 0)
	{
		prop->GetValue(&fv);
		
		float small, big;
		viewAsScrollBar->GetSteps(&small, &big);
		viewAsScrollBar->SetSteps(small, *fv.value);
	}
	else if (str.ICompare("SmallStep") == 0)
	{
		prop->GetValue(&fv);
		
		float small, big;
		viewAsScrollBar->GetSteps(&small, &big);
		viewAsScrollBar->SetSteps(*fv.value, big);
	}
	else if (str.ICompare("Min") == 0)
	{
		prop->GetValue(&fv);
		
		float min, max;
		viewAsScrollBar->GetRange(&min, &max);
		viewAsScrollBar->SetRange(*fv.value, max);
	}
	else if (str.ICompare("Max") == 0)
	{
		prop->GetValue(&fv);
		
		float min, max;
		viewAsScrollBar->GetRange(&min, &max);
		viewAsScrollBar->SetRange(min, *fv.value);
	}
	else if (str.ICompare("Proportion") == 0)
	{
		prop->GetValue(&fv);
		viewAsScrollBar->SetProportion(*fv.value);
	}
	else if (str.ICompare("Orientation") == 0)
	{
		// Here is an instance where PDesigner's object system removes
		// limitations in the API. A PScrollBar's orientation can be changed,
		// unlike BScrollBar. Nice. :)
		prop->GetValue(&iv);
		
		if (*iv.value == viewAsScrollBar->Orientation())
			return B_OK;
		
		float min, max, big, small;
		
		viewAsScrollBar->GetRange(&min, &max);
		viewAsScrollBar->GetSteps(&small, &big);
		
		PScrollBarBackend *newBar = new PScrollBarBackend(this, (orientation)*iv.value);
		newBar->MoveTo(viewAsScrollBar->Frame().LeftTop());
		newBar->ResizeTo(viewAsScrollBar->Frame().Width(), viewAsScrollBar->Frame().Height());
		newBar->SetRange(min, max);
		newBar->SetSteps(small, big);
		newBar->SetProportion(viewAsScrollBar->Proportion());
		newBar->SetValue(viewAsScrollBar->Value());
		
		if (viewAsScrollBar->Target())
		{
			BView *targetView = viewAsScrollBar->Target();
			viewAsScrollBar->SetTarget((BView*)NULL);
			newBar->SetTarget(targetView);
		}
		
		if (viewAsScrollBar->Parent())
		{
			BView *parent = viewAsScrollBar->Parent();
			viewAsScrollBar->RemoveSelf();
			parent->AddChild(newBar);
		}
		else if (viewAsScrollBar->Window())
		{
			BWindow *win = viewAsScrollBar->Window();
			viewAsScrollBar->RemoveSelf();
			win->AddChild(newBar);
		}
		delete viewAsScrollBar;
		
		return B_OK;
	}
	else if (str.ICompare("Target") == 0)
	{
		// To properly set a scrollbar target via an object handle like this,
		// we need to get a pointer from the broker via the handle, make sure it
		// will work properly, and then update values for the PScrollBar accordingly
		prop->GetValue(&iv);
		PObject *pobj = BROKER->FindObject(*iv.value);
		if (!pobj)
			return B_BAD_INDEX;
		
		PView *pview = dynamic_cast<PView*>(pobj);
		if (pobj->GetType().Compare("PView") != 0 || !pview)
			return B_BAD_DATA;
		
		BView *target = pview->GetView();
		if (target->ScrollBar(viewAsScrollBar->Orientation()))
			return B_BUSY;
		
		viewAsScrollBar->SetTarget(target);
		fPViewTarget = pview;
		return B_OK;
	}
	else
	{
		if (viewAsScrollBar->Window())
			viewAsScrollBar->Window()->Unlock();
		return PView::SetProperty(name,value,index);
	}

	if (viewAsScrollBar->Window())
		viewAsScrollBar->Window()->Unlock();
	
	return prop->GetValue(value);
}


void
PScrollBar::InitBackend(void)
{
	if (!fView)
		fView = new PScrollBarBackend(this, B_VERTICAL);
	StringValue sv("A scroll bar.");
	SetProperty("Description", &sv);
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
}


void
PScrollBar::InitProperties(void)
{
/*
	ScrollBar Properties:
		LargeStep
		Min
		Max
		Orientation (read-only)
		Proportion
		SmallStep
		Target
		Value
*/
	StringValue sv("A scrollbar class.");
	SetProperty("Description",&sv);
	
	AddProperty(new FloatProperty("LargeStep",10.0));
	AddProperty(new FloatProperty("Min",0.0));
	AddProperty(new FloatProperty("Max",100.0));
	AddProperty(new FloatProperty("LargeStep",10.0));
	
	EnumProperty *pEnum = new EnumProperty();
	pEnum->SetName("Orientation");
	pEnum->AddValuePair("Horizontal",B_HORIZONTAL);
	pEnum->AddValuePair("Vertical",B_VERTICAL);
	AddProperty(pEnum);
	
	AddProperty(new FloatProperty("Proportion",1.0));
	AddProperty(new FloatProperty("SmallStep",1.0));
	AddProperty(new IntProperty("Target",0));
	AddProperty(new FloatProperty("Value",0.0));
}


PScrollBarBackend::PScrollBarBackend(PObject *owner, orientation posture)
	:	BScrollBar(BRect(0,0,1,1), "", NULL, 0.0, 0.0, posture),
		fOwner(owner)
{
}


void
PScrollBarBackend::AttachedToWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BScrollBar::MakeFocus(value);
}


void
PScrollBarBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BScrollBar::KeyDown(bytes, count);
}


void
PScrollBarBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BScrollBar::KeyUp(bytes, count);
}


void
PScrollBarBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BScrollBar::Draw(update);
	
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
PScrollBarBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


void
PScrollBarBackend::MessageReceived(BMessage *msg)
{
	PScrollBar *view = dynamic_cast<PScrollBar*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BScrollBar::MessageReceived(msg);
}

