#include "PTextControl.h"

#include <stdio.h>
#include <Window.h>

#include <TextControl.h>

class PTextControlBackend : public AutoTextControl
{
public:
			PTextControlBackend(PObject *owner);
	void	AttachedToWindow(void);

private:
	PObject 	*fOwner;
	
};

PTextControl::PTextControl(void)
	:	PControl()
{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	// Unlike the other constructors, this one needs to init the properties
	InitProperties();
	InitBackend();
}


PTextControl::PTextControl(BMessage *msg)
	:	PControl(msg)
{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)AutoTextControl::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PTextControl::PTextControl(const char *name)
	:	PControl(name)
{
	fType = "PTextControl";
	fFriendlyType = "Checkbox";
	AddInterface("PTextControl");
	InitBackend();
}


PTextControl::PTextControl(const PTextControl &from)
	:	PControl(from)
{
	fType = "PTextControl";
	fFriendlyType = "Checkbox";
	AddInterface("PTextControl");
	InitBackend();
}


PTextControl::~PTextControl(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PTextControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTextControl"))
		return new PTextControl(data);

	return NULL;
}


status_t
PTextControl::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
		
	if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(control->Text());
	else if (str.ICompare("TextAlignment") == 0)
	{
		alignment label, text;
		control->GetAlignment(&label,&text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		alignment label, text;
		control->GetAlignment(&label,&text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("Divider") == 0)
		((FloatProperty*)prop)->SetValue(control->Divider());
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PControl::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PTextControl::SetProperty(const char *name, PValue *value, const int32 &index)
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
	
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
	
	if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		control->SetText(sv.value->String());
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		prop->GetValue(&iv);
		
		alignment label,text;
		control->GetAlignment(&label,&text);
		label = (alignment)*iv.value;
		control->SetAlignment(label,text);
	}
	else if (str.ICompare("TextAlignment") == 0)
	{
		prop->GetValue(&iv);
		
		alignment label,text;
		control->GetAlignment(&label,&text);
		text = (alignment)*iv.value;
		control->SetAlignment(label,text);
	}
	else if (str.ICompare("Divider") == 0)
	{
		prop->GetValue(&fv);
		control->SetDivider(*fv.value);
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PControl::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}

PObject *
PTextControl::Create(void)
{
	return new PTextControl();
}


PObject *
PTextControl::Duplicate(void) const
{
	return new PTextControl(*this);
}


status_t
PTextControl::RunMethod(const char *name, const BMessage &args, BMessage &outdata)
{
	// Methods:
	// SetPreferredDivider
	
	if (!name || strlen(name) < 1)
		return B_NAME_NOT_FOUND;
	
	BString str(name);
	
	if (str.ICompare("SetPreferredDivider") == 0)
		return DoSetPreferredDivider();
	else
		return PObject::RunMethod(name,args,outdata);
}


void
PTextControl::InitBackend(BView *view)
{
	fView = (view == NULL) ? new AutoTextControl(BRect(0,0,1,1),"", "", "", new BMessage) : view;
	StringValue sv("A single line text box. It sends a message when its value changes.");
	SetProperty("Description",&sv);
}


void
PTextControl::InitProperties(void)
{
	AddProperty(new StringProperty("Text",""));
	AddProperty(new FloatProperty("Divider",20.0));
	AddProperty(new IntProperty("LabelAlignment",B_ALIGN_LEFT));
	AddProperty(new IntProperty("TextAlignment",B_ALIGN_LEFT));
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
	
	AddMethod("SetPreferredDivider", METHOD_SHOW_IN_EDITOR);
}


status_t
PTextControl::DoSetPreferredDivider(void)
{
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
	if (!fView)
		return B_ERROR;
	
	if (strlen(control->Label()) > 0)
		control->SetDivider(control->StringWidth(control->Label()));
	else
		control->SetDivider(0.0);
	
	return B_OK;
}


PTextControlBackend::PTextControlBackend(PObject *owner)
	:	AutoTextControl(BRect(0,0,1,1),"","","",new BMessage()),
		fOwner(owner)
{
}

void
PTextControlBackend::AttachedToWindow(void)
{
	SetDivider(0.0);
}

