#include "PControl.h"
#include <Window.h>
#include <stdio.h>

#include "CInterface.h"
#include "PArgs.h"

/*
	Control Properties:
		Enabled
		Label
		Value
*/

PControl::PControl(void)
	:	PView()
{
	InitPControl();
}


PControl::PControl(BMessage *msg)
	:	PView(msg)
{
	InitPControl();
}


PControl::PControl(const char *name)
	:	PView(name)
{
	InitPControl();
}


PControl::PControl(const PControl &from)
	:	PView(from)
{
	InitPControl();
}


PControl::~PControl(void)
{
}


BArchivable *
PControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PControl"))
		return new PControl(data);

	return NULL;
}


PObject *
PControl::Create(void)
{
	return new PControl();
}


PObject *
PControl::Duplicate(void) const
{
	return new PControl(*this);
}


status_t
PControl::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BControl *viewAsControl = (BControl*)fView;
	
	if (str.ICompare("Enabled") == 0)
		((BoolProperty*)prop)->SetValue(viewAsControl->IsEnabled());
	else if (str.ICompare("Label") == 0)
		((StringProperty*)prop)->SetValue(viewAsControl->Label());
	else if (str.ICompare("Value") == 0)
		((IntProperty*)prop)->SetValue(viewAsControl->Value());
	else
		return PView::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PControl::SetProperty(const char *name, PValue *value, const int32 &index)
{
	// Modal, Front, and Floating properties are missing because they are read-only
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	BControl *viewAsControl = (BControl*)fView;
	
	BoolValue bv;
	IntValue iv;
	StringValue sv;
	
	if (viewAsControl->Window())
		viewAsControl->Window()->Lock();
	
	if (str.ICompare("Enabled") == 0)
	{
		prop->GetValue(&bv);
		viewAsControl->SetEnabled(bv.value);
	}
	else if (str.ICompare("Label") == 0)
	{
		prop->GetValue(&sv);
		viewAsControl->SetLabel(sv.value->String());
		viewAsControl->Invalidate();
	}
	else if (str.ICompare("Value") == 0)
	{
		prop->GetValue(&iv);
		viewAsControl->SetValue(*iv.value);
	}
	else
	{
		if (viewAsControl->Window())
			viewAsControl->Window()->Unlock();
		return PView::SetProperty(name,value,index);
	}

	if (viewAsControl->Window())
		viewAsControl->Window()->Unlock();
	
	return prop->GetValue(value);
}


void
PControl::InitBackend(BView *view)
{
	fView = (view == NULL) ? new BControl(BRect(0,0,1,1),"", "", new BMessage, 
											B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW) : 
											(BControl*) view;
}


void
PControl::InitPControl(void)
{
	fType = "PControl";
	fFriendlyType = "Generic Control";
	AddInterface("PControl");

	InitProperties();
}


void
PControl::InitProperties(void)
{
/*
	Control Properties:
		Enabled
		Label
		Value
*/
	StringValue sv("The base control class. It shouldn't normally be created by itself.");
	SetProperty("Description",&sv);
	
	AddProperty(new BoolProperty("Enabled",true));
	AddProperty(new StringProperty("Label",""));
	AddProperty(new IntProperty("Value",0));
	
	AddEvent("Invoke", "The control was activated and is to send a message.");
}


