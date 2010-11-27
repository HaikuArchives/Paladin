#include "PBox.h"
#include <Application.h>
#include <Box.h>
#include <Window.h>

#include "EnumProperty.h"
#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"

class PBoxBackend : public BBox
{
public:
			PBoxBackend(PObject *owner);
	void	AttachedToWindow(void);
	void	MakeFocus(bool value);
	void	MouseUp(BPoint pt);
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
}


void
PBoxBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
}


void
PBoxBackend::MouseUp(BPoint pt)
{
	MakeFocus(true);
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
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
