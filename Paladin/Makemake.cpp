#include "Makemake.h"

#include <File.h>
#include <String.h>

#include "Project.h"
#include "SourceFile.h"

status_t
MakeMake(Project *proj, DPath outfile)
{
	if (!proj || outfile.IsEmpty())
		return B_ERROR;
		
	BString data;
	data << "NAME= " << proj->GetTargetName() << "\n";
	
	data << "TYPE= ";
	switch (proj->TargetType())
	{
		case TARGET_SHARED_LIB:
		{
			data << "SHARED\n";
			break;
		}
		case TARGET_STATIC_LIB:
		{
			data << "STATIC\n";
			break;
		}
		case TARGET_DRIVER:
		{
			data << "DRIVER\n";
			break;
		}
		default:
		{
			data << "APP\n";
			break;
		}
	}
	
	BString projfolder = proj->GetPath().GetFolder();
	projfolder << "/";
	
	data << "SRCS=";
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
				data << " " << path;
			}
		}
	}
	data << "\n";
	
	data << "RSRCS=";
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
				data << " " << path;
			}
		}
	}
	data << "\n";
	
	data << "LIBS=";
	for (i = 0; i < proj->CountLibraries(); i++)
	{
		SourceFile *file = proj->LibraryAt(i);
		BString ext = file->GetPath().GetExtension();
		if (ext.ICompare("so") == 0 || ext.ICompare("a") == 0)
		{
			BString path = file->GetPath().GetFullPath();
			path.RemoveFirst(projfolder);
			data << " " << path;
		}
	}
	data << "\n";
	
	data << "LIBPATHS=\n";
	
	data << "SYSTEM_INCLUDE_PATHS=";
	for (i = 0; i < proj->CountSystemIncludes(); i++)
	{
		BString path = proj->SystemIncludeAt(i);
		path.CharacterEscape("' ",'\\');
		data << " " << path;
	}
	data << "\n";
	
	data << "LOCAL_INCLUDE_PATHS=";
	for (i = 0; i < proj->CountLocalIncludes(); i++)
	{
		BString path = proj->LocalIncludeAt(i).Relative();
		path.CharacterEscape("' ",'\\');
		data << " " << path;
	}
	data << "\n";
	
	data << "OPTIMIZE=";
	switch (proj->OpLevel())
	{
		case 0:
		{
			data << "NONE\n";
			break;
		}
		case 1:
		case 2:
		{
			data << "SOME\n";
			break;
		}
		case 3:
		{
			data << "FULL\n";
			break;
		}
		default:
		{
			data << "\n";
			break;
		}
	}
	
	data << "#	specify any preprocessor symbols to be defined.  The symbols will not\n"
			"#	have their values set automatically; you must supply the value (if any)\n"
			"#	to use.  For example, setting DEFINES to \"DEBUG=1\" will cause the\n"
			"#	compiler option \"-DDEBUG=1\" to be used.  Setting DEFINES to \"DEBUG\"\n"
			"#	would pass \"-DDEBUG\" on the compiler's command line.\n"
			"DEFINES=\n";
	
	data <<	"#	specify special warning levels\n"
			"#	if unspecified default warnings will be used\n"
			"#	NONE = supress all warnings\n"
			"#	ALL = enable all warnings\n"
			"WARNINGS =\n";
	
	data << "# Build with debugging symbols if set to TRUE\n"
			"SYMBOLS=\n";
	
	data << "COMPILER_FLAGS=";
	if (strlen(proj->ExtraCompilerOptions()) > 0)
		data << proj->ExtraCompilerOptions();
	data << "\n";
	
	data << "LINKER_FLAGS=";
	if (strlen(proj->ExtraLinkerOptions()) > 0)
		data << proj->ExtraLinkerOptions();
	data << "\n";
	
	if (proj->TargetType() == TARGET_DRIVER)
	{
		data << "#	For drivers only. Specify desired location of driver in the /dev\n"
				"#	hierarchy. Used by the driverinstall rule. E.g., DRIVER_PATH = video/usb will\n"
				"#	instruct the driverinstall rule to place a symlink to your driver's binary in\n"
				"#	~/add-ons/kernel/drivers/dev/video/usb, so that your driver will appear at\n"
				"#	/dev/video/usb when loaded. Default is \"misc\".\n"
				"DRIVER_PATH =\n";
	}
	
	data << "\n## include the makefile-engine\ninclude $(BUILDHOME)/etc/makefile-engine\n";
	
	BFile file(outfile.GetFullPath(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	file.Write(data.String(),data.Length());
	
	return B_OK;
}
