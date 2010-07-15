#include "PropRectEditor.h"

#include "PObject.h"
#include "PProperty.h"
#include <stdio.h>
#include <stdlib.h>

#define M_EDIT 'edit'

PropRectEditor::PropRectEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fLEditor(NULL),
		fTEditor(NULL),
		fREditor(NULL),
		fBEditor(NULL),
		fMsgr(NULL)
{
	fPropName = new BString();
	
	SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	SetFlags(Flags() | B_FRAME_EVENTS);
	BString label("RectEditor");
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
	
	fLEditor = new NumBox(Bounds(),"editor","Left:","",editMsg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	fTEditor = new NumBox(Bounds(),"editor","Top:","",editMsg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	fREditor = new NumBox(Bounds(),"editor","Right:","",editMsg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	fBEditor = new NumBox(Bounds(),"editor","Bottom:","",editMsg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	
	Update();
}


PropRectEditor::~PropRectEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fLabel->Parent())
	{
		delete fLabel;
		delete fLEditor;
		delete fTEditor;
		delete fREditor;
		delete fBEditor;
	}
	
	delete fPropName;
	
	delete fMsgr;
}


void
PropRectEditor::Update(void)
{
	if (GetProperty())
	{
		BRect r;
		GetObject()->GetRectProperty(fPropName->String(),r);
		
		char label[1024];
		sprintf(label,"%.0f",r.left);
		fLEditor->SetText(label);
		sprintf(label,"%.0f",r.top);
		fTEditor->SetText(label);
		sprintf(label,"%.0f",r.right);
		fREditor->SetText(label);
		sprintf(label,"%.0f",r.bottom);
		fBEditor->SetText(label);
	}
}


status_t
PropRectEditor::SetProperty(PObject *obj, PProperty *prop)
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
PropRectEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	delete fMsgr;
	fMsgr = new BMessenger(handler,looper);
}


bool
PropRectEditor::HandlesType(const BString &type)
{
	return (type.ICompare("RectProperty") == 0);
}


PropertyEditor *
PropRectEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropRectEditor(obj,prop);
}


void
PropRectEditor::AttachedToWindow(void)
{
	// Due to a bug in the Zeta and R5 BTextControl code, calling AddChild
	// must come BEFORE doing any of the resizing-related code
	if (!fLabel->Parent())
	{
		AddChild(fLabel);
		AddChild(fLEditor);
		AddChild(fTEditor);
		AddChild(fREditor);
		AddChild(fBEditor);
		
		if (!Bounds().IsValid())
		{
			float w,h;
			fLEditor->GetPreferredSize(&w,&h);
			
			if (Parent())
				ResizeTo(Parent()->Bounds().Width(),fLabel->Frame().Height() + (h * 2.0) + 10.0);
			else
				ResizeTo(w,fLabel->Frame().Height() + (h * 2.0) + 10.0);
			
			fLEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, h);
			fLEditor->MoveTo(5.0,fLabel->Frame().bottom + 1.0);
			
			fTEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, h);
			fTEditor->MoveTo(fLEditor->Frame().right + 4.0,fLabel->Frame().bottom + 2.0);
			
			fREditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, h);
			fREditor->MoveTo(5.0,fLEditor->Frame().bottom + 1.0);
			
			fBEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, h);
			fBEditor->MoveTo(fREditor->Frame().right + 4.0,fTEditor->Frame().bottom + 2.0);

			float labelWidth = fLEditor->StringWidth("Bottom:");
			fLEditor->SetDivider(labelWidth);
			fTEditor->SetDivider(labelWidth);
			fREditor->SetDivider(labelWidth);
			fBEditor->SetDivider(labelWidth);
		}
		fLEditor->SetTarget(this);
		fTEditor->SetTarget(this);
		fREditor->SetTarget(this);
		fBEditor->SetTarget(this);
	}
	
	delete fMsgr;
	fMsgr = new BMessenger((BHandler*)NULL,(BLooper*)Window());
}


void
PropRectEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_EDIT:
		{
			BRect r;
			r.left = atof(fLEditor->Text());
			r.top = atof(fTEditor->Text());
			r.right = atof(fREditor->Text());
			r.bottom = atof(fBEditor->Text());
			
			GetObject()->SetRectProperty(fPropName->String(),r);
			
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
PropRectEditor::FrameResized(float w, float h)
{
	float height = fLEditor->Frame().Height();
	fLEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, height);
	fTEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, height);
	fTEditor->MoveTo(fLEditor->Frame().right + 4.0,fLEditor->Frame().top);
	
	fREditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, height);
	fBEditor->ResizeTo((Bounds().Width() / 2.0) - 7.0, height);
	fBEditor->MoveTo(fREditor->Frame().right + 4.0,fREditor->Frame().top);
}


