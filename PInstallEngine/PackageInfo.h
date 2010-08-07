#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include <FindDirectory.h>
#include <stdio.h>
#include <String.h>
#include <time.h>
#include <Volume.h>

#include "ObjectList.h"
#include "DepItem.h"
#include "FileItem.h"
#include "PkgPath.h"

enum
{
	PKG_SELF_EXTRACT = 0
};

class DepItem;
class FileItem;

class PackageInfo
{
public:
							PackageInfo(void);
	status_t				LoadFromResources(void);
	status_t				LoadFromFile(const char *path);
	status_t				SaveToFile(const char *path);
	
	// The package name is the name to be displayed to the user and doesn't necessarily
	// equate with the package's filename
	void					SetName(const char *name);
	const char *			GetName(void) const;
	
	// This is a number set by the package maintainer, not from the resource data in the embedded
	// app. This comes in particularly handy for those C libraries which have none of that useful
	// version information nonsense
	void					SetPackageVersion(float ver);
	float					GetPackageVersion(void) const;
	
	int32					GetPathConstant(void) const;
	const char *			GetResolvedPath(void) const;
	void					SetInstallPath(int32 path);
	void					SetInstallPath(const char *path);
	
	// SetShowChooser isn't normally called but can be. GetShowChooser is true when the user
	// should be given the option to choose the install folder.
	void					SetShowChooser(bool value);
	bool					GetShowChooser(void) const;
	
	const char *			GetInstallFolderName(void) const;
	void					SetInstallFolderName(const char *name);
	
	dev_t					GetInstallVolume(void) const;
	void					SetInstallVolume(dev_t dev);
	
	// These are the functions for choosing the install type -- the "group" from PackageBuilder
	// They are kept up-to-date by adding and removing groups whenever a file is removed or added
	const char *			GetGroup(void) const;
	void					SetGroup(const char *name);
	int32					CountGroups(void);
	const char *			GroupAt(int32 index);
	
	
	void					SetAuthorName(const char *name);
	const char *			GetAuthorName(void) const;
	
	void					SetAuthorEmail(const char *email);
	const char *			GetAuthorEmail(void) const;
	
	void					SetAuthorURL(const char *url);
	const char *			GetAuthorURL(void) const;
	
	void					SetAppVersion(const char *ver);
	const char *			GetAppVersion(void) const;
	
	void					SetReleaseDate(time_t date);
	time_t					GetReleaseDate(void) const;
	BString					GetPrettyReleaseDate(BString format = "%x");
	
	void					AddFile(FileItem *file);
	void					RemoveFile(FileItem *file);
	int32					CountFiles(void) const;
	FileItem *				FileAt(int32 index);
	
	void					AddDependency(DepItem *file);
	void					RemoveDependency(DepItem *file);
	int32					CountDependencies(void) const;
	DepItem *				DependencyAt(int32 index);
	
	BString					MakeInfo(void);
	void					PrintInfo(FILE *fd);
	void					PrintToStream(void);
	void					DumpInfo(void);
	
private:
	void					AddGroup(const char *group);
	void					RemoveGroup(const char *group);
	BString *				FindGroup(const char *group);
	void					CullGroup(const char *group);
	
	status_t				ParsePackageInfo(BString str);
	
	BString					fName;
	float					fPackageVersion;
	time_t					fReleaseDate;
	
	PkgPath					fPath;
	dev_t					fVolumeDevID;
	BString					fInstallFolderName,
							fInstallGroup;
	bool					fShowChooser;
	
	BString					fAuthorName,
							fAuthorEmail,
							fAuthorURL,
							fAppVersion;
	
	BObjectList<FileItem>	fFiles;
	BObjectList<DepItem>	fDeps;
	BObjectList<BString>	fGroups;
};


#endif
