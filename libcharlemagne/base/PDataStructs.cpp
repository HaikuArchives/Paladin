#include "PDataStructs.h"


PAppInfo::PAppInfo(void)
{
	fType = "PAppInfo";
	fFriendlyType = "AppInfo";
	
	AddProperty(new IntProperty("threadid", -1, "ID of the main thread"));
	AddProperty(new IntProperty("teamid", -1, "ID of the app's team"));
	AddProperty(new IntProperty("port", -1, "ID of the app's communication port"));
	AddProperty(new IntProperty("flags", -1, "The app's launch flags"));
	AddProperty(new IntProperty("refdevice", -1, "The device ID for the app's entry_ref"));
	AddProperty(new IntProperty("refdirectory", -1, "The inode ID for the app's entry_ref"));
	AddProperty(new StringProperty("refname", "", "The name for the app's entry_ref"));
}


PAppInfo::PAppInfo(BMessage *msg)
	:	PData(msg)
{
	fType = "PAppInfo";
	fFriendlyType = "AppInfo";
}


PAppInfo::PAppInfo(const char *name)
	:	PData(name)
{
	fType = "PAppInfo";
	fFriendlyType = "AppInfo";
}


PAppInfo::PAppInfo(const PAppInfo &from)
	:	PData(from)
{
	fType = "PAppInfo";
	fFriendlyType = "AppInfo";
}


PAppInfo::~PAppInfo(void)
{
}


BArchivable *
PAppInfo::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PAppInfo"))
		return new PAppInfo(data);

	return NULL;
}


PData *
PAppInfo::Create(void)
{
	return new PAppInfo();
}


PData *
PAppInfo::Duplicate(void) const
{
	return new PAppInfo(*this);
}


PEntryRef::PEntryRef(void)
{
	fType = "PEntryRef";
	fFriendlyType = "AppInfo";
	
	AddProperty(new IntProperty("refdevice", -1, "The device ID for the entry_ref"));
	AddProperty(new IntProperty("refdirectory", -1, "The inode ID for the entry_ref"));
	AddProperty(new StringProperty("refname", "", "The name for the entry_ref"));
}


PEntryRef::PEntryRef(BMessage *msg)
	:	PData(msg)
{
	fType = "PEntryRef";
	fFriendlyType = "AppInfo";
}


PEntryRef::PEntryRef(const char *name)
	:	PData(name)
{
	fType = "PEntryRef";
	fFriendlyType = "AppInfo";
}


PEntryRef::PEntryRef(const PEntryRef &from)
	:	PData(from)
{
	fType = "PEntryRef";
	fFriendlyType = "AppInfo";
}


PEntryRef::~PEntryRef(void)
{
}


BArchivable *
PEntryRef::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PEntryRef"))
		return new PEntryRef(data);

	return NULL;
}


PData *
PEntryRef::Create(void)
{
	return new PEntryRef();
}


PData *
PEntryRef::Duplicate(void) const
{
	return new PEntryRef(*this);
}

