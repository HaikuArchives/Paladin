#include "FileItem.h"

#include <OS.h>
#include <Path.h>
#include <stdio.h>

#include "Globals.h"

FileItem::FileItem(void)
	:	fReplaceMode(PKG_REPLACE_ASK_ALWAYS),
		fPath("M_INSTALL_DIRECTORY"),
		fCategory("Applications"),
		fGroups(20,true),
		fLinks(20,true),
		fPlatforms(20,true)
{
}


FileItem::~FileItem(void)
{
}


const char *
FileItem::GetName(void) const
{
	return fName.String();
}


void
FileItem::SetName(const char *name)
{
	fName = name;
}


const char *
FileItem::GetInstalledName(void) const
{
	return fInstalledName.String();
}


void
FileItem::SetInstalledName(const char *name)
{
	fInstalledName = name;
}


entry_ref
FileItem::GetRef(void) const
{
	return fRef;
}


void
FileItem::SetRef(const entry_ref &ref)
{
	fRef = ref;
}


status_t
FileItem::SetRef(const char *path)
{
	BEntry entry(path);
	if (!entry.Exists())
		return B_ERROR;
	
	if (entry.InitCheck() != B_OK)
		return entry.InitCheck();
	
	entry.GetRef(&fRef);
	return B_OK;
}


PkgPath
FileItem::GetPath(ostype_t forPlatform) const
{
	if (forPlatform == OS_NONE)
		return fPath;
	
	PkgPath out(fPath);
	out.SetOS(forPlatform);
	return out;
}


void
FileItem::SetPath(const char *path)
{
	fPath.SetTo(path);
}


void
FileItem::SetPath(const PkgPath &path)
{
	fPath = path;
}


void
FileItem::SetCategory(const char *cat)
{
	fCategory = cat;
}


const char *
FileItem::GetCategory(void) const
{
	return fCategory.String();
}

			
void
FileItem::AddGroup(const char *group)
{
	if (!group || strlen(group) < 1)
		return;
	
	if (BString(group).ICompare("all") == 0)
		fGroups.MakeEmpty();
	else if (!FindItem(fGroups,group))
		fGroups.AddItem(new BString(group));
}


void
FileItem::RemoveGroup(const char *group)
{
	if (!group)
		return;

	if (BString(group).ICompare("all") == 0)
		return;
	
	BString *str = FindItem(fGroups,group);
	if (str)
		fGroups.RemoveItem(str);
}


bool
FileItem::BelongsToGroup(const char *group)
{
	if (!group || strlen(group) < 1)
		return false;
	
	if (fGroups.CountItems() == 0)
		return true;
	
	return FindItem(fGroups, group);
}


int32
FileItem::CountGroups(void) const
{
	return fGroups.CountItems();
}


const char *
FileItem::GroupAt(int32 index)
{
	BString *str = fGroups.ItemAt(index);
	return str ? str->String() : NULL;
}


BString
FileItem::GroupString(void)
{
	BString out;
	
	if (CountGroups() > 0)
		out << GroupAt(0);
	else
		out = "All";
	
	for (int32 i = 1; i < CountGroups(); i++)
		out << "," << GroupAt(i);
	
	return out;
}


void
FileItem::AddPlatform(ostype_t plat)
{
	switch (plat)
	{
		case OS_ALL:
		{
			fPlatforms.MakeEmpty();
			break;
		}
		case OS_NONE:
		{
			return;
		}
		default:
		{
			bool has_platform = false;
			for (int32 i = 0; i < fPlatforms.CountItems(); i++)
			{
				ostype_t *item = fPlatforms.ItemAt(i);
				if (*item == plat)
				{
					has_platform = true;
					break;
				}
			}
			
			if (!has_platform)
				fPlatforms.AddItem(new ostype_t(plat));
			break;
		}
	}
}


void
FileItem::RemovePlatform(ostype_t plat)
{
	if (plat == OS_ALL)
		return;
	
	ostype_t *item = NULL;
	for (int32 i = 0; i < fPlatforms.CountItems(); i++)
	{
		item = fPlatforms.ItemAt(i);
		if (*item == plat)
			break;
	}
	if (item)
		fPlatforms.RemoveItem(item);
}


bool
FileItem::BelongsToPlatform(ostype_t plat)
{
	if (fPlatforms.CountItems() == 0)
		return true;
	
	for (int32 i = 0; i < fPlatforms.CountItems(); i++)
	{
		ostype_t *item = fPlatforms.ItemAt(i);
		if (*item == plat)
			return true;
	}
	
	return false;
}


int32
FileItem::CountPlatforms(void) const
{
	return fPlatforms.CountItems();
}


ostype_t 
FileItem::PlatformAt(int32 index)
{
	ostype_t *item = fPlatforms.ItemAt(index);
	return item ? *item : OS_NONE;
}


BString
FileItem::PlatformString(void)
{
	BString out;
	
	if (CountPlatforms() > 0)
		out << OSTypeToString(PlatformAt(0));
	else
		out = "All";
	
	for (int32 i = 1; i < CountPlatforms(); i++)
		out << "," << OSTypeToString(PlatformAt(i));
	
	return out;
}


void
FileItem::AddLink(const char *link)
{
	if (!link || strlen(link) < 1)
		return;
	
	fLinks.AddItem(new BString(link));
}


void
FileItem::RemoveLink(const char *link)
{
	if (!link)
		return;

	BString *str = FindItem(fLinks,link);
	if (str)
		fLinks.RemoveItem(str);
}


bool
FileItem::HasLink(const char *link)
{
	if (!link || strlen(link) < 1)
		return false;
	
	return FindItem(fLinks, link);
}


int32
FileItem::CountLinks(void) const
{
	return fLinks.CountItems();
}


const char *
FileItem::LinkAt(int32 index)
{
	BString *str = fLinks.ItemAt(index);
	return str ? str->String() : NULL;
}


BString
FileItem::LinkString(void)
{
	BString out;
	
	OSPath ospath;
	if (CountLinks() > 0)
		out << GetFriendlyPathConstantName(ospath.StringToDir(LinkAt(0)));
	else
		out = "None";
	
	for (int32 i = 1; i < CountLinks(); i++)
		out << "," << GetFriendlyPathConstantName(ospath.StringToDir(LinkAt(i)));
	
	return out;
}


void
FileItem::SetReplaceMode(const int32 &mode)
{
	fReplaceMode = mode;
}


int32
FileItem::GetReplaceMode(void) const
{
	return fReplaceMode;
}


BString
FileItem::MakeInfo(bool getRefs)
{
	BString out;
	
	out << "FILE=" << GetName() << "\n";
	
	if (getRefs)
	{
		if (fRef.name && strlen(fRef.name) > 0)
		{
			BPath path (&fRef);
			out << "\tREF=" << path.Path() << "\n";
		}
	}
	
	if (fInstalledName.CountChars() > 0)
		out << "\tINSTALLEDNAME=" << GetInstalledName() << "\n";
	if (fCategory.CountChars() > 0 && CountLinks() > 0)
		out << "\tCATEGORY=" << GetCategory() << "\n";
	
	int32 i;
	for (i = 0; i < CountLinks(); i++)
		out << "\tLINK=" << LinkAt(i) << "\n";
	
	if (CountGroups() > 0)
	{
		out << "\tGROUP=";
		for (i = 0; i < CountGroups(); i++)
		{
			out << GroupAt(i);
			if (i < CountGroups() - 1)
				out << ",";
		}
		out << "\n";
	}
		
	if (CountPlatforms() > 0)
	{
		out << "\tPLATFORM=";
		for (i = 0; i < CountPlatforms(); i++)
		{
			BString os;
			switch (PlatformAt(i))
			{
				case OS_R5:
				{
					os = "r5";
					break;
				}
				case OS_ZETA:
				{
					os = "zeta";
					break;
				}
				case OS_HAIKU:
				{
					os = "haiku";
					break;
				}
				default:
				{
					os = "none";
					break;
				}
			}
			out << os;
			if (i < CountPlatforms() - 1)
				out << ",";
		}
		out << "\n";
	}
	return out;
}


void
FileItem::PrintToStream(int8 indent)
{
	BString tabstr;
	for (int8 i = 0; i < indent; i++)
		tabstr += "\t";
	
	BString out;
	out << tabstr << "File: " << GetName() << "\n";
	tabstr << "\t";
	
	if (GetInstalledName() && strlen(GetInstalledName()) > 0)
		out << tabstr << "Installed Name: " << GetInstalledName() << "\n";
	else
		out << tabstr << "Installed Name: " << GetName() << "\n";
	
	if (fRef.name && strlen(fRef.name) > 0)
	{
		BPath path(&fRef);
		out << tabstr << "Ref Path: " << path.Path() << "\n";
	}
	
	out << tabstr << "Path: " << fPath.Path() << "\n"
		<< tabstr << "Category: " << GetCategory() << "\n";
	
	if (CountGroups() == 0)
		out << tabstr << "Groups: all\n";
	else
	{
		out << tabstr << "Groups: ";
		for (int32 i = 0; i < CountGroups(); i++)
		{
			out << GroupAt(i);
			if (i < CountGroups() - 1)
				out << ", ";
		}
		out << "\n";
	}
	
	if (CountPlatforms() == 0)
		out << tabstr << "Platforms: all\n";
	else
	{
		out << tabstr << "Platforms: ";
		for (int32 i = 0; i < CountPlatforms(); i++)
		{
			out << OSTypeToString(PlatformAt(i));
			if (i < CountPlatforms() - 1)
				out << ", ";
		}
		out << "\n";
	}
	
	printf(out.String());
}


BString *
FileItem::FindItem(BObjectList<BString> &list,const char *string)
{
	for (int32 i = 0; i < list.CountItems(); i++)
	{
		BString *str = list.ItemAt(i);
		if (str->ICompare(string) == 0)
			return str;
	}
	
	return NULL;
}


