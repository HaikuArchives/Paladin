#include "PObject.h"
#include "PObjectBroker.h"

#include <ClassInfo.h>
#include <stdio.h>

PObject::PObject(void)
	:	fType("PObject"),
		fFriendlyType("Generic Object")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
}


PObject::PObject(BMessage *msg)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	if (msg->FindString("type",&fType) != B_OK)
		fType = "PObject";
	
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
	
	RemoveProperty(FindProperty("ObjectID"));
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
}


PObject::PObject(const char *name)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
	AddProperty(new StringProperty("Name",name));
}


PObject::PObject(const PObject &from)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	*this = from;
	RemoveProperty(FindProperty("ObjectID"));
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
}


PObject &
PObject::operator=(const PObject &from)
{
	fPropertyList->MakeEmpty();
	for (int32 i = 0; i < from.CountProperties(); i++)
		AddProperty(from.PropertyAt(i)->Duplicate());
	fType = from.fType;
	RemoveProperty(FindProperty("ObjectID"));
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
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
	delete fEventList;
	
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


status_t
PObject::RunMethod(const char *name, PArgList &in, PArgList &out)
{
	PMethod *method = FindMethod(name);
	if (!method)
		return B_NAME_NOT_FOUND;
	
	method->Run(this, in, out);
	return B_OK;
}


PMethod *
PObject::FindMethod(const char *name)
{
	if (!name)
		return NULL;
	
	for (int32 i = 0; i < fMethodList->CountItems(); i++)
	{
		PMethod *item = fMethodList->ItemAt(i);
		if (item->GetName().ICompare(name) == 0)
			return item;
	}
	return NULL;
}


PMethod *
PObject::MethodAt(const int32 &index) const
{
	return fMethodList->ItemAt(index);
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


EventData *
PObject::EventAt(const int32 &index) const
{
	return fEventList->ItemAt(index);
}


int32
PObject::CountEvents(void) const
{
	return fEventList->CountItems();
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
PObject::AddMethod(PMethod *method)
{
	if (!method)
		return B_ERROR;
	
	if (FindMethod(method->GetName().String()))
		return B_NAME_IN_USE;
	
	fMethodList->AddItem(method);
	
	return B_OK;
}


status_t
PObject::RemoveMethod(const char *name)
{
	if (!name)
		return B_ERROR;
	
	for (int32 i = 0; i < fMethodList->CountItems(); i++)
	{
		PMethod *item = fMethodList->ItemAt(i);
		if (item->GetName().ICompare(name) == 0)
		{
			fMethodList->RemoveItemAt(i);
			return B_OK;
		}
	}
	
	return B_NAME_NOT_FOUND;
}


status_t
PObject::AddEvent(const char *name, const char *description)
{
	if (!name || !description)
		return B_NAME_IN_USE;
	
	if (FindEvent(name))
		return B_OK;
	
	EventData *data = new EventData(name, description);
	fEventList->AddItem(data);
	return B_OK;
}


status_t
PObject::RemoveEvent(const char *name)
{
	EventData *data = FindEvent(name);
	if (!data)
		return B_NAME_NOT_FOUND;
	
	return fEventList->RemoveItem(data) ? B_OK : B_ERROR;
}


EventData *
PObject::FindEvent(const char *name)
{
	if (!name)
		return NULL;
	
	for (int32 i = 0; i < fEventList->CountItems(); i++)
	{
		EventData *item = fEventList->ItemAt(i);
		if (item->name.ICompare(name) == 0)
			return item;
	}
	
	return NULL;
}


status_t
PObject::RunEvent(const char *name, PArgList &in, PArgList &out)
{
	if (!name)
		return B_ERROR;
	
	return RunEvent(FindEvent(name), in, out);
}


status_t
PObject::RunEvent(EventData *data, PArgList &in, PArgList &out)
{
	if (!data)
		return B_ERROR;
	
	if (data->hook != NullPMethod)
		return data->hook(this, &in, &out);
	
	return B_OK;
}


status_t
PObject::ConnectEvent(const char *name, MethodFunction func)
{
	EventData *data = FindEvent(name);
	if (!data)
		return B_NAME_NOT_FOUND;
	
	data->hook = func;
	return B_OK;
}


PObject *
NewObject(const char *type)
{
	PObjectBroker *owner = PObjectBroker::GetBrokerInstance();
	return owner->MakeObject(type);
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
