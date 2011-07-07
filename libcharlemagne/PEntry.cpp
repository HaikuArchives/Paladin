#include "PEntry.h"

#include <Directory.h>
#include <Entry.h>
#include <Path.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

int32_t PEntryRemove(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PEntryRename(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PEntryMoveTo(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

PEntry::PEntry(void)
	:	PObject()
{
	fType = "PEntry";
	fFriendlyType = "Entry";
	AddInterface("PEntry");
	
	InitBackend();
	InitMethods();
}


PEntry::PEntry(BMessage *msg)
	:	PObject(msg)
{
	fType = "PEntry";
	fFriendlyType = "Entry";
	AddInterface("PEntry");
	
	
	
	InitBackend();
}


PEntry::PEntry(const char *name)
	:	PObject(name)
{
	fType = "PEntry";
	fFriendlyType = "Entry";
	AddInterface("PEntry");
	
	InitMethods();
	InitBackend();
}


PEntry::PEntry(const PEntry &from)
	:	PObject(from)
{
	fType = "PEntry";
	fFriendlyType = "Entry";
	AddInterface("PEntry");
	
	InitMethods();
	InitBackend();
}


PEntry::~PEntry(void)
{
	delete fBackend;
}


BArchivable *
PEntry::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PEntry"))
		return new PEntry(data);

	return NULL;
}


PObject *
PEntry::Create(void)
{
	return new PEntry();
}


PObject *
PEntry::Duplicate(void) const
{
	return new PEntry(*this);
}


status_t
PEntry::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BEntry *backend = (BEntry*)fBackend;
	gid_t outGroupValue;
	time_t outCreationTimeValue;
	time_t outAccessTimeValue;
	uid_t outOwnerValue;
	if (str.ICompare("Path") == 0)
	{
		BPath path;
		backend->GetPath(&path);
		((StringProperty*)prop)->SetValue(path.Path());
	}
	else if (str.ICompare("Exists") == 0)
		((BoolProperty*)prop)->SetValue(backend->Exists());
	else if (str.ICompare("ErrorStatus") == 0)
		((IntProperty*)prop)->SetValue(backend->InitCheck());
	else if (str.ICompare("Group") == 0)
	{
		backend->GetGroup(&outGroupValue);
		((IntProperty*)prop)->SetValue(outGroupValue);
	}
	else if (str.ICompare("CreationTime") == 0)
	{
		backend->GetCreationTime(&outCreationTimeValue);
		((IntProperty*)prop)->SetValue(outCreationTimeValue);
	}
	else if (str.ICompare("AccessTime") == 0)
	{
		backend->GetAccessTime(&outAccessTimeValue);
		((IntProperty*)prop)->SetValue(outAccessTimeValue);
	}
	else if (str.ICompare("Owner") == 0)
	{
		backend->GetOwner(&outOwnerValue);
		((IntProperty*)prop)->SetValue(outOwnerValue);
	}
	else
	{
		return PObject::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PEntry::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BEntry *backend = (BEntry*)fBackend;
	
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

	if (str.ICompare("Path") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetTo(*stringval.value);
	}
	else if (str.ICompare("Group") == 0)
	{
		prop->GetValue(&intval);
		backend->SetGroup((gid_t)*intval.value);
	}
	else if (str.ICompare("Owner") == 0)
	{
		prop->GetValue(&intval);
		backend->SetOwner((uid_t)*intval.value);
	}
	else
	{
		return PObject::SetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


BEntry *
PEntry::GetBackend(void) const
{
	return fBackend;
}


void
PEntry::InitProperties(void)
{
	SetStringProperty("Description", "A filesystem entry, such as a file or directory.");

	AddProperty(new StringProperty("Path", "", " Location of the entry on the filesystem"));
	AddProperty(new BoolProperty("Exists", false, " True if the entry actually exists in the filesystem"));
	AddProperty(new IntProperty("ErrorStatus", 0, " Returns the error status of the entry"));
	AddProperty(new IntProperty("Group", 0, " Set or get the entry's group"));
	AddProperty(new IntProperty("CreationTime", 0, " Get the entry's creation time"));
	AddProperty(new IntProperty("AccessTime", 0, " Get the entry's last-accessed time"));
	AddProperty(new IntProperty("Owner", 0, " Set or get the entry's owner"));
}


void
PEntry::InitBackend(void)
{
	if (!fBackend)
		fBackend = new BEntry();
	StringValue sv("A filesystem entry, such as a file or directory.");
	SetProperty("Description", &sv);
}


void
PEntry::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("path", PARG_STRING, " The new path to receive the entry", 0);
	pmi.AddReturnValue("error", PARG_INT32, " Success or error state of the call");
	AddMethod(new PMethod("MoveTo", PEntryMoveTo, &pmi));
	pmi.MakeEmpty();

	AddMethod(new PMethod("Remove", PEntryRemove, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("newname", PARG_STRING, " The new name of the entry", 0);
	pmi.AddReturnValue("error", PARG_INT32, " Success or error state of the call");
	AddMethod(new PMethod("Rename", PEntryRename, &pmi));
	pmi.MakeEmpty();

}


int32_t
PEntryMoveTo(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PEntry *parent = static_cast<PEntry*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BEntry *backend = (BEntry*)parent->GetBackend();
	
	PArgs args(in), outArgs(out);
	BString string;
	if (args.FindString("path", &string) != B_OK)
		return B_ERROR;
	
	BDirectory dir(string.String());
	status_t status = backend->MoveTo(&dir);
	
	outArgs.MakeEmpty();
	outArgs.AddInt32("status", status);
	
	return B_OK;
}


int32_t
PEntryRemove(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PEntry *parent = static_cast<PEntry*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BEntry *backend = (BEntry*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);


	backend->Remove();

	return B_OK;
}


int32_t
PEntryRename(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PEntry *parent = static_cast<PEntry*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BEntry *backend = (BEntry*)parent->GetBackend();
	
	PArgs args(in), outArgs(out);
	BString string;
	if (args.FindString("newname", &string) != B_OK)
		return B_ERROR;
	
	status_t status = backend->Rename(string.String());
	
	outArgs.MakeEmpty();
	outArgs.AddInt32("status", status);
	
	return B_OK;
}


