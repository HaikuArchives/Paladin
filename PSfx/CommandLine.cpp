#include "CommandLine.h"

#include <Application.h>
#include <Mime.h>
#include <Resources.h>
#include <stdio.h>
#include <stdlib.h>
#include <TypeConstants.h>

#include "Globals.h"

static PackageInfo sPkgInfo;

void PrintUsage(void);
void PrintHelp(void);

void MakePackage(BObjectList<BString> &args);
void SetPackageInfo(BObjectList<BString> &args);

void AddDependency(BObjectList<BString> &args);
void RemoveDependency(BObjectList<BString> &args);
void SetDependency(BObjectList<BString> &args, DepItem *item = NULL);

void AddFile(BObjectList<BString> &args);
void RemoveFile(BObjectList<BString> &args);
void SetFile(BObjectList<BString> &args, FileItem *item = NULL);

#define USE_TRACE
#ifdef USE_TRACE
	#define STRACE(x) printf x
#else
	#define STRACE(x) /* */
#endif

static const char *sModeArgs[] = {
	"makepkg",
	"setpkginfo",
	"adddep",
	"setdep",
	"deldep",
	"addfile",
	"setfile",
	"delfile",
	"showpkginfo",
	"dumppkg",
	NULL
};

void
PrintUsage(void)
{
	printf("Usage:\n"
			"PSfx <packagepath> <mode> <arguments>\n"
			"PSfx --help,-h: Shows this message\n"
			"PSfx showhelp: Shows comprehensive help information\n");
}


void
PrintHelp(void)
{
	printf(
		"Usage:\n"
		"PSfx --help: Shows this message\n"
		"PSfx -h: Shows this message\n"
		"PSfx showhelp: Shows comprehensive help information\n"
		"PSfx <packagepath> <mode> <arguments>\n"
		"\n"
		"To create a package or edit a package's general information:\n"
		"PSfx <packagepath> makepkg [zeta|haiku|haikugcc4]\n"
		"\n"
		"To edit a package's general information:\n"
		"PSfx <packagepath> setpkginfo appname=<name> appversion=<version>\n"
		"    author=<authorname> [authorcontact=<authoremail>] [releasedate=<date>]\n"
		"    [installfolder=<path>] [createfoldername=<foldername>] [url=<url>]\n"
		"\n"
		"To add a package dependency:\n"
		"PSfx <packagepath> adddep <depname> file <path> [url]\n"
		"PSfx <packagepath> adddep <depname> library <libraryname> [url]\n"
		"\n"
		"To edit an existing package dependency:\n"
		"PSfx <packagepath> setdep <depname> file <path> [url]\n"
		"PSfx <packagepath> setdep <depname> library <libraryname> [url]\n"
		"\n"
		"To remove a package dependency:\n"
		"PSfx <packagepath> deldep <depname>\n"
		"\n"
		"To add a file to the package:\n"
		"PSfx <packagepath> addfile <path> <installfolder> [category=<categoryname>]\n"
		"    [platform=<platformname>] [group=<groupname>]\n"
		"    [link=<path> [link=<path>]...]\n"
		"\n"
		"To change information for a file in the package:\n"
		"PSfx <packagepath> setfile <name> [installfolder=<installfolder>]"
		"    [category=<categoryname>] [platform=<platformname>]\n"
		"    [group=<groupname>] [link1=<path> [link2=<path>]...]\n"
		"\n"
		"To remove a package file:\n"
		"PSfx <packagepath> delfile <filename>\n"
		"\n"
		"To display package information:\n"
		"PSfx <packagepath> showpkginfo\n"
		"\n"
		"To display extended package information:\n"
		"PSfx <packagepath> dumppkg\n"
		"\n"
		"For detailed information on each command, see the PSfx documention\n");
}


bool
ProcessArgs(int argc, char **argv)
{
	if (argc == 1)
		return true;
	
	if (argc > 2)
		gCommandLineMode = true;
	
	int32 i;
	
	for (i = 1; i < argc; i++)
		gArgList.AddItem(new BString(argv[i]));
	
	// Check the mode
	BString *arg = gArgList.ItemAt(0);
	
	if (arg->ICompare("--help") == 0 || arg->ICompare("-h") == 0)
	{
		PrintUsage();
		return false;
	}
	
	if (arg->ICompare("showhelp") == 0)
	{
		PrintHelp();
		return false;
	}
	
	if (argc == 2)
	{
		PrintUsage();
		return false;
	}
	
	arg = gArgList.ItemAt(1);
	
	bool found = false;
	i = 0;
	while (sModeArgs[i])
	{
		if (arg->ICompare(sModeArgs[i]) == 0)
		{
			found = true;
			break;
		}
		i++;
	}
	
	if (!found)
	{
		printf("Unrecognized option '%s'\n",arg->String());
		return false;
	}

	return true;
}


void
DoCommandLine(void)
{
/*
	"setpkginfo",
	"adddep",
	"setdep",
	"deldep",
	"addfile",
	"setfile",
	"delfile",
	"showpkginfo",
	"dumppkg",
*/

	BString *argone = gArgList.ItemAt(0);
	BString *argtwo = gArgList.ItemAt(1);
	
	if (argtwo->ICompare("makepkg") == 0)
	{
		MakePackage(gArgList);
		return;
	}
	
	if (sPkgInfo.LoadFromFile(argone->String()) != B_OK)
	{
		printf("Couldn't load package %s\n", argone->String());
		gReturnValue = -1;
		return;
	}
	
	if (argtwo->ICompare("addfile") == 0)
		AddFile(gArgList);
	else if (argtwo->ICompare("setfile") == 0)
		SetFile(gArgList);
	else if (argtwo->ICompare("delfile") == 0)
		RemoveFile(gArgList);
	else if (argtwo->ICompare("adddep") == 0)
		AddDependency(gArgList);
	else if (argtwo->ICompare("setdep") == 0)
		SetDependency(gArgList);
	else if (argtwo->ICompare("deldep") == 0)
		RemoveDependency(gArgList);
	else if (argtwo->ICompare("setpkginfo") == 0)
		SetPackageInfo(gArgList);
	else if (argtwo->ICompare("showpkginfo") == 0)
		sPkgInfo.PrintToStream();
	else if (argtwo->ICompare("dumppkg") == 0)
		sPkgInfo.DumpInfo();
}


void
MakePackage(BObjectList<BString> &args)
{
	BString pkgPath(args.ItemAt(0)->String());
	
	BString stubName("installstub");
	
	if (args.CountItems() == 3)
	{
		// Platform has been specified. If it's not one of the ones
		// required, bomb out.
		if (args.ItemAt(1)->ICompare("zeta") == 0)
			stubName << ".zeta";
		else
		if (args.ItemAt(1)->ICompare("haiku") == 0)
			stubName << ".haiku.gcc2";
		else
		if (args.ItemAt(1)->ICompare("HaikuGCC4") == 0)
			stubName << ".haiku.gcc4";
		else
		{
			printf("Platform must be Zeta, Haiku, or HaikuGCC4\n");
			return;
		}
	}
	else
	{
		if (gPlatformName.ICompare("Zeta") == 0)
			stubName << ".zeta";
		else
		if (gPlatformName.ICompare("haiku") == 0)
			stubName << ".haiku.gcc2";
		else
		if (gPlatformName.ICompare("HaikuGCC4") == 0)
			stubName << ".haiku.gcc4";
	}
	
	BResources *res = be_app->AppResources();
	
	size_t size;
	int32 id;
	if (!res->GetResourceInfo(B_RAW_TYPE, stubName.String(), &id, &size))
	{
		printf("PSfx has been corrupted and can no longer create new packages. You can, "
				"however, edit existing ones. Reinstalling PSfx would be a good idea.\n");
		gReturnValue = -2;
		return;
	}
	
	const int8 *installstub = (const int8 *)res->LoadResource(B_RAW_TYPE,id, &size);
	BFile file(pkgPath.String(), B_READ_WRITE | B_CREATE_FILE | B_FAIL_IF_EXISTS);
	if (file.InitCheck() != B_OK)
	{
		if (file.InitCheck() == B_FILE_EXISTS)
			printf("PSfx couldn't create a package because the file already exists.\n");
		else
			printf("PSfx couldn't create the package. Sorry.\n");
		gReturnValue = -3;
		return;
	}
	
	file.Write(installstub,size);
	
	mode_t permissions;
	file.GetPermissions(&permissions);
	file.SetPermissions(permissions | S_IXUSR | S_IXGRP | S_IXOTH);
	
	update_mime_info(pkgPath.String(), 0, 1, 0);
	
	file.Unset();
	sPkgInfo.SaveToFile(pkgPath.String());
	
}


void
SetPackageInfo(BObjectList<BString> &args)
{
//		"PSfx setpkginfo <packagepath> appname=<name> appversion=<version>\n"
//		"    author=<authorname> [authorcontact=<authoremail>] [releasedate=<date>]\n"
//		"    [installfolder=<path>] [createfoldername=<foldername>] [url=<url>]\n"
	BString	pkgpath,
			appname,
			appversion,
			authorname,
			authorcontact,
			releasedatestr,
			installfolder,
			createfoldername,
			url;
	time_t	releasedate = 0;
	
	pkgpath = *args.ItemAt(0);
	// arg[1] == 'setpkginfo' command
	for (int32 i = 2; i < args.CountItems(); i++)
	{
		BString *arg = args.ItemAt(i);
		
		int32 pos = arg->IFindFirst("=");
		if (pos == B_ERROR)
		{
			printf("Unrecognized option %s\n",arg->String());
			gReturnValue = -1;
			return;
		}
		
		BString key = *arg;
		key.Truncate(pos);
		
		BString value = arg->String() + pos + 1;
		
		if (key.ICompare("appname") == 0)
			appname = value;
		else if (key.ICompare("appversion") == 0)
			appversion = value;
		else if (key.ICompare("author") == 0)
			authorname = value;
		else if (key.ICompare("authorcontact") == 0)
			authorcontact = value;
		else if (key.ICompare("releasedate") == 0)
		{
			releasedatestr = value;
			
			BString command("date --date=\"");
			command << releasedatestr.String() << "\" +\"%s\"";
			FILE *fd = popen(command.String(),"r");
			if (fd)
			{
				BString out;
				char buffer[32];
				while (fgets(buffer,32,fd))
					out += buffer;
				pclose(fd);
				
				if (out.IFindFirst("invalid date") >= 0)
				{
					printf("Didn't recognize date %s\n",value.String());
					gReturnValue = -1;
					return;
				}
				
				releasedate = atol(out.String());
			}
		}
		else if (key.ICompare("installfolder") == 0)
			installfolder = value;
		else if (key.ICompare("createfoldername") == 0)
			createfoldername = value;
		else if (key.ICompare("url") == 0)
			url = value;
		else
		{
			printf("Unrecognized option %s\n",arg->String());
			gReturnValue = -1;
			return;
		}
	}
	
	if (appname.CountChars() < 1 || appversion.CountChars() < 1 || authorname.CountChars() < 1)
	{
		printf("Program name, program version, and author name are required.\n");
		gReturnValue = -1;
		return;
	}
	
	sPkgInfo.SetName(appname.String());
	sPkgInfo.SetAppVersion(appversion.String());
	sPkgInfo.SetAuthorName(authorname.String());
	
	if (authorcontact.CountChars() > 0)
		sPkgInfo.SetAuthorEmail(authorcontact.String());
	if (releasedatestr.CountChars() > 0)
		sPkgInfo.SetReleaseDate(releasedate);
	if (installfolder.CountChars() > 0)
		sPkgInfo.SetPath(installfolder.String());
	if (createfoldername.CountChars() > 0)
		sPkgInfo.SetInstallFolderName(createfoldername.String());
	if (url.CountChars() > 0)
		sPkgInfo.SetAuthorURL(url.String());
	
	if (sPkgInfo.SaveToFile(pkgpath.String()) != B_OK)
	{
		printf("Couldn't update the package info.\n");
		gReturnValue = -4;
	}
}


void
AddDependency(BObjectList<BString> &args)
{
	if (args.CountItems() < 5 || args.CountItems() > 6)
	{
		printf("Adding a dependency:\n"
				"PSfx <packagepath> adddep <depname> file <path> [url]\n"
				"PSfx <packagepath> adddep <depname> library <libraryname> [url]\n");
		gReturnValue = -1;
		return;
	}
	
	DepItem *item = new DepItem();
	sPkgInfo.AddDependency(item);
	
	SetDependency(args,item);
}


void
SetDependency(BObjectList<BString> &args, DepItem *item)
{
	if (args.CountItems() < 5 || args.CountItems() > 6)
	{
		printf("Editing a dependency:\n"
				"PSfx <packagepath> setdep <depname> file <path> [url]\n"
				"PSfx <packagepath> setdep <depname> library <libraryname> [url]\n");
		gReturnValue = -1;
		return;
	}
	
	BString depname(args.ItemAt(2)->String());
	if (!item)
	{
		for (int32 i = 0; i < sPkgInfo.CountDependencies(); i++)
		{
			DepItem *dep = sPkgInfo.DependencyAt(i);
			if (depname.ICompare(dep->GetName()) == 0)
			{
				item = dep;
				break;
			}
		}
		
		if (!item)
		{
			printf("Dependency %s doesn't exist in this package\n", depname.String());
			gReturnValue = -1;
			return;
		}
	}
	else
		item->SetName(depname.String());
	
	BString pkgpath = *args.ItemAt(0);
	// arg[1] == 'setdep' or 'adddep' command
	
	item->SetType(args.ItemAt(3)->String());
	item->SetPath(args.ItemAt(4)->String());
	
	if (args.ItemAt(5))
		item->SetURL(args.ItemAt(5)->String());
	else
		item->SetURL("");
	
	if (sPkgInfo.SaveToFile(pkgpath.String()) != B_OK)
	{
		printf("Couldn't update the package info.\n");
		gReturnValue = -4;
	}
}


void
RemoveDependency(BObjectList<BString> &args)
{
//	"PSfx <packagepath> deldep <depname>\n"
	if (args.CountItems() != 3)
	{
		printf("Deleting a dependency: PSfx <packagepath> deldep <depname>\n");
		gReturnValue = -1;
		return;
	}
	
	DepItem *item = NULL;
	BString depname(args.ItemAt(2)->String());
	if (!item)
	{
		for (int32 i = 0; i < sPkgInfo.CountDependencies(); i++)
		{
			DepItem *dep = sPkgInfo.DependencyAt(i);
			if (depname.ICompare(dep->GetName()) == 0)
			{
				item = dep;
				break;
			}
		}
		
		if (!item)
		{
			printf("Dependency %s doesn't exist in this package\n", depname.String());
			gReturnValue = -1;
			return;
		}
	}
	
	BString pkgpath = *args.ItemAt(0);
	sPkgInfo.RemoveDependency(item);
	
	if (sPkgInfo.SaveToFile(pkgpath.String()) != B_OK)
	{
		printf("Couldn't update the package info.\n");
		gReturnValue = -4;
	}
}


void
AddFile(BObjectList<BString> &args)
{
	if (args.CountItems() < 3)
	{
		printf("To add a file to the package:\n"
			"PSfx <packagepath> addfile <path> [installfolder=<installfolder>] "
			"[category=<categoryname>] [platform=<platformname>]\n"
			"[group=<groupname>] [link1=<path> [link2=<path>]...]\n");
		gReturnValue = -1;
		return;
	}
	
	FileItem *item = new FileItem();
	sPkgInfo.AddFile(item);
	
	SetFile(args,item);
}


void
SetFile(BObjectList<BString> &args, FileItem *item)
{
	STRACE(("SetFile\n"));
	
	if (args.CountItems() < 3)
	{
		printf("To edit an existing file entry in the package:\n"
			"PSfx <packagepath> setfile <path> [installfolder=<installfolder>] "
			"[category=<categoryname>] [platform=<platformname>]\n"
			"[group=<groupname>] [link1=<path> [link2=<path>]...]\n");
		gReturnValue = -1;
		return;
	}
	
	BString filename(args.ItemAt(2)->String());
	STRACE(("File name: %s\n", filename.String()));
	
	if (!item)
	{
		for (int32 i = 0; i < sPkgInfo.CountFiles(); i++)
		{
			FileItem *file = sPkgInfo.FileAt(i);
			if (filename.ICompare(file->GetName()) == 0)
			{
				item = file;
				break;
			}
		}
		
		if (!item)
		{
			printf("File %s doesn't exist in this package\n", filename.String());
			gReturnValue = -1;
			return;
		}
	}
	else
		item->SetName(filename.String());
	
	BString	pkgpath,
			filepath,
			installfolder,
			category,
			platform,
			group;
	
	BObjectList<BString> linklist(20,true);
	
	pkgpath = *args.ItemAt(0);
	// arg[1] == 'setpkginfo' command
	filepath = *args.ItemAt(2);
	
	for (int32 i = 3; i < args.CountItems(); i++)
	{
		BString *arg = args.ItemAt(i);
		
		int32 pos = arg->IFindFirst("=");
		if (pos == B_ERROR)
		{
			printf("Unrecognized option %s\n",arg->String());
			gReturnValue = -1;
			return;
		}
		
		BString key = *arg;
		key.Truncate(pos);
		
		BString value = arg->String() + pos + 1;
		
		if (key.ICompare("installfolder") == 0)
		{
			STRACE(("install folder: %s\n", value.String()));
			installfolder = value;
		}
		else if (key.ICompare("category") == 0)
		{
			STRACE(("category: %s\n", value.String()));
			category = value;
		}
		else if (key.ICompare("platform") == 0)
		{
			STRACE(("platform: %s\n", value.String()));
			platform = value;
		}
		else if (key.ICompare("group") == 0)
		{
			STRACE(("group: %s\n", value.String()));
			group = value;
		}
		else if (key.ICompare("link") == 0)
		{
			STRACE(("link: %s\n", value.String()));
			linklist.AddItem(new BString(value));
		}
		else
		{
			printf("Unrecognized option %s\n",arg->String());
			gReturnValue = -1;
			return;
		}
	}
	
	if (filepath.CountChars() < 1)
	{
		printf("File path is required.\n");
		gReturnValue = -1;
		return;
	}
	
	item->SetName(filepath.String());
	STRACE(("Setting path: %s\n", installfolder.String()));
	item->SetPath(installfolder.String());
	
	if (category.CountChars() > 0)
		item->SetCategory(category.String());
	
	if (platform.CountChars() > 0)
	{
		int32 commapos = platform.FindLast(",");
		ostype_t osType;
		if (commapos >= 0)
		{
			while (commapos >= 0)
			{
				osType = OS_NONE;
				BString os = platform.String() + commapos + 1;
				if (os.ICompare("all") == 0)
					osType = OS_ALL;
				else
					osType = StringToOSType(os.String());
				item->AddPlatform(osType);
				platform.Truncate(commapos);
				commapos = platform.FindLast(",");
			}
			item->AddPlatform(osType);
		}
		else
		{
			if (platform.ICompare("all") == 0)
				osType = OS_ALL;
			else
				osType = StringToOSType(platform.String());
			item->AddPlatform(osType);
		}
	}
	
	if (group.CountChars() > 0)
	{
		int32 commapos = group.FindLast(",");
		while (commapos >= 0)
		{
			item->AddGroup(group.String() + commapos + 1);
			group.Truncate(commapos);
			commapos = group.FindLast(",");
		}
		item->AddGroup(group.String());
	}
	
	if (linklist.CountItems() > 0)
	{
		// This is to prevent duplicates when editing a file entry
		while (item->CountLinks() > 0)
			item->RemoveLink(item->LinkAt(0));
		
		for (int32 i = 0; i < linklist.CountItems(); i++)
		{
			BString *link = linklist.ItemAt(i);
			item->AddLink(link->String());
		}
	}
	
	if (sPkgInfo.SaveToFile(pkgpath.String()) != B_OK)
	{
		printf("Couldn't update the package info.\n");
		gReturnValue = -4;
	}
}


void
RemoveFile(BObjectList<BString> &args)
{
//	"PSfx <packagepath> delfile <filename>\n"
	if (args.CountItems() != 3)
	{
		printf("Deleting a file entry:\n"
				"PSfx <packagepath> delfile <filename>\n");
		gReturnValue = -1;
		return;
	}
	
	FileItem *item = NULL;
	BString filename(args.ItemAt(2)->String());
	if (!item)
	{
		for (int32 i = 0; i < sPkgInfo.CountFiles(); i++)
		{
			FileItem *file = sPkgInfo.FileAt(i);
			if (filename.ICompare(file->GetName()) == 0)
			{
				item = file;
				break;
			}
		}
		
		if (!item)
		{
			printf("File %s doesn't exist in this package\n", filename.String());
			gReturnValue = -1;
			return;
		}
	}
	
	BString pkgpath = *args.ItemAt(0);
	sPkgInfo.RemoveFile(item);
	
	if (sPkgInfo.SaveToFile(pkgpath.String()) != B_OK)
	{
		printf("Couldn't update the package info.\n");
		gReturnValue = -4;
	}
}
