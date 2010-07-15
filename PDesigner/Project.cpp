#include "Project.h"

Project::Project(void)
{
	fName = new BString("Untitled"),
	fList = new BObjectList<PObject>(20,true);
}


Project::~Project(void)
{
	delete fName;
	delete fList;
}


void
Project::SetName(const char *name)
{
	*fName = name;
}


const char *
Project::GetName(void) const
{
	return fName->String();
}


void
Project::AddObject(PObject *obj, const int32 &index)
{
	if (index < 0)
		fList->AddItem(obj);
	else
		fList->AddItem(obj,index);
}


void
Project::RemoveObject(PObject *obj)
{
	fList->RemoveItem(obj);
}


PObject *
Project::RemoveObject(const int32 &index)
{
	return fList->RemoveItemAt(index);
}


int32
Project::CountObjects(void) const
{
	return fList->CountItems();
}


PObject *
Project::ObjectAt(const int32 &index)
{
	return fList->ItemAt(index);
}

