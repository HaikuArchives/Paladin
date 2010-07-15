#include "PropBoolEditor.h"

#include "PObject.h"
#include "PProperty.h"
#include <stdio.h>

#define M_EDIT 'edit'

PropBoolEditor::PropBoolEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fEditor(NULL),
		fMsgr(NULL),
		fPropName(NULL)
{
	fPropName = new BString();
	
	BString label("BoolEditor");
	if (GetProperty())
		label = GetProperty()->GetName();
	
	BMessage *editMsg = new BMessage(M_EDIT);
	
	if (obj && prop)
	{
		editMsg->AddPointer("object",obj);
		editMsg->AddPointer("property",prop);
		editMsg->AddInt64("objectid",obj->GetID());
		editMsg->AddString("name",prop->GetName());
		*fPropName = prop->GetName();
	}
	
	fEditor = new BCheckBox(Bounds(),"editor",label.String(), editMsg,
							B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	BoolProperty *bprop = dynamic_cast<BoolProperty*>(prop);
	if (bprop)
	{
		BoolValue bvalue;
		bprop->GetValue(&bvalue);
		fEditor->SetValue(bvalue.value ? B_CONTROL_ON : B_CONTROL_OFF);
	}
}


PropBoolEditor::~PropBoolEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fEditor->Parent())
		delete fEditor;
	
	delete fPropName;
	
	delete fMsgr;
}


void
PropBoolEditor::Update(void)
{
	if (GetProperty())
	{
		BoolValue bv;
		GetProperty()->GetValue(&bv);
		fEditor->SetValue(*bv.value ? B_CONTROL_ON : B_CONTROL_OFF);
	}	
}


status_t
PropBoolEditor::SetProperty(PObject *obj, PProperty *prop)
{
	if (!prop || !HandlesType(prop->GetType()))
		return B_BAD_VALUE;
	
	// Calling the inherited version before doing anything else is required
	PropertyEditor::SetProperty(obj,prop);
	fEditor->SetLabel(prop->GetName().String());
	*fPropName = prop->GetName();
	
	BoolValue bv;
	prop->GetValue(&bv);
	fEditor->SetValue(*bv.value ? B_CONTROL_ON : B_CONTROL_OFF);
	
	return B_OK;
}


void
PropBoolEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	delete fMsgr;
	fMsgr = new BMessenger(handler,looper);
}


bool
PropBoolEditor::HandlesType(const BString &type)
{
	return (type.ICompare("BoolProperty") == 0);
}


PropertyEditor *
PropBoolEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropBoolEditor(obj,prop);
}


void
PropBoolEditor::AttachedToWindow(void)
{
	if (!fEditor->Parent())
	{
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
		}
		AddChild(fEditor);
		fEditor->SetTarget(this);
	}
	
	delete fMsgr;
	fMsgr = new BMessenger((BHandler*)NULL,(BLooper*)Window());
}


void
PropBoolEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_EDIT:
		{
			GetObject()->SetBoolProperty(fPropName->String(),fEditor->Value() == B_CONTROL_ON);
			
			BMessage editMsg(*msg);
			editMsg.what = M_PROPERTY_EDITED;
			fMsgr->SendMessage(&editMsg);
			break;
		}
		default:
		{
			PropertyEditor::MessageReceived(msg);
			break;
		}
	}
}

	
