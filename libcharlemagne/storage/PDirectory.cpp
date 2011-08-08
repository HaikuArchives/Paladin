#include "PDirectory.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

int32_t PDirectoryCreateFile(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PDirectoryGetEntries(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PDirectoryContains(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PDirectoryCreateSymLink(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PDirectoryCreateDirectory(void *pobject, void *in, void *out, void *ptr = NULL);

PDirectory::PDirectory(void)
	:	PNode()

{
	fType = "PDirectory";
	fFriendlyType = "Directory";
	AddInterface("PDirectory");
	
	InitBackend();
	InitMethods();
}


PDirectory::PDirectory(BMessage *msg)
	:	PNode(msg)

{
	fType = "PDirectory";
	fFriendlyType = "Directory";
	AddInterface("PDirectory");
	
	
	
	InitBackend();
}


PDirectory::PDirectory(const char *name)
	:	PNode(name)

{
	fType = "PDirectory";
	fFriendlyType = "Directory";
	AddInterface("PDirectory");
	
	InitMethods();
	InitBackend();
}


PDirectory::PDirectory(const PDirectory &from)
	:	PNode(from)

{
	fType = "PDirectory";
	fFriendlyType = "Directory";
	AddInterface("PDirectory");
	
	InitMethods();
	InitBackend();
}


PDirectory::~PDirectory(void)
{
	delete fBackend;
}


BArchivable *
PDirectory::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PDirectory"))
		return new PDirectory(data);

	return NULL;
}


PObject *
PDirectory::Create(void)
{
	return new PDirectory();
}


PObject *
PDirectory::Duplicate(void) const
{
	return new PDirectory(*this);
}


status_t
PDirectory::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BDirectory *backend = (BDirectory*)fBackend;
	if (str.ICompare("IsRoot") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsRootDirectory());
	else if (str.ICompare("EntryCount") == 0)
		((IntProperty*)prop)->SetValue(backend->CountEntries());
	else
	{
		return PNode::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


BDirectory *
PDirectory::GetBackend(void) const
{
	return fBackend;
}


void
PDirectory::InitProperties(void)
{
	SetStringProperty("Description", "An object representing a directory in the filesystem");

	AddProperty(new BoolProperty("IsRoot", false, " True if the directory is the root of a volume"));
	AddProperty(new IntProperty("EntryCount", 0, " The number of entries in the directory"));
}


void
PDirectory::InitBackend(void)
{
	if (!fBackend)
		fBackend = new BDirectory();
	StringValue sv("An object representing a directory in the filesystem");
	SetProperty("Description", &sv);
}


void
PDirectory::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("path", B_STRING_TYPE, " A relative path or filename", 0);
	pmi.AddArg("type", B_INT32_TYPE, " An optional node type", 0);
	pmi.AddReturnValue("value", B_BOOL_TYPE, " True if the directory (or a subdirectory) constains the path");
	AddMethod(new PMethod("Contains", PDirectoryContains, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("path", B_STRING_TYPE, " A relative path", 0);
	pmi.AddReturnValue("path", B_STRING_TYPE, " The full path of the created directory");
	pmi.AddReturnValue("status", B_INT32_TYPE, " The error status of the call");
	AddMethod(new PMethod("CreateDirectory", PDirectoryCreateDirectory, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("path", B_STRING_TYPE, " A relative path", 0);
	pmi.AddArg("dontclobber", B_BOOL_TYPE, " Fail if the file already exists if true", 0);
	pmi.AddReturnValue("status", B_INT32_TYPE, " The error status of the call");
	AddMethod(new PMethod("CreateFile", PDirectoryCreateFile, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("path", B_STRING_TYPE, " A relative path", 0);
	pmi.AddArg("target", B_STRING_TYPE, " Path of the link's target", 0);
	pmi.AddReturnValue("status", B_INT32_TYPE, " The error status of the call");
	AddMethod(new PMethod("CreateSymLink", PDirectoryCreateSymLink, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("entries", B_LIST_TYPE, " A list of filenames in the directory");
	AddMethod(new PMethod("GetEntries", PDirectoryGetEntries, &pmi));
	pmi.MakeEmpty();

}


int32_t
PDirectoryContains(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PDirectory *parent = static_cast<PDirectory*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BDirectory *backend = (BDirectory*)parent->GetBackend();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	BString path;
	if (inArgs->FindString("path", &path) != B_OK)
		return B_ERROR;

	int32 type;
	if (inArgs->FindInt32("type", &type) != B_OK)
		return B_ERROR;

	bool outValue1;

	outValue1 = backend->Contains(path.String(), type);

	outArgs->MakeEmpty();

	return B_OK;
}


int32_t
PDirectoryCreateDirectory(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PDirectory *parent = static_cast<PDirectory*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BDirectory *backend = (BDirectory*)parent->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		return B_ERROR;
	
	BDirectory newDir;
	status_t status = backend->CreateDirectory(path.String(), &newDir);
	
	outArgs->MakeEmpty();
	
	if (status == B_OK)
	{
		BEntry entry;
		status = newDir.GetEntry(&entry);
		if (status == B_OK)
		{
			BPath dirPath;
			status = entry.GetPath(&dirPath);
			outArgs->AddString("path", dirPath.Path());
		}
	}
	
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PDirectoryCreateFile(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PDirectory *parent = static_cast<PDirectory*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BDirectory *backend = (BDirectory*)parent->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		return B_ERROR;
	
	bool dontClobber = false;
	if (args->FindBool("dontclobber", &dontClobber) != B_OK)
		return B_ERROR;
	
	BFile newFile;
	status_t status = backend->CreateFile(path.String(), &newFile,
										dontClobber);
	
	outArgs->MakeEmpty();
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PDirectoryCreateSymLink(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PDirectory *parent = static_cast<PDirectory*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BDirectory *backend = (BDirectory*)parent->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		return B_ERROR;
	
	BString target;
	if (args->FindString("target", &target) != B_OK)
		return B_ERROR;
	
	BSymLink link;
	status_t status = backend->CreateSymLink(path.String(),
											target.String(), &link);
	
	outArgs->MakeEmpty();
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PDirectoryGetEntries(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PDirectory *parent = static_cast<PDirectory*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BDirectory *backend = (BDirectory*)parent->GetBackend();
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	PArgs nameList;
	backend->Rewind();
	entry_ref ref;
	while (backend->GetNextRef(&ref) == B_OK)
	{
		if (ref.name)
			outArgs->AddString("name", ref.name);
	}
	
	return B_OK;
}


