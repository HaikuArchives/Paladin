#include "PProject.h"

PProject::PProject(void)
{
	fType = "PProject";
	AddInterface("PProject");
	InitProperties();
}


PProject::PProject(BMessage *msg)
	:	PObject(msg)
{
	fType = "PProject";
	AddInterface("PProject");
	BString str;
	if (msg->FindString("Name",&str) != B_OK)
		str = "MyProject";
	
	AddProperty(new StringProperty("Name",str));
}


PProject::PProject(const char *name)
{
	fType = "PProject";
	AddInterface("PProject");
	AddProperty(new StringProperty("Name",name));
}


PProject::PProject(const PProject &proj)
{
	fType = "PProject";
	AddInterface("PProject");
	debugger("Implement PPRoject copy constructor");
}


PObject *
PProject::Create(void)
{
	return new PProject();
}


PObject *
PProject::Duplicate(void) const
{
	return new PProject(*this);
}

	
BArchivable *
PProject::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PProject"))
		return new PProject(data);

	return NULL;
}


void
PProject::InitProperties(void)
{
	AddProperty(new StringProperty("Name","Untitled Project"));
}
