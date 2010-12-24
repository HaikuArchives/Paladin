#include "PLabel.h"

#include <Application.h>
#include <Window.h>

#include "EnumProperty.h"
#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"
#include "PArgs.h"

class PLabelBackend : public BStringView
{
public:
			PLabelBackend(PObject *owner);
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
	PObject	*fOwner;
};

PLabel::PLabel(void)
	:	PView()
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitProperties();
	InitBackend();
}


PLabel::PLabel(BMessage *msg)
	:	PView(msg)
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BStringView::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PLabel::PLabel(const char *name)
	:	PView(name)
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitProperties();
	InitBackend();
}


PLabel::PLabel(const PLabel &from)
	:	PView(from)
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitProperties();
	InitBackend();
}


PLabel::~PLabel(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PLabel::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PLabel"))
		return new PLabel(data);

	return NULL;
}


status_t
PLabel::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BStringView *fLabel = (BStringView*)fView;
	
	if (str.ICompare("Alignment") == 0)
		((EnumProperty*)prop)->SetValue(fLabel->Alignment());
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(fLabel->Text());
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		
		return PObject::GetProperty(name,value,index);
	}

	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PLabel::SetProperty(const char *name, PValue *value, const int32 &index)
{
/*
	PLabel Properties:
		All PView Properties
		
		Alignment
		Text
*/
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	IntValue iv;
	StringValue sv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BStringView *fLabel = (BStringView*)fView;
	
	if (str.ICompare("Alignment") == 0)
	{
		prop->GetValue(&iv);
		fLabel->SetAlignment((alignment)*iv.value);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		fLabel->SetText(sv.value->String());
		fLabel->Invalidate();
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
PLabel::Create(void)
{
	return new PLabel();
}


PObject *
PLabel::Duplicate(void) const
{
	return new PLabel(*this);
}

void
PLabel::InitBackend(BView *view)
{
	fView = (view == NULL) ? new PLabelBackend(this) : view;
	StringValue sv("A basic button object. It sends a message when clicked.");
	SetProperty("Description",&sv);
}


void
PLabel::InitProperties(void)
{
/*
	PLabel Properties:
		All PView Properties
		
		Alignment
		Text
*/

	StringValue sv("A string label");
	SetProperty("Description", &sv);
	
	EnumProperty *prop = new EnumProperty();
	prop->SetName("Alignment");
	prop->AddValuePair("Left", B_ALIGN_LEFT);
	prop->AddValuePair("Right", B_ALIGN_RIGHT);
	prop->AddValuePair("Center", B_ALIGN_CENTER);
	prop->SetValue((int32)B_ALIGN_LEFT);
	AddProperty(prop);
	
	AddProperty(new StringProperty("Text", "", "The label's text"));
}


PLabelBackend::PLabelBackend(PObject *owner)
	:	BStringView(BRect(0,0,1,1),"", ""),
		fOwner(owner)
{
}


void
PLabelBackend::AttachedToWindow(void)
{
	BStringView::AttachedToWindow();
	
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PLabelBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PLabelBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PLabelBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PLabelBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("active", value);
	fOwner->RunEvent("FocusChanged", in.ListRef(), out.ListRef());
}


void
PLabelBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PLabelBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
}


void
PLabelBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStringView::KeyDown(bytes, count);
}


void
PLabelBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStringView::KeyUp(bytes, count);
}


void
PLabelBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PLabelBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PLabelBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PLabelBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
}


void
PLabelBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BStringView::Draw(update);
	
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
PLabelBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


void
PLabelBackend::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_FLOATER_ACTION:
		{
			int32 action;
			if (msg->FindInt32("action", &action) != B_OK)
				break;
			
			float dx, dy;
			msg->FindFloat("dx", &dx);
			msg->FindFloat("dy", &dy);
			
			FloaterBroker *broker = FloaterBroker::GetInstance();
			
			switch (action)
			{
				case FLOATER_MOVE:
				{
					MoveBy(dx, dy);
					broker->NotifyFloaters((PView*)fOwner, FLOATER_MOVE);
					break;
				}
				case FLOATER_RESIZE:
				{
					ResizeBy(dx, dy);
					broker->NotifyFloaters((PView*)fOwner, FLOATER_RESIZE);
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}
