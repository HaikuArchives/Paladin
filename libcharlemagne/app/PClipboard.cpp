#include "PClipboard.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

PClipboard::PClipboard(void)
	:	PObject()

{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	
	InitBackend();
	InitMethods();
}


PClipboard::PClipboard(BMessage *msg)
	:	PObject(msg)

{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	
	
	
	InitBackend();
}


PClipboard::PClipboard(const char *name)
	:	PObject(name)

{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	
	InitMethods();
	InitBackend();
}


PClipboard::PClipboard(const PClipboard &from)
	:	PObject(from)

{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	
	InitMethods();
	InitBackend();
}


PClipboard::~PClipboard(void)
{
}


BArchivable *
PClipboard::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PClipboard"))
		return new PClipboard(data);

	return NULL;
}


PObject *
PClipboard::Create(void)
{
	return new PClipboard();
}


PObject *
PClipboard::Duplicate(void) const
{
	return new PClipboard(*this);
}


status_t
PClipboard::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BClipboard *backend = (BClipboard*)fBackend;
	if (str.ICompare("Locked") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsLocked());
	else if (str.ICompare("SystemCount") == 0)
		((IntProperty*)prop)->SetValue(backend->SystemCount());
	else if (str.ICompare("Name") == 0)
		((StringProperty*)prop)->SetValue(backend->Name());
	else if (str.ICompare("LocalCount") == 0)
		((IntProperty*)prop)->SetValue(backend->LocalCount());
	else
	{
		return PObject::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PClipboard::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BClipboard *backend = (BClipboard*)fBackend;
	
	BoolValue boolval;
	CharValue charval;
	ColorValue colorval;
	FloatValue floatval;
	IntValue intval;
	PointValue pointval;
	RectValue rectval;
	StringValue stringval;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;

	if (str.ICompare("Locked") == 0)
	{
		prop->GetValue(&boolval);
		if (*boolval.value)
			backend->Lock();
		else
			backend->Unlock();
	}
	else
	{
		return PObject::SetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


BClipboard *
PClipboard::GetBackend(void) const
{
	return fBackend;
}


void
PClipboard::InitProperties(void)
{
	SetStringProperty("Description", "A representation of the clipboard");

	AddProperty(new BoolProperty("Locked", false));
	AddProperty(new IntProperty("SystemCount", 0));
	AddProperty(new StringProperty("Name", 0));
	AddProperty(new IntProperty("LocalCount", 0));
}


void
PClipboard::InitBackend(void)
{
	if (!fBackend)
		fBackend = be_clipboard;
	StringValue sv("A representation of the clipboard");
	SetProperty("Description", &sv);
}


void
PClipboard::InitMethods(void)
{
	PMethodInterface pmi;
	
}


