#include "PBitmap.h"

#include <Bitmap.h>

#include "CInterface.h"
#include "PArgs.h"

//int32_t PBitmapClear(void *pobject, PArgList *in, PArgList *out);

PBitmap::PBitmap(void)
	:	fBitmap(NULL)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	
	InitProperties();
	InitMethods();
	InitBackend();
}


PBitmap::PBitmap(BMessage *msg)
	:	PObject(msg),
		fBitmap(NULL)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	InitBackend();
}


PBitmap::PBitmap(const char *name)
	:	PObject(name),
		fBitmap(NULL)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	InitMethods();
	InitBackend();
}


PBitmap::PBitmap(const PBitmap &from)
	:	PObject(from),
		fBitmap(NULL)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	InitMethods();
	InitBackend();
}


PBitmap::~PBitmap(void)
{
}


BArchivable *
PBitmap::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PBitmap"))
		return new PBitmap(data);

	return NULL;
}


status_t
PBitmap::GetProperty(const char *name, PValue *value, const int32 &index) const
{
/*
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (str.ICompare("LocalCount") == 0)
		((IntProperty*)prop)->SetValue(clp->LocalCount());
	else if (str.ICompare("Locked") == 0)
		((BoolProperty*)prop)->SetValue(clp->IsLocked());
	else if (str.ICompare("Name") == 0)
		((StringProperty*)prop)->SetValue(clp->Name());
	else if (str.ICompare("SystemCount") == 0)
		((IntProperty*)prop)->SetValue(clp->SystemCount());
	else
		return PObject::GetProperty(name,value,index);
	
	return prop->GetValue(value);
*/
}


status_t
PBitmap::SetProperty(const char *name, PValue *value, const int32 &index)
{
/*	if (!name || !value)
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
			fBitmap->Lock();
		else
			fBitmap->Unlock();
	}
	else
		return PObject::SetProperty(name,value,index);
	
	return prop->GetValue(value);
*/
}


PObject *
PBitmap::Create(void)
{
	return new PBitmap();
}


PObject *
PBitmap::Duplicate(void) const
{
	return new PBitmap(*this);
}

BBitmap *
PBitmap::GetBackend(void)
{
	return fBitmap;
}


void
PBitmap::InitBackend(void)
{
	StringValue sv("A bitmap object");
	SetProperty("Description",&sv);
}


void
PBitmap::InitProperties(void)
{
}


void
PBitmap::InitMethods(void)
{
	PMethodInterface pmi;
	pmi.AddReturnValue("status", PARG_INT32);
//	AddMethod(new PMethod("Clear", PBitmapClear, &pmi));
}


int32_t
PBitmapClear(void *pobject, PArgList *in, PArgList *out)
{
/*	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BBitmap *clip = parent->GetBackend();
	
	PArgs args(out);
	args.AddInt32("status", clip->Clear());
*/	
	return B_OK;
}


