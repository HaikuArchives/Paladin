#include "PropPointEditor.h"

#include "PObject.h"
#include "PProperty.h"
#include <stdio.h>
#include <stdlib.h>

#define M_EDIT 'edit'

PropPointEditor::PropPointEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fXEditor(NULL),
		fYEditor(NULL),
		fMsgr(NULL)
{
	fPropName = new BString();
	
	SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	SetFlags(Flags() | B_FRAME_EVENTS);
	BString label("PointEditor");
	BString text;
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
	
	fLabel = new BStringView(BRect(0,2,1,3),"label",label.String(),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fLabel->ResizeToPreferred();
	
	fXEditor = new NumBox(Bounds(),"editor","X:","",editMsg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	fYEditor = new NumBox(Bounds(),"editor","Y:","",editMsg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	
	Update();
}


PropPointEditor::~PropPointEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fLabel->Parent())
	{
		delete fXEditor;
		delete fYEditor;
	}
	
	delete fPropName;
	
	delete fMsgr;
}


void
PropPointEditor::Update(void)
{
	if (GetProperty())
	{
		BPoint pt;
		GetObject()->GetPointProperty(fPropName->String(),pt);
		
		char label[1024];
		sprintf(label,"%.0f",pt.x);
		fXEditor->SetText(label);
		sprintf(label,"%.0f",pt.y);
		fYEditor->SetText(label);
	}
}


status_t
PropPointEditor::SetProperty(PObject *obj, PProperty *prop)
{
	if (!prop || !HandlesType(prop->GetType()))
		return B_BAD_VALUE;
	*fPropName = prop->GetName();
	
	// Calling the inherited version before doing anything else is required
	PropertyEditor::SetProperty(obj,prop);
	fLabel->SetText(prop->GetName().String());
	Update();
	return B_OK;
}


void
PropPointEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	delete fMsgr;
	fMsgr = new BMessenger(handler,looper);
}


bool
PropPointEditor::HandlesType(const BString &type)
{
	return (type.ICompare("PointProperty") == 0);
}


PropertyEditor *
PropPointEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropPointEditor(obj,prop);
}


void
PropPointEditor::AttachedToWindow(void)
{
	// Due to a bug in the Zeta and R5 BTextControl code, calling AddChild
	// must come BEFORE doing any of the resizing-related code
	if (!fLabel->Parent())
	{
		AddChild(fLabel);
		AddChild(fXEditor);
		AddChild(fYEditor);
		
		if (!Bounds().IsValid())
		{
			float w,h;
			fXEditor->GetPreferredSize(&w,&h);
			
			if (Parent())
				ResizeTo(Parent()->Bounds().Width(),(h * 2.0) + 4.0);
			else
				ResizeTo(w,h + 4.0);
			
			fXEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, h);
			fXEditor->MoveTo(5.0,fLabel->Frame().bottom + 1.0);
			
			fYEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, h);
			fYEditor->MoveTo(fXEditor->Frame().right + 4.0,fLabel->Frame().bottom + 1.0);
			
			float labelWidth = fXEditor->StringWidth("X:");
			fXEditor->SetDivider(labelWidth);
			fYEditor->SetDivider(labelWidth);
		}
		fXEditor->SetTarget(this);
		fYEditor->SetTarget(this);
	}
	
	delete fMsgr;
	fMsgr = new BMessenger((BHandler*)NULL,(BLooper*)Window());
}


void
PropPointEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_EDIT:
		{
			BPoint pt;
			pt.x = atof(fXEditor->Text());
			pt.y = atof(fYEditor->Text());
			
			GetObject()->SetPointProperty(fPropName->String(),pt);
			
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

	
void
PropPointEditor::FrameResized(float w, float h)
{
	fXEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, fXEditor->Frame().Height());
	fYEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, fYEditor->Frame().Height());
	fYEditor->MoveTo(fXEditor->Frame().right + 4.0,fXEditor->Frame().top);
}


