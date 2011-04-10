#include "PListView.h"

#include <Application.h>
#include <ListView.h>
#include <stdio.h>
#include <Window.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PListViewBackend : public BListView
{
public:
			PListViewBackend(PObject *owner);

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


PListView::PListView(void)
	:	PView(true)
{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PListView::PListView(BMessage *msg)
	:	PView(msg, true)
{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PListViewBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PListView::PListView(const char *name)
	:	PView(name, true)
{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	InitMethods();
	InitBackend();
}


PListView::PListView(const PListView &from)
	:	PView(from, true)
{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	InitMethods();
	InitBackend();
}


PListView::~PListView(void)
{
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
	
	BListView *backend = (BListView*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("ItemCount") == 0)
		((IntProperty*)prop)->SetValue(backend->CountItems());
	else if (str.ICompare("SelectionType") == 0)
		((EnumProperty*)prop)->SetValue(backend->ListType());
	else if (str.ICompare("PreferredHeight") == 0)
	{
		if (backend->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(30);
		else
		{
			float pw, ph;
			backend->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(ph);
		}
	}
	else if (str.ICompare("PreferredWidth") == 0)
	{
		if (backend->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(100);
		else
		{
			float pw, ph;
			backend->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(pw);
		}
	}
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
PListView::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BListView *backend = (BListView*)fView;
	
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

	else if (str.ICompare("SelectionType") == 0)
	{
		prop->GetValue(&intval);
		backend->SetListType((list_view_type)*intval.value);
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
PListView::InitBackend(void)
{
}


void
PListView::InitProperties(void)
{
	SetStringProperty("Description", "A list");

	AddProperty(new IntProperty("ItemCount", 0, "The number of items in the list. Read-only."));

	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("SelectionType");
	prop->SetValue((int32)B_SINGLE_SELECTION_LIST);
	prop->SetDescription("The list's selection mode");
	prop->AddValuePair("Single", B_SINGLE_SELECTION_LIST);
	prop->AddValuePair("Multiple", B_MULTIPLE_SELECTION_LIST);
	AddProperty(prop);

}


void
PListView::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PListViewBackend::PListViewBackend(PObject *owner)
	:	BListView(BRect(0,0,1,1),"", B_SINGLE_SELECTION_LIST),
		fOwner(owner)
{
}


void
PListViewBackend::AttachedToWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AttachedToWindow();
}


void
PListViewBackend::DetachedFromWindow()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::DetachedFromWindow();
}


void
PListViewBackend::AllAttached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AllAttached();
}


void
PListViewBackend::AllDetached()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AllDetached();
}


void
PListViewBackend::Pulse()
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::Pulse();
}


void
PListViewBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MakeFocus(param1);
}


void
PListViewBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::FrameMoved(param1);
}


void
PListViewBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::FrameResized(param1, param2);
}


void
PListViewBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseDown(param1);
}


void
PListViewBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseUp(param1);
}


void
PListViewBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseMoved(param1, param2, param3);
}


void
PListViewBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::WindowActivated(param1);
}


void
PListViewBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::Draw(param1);
}


void
PListViewBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::DrawAfterChildren(param1);
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


