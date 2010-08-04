#include "DNode.h"
#include <TypeConstants.h>
#include <string.h>

DNode::DNode(void)
	:	BNode()
{
}


DNode::DNode(const entry_ref *ref)
	:	BNode(ref),
		fRef(*ref)
{
}


DNode::DNode(const BEntry *entry)
	:	BNode(entry)
{
	if (entry)
		entry->GetRef(&fRef);
}


DNode::DNode(const char *path)
	:	BNode(path)
{
	BEntry entry(path);
	if (entry.InitCheck() == B_OK)
		entry.GetRef(&fRef);
}


DNode::DNode(const BDirectory *dir, const char *path)
	:	BNode(dir,path)
{
	BDirectory d(dir,path);
	if (d.InitCheck() == B_OK);
	{
		BEntry entry;
		d.GetEntry(&entry);
		entry.GetRef(&fRef);
	}
}


DNode::DNode(const DNode &node)
	:	BNode(node)
{
	fRef = node.fRef;
}


status_t
DNode::Set(const entry_ref *ref)
{
	status_t status = BNode::SetTo(ref);
	fRef = ref ? *ref : entry_ref();
	return status;
}


status_t
DNode::Set(const BEntry *entry)
{
	status_t status = BNode::SetTo(entry);
	if (status == B_OK)
		entry->GetRef(&fRef);
	else
		fRef = entry_ref();
	return status;
}


status_t
DNode::Set(const char *path)
{
	status_t status = BNode::SetTo(path);
	if (status == B_OK)
	{
		BEntry entry(path);
		entry.GetRef(&fRef);
	}
	else
		fRef = entry_ref();
	return status;
}


status_t
DNode::Set(const BDirectory *dir, const char *path)
{
	status_t status = BNode::SetTo(dir,path);
	if (status == B_OK);
	{
		BEntry entry;
		BDirectory d(dir,path);
		d.GetEntry(&entry);
		entry.GetRef(&fRef);
	}
	return status;
}


ssize_t
DNode::WriteAttrString(const char *attr, const char *data)
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	if (!data || strlen(data) < 1)
		return 0;
	
	return WriteAttr(attr,B_STRING_TYPE,0,data,strlen(data) + 1);
}


ssize_t
DNode::WriteAttrInt32(const char *attr, const int32 &data)
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	return WriteAttr(attr,B_INT32_TYPE,0,&data,sizeof(int32));
}


ssize_t
DNode::ReadAttrInt32(const char *attr, int32 *data) const
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	return ReadAttr(attr,B_INT32_TYPE,0,data,sizeof(int32));
}


ssize_t
DNode::WriteAttrInt64(const char *attr, const int64 &data)
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	return WriteAttr(attr,B_INT64_TYPE,0,&data,sizeof(int64));
}


ssize_t
DNode::ReadAttrInt64(const char *attr, int64 *data) const
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	return ReadAttr(attr,B_INT64_TYPE,0,data,sizeof(int64));
}


ssize_t
DNode::WriteAttrBool(const char *attr, const bool &data)
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	return WriteAttr(attr,B_BOOL_TYPE,0,&data,sizeof(bool));
}


ssize_t
DNode::ReadAttrBool(const char *attr, bool *data) const
{
	if (InitCheck() != B_OK)
		return InitCheck();
	
	return ReadAttr(attr,B_BOOL_TYPE,0,data,sizeof(bool));
}


entry_ref
DNode::GetRef(void) const
{
	return fRef;
}


bool
DNode::Exists(void) const
{
	return BEntry(&fRef).Exists();
}
