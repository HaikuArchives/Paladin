#include "PropColorEditor.h"

#include "ColorWell.h"
#include "MsgDefs.h"
#include "PObject.h"
#include "PProperty.h"

#define M_EDIT 'edit'

PropColorEditor::PropColorEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fLabel(NULL),
		fColorWell(NULL),
		fMsgr(NULL)
{
	BString label("ColorEditor");
	if (GetProperty())
		label = GetProperty()->GetName();
	
	fLabel = new BStringView(BRect(5,0,6,1),"label",label.String(),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fColorWell = new ColorWell(BRect(0,0,1,1),"colorwell",new BMessage(M_EDIT),
								B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	if (obj && prop)
	{
		rgb_color col;
		obj->GetColorProperty(prop->GetName().String(),col);
		fColorWell->SetValue(col);
	}
}


PropColorEditor::~PropColorEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fLabel->Parent())
	{
		delete fLabel;
		delete fColorWell;
	}
	
	delete fMsgr;
}


void
PropColorEditor::Update(void)
{
	if (GetProperty())
	{
		rgb_color c;
		GetObject()->GetColorProperty(GetProperty()->GetName().String(),c);
		fColorWell->SetValue(c);
	}
}


status_t
PropColorEditor::SetProperty(PObject *obj, PProperty *prop)
{
	if (!prop || !HandlesType(prop->GetType()))
		return B_BAD_VALUE;
	
	// Calling the inherited version before doing anything else is required
	PropertyEditor::SetProperty(obj,prop);
	fLabel->SetText(prop->GetName().String());
	
	ColorValue cv;
	prop->GetValue(&cv);
	fColorWell->SetValue(*cv.value);
	
	return B_OK;
}


void
PropColorEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	delete fMsgr;
	fMsgr = new BMessenger(handler,looper);
}


bool
PropColorEditor::HandlesType(const BString &type)
{
	return (type.ICompare("ColorProperty") == 0);
}


PropertyEditor *
PropColorEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropColorEditor(obj,prop);
}


void
PropColorEditor::AttachedToWindow(void)
{
	if (!fLabel->Parent())
	{
		if (!Bounds().IsValid())
		{
			float w,h;
			fLabel->GetPreferredSize(&w,&h);
			ResizeTo(Window()->Bounds().Width(),h);
			
			fLabel->ResizeTo(MIN(w,(Bounds().Width() / 2.0) - 5.0), h);
			fColorWell->ResizeTo((Bounds().Width() / 2.0) - 5.0, h);
			fColorWell->MoveTo(Bounds().Width() / 2.0,0);
		}
		AddChild(fLabel);
		AddChild(fColorWell);
		fColorWell->SetTarget(this);
	}
	
	delete fMsgr;
	fMsgr = new BMessenger((BHandler*)NULL,(BLooper*)Window());
}


void
PropColorEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_EDIT:
		{
			rgb_color color = fColorWell->ValueAsColor();
			
			GetObject()->SetColorProperty(GetProperty()->GetName().String(),color);
			
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


