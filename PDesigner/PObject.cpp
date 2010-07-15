#include "PObject.h"
#include "PObjectBroker.h"
#include <stdio.h>
#include <ClassInfo.h>

PObject::PObject(void)
	:	fType("PObject"),
		fFriendlyType("Generic Object")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<BString>(20,true);
	
	PObjectBroker::RegisterObject(this);
}


PObject::PObject(BMessage *msg)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<BString>(20,true);
	
	PObjectBroker::RegisterObject(this);
	if (msg->FindString("type",&fType) != B_OK)
		fType = "PObject";
	
	int32 i = 0; 
	BMessage propmsg;
	PObjectBroker const *owner = PObjectBroker::GetBrokerInstance();
	while (msg->FindMessage("property",i++,&propmsg) == B_OK)
	{
		BString ptype;
		if (propmsg.FindString("type",&ptype) != B_OK)
			continue;
		PProperty *p = owner->MakeProperty(ptype.String(),&propmsg);
		if (p)
			AddProperty(p);
	}
}


PObject::PObject(const char *name)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<BString>(20,true);
	
	PObjectBroker::RegisterObject(this);
	AddProperty(new StringProperty("Name",name));
}


PObject::PObject(const PObject &from)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<BString>(20,true);
	
	PObjectBroker::RegisterObject(this);
	*this = from;
}


PObject &
PObject::operator=(const PObject &from)
{
	fPropertyList->MakeEmpty();
	for (int32 i = 0; i < from.CountProperties(); i++)
		AddProperty(from.PropertyAt(i)->Duplicate());
	fType = from.fType;
	return *this;
}


PProperty *
PObject::operator[](const char *name)
{
	return FindProperty(name);
}


PProperty *
PObject::operator[](const BString &name)
{
	return FindProperty(name);
}


PObject::~PObject(void)
{
	delete fPropertyList;
	delete fInterfaceList;
	delete fMethodList;
	
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	broker->UnregisterObject(this);
}


PObject *
PObject::Create(void)
{
	return new PObject();
}


PObject *
PObject::Duplicate(void) const
{
	return new PObject(*this);
}

	
BArchivable *
PObject::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PObject"))
		return new PObject(data);

	return NULL;
}


status_t
PObject::Archive(BMessage *data, bool deep) const
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


uint64
PObject::GetID(void) const
{
	return fObjectID;
}


int32
PObject::CountProperties(const char *name) const
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
PObject::PropertyAt(const int32 &index) const
{
	PropertyData *d = fPropertyList->ItemAt(index);
	return d ? d->value : NULL;
}


int32
PObject::IndexOfProperty(PProperty *p) const
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
PObject::FindProperty(const BString &name, const int32 &index) const
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
PObject::FindProperty(const char *name, const int32 &index) const
{
	return FindProperty(BString(name),index);
}


bool
PObject::AddProperty(PProperty *p, uint32 flags, int32 index)
{
	if (!p)
		return false;
	
	if (FindProperty(p->GetName()) && (FlagsForProperty(p) & PROPERTY_ALLOW_MULTIPLE == 0))
		return false;
	
	if (index >= 0)
		fPropertyList->AddItem(new PropertyData(p,flags),index);
	else
		fPropertyList->AddItem(new PropertyData(p,flags));
	return true;
}


PProperty *
PObject::RemoveProperty(const int32 &index)
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
PObject::RemoveProperty(PProperty *p)
{
	int32 index = IndexOfProperty(p);
	if (index < 0)
		return;
	fPropertyList->RemoveItemAt(index);
}


uint32
PObject::PropertyFlagsAt(const int32 &index) const
{
	PropertyData *d = fPropertyList->ItemAt(index);
	return d ? d->flags : 0;
}


void
PObject::SetFlagsForProperty(PProperty *p, const int32 &flags)
{
	PropertyData *pdata = fPropertyList->ItemAt(IndexOfProperty(p));
	pdata->flags = flags;
}


uint32
PObject::FlagsForProperty(PProperty *p) const
{
	return PropertyFlagsAt(IndexOfProperty(p));
}


status_t
PObject::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	PProperty *p = FindProperty(name,index);
	if (!p)
		return B_NAME_NOT_FOUND;
	
	return p->SetValue(value);
}


status_t
PObject::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	PProperty *p = FindProperty(name,index);
	if (!p)
		return B_NAME_NOT_FOUND;
	
	return p->GetValue(value);
}


status_t
PObject::SetStringProperty(const char *name, const char *value, const int32 &index)
{
	StringValue v(value);
	return SetProperty(name,&v,index);
}


status_t
PObject::SetIntProperty(const char *name, const int64 &value, const int32 &index)
{
	IntValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PObject::SetBoolProperty(const char *name, const bool &value, const int32 &index)
{
	BoolValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PObject::SetFloatProperty(const char *name, const float &value, const int32 &index)
{
	FloatValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PObject::SetRectProperty(const char *name, const BRect &value, const int32 &index)
{
	RectValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PObject::SetPointProperty(const char *name, const BPoint &value, const int32 &index)
{
	PointValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PObject::SetColorProperty(const char *name, const rgb_color &value, const int32 &index)
{
	ColorValue v(value);
	return SetProperty(name, &v, index);
}


status_t
PObject::SetColorProperty(const char *name, const uint8 &red, const uint8 &green,
						const uint8 &blue, const uint8 &alpha,const int32 &index)
{
	ColorValue v(red,green,blue,alpha);
	return SetProperty(name, &v, index);
}


status_t
PObject::GetStringProperty(const char *name, BString &value, const int32 &index)
{
	StringValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::GetIntProperty(const char *name, int64 &value, const int32 &index)
{
	IntValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::GetBoolProperty(const char *name, bool &value, const int32 &index)
{
	BoolValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::GetFloatProperty(const char *name, float &value, const int32 &index)
{
	FloatValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::GetRectProperty(const char *name, BRect &value, const int32 &index)
{
	RectValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::GetPointProperty(const char *name, BPoint &value, const int32 &index)
{
	PointValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::GetColorProperty(const char *name, rgb_color &value, const int32 &index)
{
	ColorValue v;
	status_t status = GetProperty(name,&v,index);
	if (status == B_OK)
		value = *v.value;
	return status;
}


status_t
PObject::RunMethod(const char *name, const BMessage &args, BMessage &outdata)
{
	return B_OK;
}


BString
PObject::MethodAt(const int32 &index) const
{
	BString *method = fMethodList->ItemAt(index);
	return method ? *method : BString();
}


int32
PObject::CountMethods(void) const
{
	return fMethodList->CountItems();
}


BString
PObject::GetType(void) const
{
	return fType;
}

			
BString
PObject::GetFriendlyType(void) const
{
	return fFriendlyType;
}

			
bool
PObject::UsesInterface(const char *name)
{
	if (!name)
		return false;
	
	for (int32 i = 0; i < fInterfaceList->CountItems(); i++)
	{
		BString *str = fInterfaceList->ItemAt(i);
		if (str && str->ICompare(name) == 0)
			return true;
	}
	return false;
}


bool
PObject::UsesInterface(const BString &name)
{
	return UsesInterface(name.String());
}


BString
PObject::InterfaceAt(const int32 &index) const
{
	BString *str = fInterfaceList->ItemAt(index);
	return str ? *str : BString();
}


int32
PObject::CountInterfaces(void) const
{
	return fInterfaceList->CountItems();
}


void
PObject::PrintToStream(void)
{
	printf("Object:\nInterfaces:\n");
	for (int32 i = 0; i < CountInterfaces(); i++)
		printf("\t%s\n",InterfaceAt(i).String());
	
	for (int32 i = 0; i < fPropertyList->CountItems(); i++)
	{
		PProperty *p = PropertyAt(i);
		printf("Property: Name is %s, Type is %s, Value is %s\n",p->GetName().String(),
				p->GetType().String(), p->GetValueAsString().String());
	}
}


void
PObject::AddInterface(const char *name)
{
	if (!name || UsesInterface(name))
		return;
	
	fInterfaceList->AddItem(new BString(name));
}


void
PObject::RemoveInterface(const char *name)
{
	if (!name)
		return;
	
	for (int32 i = 0; i < fInterfaceList->CountItems(); i++)
	{
		BString *str = fInterfaceList->ItemAt(i);
		if (str && str->ICompare(name) == 0)
		{
			fInterfaceList->RemoveItemAt(i);
			delete str;
			return;
		}
	}
}


status_t
PObject::AddMethod(const char *name)
{
	if (!name)
		return B_ERROR;
	
	for (int32 i = 0; i < fMethodList->CountItems(); i++)
	{
		BString *item = fMethodList->ItemAt(i);
		if (item->ICompare(name) == 0)
			return B_NAME_IN_USE;
	}
	
	fMethodList->AddItem(new BString(name));
	
	return B_OK;
}


status_t
PObject::RemoveMethod(const char *name)
{
	if (!name)
		return B_ERROR;
	
	for (int32 i = 0; i < fMethodList->CountItems(); i++)
	{
		BString *item = fMethodList->ItemAt(i);
		if (item->ICompare(name) == 0)
		{
			fMethodList->RemoveItemAt(i);
			return B_OK;
		}
	}
	
	return B_NAME_NOT_FOUND;
}


PObject *
NewObject(const char *type)
{
	PObjectBroker *owner = PObjectBroker::GetBrokerInstance();
	return owner->MakeObject(type);
}


PProperty *	NewProperty(const char *type)
{
	PObjectBroker *owner = PObjectBroker::GetBrokerInstance();
	return owner->MakeProperty(type);
}


PObject *
UnflattenObject(BMessage *msg)
{
	PObjectBroker *owner = PObjectBroker::GetBrokerInstance();
	BString type;
	if (msg->FindString("class",0,&type) != B_OK)
		return NULL;
	return owner->MakeObject(type.String(),msg);
}
