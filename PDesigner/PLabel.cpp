#include "PLabel.h"

#include <Application.h>
#include <Window.h>

#include "EnumProperty.h"
#include "MsgDefs.h"

class PLabelBackend : public BStringView
{
public:
			PLabelBackend(PObject *owner);
	void	MakeFocus(bool value);
	void	MouseUp(BPoint pt);

private:
	PObject	*fOwner;
};

PLabel::PLabel(void)
	:	PView()
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitProperties();
	InitBackend();
}


PLabel::PLabel(BMessage *msg)
	:	PView(msg)
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BStringView::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PLabel::PLabel(const char *name)
	:	PView(name)
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitProperties();
	InitBackend();
}


PLabel::PLabel(const PLabel &from)
	:	PView(from)
{
	fType = "PLabel";
	fFriendlyType = "Label";
	AddInterface("PLabel");
	
	InitProperties();
	InitBackend();
}


PLabel::~PLabel(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PLabel::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PLabel"))
		return new PLabel(data);

	return NULL;
}


status_t
PLabel::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BStringView *fLabel = (BStringView*)fView;
	
	if (str.ICompare("Alignment") == 0)
		((EnumProperty*)prop)->SetValue(fLabel->Alignment());
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(fLabel->Text());
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		
		return PObject::GetProperty(name,value,index);
	}

	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PLabel::SetProperty(const char *name, PValue *value, const int32 &index)
{
/*
	PLabel Properties:
		All PView Properties
		
		Alignment
		Text
*/
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	IntValue iv;
	StringValue sv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BStringView *fLabel = (BStringView*)fView;
	
	if (str.ICompare("Alignment") == 0)
	{
		prop->GetValue(&iv);
		fLabel->SetAlignment((alignment)*iv.value);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		fLabel->SetText(sv.value->String());
		fLabel->Invalidate();
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
PLabel::Create(void)
{
	return new PLabel();
}


PObject *
PLabel::Duplicate(void) const
{
	return new PLabel(*this);
}

void
PLabel::InitBackend(BView *view)
{
	fView = (view == NULL) ? new PLabelBackend(this) : view;
	StringValue sv("A basic button object. It sends a message when clicked.");
	SetProperty("Description",&sv);
}


void
PLabel::InitProperties(void)
{
/*
	PLabel Properties:
		All PView Properties
		
		Alignment
		Text
*/

	StringValue sv("A string label");
	SetProperty("Description", &sv);
	
	EnumProperty *prop = new EnumProperty();
	prop->SetName("Alignment");
	prop->AddValuePair("Left", B_ALIGN_LEFT);
	prop->AddValuePair("Right", B_ALIGN_RIGHT);
	prop->AddValuePair("Center", B_ALIGN_CENTER);
	prop->SetValue((int32)B_ALIGN_LEFT);
	AddProperty(prop);
	
	AddProperty(new StringProperty("Text", "", "The label's text"));
}


PLabelBackend::PLabelBackend(PObject *owner)
	:	BStringView(BRect(0,0,1,1),"", ""),
		fOwner(owner)
{
}


void
PLabelBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BStringView::MakeFocus(value);
}


void
PLabelBackend::MouseUp(BPoint pt)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
	
	BStringView::MouseUp(pt);
}

