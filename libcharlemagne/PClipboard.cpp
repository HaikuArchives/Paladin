#include "PClipboard.h"

#include <Application.h>
#include <Window.h>

#include "CInterface.h"
#include "PArgs.h"

int32_t PClipboardClear(void *pobject, PArgList *in, PArgList *out);
int32_t PClipboardCommit(void *pobject, PArgList *in, PArgList *out);
int32_t PClipboardData(void *pobject, PArgList *in, PArgList *out);
int32_t PClipboardDataSource(void *pobject, PArgList *in, PArgList *out);
int32_t PClipboardRevert(void *pobject, PArgList *in, PArgList *out);


PClipboard::PClipboard(void)
	:	fClipboard(NULL)
{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	
	InitProperties();
	InitMethods();
	InitBackend();
}


PClipboard::PClipboard(BMessage *msg)
	:	PHandler(msg),
		fClipboard(NULL)
{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	InitBackend();
}


PClipboard::PClipboard(const char *name)
	:	PHandler(name),
		fClipboard(NULL)
{
	fType = "PClipboard";
	fFriendlyType = "Clipboard";
	AddInterface("PClipboard");
	InitMethods();
	InitBackend();
}


PClipboard::PClipboard(const PClipboard &from)
	:	PHandler(from),
		fClipboard(NULL)
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


status_t
PClipboard::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BClipboard *clp = dynamic_cast<BClipboard*>(fClipboard);
		
	if (str.ICompare("LocalCount") == 0)
		((IntProperty*)prop)->SetValue(clp->LocalCount());
	else if (str.ICompare("Locked") == 0)
		((BoolProperty*)prop)->SetValue(clp->IsLocked());
	else if (str.ICompare("Name") == 0)
		((StringProperty*)prop)->SetValue(clp->Name());
	else if (str.ICompare("SystemCount") == 0)
		((IntProperty*)prop)->SetValue(clp->SystemCount());
	else
		return PHandler::GetProperty(name,value,index);
	
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
	
	BoolValue bv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (str.ICompare("Locked") == 0)
	{
		prop->GetValue(&bv);
		if (*bv.value)
			fClipboard->Lock();
		else
			fClipboard->Unlock();
	}
	else
		return PHandler::SetProperty(name,value,index);
	
	return prop->GetValue(value);
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

BClipboard *
PClipboard::GetBackend(void)
{
	return fClipboard;
}


void
PClipboard::InitBackend(void)
{
	fClipboard = new BClipboard("clipboard");
	StringValue sv("A temporary data storage device.");
	SetProperty("Description",&sv);
}


void
PClipboard::InitProperties(void)
{
	AddProperty(new IntProperty("LocalCount",0), PROPERTY_READ_ONLY);
	AddProperty(new BoolProperty("Locked",false));
	AddProperty(new StringProperty("Name","clipboard"), PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("SystemCount",0), PROPERTY_READ_ONLY);
}


void
PClipboard::InitMethods(void)
{
	PMethodInterface pmi;
	pmi.AddReturnValue("status", PARG_INT32);
	AddMethod(new PMethod("Clear", PClipboardClear, &pmi));
	AddMethod(new PMethod("Commit", PClipboardCommit, &pmi));
	AddMethod(new PMethod("Revert", PClipboardRevert, &pmi));
	
	// Because the data can be anything (or nothing), there isn't a defined
	// return value in the interface.
	AddMethod(new PMethod("Data", PClipboardData));
}


int32_t
PClipboardClear(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PClipboard *parent = static_cast<PClipboard*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BClipboard *clip = parent->GetBackend();
	
	PArgs args(out);
	args.AddInt32("status", clip->Clear());
	
	return B_OK;
}


int32_t
PClipboardCommit(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PClipboard *parent = static_cast<PClipboard*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BClipboard *clip = parent->GetBackend();
	
	PArgs args(out);
	args.AddInt32("status", clip->Commit());
	
	return B_OK;
}


int32_t
PClipboardData(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PClipboard *parent = static_cast<PClipboard*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BClipboard *clip = parent->GetBackend();
	
	if (clip->Data() )
	{
		parent->ConvertMsgToArgs(*clip->Data(), *out);
		return B_OK;
	}
	
	return B_ERROR;
}


int32_t
PClipboardRevert(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PClipboard *parent = static_cast<PClipboard*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BClipboard *clip = parent->GetBackend();
	
	PArgs args(out);
	args.AddInt32("status", clip->Revert());
	
	return B_OK;
}
