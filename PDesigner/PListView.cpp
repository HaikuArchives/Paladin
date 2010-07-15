#include "PListView.h"
#include <Window.h>

PListItem::PListItem(void)
	:	PObject()
{
	fType = "PListItem";
	AddInterface("PListItem");
	InitProperties();
	InitBackend();
}


PListItem::PListItem(BMessage *msg)
	:	PObject(msg)
{
	fType = "PListItem";
	AddInterface("PListItem");
	InitBackend();
}


PListItem::PListItem(const char *name)
	:	PObject(name)
{
	fType = "PListItem";
	AddInterface("PListItem");
	InitProperties();
	InitBackend();
}


PListItem::PListItem(const PListItem &from)
	:	PObject(from)
{
	fType = "PListItem";
	AddInterface("PListItem");
	InitBackend();
}


PListItem::~PListItem(void)
{
	delete fListItem;
}


BArchivable *
PListItem::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PListItem"))
		return new PListItem(data);

	return NULL;
}


PObject *
PListItem::Create(void)
{
	return new PListItem();
}


PObject *
PListItem::Duplicate(void) const
{
	return new PListItem(*this);
}


status_t
PListItem::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (str.ICompare("Enabled") == 0)
		((BoolProperty*)prop)->SetValue(fListItem->IsEnabled());
	else if (str.ICompare("Expanded") == 0)
		((BoolProperty*)prop)->SetValue(fListItem->IsExpanded());
	else if (str.ICompare("Height") == 0)
		((FloatProperty*)prop)->SetValue(fListItem->Height());
	else if (str.ICompare("OutlineLevel") == 0)
		((IntProperty*)prop)->SetValue(fListItem->OutlineLevel());
	else if (str.ICompare("Selected") == 0)
		((BoolProperty*)prop)->SetValue(fListItem->IsSelected());
	else if (str.ICompare("Width") == 0)
		((FloatProperty*)prop)->SetValue(fListItem->Width());
	else
		return PObject::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PListItem::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	BoolValue bv;
	IntValue iv;
	FloatValue fv;
	StringValue sv;
	
	if (str.ICompare("Enabled") == 0)
	{
		prop->GetValue(&bv);
		fListItem->SetEnabled(bv.value);
	}
	else if (str.ICompare("Expanded") == 0)
	{
		prop->GetValue(&bv);
		fListItem->SetExpanded(bv.value);
	}
	else if (str.ICompare("Height") == 0)
	{
		prop->GetValue(&fv);
		fListItem->SetHeight(fv.value);
	}
	else if (str.ICompare("Expanded") == 0)
	{
		prop->GetValue(&bv);
		fListItem->SetExpanded(bv.value);
	}
//	else if (str.ICompare("OutlineLevel") == 0)
//		return B_READ_ONLY;
	else if (str.ICompare("Width") == 0)
	{
		prop->GetValue(&fv);
		fListItem->SetWidth(fv.value);
	}
	else
		return PObject::SetProperty(name,value,index);
	
	return prop->GetValue(value);
}


BListItem *
PListItem::GetListItem(void)
{
	return fListItem;
}


void
PListItem::InitProperties(void)
{
	AddProperty(new BoolProperty("Enabled",true));
	AddProperty(new BoolProperty("Expanded",false));
	AddProperty(new FloatProperty("Height",10));
	AddProperty(new IntProperty("OutlineLevel",0),PROPERTY_READ_ONLY);
	AddProperty(new FloatProperty("Width",100));
}


void
PListItem::InitBackend(void)
{
}


#pragma mark PStringItem


PStringItem::PStringItem(void)
	:	PListItem()
{
	fType = "PStringItem";
	AddInterface("PStringItem");
	InitProperties();
	InitBackend();
}


PStringItem::PStringItem(BMessage *msg)
	:	PListItem(msg)
{
	fType = "PStringItem";
	AddInterface("PStringItem");
	InitBackend();
}


PStringItem::PStringItem(const char *name)
	:	PListItem(name)
{
	fType = "PStringItem";
	AddInterface("PStringItem");
	InitBackend();
}


PStringItem::PStringItem(const PStringItem &from)
	:	PListItem(from)
{
	fType = "PStringItem";
	AddInterface("PStringItem");
	InitBackend();
}


PStringItem::~PStringItem(void)
{
	// fListItem is deleted for us by ~PListItem()
}


BArchivable *
PStringItem::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PStringItem"))
		return new PStringItem(data);

	return NULL;
}


PObject *
PStringItem::Create(void)
{
	return new PStringItem();
}


PObject *
PStringItem::Duplicate(void) const
{
	return new PStringItem(*this);
}


status_t
PStringItem::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(((BStringItem*)fListItem)->Text());
	else
		return PListItem::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PStringItem::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	StringValue sv;
	
	if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		((BStringItem*)fListItem)->SetText(sv.value.String());
	}
	else
		return PListItem::SetProperty(name,value,index);
	
	return prop->GetValue(value);
}


void
PStringItem::InitProperties(void)
{
	AddProperty(new StringProperty("Text",""));
}


void
PStringItem::InitBackend(void)
{
	// This is to bypass the special implementation. If we don't, this will crash
	StringValue sv;
	PListItem::GetProperty("Text",&sv);
	fListItem = new BStringItem(sv.value.String());
}


#pragma mark PListView

	
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

	
bool
PListView::AddProperty(PProperty *p, uint32 flags)
{
	if (!p)
		return false;
	
	if (p->GetType().ICompare("ObjectProperty") == 0)
	{
		if (p->GetName().ICompare("Items") == 0)
		{
			ObjectValue itemObject;
			p->GetValue(&itemObject);
			if (!itemObject.value)
				return false;
			
			if (!itemObject.value->UsesInterface("PListItem"))
				return false;
			
			status_t status = PObject::AddProperty(p,flags | PROPERTY_ALLOW_MULTIPLE);
			if (status)
			{
				PListItem *pitem = (PListItem*)itemObject.value;
				BListView *fListView = (BListView*)fView;
				
				if (fListView->Window())
					fListView->Window()->Lock();
				fListView->AddItem(pitem->GetListItem());
				if (fListView->Window())
					fListView->Window()->Unlock();
			}
			return status;
		}
	}
	
	return PObject::AddProperty(p,flags);
}


PProperty *
PListView::RemoveProperty(const int32 &index)
{
	PProperty *p = PropertyAt(index);
	
	if (!p)
		return NULL;
	
	if (p->GetType().ICompare("ObjectProperty") == 0)
	{
		ObjectValue objv;
		p->GetValue(&objv);
		if (!objv.value)
			return p;
		
		if (p->GetName().ICompare("Items") == 0)
		{
			BListView *fListView = (BListView*)fView;
			PListItem *pitem = (PListItem*)objv.value;
			fListView->RemoveItem(pitem->GetListItem());
		}
	}
	
	PObject::RemoveProperty(index);
	return p;
}


void
PListView::RemoveProperty(PProperty *p)
{
	int32 index = IndexOfProperty(p);
	if (index < 0)
		return;
	
	PProperty *out = RemoveProperty(index);
	delete out;
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
	
	// Items, InvocationMessage, and SelectionMessage are treated as regular properties
	if (str.ICompare("ItemCount") == 0)
		((IntProperty*)prop)->SetValue(fListView->CountItems());
	else if (str.ICompare("SelectionType") == 0)
		((IntProperty*)prop)->SetValue(fListView->ListType());
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
		
		Items
		ItemCount
		InvocationMessage
		SelectionMessage
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
	
	BoolValue bv;
	IntValue iv;
	MessageValue mv;
	ObjectValue ov;
	
	if (str.ICompare("Items") == 0)
	{
		// TODO: Implement
		debugger("Implement SetProperty for PListView::Items");
		return B_ERROR;
		
/*		// Calling SetProperty for Items is a little strange. The result? The old BListItem
		// is replaced by the new one.
		prop->GetValue(&ov);
		PListItem *oldPItem = (PListItem*)ov.value;
		int32 itemIndex = fListView->IndexOf(oldPItem->GetListItem());
		
		// WATCHPOINT: According to research (but not testing), this does NOT delete the replaced item
		if (value->GetType().ICompare("ObjectValue"))
			return B_BAD_VALUE;
		ObjectValue *newValue = (ObjectValue*)value;
		if (!newValue->value)
			return B_ERROR;
		
		if (!newValue->value->UsesInterface("PListItem"))
			return B_BAD_VALUE;
		
		if (fListView->Window())
			fListView->Lock();
		fListView->ReplaceItem(itemIndex,newValue->GetListItem());
		
		// Just in case...
		fListView->RemoveItem(oldPItem->GetListItem());
		if (fListView->Window())
			fListView->Unlock();
		
		delete oldPItem.value;
*/		
	}
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (str.ICompare("ItemCount") == 0)
		return B_READ_ONLY;
	else if (str.ICompare("InvocationMessage") == 0)
	{
		prop->GetValue(&mv);
		fListView->SetInvocationMessage(new BMessage(mv.value));
	}
	else if (str.ICompare("SelectionMessage") == 0)
	{
		prop->GetValue(&mv);
		fListView->SetSelectionMessage(new BMessage(mv.value));
	}
	else if (str.ICompare("SelectionType") == 0)
	{
		prop->GetValue(&iv);
		fListView->SetListType((list_view_type)iv.value);
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
	fView = (view == NULL) ? new BListView(BRect(0,0,1,1),"") : view;
}


void
PListView::InitProperties(void)
{
/*
	PListView Properties:
		All PView Properties
		
		Items
		ItemCount
		InvocationMessage
		SelectionMessage
		SelectionType
*/
	StringValue sv("An item-based list control");
	SetProperty("Description",&sv);
	
	AddProperty(new IntProperty("ItemCount",0,"The number of the items in the view. Read-only"),
				PROPERTY_READ_ONLY);
	AddProperty(new MessageProperty("InvocationMessage",0,"Message sent when an item is double-clicked"));
	AddProperty(new MessageProperty("SelectionMessage",0,"Message sent when an item is selected"));
	AddProperty(new IntProperty("SelectionType",B_SINGLE_SELECTION_LIST));
}

