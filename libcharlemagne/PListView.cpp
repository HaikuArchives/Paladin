#include "PListView.h"

#include <Application.h>
#include <Window.h>

#include "EnumProperty.h"
#include "PArgs.h"

class PListViewBackend : public BListView
{
public:
			PListViewBackend(PObject *owner);
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


PListView::PListView(void)
	:	PView(false)
{
	fType = "PListView";
	AddInterface("PListView");
	
	InitBackend();
	InitProperties();
}


PListView::PListView(BMessage *msg)
	:	PView(msg, false)
{
	fType = "PListView";
	AddInterface("PListView");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BListView::Instantiate(&viewmsg);
	
	InitBackend();
	InitProperties();
}


PListView::PListView(const char *name)
	:	PView(name, false)
{
	fType = "PListView";
	AddInterface("PListView");
	InitBackend();
	InitProperties();
}


PListView::PListView(const PListView &from)
	:	PView(from, false)
{
	fType = "PListView";
	AddInterface("PListView");
	InitBackend();
	InitProperties();
}


PListView::~PListView(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PListView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PListView"))
		return new PListView(data);

	return NULL;
}


PObject *
PListView::Create(void)
{
	return new PListView();
}


PObject *
PListView::Duplicate(void) const
{
	return new PListView(*this);
}

	
status_t
PListView::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BListView *fListView = (BListView*)fView;
	
	if (str.ICompare("ItemCount") == 0)
		((IntProperty*)prop)->SetValue(fListView->CountItems());
	else if (str.ICompare("SelectionType") == 0)
		((IntProperty*)prop)->SetValue(fListView->ListType());
	else if (str.ICompare("PreferredWidth") == 0)
	{
		if (fListView->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(100);
		else
		{
			float pw, ph;
			fListView->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(pw);
		}
	}
	else if (str.ICompare("PreferredHeight") == 0)
	{
		if (fListView->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(30);
		else
		{
			float pw, ph;
			fListView->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(ph);
		}
	}
	else
		return PObject::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PListView::SetProperty(const char *name, PValue *value, const int32 &index)
{
/*
	PListView Properties:
		All PView Properties
		
		ItemCount
		SelectionType
*/
	// ItemCount is read-only
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BListView *fListView = (BListView*)fView;
	
	IntValue iv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (str.ICompare("SelectionType") == 0)
	{
		prop->GetValue(&iv);
		fListView->SetListType((list_view_type)*iv.value);
	}
	else
		return PView::SetProperty(name,value,index);
	
	return prop->GetValue(value);
}

	
void
PListView::InitBackend(void)
{
	if (!fView)
		fView = new PListViewBackend(this);
}


void
PListView::InitProperties(void)
{
/*
	PListView Properties:
		All PView Properties
		
		ItemCount
		SelectionType
*/
	StringValue sv("An item-based list control");
	SetProperty("Description",&sv);
	
	AddProperty(new IntProperty("ItemCount",0,"The number of the items in the view. Read-only"),
				PROPERTY_READ_ONLY);

	EnumProperty *prop = new EnumProperty();
	prop->SetName("SelectionType");
	prop->AddValuePair("Single", B_SINGLE_SELECTION_LIST);
	prop->AddValuePair("Multiple", B_MULTIPLE_SELECTION_LIST);
	prop->SetDescription("The list's item selection mode.");
	prop->SetValue((int32)B_SINGLE_SELECTION_LIST);
	AddProperty(prop);
}


PListViewBackend::PListViewBackend(PObject *owner)
	:	BListView(BRect(0,0,1,1),"", B_SINGLE_SELECTION_LIST),
		fOwner(owner)
{
}


void
PListViewBackend::AttachedToWindow(void)
{
	BListView::AttachedToWindow();
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AttachedToWindow();
}


void
PListViewBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AllAttached();
}


void
PListViewBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::DetachedFromWindow();
}


void
PListViewBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AllDetached();
}


void
PListViewBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MakeFocus(value);
}


void
PListViewBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::FrameMoved(pt);
}


void
PListViewBackend::FrameResized(float w, float h)
{
	BListView::FrameResized(w,h);
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::FrameResized(w, h);
}


void
PListViewBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::KeyDown(bytes, count);
}


void
PListViewBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::KeyUp(bytes, count);
}


void
PListViewBackend::MouseDown(BPoint pt)
{
	BListView::MouseDown(pt);
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseDown(pt);
}


void
PListViewBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseUp(pt);
}


void
PListViewBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseMoved(pt, transit, msg);
}


void
PListViewBackend::WindowActivated(bool active)
{
	BListView::WindowActivated(active);
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::WindowActivated(active);
}


void
PListViewBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BListView::Draw(update);
	
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
PListViewBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::DrawAfterChildren(update);
}


void
PListViewBackend::MessageReceived(BMessage *msg)
{
	PListView *view = dynamic_cast<PListView*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BListView::MessageReceived(msg);
}
