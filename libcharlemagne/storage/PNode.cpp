#include "PNode.h"

#include <Application.h>
#include <fs_attr.h>
#include <Node.h>
#include <Window.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

int32_t PNodeGetAttrInfo(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PNodeGetAttrNames(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PNodeReadAttr(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PNodeRemoveAttr(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PNodeRenameAttr(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PNodeSync(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PNodeWriteAttr(void *pobject, void *in, void *out, void *ptr = NULL);

PNode::PNode(void)
	:	PObject()
{
	fType = "PNode";
	fFriendlyType = "Node";
	AddInterface("PNode");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PNode::PNode(BMessage *msg)
	:	PObject(msg)
{
	fType = "PNode";
	fFriendlyType = "Node";
	AddInterface("PNode");
	
	InitBackend();
}


PNode::PNode(const char *name)
	:	PObject(name)
{
	fType = "PNode";
	fFriendlyType = "Node";
	AddInterface("PNode");
	
	InitMethods();
	InitBackend();
}


PNode::PNode(const PNode &from)
	:	PObject(from)
{
	fType = "PNode";
	fFriendlyType = "Node";
	AddInterface("PNode");
	
	InitMethods();
	InitBackend();
}


PNode::~PNode(void)
{
	delete fBackend;
}


BArchivable *
PNode::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PNode"))
		return new PNode(data);

	return NULL;
}


PObject *
PNode::Create(void)
{
	return new PNode();
}


PObject *
PNode::Duplicate(void) const
{
	return new PNode(*this);
}


status_t
PNode::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BNode *backend = (BNode*)fBackend;
	if (str.ICompare("Locked") == 0)
		((BoolProperty*)prop)->SetValue(fIsLocked);
	else if (str.ICompare("Path") == 0)
		((StringProperty*)prop)->SetValue(fPath.String());
	else if (str.ICompare("Status") == 0)
		((IntProperty*)prop)->SetValue(backend->InitCheck());
	else
	{
		return PObject::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PNode::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BoolValue boolval;
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
			fBackend->Lock();
		else
			fBackend->Unlock();
		fIsLocked = *boolval.value;
	}
	else if (str.ICompare("Path") == 0)
	{
		prop->GetValue(&stringval);
		fPath = *stringval.value;
		fBackend->SetTo(stringval.value->String());
	}
	else
	{
		return PObject::SetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


BNode *
PNode::GetBackend(void) const
{
	return fBackend;
}


void
PNode::InitBackend(void)
{
	fBackend = new BNode();
}


void
PNode::InitProperties(void)
{
	SetStringProperty("Description", "A chunk of data in the filesystem");

	AddProperty(new BoolProperty("Locked", 0));
	AddProperty(new StringProperty("Path", 0));
	AddProperty(new IntProperty("Status", PROPERTY_READ_ONLY));
}


void
PNode::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("name", B_STRING_TYPE, "Name of the attribute to get information on", 0);
	pmi.AddReturnValue("type", B_INT32_TYPE, "Type constant for the attribute");
	pmi.AddReturnValue("size", B_INT64_TYPE, "Size of the attribute");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status from the call");
	AddMethod(new PMethod("GetAttrInfo", PNodeGetAttrInfo, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("names", B_STRING_TYPE, "List containing names of all "
											"of the node's attributes");
	AddMethod(new PMethod("GetAttrNames", PNodeGetAttrNames, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("name", B_STRING_TYPE, "Name of the attribute", 0);
	pmi.AddArg("pointer", B_POINTER_TYPE, "Pointer to a data buffer to hold the info", 0);
	pmi.AddArg("size", B_INT64_TYPE, "Length of the data to read", 0);
	pmi.AddReturnValue("bytesread", B_INT32_TYPE, "Number of bytes read");
	AddMethod(new PMethod("ReadAttr", PNodeReadAttr, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("name", B_STRING_TYPE, "Name of the attribute to remove", 0);
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status from the call");
	AddMethod(new PMethod("RemoveAttr", PNodeRemoveAttr, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("name", B_STRING_TYPE, "Name of the attribute to rename", 0);
	pmi.AddArg("newname", B_STRING_TYPE, "New name for the attribute", 0);
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status from the call");
	AddMethod(new PMethod("RenameAttr", PNodeRenameAttr, &pmi));
	pmi.MakeEmpty();
	
	AddMethod(new PMethod("Sync", PNodeSync, &pmi));
	
	pmi.AddArg("name", B_STRING_TYPE, "Name of the attribute", 0);
	pmi.AddArg("type", B_INT32_TYPE, "Type of the data to write", 0);
	pmi.AddArg("pointer", B_POINTER_TYPE, "Pointer to a data buffer holding the info", 0);
	pmi.AddArg("size", B_INT64_TYPE, "Length of the data to write", 0);
	pmi.AddReturnValue("byteswritten", B_INT32_TYPE, "Number of bytes written");
	AddMethod(new PMethod("WriteAttr", PNodeWriteAttr, &pmi));
	pmi.MakeEmpty();
	
}


int32_t
PNodeGetAttrInfo(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	BString name;
	if (args->FindString("name", &name) != B_OK)
		return B_ERROR;
	
	attr_info info;
	status_t status = backend->GetAttrInfo(name.String(), &info);
	
	outArgs->AddInt32("type", info.type);
	outArgs->AddInt64("size", info.size);
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PNodeGetAttrNames(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
		
	char nameBuffer[B_ATTR_NAME_LENGTH];
	backend->RewindAttrs();
	
	while (backend->GetNextAttrName(nameBuffer) == B_OK)
	{
		if (strlen(nameBuffer))
			outArgs->AddString("name", nameBuffer);
	}
	
	return B_OK;
}


int32_t
PNodeReadAttr(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	BString name;
	if (args->FindString("name", &name) != B_OK)
		return B_ERROR;
	
	void *buffer = NULL;
	if (args->FindPointer("pointer", &buffer) != B_OK)
		return B_ERROR;
	
	off_t size;
	if (args->FindInt64("size", &size) != B_OK)
		return B_ERROR;
	
	ssize_t bytesRead = backend->ReadAttr(name.String(), 0, 0, buffer, size);
	
	outArgs->AddInt32("bytesread", bytesRead);
	
	return B_OK;
}


int32_t
PNodeRemoveAttr(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	BString name;
	if (args->FindString("name", &name) != B_OK)
		return B_ERROR;
	
	status_t status = backend->RemoveAttr(name.String());
	
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PNodeRenameAttr(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	BString name, newname;
	if (args->FindString("name", &name) != B_OK ||
		args->FindString("newname", &newname) != B_OK)
		return B_ERROR;
	
	status_t status = backend->RenameAttr(name.String(), newname.String());
	
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PNodeSync(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	backend->Sync();
	
	return B_OK;
}


int32_t
PNodeWriteAttr(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PNode *pnode = static_cast<PNode*>(pobject);
	if (!pnode)
		return B_BAD_TYPE;
	
	BNode *backend = pnode->GetBackend();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	BString name;
	if (args->FindString("name", &name) != B_OK)
		return B_ERROR;
	
	void *buffer = NULL;
	if (args->FindPointer("pointer", &buffer) != B_OK)
		return B_ERROR;
	
	type_code type;
	if (args->FindInt32("type", (int32*)&type) != B_OK)
		return B_ERROR;
	
	off_t size;
	if (args->FindInt64("size", &size) != B_OK)
		return B_ERROR;
	
	ssize_t bytesWritten = backend->WriteAttr(name.String(), type, 0, buffer, size);
	
	outArgs->AddInt32("byteswritten", bytesWritten);
	
	return B_OK;
}


