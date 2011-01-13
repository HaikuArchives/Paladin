#include "PData.h"
#include <stdio.h>
#include <ClassInfo.h>

PData::PData(void)
	:	fType("PData"),
		fFriendlyType("Generic Data Container")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
}


PData::PData(BMessage *msg)
	:	fType("PData")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	
	if (msg->FindString("type",&fType) != B_OK)
		fType = "PData";
	
	int32 i = 0; 
	BMessage propmsg;
	while (msg->FindMessage("property",i++,&propmsg) == B_OK)
	{
		BString ptype;
		if (propmsg.FindString("type",&ptype) != B_OK)
			continue;
		PProperty *p = gPropertyRoster.MakeProperty(ptype.String(),&propmsg);
		if (p)
			AddProperty(p);
	}
}


PData::PData(const char *name)
	:	fType("PData")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
}


PData::PData(const PData &from)
	:	fType("PData")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	*this = from;
}


PData &
PData::operator=(const PData &from)
{
	fPropertyList->MakeEmpty();
	for (int32 i = 0; i < from.CountProperties(); i++)
		AddProperty(from.PropertyAt(i)->Duplicate());
	fType = from.fType;
	return *this;
}


PProperty *
PData::operator[](const char *name)
{
	return FindProperty(name);
}


PProperty *
PData::operator[](const BString &name)
{
	return FindProperty(name);
}


PData::~PData(void)
{
	delete fPropertyList;
}


PData *
PData::Create(void)
{
	return new PData();
}


PData *
PData::Duplicate(void) const
{
	return new PData(*this);
}

	
BArchivable *
PData::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PData"))
		return new PData(data);

	return NULL;
}


status_t
PData::Archive(BMessage *data, bool deep) const
{
	status_t status = BArchivable::Archive(data, deep);
	status = data->AddString("type",fType);
	if (status != B_OK)
		return status;
	
	for (int32 i = 0; i < CountProperties(); i++)
	{
		PropertyData *d = fPropertyList->ItemAt(i);
		BMessage msg;
		d->value->Archive(&msg);
		status = data->AddMessage("property",&msg);
		if (status != B_OK)
			return status;
		status = data->AddInt32("propertyflags",d->flags);
		if (status != B_OK)
			return status;
	}
	
	if (status == B_OK)
		status = data->AddString("class",fType.String());
	
	return status;
}


int32
PData::CountProperties(const char *name) const
{
	int32 count = fPropertyList->CountItems();
	if (!name)
		return count;
	
	int32 propertyCount = 0;
	for (int32 i = 0; i < count; i++)
	{
		PProperty *p = PropertyAt(i);
		if (p->GetName().ICompare(name) == 0)
			propertyCount++;
	}
	
	return propertyCount;
}


PProperty *
PData::PropertyAt(const int32 &index) const
{
	PropertyData *d = fPropertyList->ItemAt(index);
	return d ? d->value : NULL;
}


int32
PData::IndexOfProperty(PProperty *p) const
{
	if (!p)
		return -1;
	
	for (int32 i = 0; i < fPropertyList->CountItems(); i++)
	{
		PProperty *item = PropertyAt(i);
		if (item == p)
			return i;
	}
	
	return -1;
}


PProperty *
PData::FindProperty(const BString &name, const int32 &index) const
{
	if (name.CountChars() < 1)
		return NULL;
	
	int32 current = 0;
	for (int32 i = 0; i < fPropertyList->CountItems(); i++)
	{
		PProperty *p = PropertyAt(i);
		if (p->GetName().Compare(name) == 0)
		{
			if (current == index)
				return p;
			else
				current++;
		}
	}
	
	return NULL;
}


PProperty *
PData::FindProperty(const char *name, const int32 &index) const
{
	return FindProperty(BString(name),index);
}


bool
PData::AddProperty(PProperty *p, uint32 flags, int32 index)
{
	if (!p)
		return false;
	
	if (FindProperty(p->GetName()) && ((FlagsForProperty(p) & PROPERTY_ALLOW_MULTIPLE) == 0))
		return false;
	
	if (index >= 0)
		fPropertyList->AddItem(new PropertyData(p,flags),index);
	else
		fPropertyList->AddItem(new PropertyData(p,flags));
	return true;
}


PProperty *
PData::RemoveProperty(const int32 &index)
{
	PropertyData *d = fPropertyList->RemoveItemAt(index);
	if (d)
	{
		PProperty *p = d->value;
		d->value = NULL;
		delete d;
		return p;
	}
	
	return NULL;
}


void
PData::RemoveProperty(PProperty *p)
{
	int32 index = IndexOfProperty(p);
	if (index < 0)
		return;
	fPropertyList->RemoveItemAt(index);
}


uint32
PData::PropertyFlagsAt(const int32 &index) const
{
	PropertyData *d = fPropertyList->ItemAt(index);
	return d ? d->flags : 0;
}


void
PData::SetFlagsForProperty(PProperty *p, const int32 &flags)
{
	PropertyData *pdata = fPropertyList->ItemAt(IndexOfProperty(p));
	pdata->flags = flags;
}


uint32
PData::FlagsForProperty(PProperty *p) const
{
	return PropertyFlagsAt(IndexOfProperty(p));
}


status_t
PData::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	PProperty *p = FindProperty(name,index);
	if (!p)
		return B_NAME_NOT_FOUND;
	
	return p->SetValue(value);
}


status_t
PData::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	PProperty *p = FindProperty(name,index);
	if (!p)
		return B_NAME_NOT_FOUND;
	
	return p->GetValue(value);
}


status_t
PData::SetStringProperty(const char *name, const char *value, const int32 &index)
{
	StringValue v(value);
	return SetProperty(name,&v,index);
}


status_t
PData::SetIntProperty(const char *name, const int64 &value, const int32 &index)
{
	IntValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PData::SetBoolProperty(const char *name, const bool &value, const int32 &index)
{
	BoolValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PData::SetFloatProperty(const char *name, const float &value, const int32 &index)
{
	FloatValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PData::SetRectProperty(const char *name, const BRect &value, const int32 &index)
{
	RectValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PData::SetPointProperty(const char *name, const BPoint &value, const int32 &index)
{
	PointValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PData::SetColorProperty(const char *name, const rgb_color &value, const int32 &index)
{
	ColorValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PData::SetColorProperty(const char *name, const uint8 &red, const uint8 &green,
						const uint8 &blue, const uint8 &alpha,const int32 &index)
{
	ColorValue v(red,green,blue,alpha);
	return SetProperty(name, &v, index);
}


status_t
PData::GetStringProperty(const char *name, BString &value, const int32 &index)
{
	StringValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PData::GetIntProperty(const char *name, int64 &value, const int32 &index)
{
	IntValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PData::GetBoolProperty(const char *name, bool &value, const int32 &index)
{
	BoolValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PData::GetFloatProperty(const char *name, float &value, const int32 &index)
{
	FloatValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PData::GetRectProperty(const char *name, BRect &value, const int32 &index)
{
	RectValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PData::GetPointProperty(const char *name, BPoint &value, const int32 &index)
{
	PointValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PData::GetColorProperty(const char *name, rgb_color &value, const int32 &index)
{
	ColorValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


BString
PData::GetType(void) const
{
	return fType;
}

			
BString
PData::GetFriendlyType(void) const
{
	return fFriendlyType;
}

			
void
PData::PrintToStream(void)
{
	for (int32 i = 0; i < fPropertyList->CountItems(); i++)
	{
		PProperty *p = PropertyAt(i);
		printf("Property: Name is %s, Type is %s, Value is %s\n",p->GetName().String(),
				p->GetType().String(), p->GetValueAsString().String());
	}
}
