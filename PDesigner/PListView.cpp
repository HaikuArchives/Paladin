#include "PListView.h"

#include <Application.h>
#include <Window.h>

#include "EnumProperty.h"
#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"

class PListViewBackend : public BListView
{
public:
			PListViewBackend(PObject *owner);
	void	MakeFocus(bool value);
	void	MouseUp(BPoint pt);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};


PListView::PListView(void)
	:	PView()
{
	fType = "PListView";
	AddInterface("PListView");
	
	InitBackend();
	InitProperties();
}


PListView::PListView(BMessage *msg)
	:	PView(msg)
{
	fType = "PListView";
	AddInterface("PListView");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BListView::Instantiate(&viewmsg);
	
	InitBackend(view);
	InitProperties();
}


PListView::PListView(const char *name)
	:	PView(name)
{
	fType = "PListView";
	AddInterface("PListView");
	InitBackend();
	InitProperties();
}


PListView::PListView(const PListView &from)
	:	PView(from)
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
PListView::InitBackend(BView *view)
{
	if (fView)
		delete fView;
	fView = (view == NULL) ? new PListViewBackend(this) : view;
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
PListViewBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BListView::MakeFocus(value);
}


void
PListViewBackend::MouseUp(BPoint pt)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BListView::MouseUp(pt);
}


void
PListViewBackend::MessageReceived(BMessage *msg)
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
