#include "TemplateManager.h"

#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <stdio.h>

#include "DPath.h"
#include "Project.h"
#include "TextFile.h"

TemplateManager::TemplateManager(void)
	:	fList(20,true)
{
}


void
TemplateManager::ScanFolder(const char *path)
{
	if (!path || !BEntry(path).IsDirectory())
		return;
	
	fList.MakeEmpty();
	BDirectory dir(path);
	dir.Rewind();
	
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		if (!BEntry(&ref).IsDirectory())
			continue;
		
		AddTemplate(ref);
	}
}


ProjectTemplate *
TemplateManager::TemplateAt(int32 index)
{
	return fList.ItemAt(index);
}


void
TemplateManager::AddTemplate(entry_ref ref)
{
	BEntry entry(&ref);
	if (entry.IsDirectory())
	{
		ProjectTemplate *t = new ProjectTemplate(ref);
		if (t->InitCheck() != B_OK)
		{
			delete t;
			return;
		}
		
		fList.AddItem(t);
	}
}


void
TemplateManager::RemoveTemplateAt(int32 index)
{
	fList.RemoveItemAt(index);
}


int32
TemplateManager::CountTemplates(void) const
{
	return fList.CountItems();
}


ProjectTemplate::ProjectTemplate(entry_ref ref)
	:	fStatus(B_NO_INIT),
		fRefList(20,true),
		fTargetType(-1),
		fLibList(20,true)
{
	Load(ref);
}


status_t
ProjectTemplate::InitCheck(void) const
{
	return fStatus;
}


int32
ProjectTemplate::CountFiles(void) const
{
	return fRefList.CountItems();
}


entry_ref *
ProjectTemplate::FileAt(int32 index)
{
	return fRefList.ItemAt(index);
}


const char *
ProjectTemplate::ProjectFileName(void) const
{
	return fProjFileName.String();
}


int32
ProjectTemplate::TargetType(void) const
{
	return fTargetType;
}


entry_ref
ProjectTemplate::GetRef(void) const
{
	return fRef;
}


void
ProjectTemplate::PrintToStream(void)
{
	BString typeString;
	switch (fTargetType)
	{
		case TARGET_APP:
			typeString = "Application";
			break;
		case TARGET_SHARED_LIB:
			typeString = "Shared Library / Addon";
			break;
		case TARGET_STATIC_LIB:
			typeString = "Static Library";
			break;
		case TARGET_DRIVER:
			typeString = "Driver";
			break;
		default:
			typeString = "Unknown";
			break;
	}
	
	printf("Template %s (%s)\n", DPath(fRef).GetFullPath(),typeString.String());
	if (fRefList.CountItems() > 0)
		for (int32 i = 0; i < fRefList.CountItems(); i++)
			printf("\t%s\n",fRefList.ItemAt(i)->name);
}


int32
ProjectTemplate::CountLibs(void) const
{
	return fLibList.CountItems();
}


entry_ref *
ProjectTemplate::LibAt(int32 index)
{
	return fLibList.ItemAt(index);
}


void
ProjectTemplate::Load(entry_ref ref)
{
	BEntry entry(&ref);
	if (!entry.IsDirectory())
	{
		fStatus = B_BAD_VALUE;
		return;
	}
	
	DPath path(ref);
	path.Append("TEMPLATEINFO");
	entry.SetTo(path.GetFullPath());

	fTargetType = TARGET_APP;
	if (entry.Exists())
	{
		TextFile file(path.GetFullPath(), B_READ_ONLY);
		BString linedata = file.ReadLine();
		while (linedata.CountChars() > 0)
		{
			int32 pos = linedata.FindFirst("=");
			if (pos >= 0)
			{
				BString key, value;
				key = linedata;
				key.Truncate(pos);
				value = linedata.String() + pos + 1;
				if (key.ICompare("TYPE") == 0)
				{
					if (key.ICompare("Shared") == 0)
						fTargetType = TARGET_SHARED_LIB;
					else if (key.ICompare("Static") == 0)
						fTargetType = TARGET_STATIC_LIB;
					else if (key.ICompare("Driver") == 0)
						fTargetType = TARGET_DRIVER;
				}
				else if (key.ICompare("LIB") == 0)
				{
					entry_ref ref = RefForLib(value);
					if (ref.name)
						fLibList.AddItem(new entry_ref(ref));
				}
				else if (key.ICompare("PROJFILE") == 0)
				{
					fProjFileName = value;
				}
			}
			linedata = file.ReadLine();
		}
	}
	
	fRef = ref;
	
	BDirectory dir(&ref);
	dir.Rewind();
	while (dir.GetNextRef(&ref) == B_OK)
	{
		if (strcmp(ref.name,"TEMPLATEINFO") == 0)
			continue;
		fRefList.AddItem(new entry_ref(ref));
	}
	
	fStatus = B_OK;
}


entry_ref
ProjectTemplate::RefForLib(const BString &path)
{
	BString str(path);
	BPath bpath;
	if (str.IFindFirst("B_BEOS_LIB_DIRECTORY") >= 0)
	{
		find_directory(B_BEOS_LIB_DIRECTORY,&bpath);
		str.IReplaceFirst("B_BEOS_LIB_DIRECTORY",bpath.Path());
	}
	else if (str.IFindFirst("B_SYSTEM_LIB_DIRECTORY") >= 0)
	{
		find_directory(B_SYSTEM_LIB_DIRECTORY,&bpath);
		str.IReplaceFirst("B_SYSTEM_LIB_DIRECTORY",bpath.Path());
	}
	
	entry_ref ref;
	BEntry entry(str.String());
	entry.GetRef(&ref);
	
	return ref;
}
