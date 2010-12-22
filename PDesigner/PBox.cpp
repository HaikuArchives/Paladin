#include "PBox.h"
#include <Application.h>
#include <Box.h>
#include <Window.h>

#include "EnumProperty.h"
#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"
#include "PArgs.h"

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
	InitBackend();
}


PBox::PBox(BMessage *msg)
	:	PView(msg)
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BBox::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PBox::PBox(const char *name)
	:	PView(name)
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
	InitBackend();
}


PBox::PBox(const PBox &from)
	:	PView(from)
{
	fType = "PBox";
	fFriendlyType = "Box";
	AddInterface("PBox");
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
PBox::InitBackend(BView *view)
{
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


PBoxBackend::PBoxBackend(PObject *owner)
	:	BBox(BRect(0,0,1,1),"Label"),
		fOwner(owner)
{
	
}


void
PBoxBackend::AttachedToWindow(void)
{
	fOwner->SetColorProperty("BackColor",ViewColor());
	
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in.ListRef(), out.ListRef());
}


void
PBoxBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in.ListRef(), out.ListRef());
}


void
PBoxBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in.ListRef(), out.ListRef());
}


void
PBoxBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in.ListRef(), out.ListRef());
}


void
PBoxBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("active", value);
	fOwner->RunEvent("FocusChanged", in.ListRef(), out.ListRef());
}


void
PBoxBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in.ListRef(), out.ListRef());
}


void
PBoxBackend::FrameResized(float w, float h)
{
	BBox::FrameResized(w, h);
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in.ListRef(), out.ListRef());
}


void
PBoxBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddString("bytes", bytes);
	in.AddInt32("count", count);
	fOwner->RunEvent("KeyDown", in.ListRef(), out.ListRef());
}


void
PBoxBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddString("bytes", bytes);
	in.AddInt32("count", count);
	fOwner->RunEvent("KeyUp", in.ListRef(), out.ListRef());
}


void
PBoxBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in.ListRef(), out.ListRef());
}


void
PBoxBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in.ListRef(), out.ListRef());
}


void
PBoxBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in.ListRef(), out.ListRef());
}


void
PBoxBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in.ListRef(), out.ListRef());
}


void
PBoxBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook != NullPMethod)
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
	fOwner->RunEvent("DrawAfterChildren", in.ListRef(), out.ListRef());
}


void
PBoxBackend::MessageReceived(BMessage *msg)
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
			BBox::MessageReceived(msg);
			break;
		}
	}
}

