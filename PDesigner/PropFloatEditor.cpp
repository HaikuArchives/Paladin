#include "PropFloatEditor.h"

#include "PObject.h"
#include "PProperty.h"
#include <stdio.h>

#define M_EDIT 'edit'

PropFloatEditor::PropFloatEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fEditor(NULL),
		fMsgr(NULL)
{
	fPropName = new BString();
	
	SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	BString label("IntegerEditor");
	BString text;
	if (GetProperty())
	{
		label = GetProperty()->GetName();
		text = GetProperty()->GetValueAsString();
	}
	
	BMessage *editMsg = new BMessage(M_EDIT);
	
	if (obj && prop)
	{
		editMsg->AddPointer("object",obj);
		editMsg->AddPointer("property",prop);
		editMsg->AddInt64("objectid",obj->GetID());
		editMsg->AddString("name",prop->GetName());
		*fPropName = prop->GetName();
	}
	
	fEditor = new NumBox(Bounds(),"editor",label.String(),text.String(),
									editMsg, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
}


PropFloatEditor::~PropFloatEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fEditor->Parent())
		delete fEditor;
	
	delete fPropName;
	delete fMsgr;
}


void
PropFloatEditor::Update(void)
{
	if (GetProperty())
		fEditor->SetText(GetProperty()->GetValueAsString().String());
}


status_t
PropFloatEditor::SetProperty(PObject *obj, PProperty *prop)
{
	if (!prop || !HandlesType(prop->GetType()))
		return B_BAD_VALUE;
	
	*fPropName = prop->GetName();
	
	// Calling the inherited version before doing anything else is required
	PropertyEditor::SetProperty(obj,prop);
	fEditor->SetLabel(prop->GetName().String());
	fEditor->SetDivider(fEditor->StringWidth(prop->GetName().String()) + 5.0);
	
	fEditor->SetText(prop->GetValueAsString().String());
	
	return B_OK;
}


void
PropFloatEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	delete fMsgr;
	fMsgr = new BMessenger(handler,looper);
}


bool
PropFloatEditor::HandlesType(const BString &type)
{
	return (type.ICompare("FloatProperty") == 0);
}


PropertyEditor *
PropFloatEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropFloatEditor(obj,prop);
}


void
PropFloatEditor::AttachedToWindow(void)
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
		
		fEditor->SetTarget(this);
	}
	
	delete fMsgr;
	fMsgr = new BMessenger((BHandler*)NULL,(BLooper*)Window());
}


void
PropFloatEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_EDIT:
		{
			StringValue svalue(fEditor->Text());
			GetObject()->SetProperty(fPropName->String(),&svalue);
			
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

	
