#include "PropEnumEditor.h"

#include "PObject.h"
#include "PProperty.h"
#include "EnumProperty.h"
#include <stdio.h>

#define M_EDIT 'edit'

PropEnumEditor::PropEnumEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fEditor(NULL),
		fMsgr(NULL)
{
	fPropName = new BString();
	BString label("EnumEditor");
	if (GetProperty())
	{
		*fPropName = GetProperty()->GetName();
		label = *fPropName;
	}
	
	EnumProperty *eprop = dynamic_cast<EnumProperty*>(prop);
	
	fMenu = new BMenu("Enum");
	fMenu->SetLabelFromMarked(true);
	fMenu->SetRadioMode(true);
	
	if (eprop)
	{
		for (int32 i = 0; i < eprop->CountValuePairs(); i++)
		{
			BMessage *msg = new BMessage(M_EDIT);
			msg->AddInt32("value",eprop->PairValueAt(i));
			fMenu->AddItem(new BMenuItem(eprop->PairNameAt(i).String(),msg));
		}
	}
	
	fEditor = new BMenuField(Bounds(),"editor",label.String(), fMenu,
							B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	

	if (eprop)
	{
		BMenuItem *item = fMenu->FindItem(eprop->GetValueAsString().String());
		if (item)
			item->SetMarked(true);
	}
}


PropEnumEditor::~PropEnumEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fEditor->Parent())
		delete fEditor;
	
	delete fPropName;
}


void
PropEnumEditor::Update(void)
{
	SetProperty(GetObject(),GetProperty());
}


status_t
PropEnumEditor::SetProperty(PObject *obj, PProperty *prop)
{
	if (!prop || !HandlesType(prop->GetType()))
		return B_BAD_VALUE;
	
	// Calling the inherited version before doing anything else is required
	PropertyEditor::SetProperty(obj,prop);
	fEditor->SetLabel(prop->GetName().String());
	fEditor->SetDivider(fEditor->StringWidth(prop->GetName().String()) + 5.0);
	
	EnumProperty *eprop = dynamic_cast<EnumProperty*>(prop);
	if (!eprop)
		return B_ERROR;
	
	*fPropName = eprop->GetName();
	
	while (fMenu->CountItems())
		delete fMenu->RemoveItem(0L);
	
	for (int32 i = 0; i < eprop->CountValuePairs(); i++)
	{
		BMessage *msg = new BMessage(M_EDIT);
		msg->AddInt32("value",eprop->PairValueAt(i));
		fMenu->AddItem(new BMenuItem(eprop->PairNameAt(i).String(),msg));
	}
	
	BMenuItem *item = fMenu->FindItem(eprop->GetValueAsString().String());
	if (item)
		item->SetMarked(true);
	
	return B_OK;
}


void
PropEnumEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	delete fMsgr;
	fMsgr = new BMessenger(handler,looper);
}


bool
PropEnumEditor::HandlesType(const BString &type)
{
	return (type.ICompare("EnumProperty") == 0);
}


PropertyEditor *
PropEnumEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new PropEnumEditor(obj,prop);
}


void
PropEnumEditor::AttachedToWindow(void)
{
	// Due to a bug in the Zeta and R5 BTextControl code, calling AddChild
	// must come BEFORE doing any of the resizing-related code
	if (!fEditor->Parent())
	{
		AddChild(fEditor);
		
		if (!Bounds().IsValid())
		{
			float w,h;
			fEditor->GetPreferredSize(&w,&h);
			
			if (Parent())
				ResizeTo(Parent()->Bounds().Width(),h + 4.0);
			else
				ResizeTo(w,h + 4.0);
			
			fEditor->ResizeTo(Bounds().Width() - 10.0, Bounds().Height() - 4.0);
			fEditor->MoveTo(5.0,2.0);
			
			float labelWidth = fEditor->StringWidth(GetProperty()->GetName().String()) + 5.0;
			if (labelWidth < fEditor->Bounds().Width() / 2.0)
				fEditor->SetDivider(labelWidth);
			else
				fEditor->SetDivider(fEditor->Bounds().Width() / 2.0);
		}
	}	
	fMenu->SetTargetForItems(this);
	delete fMsgr;
	fMsgr = new BMessenger((BHandler*)NULL,(BLooper*)Window());
}


void
PropEnumEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_EDIT:
		{
			int32 value;
			if (msg->FindInt32("value",&value) == B_OK)
				HandleEdit(value);
			break;
		}
		default:
		{
			PropertyEditor::MessageReceived(msg);
			break;
		}
	}
}


void
PropEnumEditor::HandleEdit(int32 value)
{
	PObject *obj = GetObject();
	PProperty *prop = obj->FindProperty(fPropName->String());
	obj->SetIntProperty(fPropName->String(),value);
	
	BMessage *editMsg = new BMessage(M_PROPERTY_EDITED);
	
	if (obj && prop)
	{
		editMsg->AddPointer("object",obj);
		editMsg->AddPointer("property",prop);
		editMsg->AddInt64("objectid",obj->GetID());
		editMsg->AddString("name",prop->GetName());
		*fPropName = prop->GetName();
	}
	
	fMsgr->SendMessage(editMsg);
}


PropEnumFlagEditor::PropEnumFlagEditor(PObject *obj, PProperty *prop)
	:	PropEnumEditor(obj,prop)
{
	fMenu->SetRadioMode(false);
	fMenu->SetLabelFromMarked(false);
	fMenu->Superitem()->SetLabel("Click to Edit");
	
	SetProperty(obj,prop);
}


PropEnumFlagEditor::~PropEnumFlagEditor(void)
{
}


void
PropEnumFlagEditor::Update(void)
{
	SetProperty(GetObject(),GetProperty());
}


status_t
PropEnumFlagEditor::SetProperty(PObject *obj, PProperty *prop)
{
	if (!prop || !HandlesType(prop->GetType()))
		return B_BAD_VALUE;
	
	// Calling the inherited version before doing anything else is required
	PropEnumEditor::SetProperty(obj,prop);
	EnumFlagProperty *eprop = dynamic_cast<EnumFlagProperty*>(prop);
	if (!eprop)
		return B_ERROR;
	*fPropName = eprop->GetName();
	
	while (fMenu->CountItems())
		delete fMenu->RemoveItem(0L);
	
	for (int32 i = 0; i < eprop->CountValuePairs(); i++)
	{
		BMessage *msg = new BMessage(M_EDIT);
		msg->AddInt32("value",eprop->PairValueAt(i));
		fMenu->AddItem(new BMenuItem(eprop->PairNameAt(i).String(),msg));
	}
	
	IntValue iv;
	prop->GetValue(&iv);
	int32 value = *iv.value;
	for (int32 i = 0; i < fMenu->CountItems(); i++)
	{
		BMenuItem *item = fMenu->ItemAt(i);
		int32 temp;
		if (!item || item->Message()->FindInt32("value",&temp) != B_OK)
			continue;
		
		if (value & temp)
			item->SetMarked(true);
		else
			item->SetMarked(false);
	}
	
	return B_OK;
}

PropertyEditor *
PropEnumFlagEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new PropEnumFlagEditor(obj,prop);
}


void
PropEnumFlagEditor::HandleEdit(int32 value)
{
	int32 newValue = 0;
	for (int32 i = 0; i < fMenu->CountItems(); i++)
	{
		BMenuItem *item = fMenu->ItemAt(i);
		int32 temp;
		if (!item || item->Message()->FindInt32("value",&temp) != B_OK)
			continue;
		
		if (temp == value)
		{
			if (item->IsMarked())
			{
				item->SetMarked(false);
				continue;
			}
			else
				item->SetMarked(true);
		}
		
		if (item->IsMarked())
			newValue |= temp;
	}
	
	PropEnumEditor::HandleEdit(newValue);
}


