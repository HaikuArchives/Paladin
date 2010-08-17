#include "PackageInfo.h"

#include <Application.h>
#include <File.h>
#include <Path.h>
#include <Resources.h>
#include <stdlib.h>
#include <TypeConstants.h>
#include <VolumeRoster.h>

#include "Globals.h"

PackageInfo::PackageInfo(void)
	:	fPackageVersion(1.0),
		fReleaseDate(real_time_clock()),
		fPath("B_APPS_DIRECTORY"),
		fShowChooser(false),
		fFiles(20,true),
		fDeps(20,true),
		fGroups(20,true)
{
	BVolumeRoster roster;
	BVolume vol;
	roster.GetBootVolume(&vol);
	fVolumeDevID = vol.Device();
}


status_t
PackageInfo::LoadFromResources(void)
{
	MakeEmpty();
	BResources *res = be_app->AppResources();
	res->PreloadResourceType();
	
	size_t size;
	int32 id;
	if (res->GetResourceInfo(B_STRING_TYPE,"AppName",&id,&size))
		SetName((const char *)res->LoadResource(B_STRING_TYPE,"AppName",&size));
	else
		return B_ERROR;
		
	if (res->GetResourceInfo(B_STRING_TYPE,"AppVersion",&id,&size))
		SetAppVersion((const char *)res->LoadResource(B_STRING_TYPE,"AppVersion",&size));
	else
		return B_ERROR;
	
	BString pkginfo;
	if (res->GetResourceInfo(B_STRING_TYPE,"PkgInfo",&id,&size))
		pkginfo = (const char *)res->LoadResource(B_STRING_TYPE,"PkgInfo",&size);
	else
		return B_ERROR;
	
	return ParsePackageInfo(pkginfo);
}

status_t
PackageInfo::LoadFromFile(const char *path)
{
	BFile file(path,B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	MakeEmpty();
	
	BResources res(&file);
	res.PreloadResourceType();
	
	size_t size;
	int32 id;
	if (res.GetResourceInfo(B_STRING_TYPE,"AppName",&id,&size))
		SetName((const char *)res.LoadResource(B_STRING_TYPE,"AppName",&size));
	else
		return B_ERROR;
		
	if (res.GetResourceInfo(B_STRING_TYPE,"AppVersion",&id,&size))
		SetAppVersion((const char *)res.LoadResource(B_STRING_TYPE,"AppVersion",&size));
	else
		return B_ERROR;
	
	BString pkginfo;
	if (res.GetResourceInfo(B_STRING_TYPE,"PkgInfo",&id,&size))
		pkginfo = (const char *)res.LoadResource(B_STRING_TYPE,"PkgInfo",&size);
	else
		return B_ERROR;
	
	return ParsePackageInfo(pkginfo);
}


status_t
PackageInfo::SaveToFile(const char *path, bool clobber)
{
	int32 fileFlags = B_READ_WRITE | B_CREATE_FILE;
	if (clobber)
		fileFlags |= B_ERASE_FILE;
	BFile file(path,fileFlags);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	BResources res(&file);
	res.AddResource(B_STRING_TYPE, 1, fName.String(), fName.Length(), "AppName");
	res.AddResource(B_STRING_TYPE, 2, fAppVersion.String(), fAppVersion.Length(), "AppVersion");
	
	BString info(MakeInfo());
	res.AddResource(B_STRING_TYPE, 3, info.String(), info.Length(), "PkgInfo");
	return res.Sync();
}


void
PackageInfo::SetName(const char *name)
{
	fName = name;
}


const char *
PackageInfo::GetName(void) const
{
	return fName.String();
}


void
PackageInfo::SetPackageVersion(float ver)
{
	fPackageVersion = ver;
}


float
PackageInfo::GetPackageVersion(void) const
{
	return fPackageVersion;
}


PkgPath
PackageInfo::GetPath(void) const
{
	return fPath;
}


void
PackageInfo::SetPath(const PkgPath &path)
{
	fPath = path;
}


void
PackageInfo::SetPath(const char *path)
{
	fPath.SetTo(path);
}


void
PackageInfo::SetShowChooser(bool value)
{
	fShowChooser = true;
}


bool
PackageInfo::GetShowChooser(void) const
{
	return fShowChooser;
}

	
const char *
PackageInfo::GetInstallFolderName(void) const
{
	return fInstallFolderName.String();
}


void
PackageInfo::SetInstallFolderName(const char *name)
{
	fInstallFolderName = name;
}

	
dev_t
PackageInfo::GetInstallVolume(void) const
{
	return fVolumeDevID;
}


void
PackageInfo::SetInstallVolume(dev_t dev)
{
	fVolumeDevID = dev;
}

	
void
PackageInfo::SetGroup(const char *name)
{
	fInstallGroup = name;
}


const char *
PackageInfo::GetGroup(void) const
{
	return fInstallGroup.String();
}


int32
PackageInfo::CountGroups(void)
{
	return fGroups.CountItems();
}


const char *
PackageInfo::GroupAt(int32 index)
{
	BString *str = fGroups.ItemAt(index);
	return str ? str->String() : NULL;
}


void
PackageInfo::SetAuthorName(const char *name)
{
	fAuthorName = name;
}


const char *
PackageInfo::GetAuthorName(void) const
{
	return fAuthorName.String();
}


void
PackageInfo::SetAuthorEmail(const char *email)
{
	fAuthorEmail = email;
}


const char *
PackageInfo::GetAuthorEmail(void) const
{
	return fAuthorEmail.String();
}


void
PackageInfo::SetAuthorURL(const char *url)
{
	fAuthorURL = url;
}


const char *
PackageInfo::GetAuthorURL(void) const
{
	return fAuthorURL.String();
}


void
PackageInfo::SetAppVersion(const char *ver)
{
	fAppVersion = ver;
}


const char *
PackageInfo::GetAppVersion(void) const
{
	return fAppVersion.String();
}


void
PackageInfo::SetReleaseDate(time_t date)
{
	fReleaseDate = date;
}


time_t
PackageInfo::GetReleaseDate(void) const
{
	return fReleaseDate;
}

	
BString
PackageInfo::GetPrettyReleaseDate(BString format)
{
	char buffer[64];
	strftime(buffer,64,format.String(),localtime(&fReleaseDate));
	return BString(buffer);
}


void
PackageInfo::AddFile(FileItem *file)
{
	if (!file)
		return;
	
	fFiles.AddItem(file);
	for (int32 i = 0; i < file->CountGroups(); i++)
		AddGroup(file->GroupAt(i));
}


void
PackageInfo::RemoveFile(FileItem *file)
{
	int32 index = fFiles.IndexOf(file);
	if (index < 0)
		return;
	
	fFiles.RemoveItemAt(index);
	for (int32 i = 0; i < file->CountGroups(); i++)
		CullGroup(file->GroupAt(i));
	
	delete file;
}


int32
PackageInfo::CountFiles(void) const
{
	return fFiles.CountItems();
}


FileItem *
PackageInfo::FileAt(int32 index)
{
	return fFiles.ItemAt(index);
}


void
PackageInfo::AddDependency(DepItem *dep)
{
	fDeps.AddItem(dep);
}


void
PackageInfo::RemoveDependency(DepItem *dep)
{
	fDeps.RemoveItem(dep);
}


int32
PackageInfo::CountDependencies(void) const
{
	return fDeps.CountItems();
}


DepItem *
PackageInfo::DependencyAt(int32 index)
{
	return fDeps.ItemAt(index);
}


BString
PackageInfo::MakeInfo(void)
{
	BString out;
	
	char buffer[32];
	sprintf(buffer,"%.1f",GetPackageVersion());
	
	out << "PFXPROJECT=Always first line\n"
		<< "PKGVERSION=" << buffer
		<< "\nPKGNAME=" << GetName()
		<< "\nTYPE=SelfExtract"
		<< "\nINSTALLFOLDER=" << fPath.Path() << "\n";
	
	if (fAuthorName.CountChars() > 0)
		out << "AUTHORNAME=" << fAuthorName << "\n";
	if (fAuthorEmail.CountChars() > 0)
		out << "CONTACT=" << fAuthorEmail << "\n";
	if (fAuthorURL.CountChars() > 0)
		out << "URL=" << fAuthorURL << "\n";
	out << "RELEASEDATE=" << fReleaseDate
		<< "\nAPPVERSION=" << (fAppVersion.CountChars() > 0 ? fAppVersion.String() : "0.0.1") << "\n";
	
	for (int32 i = 0; i < fDeps.CountItems(); i++)
	{
		DepItem *depItem = fDeps.ItemAt(i);
		out << depItem->MakeInfo();
	}

	for (int32 i = 0; i < fFiles.CountItems(); i++)
	{
		FileItem *fileItem = fFiles.ItemAt(i);
		out << fileItem->MakeInfo();
	}
	return out;
}


void
PackageInfo::PrintInfo(FILE *fd)
{
	if (!fd)
		fd = stdout;
	
	fprintf(fd, MakeInfo().String());
}


void
PackageInfo::PrintToStream(void)
{
	printf("Package:\n"
			"----------------\n"
			"Name : %s\n"
			"Package Version: %.1f\n"
			"Install Path: %s\n"
			"User can change install path: %s\n"
			"Author Name: %s\n"
			"Author E-mail: %s\n"
			"Author website: %s\n"
			"App version: %s\n"
			"Release Date: %s\n",
			GetName(), GetPackageVersion(),	fPath.Path(), (GetShowChooser() ? "yes" : "no"),
			GetAuthorName(), GetAuthorEmail(), GetAuthorURL(), GetAppVersion(),
			GetPrettyReleaseDate().String());
	
	if (fGroups.CountItems() > 0)
	{
		printf("Install Groups: \n");
		for (int32 i = 0; i < fGroups.CountItems(); i++)
		{
			BString *string = fGroups.ItemAt(i);
			printf("\t%s\n",string->String());
		}
	}
	else
		printf("All files belong to the same install group\n");
	
	if (fFiles.CountItems() > 0)
	{
		printf("Files:\n");
		for (int32 i = 0; i < fFiles.CountItems(); i++)
		{
			FileItem *fileItem = fFiles.ItemAt(i);
			printf("\t%s\n",fileItem->GetName());
		}
	}
	else
		printf("Files: none\n");
	
	if (fDeps.CountItems() > 0)
	{
		printf("Dependencies:\n");
		for (int32 i = 0; i < fDeps.CountItems(); i++)
		{
			DepItem *depItem = fDeps.ItemAt(i);
			printf("\t%s\n",depItem->GetName());
		}
	}
	else
		printf("Dependencies: none\n");
}


void
PackageInfo::DumpInfo(void)
{
	printf("Package:\n"
			"----------------\n"
			"Name : %s\n"
			"Package Version: %.1f\n"
			"Install Path: %s\n"
			"Show install path chooser: %s\n"
			"Author Name: %s\n"
			"Author E-mail: %s\n"
			"Author website: %s\n"
			"App version: %s\n"
			"Release Date: %s\n",
			GetName(), GetPackageVersion(),	fPath.Path(),  (GetShowChooser() ? "yes" : "no"),
			GetAuthorName(), GetAuthorEmail(), GetAuthorURL(), GetAppVersion(),
			GetPrettyReleaseDate().String());
	
	for (int32 i = 0; i < fFiles.CountItems(); i++)
	{
		FileItem *fileItem = fFiles.ItemAt(i);
		fileItem->PrintToStream();
	}
	
	for (int32 i = 0; i < fDeps.CountItems(); i++)
	{
		DepItem *depItem = fDeps.ItemAt(i);
		depItem->PrintToStream();
	}
}


void
PackageInfo::MakeEmpty(void)
{
	fFiles.MakeEmpty();
	fDeps.MakeEmpty();
	fGroups.MakeEmpty();
	
	fName = "";
	fPackageVersion = 0.0;
	fReleaseDate = 0;
	fPath.SetTo("M_INSTALL_DIRECTORY");
	
	BVolumeRoster roster;
	BVolume vol;
	roster.GetBootVolume(&vol);
	fVolumeDevID = vol.Device();
	
	fInstallFolderName = "";
	fInstallGroup = "";
	fShowChooser = false;
	
	fAuthorName = "";
	fAuthorEmail = "";
	fAuthorURL = "";
	fAppVersion = "";
}


void
PackageInfo::AddGroup(const char *group)
{
	if (!group || strlen(group) < 1)
		return;
	
	if (BString(group).ICompare("all") == 0)
		return;
	
	if (!FindGroup(group))
		fGroups.AddItem(new BString(group));
}


void
PackageInfo::RemoveGroup(const char *group)
{
	if (!group)
		return;

	if (BString(group).ICompare("all") == 0)
		return;
	
	BString *str = FindGroup(group);
	if (str)
		fGroups.RemoveItem(str);
}


BString *
PackageInfo::FindGroup(const char *group)
{
	if (!group || strlen(group) < 1)
		return NULL;
	
	for (int32 i = 0; i < fGroups.CountItems(); i++)
	{
		BString *str = fGroups.ItemAt(i);
		if (str->ICompare(group) == 0)
			return str;
	}
	
	return NULL;
}


void
PackageInfo::CullGroup(const char *group)
{
	if (!FindGroup(group))
		return;
	
	bool found = false;
	for (int32 i = 0; i < CountFiles(); i++)
	{
		FileItem *file = FileAt(i);
		if (file->BelongsToGroup(group))
		{
			found = true;
			break;
		}
	}
	
	if (!found)
		RemoveGroup(group);
}


status_t
PackageInfo::ParsePackageInfo(BString str)
{
	char *pkgstr = NULL;
	char pkgdata[str.Length() + 1];
	sprintf(pkgdata,"%s",str.String());
	
	status_t status = B_OK;
	FileItem *fileItem = NULL;
	DepItem *depItem = NULL;
	pkgstr = strtok(pkgdata,"\n");
	int32 lineNumber = 0;
	
	while (pkgstr)
	{
		lineNumber++;
		
		BString key(pkgstr), value;
		int8 level = 0;
		int32 pos = key.FindFirst("=");
		if (pos < 0 || key.CountChars() < 1 || key[0] == '#')
		{
			pkgstr = strtok(NULL,"\n");
			continue;
		}
		
		value = key.String() + pos + 1;
		key.Truncate(pos);
		if (key.FindFirst("\t") == 0)
		{
			key.RemoveFirst("\t");
			level++;
		}
		
		if (key.CountChars() < 1)
		{
			status = B_ERROR;
			break;
		}
		
		if (level == 0)
		{
			if (key.ICompare("INSTALLFOLDER") == 0)
			{
				if (value.ICompare("askuser") == 0)
					SetShowChooser(true);
				else
					fPath.SetTo(value.String());
			}
			else if (key.ICompare("INSTALLFOLDERNAME") == 0)
				SetInstallFolderName(value.String());
			else if (key.ICompare("PKGNAME") == 0)
				SetName(value.String());
			else if (key.ICompare("AUTHORNAME") == 0)
				SetAuthorName(value.String());
			else if (key.ICompare("CONTACT") == 0)
				SetAuthorEmail(value.String());
			else if (key.ICompare("URL") == 0)
				SetAuthorURL(value.String());
			else if (key.ICompare("RELEASEDATE") == 0)
				SetReleaseDate(atol(value.String()));
			else if (key.ICompare("APPVERSION") == 0)
				SetPackageVersion(atof(value.String()));
			else if (key.ICompare("APPVERSION") == 0)
				SetAppVersion(value.String());
			else if (key.ICompare("FILE") == 0)
			{
				if (depItem)
				{
					AddDependency(depItem);
					depItem = NULL;
				}
				if (fileItem)
					AddFile(fileItem);
				fileItem = new FileItem;
				fileItem->SetName(value.String());
			}
			else if (key.ICompare("DEPENDENCY") == 0)
			{
				if (fileItem)
				{
					AddFile(fileItem);
					fileItem = NULL;
				}
				if (depItem)
					AddDependency(depItem);
				depItem = new DepItem;
				depItem->SetName(value.String());
			}
		}
		else
		{
			if (fileItem)
			{
				if (key.ICompare("INSTALLEDNAME") == 0)
					fileItem->SetInstalledName(value.String());
				else if (key.ICompare("INSTALLFOLDER") == 0)
					fileItem->SetPath(value.String());
				else if (key.ICompare("LINK") == 0)
					fileItem->AddLink(value.String());
				else if (key.ICompare("CATEGORY") == 0)
					fileItem->SetCategory(value.String());
				else if (key.ICompare("PLATFORM") == 0)
				{
					int32 commapos = value.FindLast(",");
					ostype_t osType;
					if (commapos >= 0)
					{
						while (commapos >= 0)
						{
							osType = OS_NONE;
							BString os = value.String() + commapos + 1;
							if (os.ICompare("all") == 0)
								osType = OS_ALL;
							else
								osType = StringToOSType(os.String());
							fileItem->AddPlatform(osType);
							value.Truncate(commapos);
							commapos = value.FindLast(",");
						}
						fileItem->AddPlatform(osType);
					}
					else
					{
						if (value.ICompare("all") == 0)
							osType = OS_ALL;
						else
							osType = StringToOSType(value.String());
						fileItem->AddPlatform(osType);
					}
				}
				else if (key.ICompare("GROUP") == 0)
				{
					int32 commapos = value.FindLast(",");
					while (commapos >= 0)
					{
						fileItem->AddGroup(value.String() + commapos + 1);
						value.Truncate(commapos);
						commapos = value.FindLast(",");
					}
					fileItem->AddGroup(value.String());
				}
				else
					printf("Skipping unknown line %ld in file %s\n",lineNumber, fileItem->GetName());
			}
			else if (depItem)
			{
				if (key.ICompare("FILENAME") == 0)
					depItem->SetFileName(value.String());
				else if (key.ICompare("TYPE") == 0)
					depItem->SetType(value.String());
				else if (key.ICompare("PATH") == 0)
					depItem->SetPath(value.String());
				else if (key.ICompare("DEPURL") == 0)
					depItem->SetURL(value.String());
				else
					printf("Skipping unknown line %ld in dependency %s\n",lineNumber, depItem->GetName());
			}
			else
			{
				printf("Bad package syntax: %s:%s in line %ld\n",key.String(), value.String(),
						lineNumber);
			}
		}
		
		pkgstr = strtok(NULL,"\n");
	}
	
	if (fileItem)
		AddFile(fileItem);
	if (depItem)
		AddDependency(depItem);
	
	return status;
}


