#include "PFile.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

int32_t PFileWriteAt(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PFileRead(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PFileReadAt(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PFileWrite(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

PFile::PFile(void)
	:	PNode()

{
	fType = "PFile";
	fFriendlyType = "File";
	AddInterface("PFile");
	
	InitBackend();
	InitMethods();
}


PFile::PFile(BMessage *msg)
	:	PNode(msg)

{
	fType = "PFile";
	fFriendlyType = "File";
	AddInterface("PFile");
	
	
	
	InitBackend();
}


PFile::PFile(const char *name)
	:	PNode(name)

{
	fType = "PFile";
	fFriendlyType = "File";
	AddInterface("PFile");
	
	InitMethods();
	InitBackend();
}


PFile::PFile(const PFile &from)
	:	PNode(from)

{
	fType = "PFile";
	fFriendlyType = "File";
	AddInterface("PFile");
	
	InitMethods();
	InitBackend();
}


PFile::~PFile(void)
{
	delete fBackend;
}


BArchivable *
PFile::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PFile"))
		return new PFile(data);

	return NULL;
}


PObject *
PFile::Create(void)
{
	return new PFile();
}


PObject *
PFile::Duplicate(void) const
{
	return new PFile(*this);
}


status_t
PFile::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BFile *backend = (BFile*)fBackend;
	off_t outSizeValue;
	if (str.ICompare("IsWritable") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsWritable());
	else if (str.ICompare("Size") == 0)
	{
		backend->GetSize(&outSizeValue);
		((IntProperty*)prop)->SetValue(outSizeValue);
	}
	else if (str.ICompare("Position") == 0)
		((IntProperty*)prop)->SetValue(backend->Position());
	else if (str.ICompare("IsReadable") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsReadable());
	else
	{
		return PNode::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PFile::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BFile *backend = (BFile*)fBackend;
	
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

	else if (str.ICompare("Size") == 0)
	{
		prop->GetValue(&intval);
		backend->SetSize(*intval.value);
	}
	else
	{
		return PNode::SetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


BFile *
PFile::GetBackend(void) const
{
	return fBackend;
}


void
PFile::InitProperties(void)
{
	SetStringProperty("Description", "An object representing the data in a file");

	AddProperty(new BoolProperty("IsWritable", true, " True if the file is writable"));
	AddProperty(new IntProperty("Size", 0, " The size of the file"));
	AddProperty(new IntProperty("Position", 0, " The current location within the file"));
	AddProperty(new BoolProperty("IsReadable", true, " True if the file is readable"));
}


void
PFile::InitBackend(void)
{
	if (!fBackend)
		fBackend = new BFile();
	StringValue sv("An object representing the data in a file");
	SetProperty("Description", &sv);
}


void
PFile::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("buffer", PARG_POINTER, " A buffer to hold the data", 0);
	pmi.AddArg("size", PARG_INT32, " The size of the buffer", 0);
	pmi.AddReturnValue("bytesread", PARG_INT32, " The number of bytes actually read");
	AddMethod(new PMethod("Read", PFileRead, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", PARG_INT32, " Offset in the file to start reading from", 0);
	pmi.AddArg("buffer", PARG_POINTER, " A buffer to hold the data", 0);
	pmi.AddArg("size", PARG_INT32, " The size of the buffer", 0);
	pmi.AddReturnValue("bytesread", PARG_INT32, " The number of bytes actually read");
	AddMethod(new PMethod("ReadAt", PFileReadAt, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("buffer", PARG_POINTER, " A buffer holding the data to write", 0);
	pmi.AddArg("size", PARG_INT32, " The size of the buffer", 0);
	pmi.AddReturnValue("bytesread", PARG_INT32, " The number of bytes actually written");
	AddMethod(new PMethod("Write", PFileWrite, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", PARG_INT32, " Offset in the file to start reading from", 0);
	pmi.AddArg("buffer", PARG_POINTER, " A buffer holding the data to write", 0);
	pmi.AddArg("size", PARG_INT32, " The size of the buffer", 0);
	pmi.AddReturnValue("bytesread", PARG_INT32, " The number of bytes actually written");
	AddMethod(new PMethod("WriteAt", PFileWriteAt, &pmi));
	pmi.MakeEmpty();

}


int32_t
PFileRead(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PFile *parent = static_cast<PFile*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BFile *backend = (BFile*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);

	void * buffer;
	if (inArgs.FindPointer("buffer", &buffer) != B_OK)
		return B_ERROR;

	int32 size;
	if (inArgs.FindInt32("size", &size) != B_OK)
		return B_ERROR;

	int32 outValue1;

	outValue1 = backend->Read(buffer, size);

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PFileReadAt(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PFile *parent = static_cast<PFile*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BFile *backend = (BFile*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);

	int32 offset;
	if (inArgs.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	void * buffer;
	if (inArgs.FindPointer("buffer", &buffer) != B_OK)
		return B_ERROR;

	int32 size;
	if (inArgs.FindInt32("size", &size) != B_OK)
		return B_ERROR;

	int32 outValue1;

	outValue1 = backend->ReadAt(offset, buffer, size);

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PFileWrite(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PFile *parent = static_cast<PFile*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BFile *backend = (BFile*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);

	void * buffer;
	if (inArgs.FindPointer("buffer", &buffer) != B_OK)
		return B_ERROR;

	int32 size;
	if (inArgs.FindInt32("size", &size) != B_OK)
		return B_ERROR;

	int32 outValue1;

	outValue1 = backend->Write(buffer, size);

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PFileWriteAt(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PFile *parent = static_cast<PFile*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BFile *backend = (BFile*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);

	int32 offset;
	if (inArgs.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	void * buffer;
	if (inArgs.FindPointer("buffer", &buffer) != B_OK)
		return B_ERROR;

	int32 size;
	if (inArgs.FindInt32("size", &size) != B_OK)
		return B_ERROR;

	int32 outValue1;

	outValue1 = backend->WriteAt(offset, buffer, size);

	outArgs.MakeEmpty();

	return B_OK;
}


