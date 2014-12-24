#include "Makemake.h"

#include <File.h>
#include <String.h>

#include "Project.h"
#include "Makefile.h"
#include "SourceFile.h"

static BString _SerializeBool(bool val)
{
	return val ? "TRUE" : "FALSE";
}
//static BString _SerializeStringList(BStringList val)
//{
//	return val.Join(" \\\n\t");
//}

status_t
MakeMake(Project *proj, DPath outfile)
{
	if (!proj || outfile.IsEmpty())
		return B_ERROR;
		
	BString mkfile(template_makefile);

	mkfile.ReplaceFirst("$@NAME@$", proj->GetTargetName());
	
	BString typeString;
	switch (proj->TargetType())
	{
	case TARGET_SHARED_LIB:
		typeString = "SHARED";
	break;
	case TARGET_STATIC_LIB:
		typeString = "STATIC";
	break;
	case TARGET_DRIVER:
		typeString = "DRIVER";
	break;
	default:
		typeString = "APP";
	break;
	}
	mkfile.ReplaceFirst("$@TYPE@$", typeString);
	
	BString projfolder = proj->GetPath().GetFolder();
	projfolder << "/";
	
	BString srcsString;
	int32 i,j;
	for (i = 0; i < proj->CountGroups(); i++)
	{
		SourceGroup *group = proj->GroupAt(i);
		for (j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			BString ext = file->GetPath().GetExtension();
			if (ext.ICompare("c") == 0 || ext.ICompare("cpp") == 0 ||
				ext.ICompare("cc") == 0 || ext.ICompare("cxx") == 0)
			{
				BString path = file->GetPath().GetFullPath();
				path.RemoveFirst(projfolder);
				path.CharacterEscape("' ",'\\');
				srcsString << " " << path;
			}
		}
	}
	mkfile.ReplaceFirst("$@SRCS@$", srcsString);
	
	BString rsrcsString;
	for (i = 0; i < proj->CountGroups(); i++)
	{
		SourceGroup *group = proj->GroupAt(i);
		for (j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			BString ext = file->GetPath().GetExtension();
			if (ext.ICompare("rsrc") == 0)
			{
				BString path = file->GetPath().GetFullPath();
				path.RemoveFirst(projfolder);
				rsrcsString << " " << path;
			}
		}
	}
	mkfile.ReplaceFirst("$@RSRCS@$", rsrcsString);
	
	BString libsString;
	for (i = 0; i < proj->CountLibraries(); i++)
	{
		SourceFile *file = proj->LibraryAt(i);
		BString ext = file->GetPath().GetExtension();
		if (ext.ICompare("so") == 0 || ext.ICompare("a") == 0)
		{
			BString path = file->GetPath().GetFullPath();
			path.RemoveFirst(projfolder);
			libsString << " " << path;
		}
	}
	mkfile.ReplaceFirst("$@LIBS@$", libsString);
	
	BString inclPaths;
	for (i = 0; i < proj->CountSystemIncludes(); i++)
	{
		BString path = proj->SystemIncludeAt(i);
		path.CharacterEscape("' ",'\\');
		inclPaths << " " << path;
	}
	mkfile.ReplaceFirst("$@SYSTEM_INCLUDE_PATHS@$", inclPaths);
	
	inclPaths = "";
	for (i = 0; i < proj->CountLocalIncludes(); i++)
	{
		BString path = proj->LocalIncludeAt(i).Relative();
		path.CharacterEscape("' ",'\\');
		inclPaths << " " << path;
	}
	mkfile.ReplaceFirst("$@LOCAL_INCLUDE_PATHS@$", inclPaths);
	
	BString optimizeString;
	switch (proj->OpLevel())
	{
	case 0:
		optimizeString = "NONE";
	break;
	case 1:
	case 2:
		optimizeString = "SOME";
	break;
	case 3:
		optimizeString = "FULL";
	break;
	default:
	break;
	}
	mkfile.ReplaceFirst("$@OPTIMIZE@$", optimizeString);
		
	mkfile.ReplaceFirst("$@COMPILER_FLAGS@$", proj->ExtraCompilerOptions());
	mkfile.ReplaceFirst("$@LINKER_FLAGS@$", proj->ExtraLinkerOptions());
	
	// Unused stuff
	mkfile.ReplaceFirst("$@APP_MIME_SIG@$", "");
	mkfile.ReplaceFirst("$@RDEFS@$", "");
	mkfile.ReplaceFirst("$@LIBPATHS@$", "");
	mkfile.ReplaceFirst("$@LOCALES@$", "");
	mkfile.ReplaceFirst("$@DEFINES@$", "");
	mkfile.ReplaceFirst("$@WARNINGS@$", "");
	mkfile.ReplaceFirst("$@SYMBOLS@$", "");
	mkfile.ReplaceFirst("$@DEBUGGER@$", "");
	mkfile.ReplaceFirst("$@DRIVER_PATH@$", "");
	
	BFile file(outfile.GetFullPath(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	if (file.Write(mkfile.String(), mkfile.Length()) == mkfile.Length())
		return B_OK;
	else
		return B_ERROR;
}
