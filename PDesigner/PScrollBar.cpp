#include "PScrollBar.h"

#include <Window.h>
#include <stdio.h>

/*
	ScrollBar Properties:
		LargeStep
		Min
		Max
		Orientation (read-only)
		Proportion
		SmallStep
		Target
		Value
*/

PScrollBar::PScrollBar(void)
	:	PView()
{
	InitPScrollBar();
}


PScrollBar::PScrollBar(BMessage *msg)
	:	PView(msg)
{
	InitPScrollBar();
}


PScrollBar::PScrollBar(const char *name)
	:	PView(name)
{
	InitPScrollBar();
}


PScrollBar::PScrollBar(const PScrollBar &from)
	:	PView(from)
{
	InitPScrollBar();
}


PScrollBar::~PScrollBar(void)
{
}


BArchivable *
PScrollBar::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PScrollBar"))
		return new PScrollBar(data);

	return NULL;
}


PObject *
PScrollBar::Create(void)
{
	return new PScrollBar();
}


PObject *
PScrollBar::Duplicate(void) const
{
	return new PScrollBar(*this);
}


status_t
PScrollBar::GetProperty(const char *name, PValue *value, const int32 &index) const
{
/*
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BScrollBar *viewAsScrollBar = (BScrollBar*)fView;
	
	if (str.ICompare("Enabled") == 0)
		((BoolProperty*)prop)->SetValue(viewAsScrollBar->IsEnabled());
	else if (str.ICompare("Label") == 0)
		((StringProperty*)prop)->SetValue(viewAsScrollBar->Label());
	else if (str.ICompare("Value") == 0)
		((IntProperty*)prop)->SetValue(viewAsScrollBar->Value());
	else
		return PView::GetProperty(name,value,index);
	
	return prop->GetValue(value);
*/ return B_ERROR;
}


status_t
PScrollBar::SetProperty(const char *name, PValue *value, const int32 &index)
{
/*
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
	
	BScrollBar *viewAsScrollBar = (BScrollBar*)fView;
	
	BoolValue bv;
	IntValue iv;
	StringValue sv;
	
	if (viewAsScrollBar->Window())
		viewAsScrollBar->Window()->Lock();
	
	if (str.ICompare("Enabled") == 0)
	{
		prop->GetValue(&bv);
		viewAsScrollBar->SetEnabled(bv.value);
	}
	else if (str.ICompare("Label") == 0)
	{
		prop->GetValue(&sv);
		viewAsScrollBar->SetLabel(sv.value->String());
		viewAsScrollBar->Invalidate();
	}
	else
	{
		if (viewAsScrollBar->Window())
			viewAsScrollBar->Window()->Unlock();
		return PView::SetProperty(name,value,index);
	}

	if (viewAsScrollBar->Window())
		viewAsScrollBar->Window()->Unlock();
	
	return prop->GetValue(value);
*/ return B_ERROR;
}


void
PScrollBar::InitBackend(BView *view)
{
	// This will be handled by child classes
}


void
PScrollBar::InitPScrollBar(void)
{
	fType = "PScrollBar";
	fFriendlyType = "Generic ScrollBar";
	AddInterface("PScrollBar");

	InitProperties();
}


void
PScrollBar::InitProperties(void)
{
/*
	ScrollBar Properties:
		LargeStep
		Min
		Max
		Orientation (read-only)
		Proportion
		SmallStep
		Target
		Value
*/
	StringValue sv("A scrollbar class. It is not used directly.");
	SetProperty("Description",&sv);
	
	AddProperty(new FloatProperty("LargeStep",10.0));
	AddProperty(new FloatProperty("Min",0.0));
	AddProperty(new FloatProperty("Max",100.0));
	AddProperty(new FloatProperty("LargeStep",10.0));
	
	EnumProperty *pEnum = new EnumProperty();
	pEnum->SetName("Orientation");
	pEnum->AddValuePair("Horizontal",B_HORIZONTAL);
	pEnum->AddValuePair("Vertical",B_VERTICAL);
	AddProperty(pEnum);
	
	AddProperty(new FloatProperty("Proportion",1.0));
	AddProperty(new FloatProperty("SmallStep",1.0));
	AddProperty(new IntProperty("Target",0));
	AddProperty(new FloatProperty("Value",0.0));
}
