#include "PkgPath.h"

#include <OS.h>
#include <Path.h>
#include <VolumeRoster.h>

#include "Globals.h"
#include "PackageInfo.h"

static PkgPath sPackageInstallDir("B_APPS_DIRECTORY");


PkgPath::PkgPath(void)
{
	BVolumeRoster roster;
	roster.GetBootVolume(&fVolume);
	
	SetTo(B_APPS_DIRECTORY, &fVolume);
}


PkgPath::PkgPath(const char *stringpath)
{
	BVolumeRoster roster;
	roster.GetBootVolume(&fVolume);
	
	SetTo(stringpath);
}


PkgPath::PkgPath(const PkgPath &from)
{
	*this = from;
}


PkgPath &
PkgPath::operator=(const PkgPath &from)
{
	fPath = from.fPath;
	return *this;
}


void
PkgPath::SetTo(const char *stringpath)
{
	fPath = stringpath;
}


void
PkgPath::SetTo(int32 pathid, BVolume *vol)
{
	if (pathid == M_INSTALL_DIRECTORY)
		return;
	
	OSPath os;
	fPath = os.DirToString(pathid);
	
	if (vol)
		fVolume = *vol;
	else
	{
		BVolumeRoster roster;
		roster.GetBootVolume(&fVolume);
	}
}


int32
PkgPath::ResolveToConstant(void) const
{
	if ((fPath.ByteAt(0) != 'M' && fPath.ByteAt(0) != 'B') || fPath.CountChars() < 1)
		return B_ERROR;
	
	if (fPath[1] != '_')
		return B_ERROR;
	
	BString temp(fPath);
	
	int32 slashpos = temp.FindFirst("/");
	if (slashpos >= 0)
		return M_CUSTOM_DIRECTORY;
	
	OSPath os;
	return os.StringToDir(temp.String());
}


BString
PkgPath::ResolveToString(void) const
{
	if ((fPath.FindFirst("M_") != 0 && fPath.FindFirst("B_") != 0) || fPath.CountChars() < 1)
		return fPath;
	
	BString temp(fPath);
	int32 slashpos = temp.FindFirst("/");
	if (slashpos >= 0)
		temp.Truncate(slashpos);
	
	BString out(fPath);
	
	if (temp == "M_INSTALL_DIRECTORY")
		out.ReplaceFirst("M_INSTALL_DIRECTORY", sPackageInstallDir.ResolveToString().String());
	else
	{
		OSPath os;
		int32 dirWhich = os.StringToDir(temp.String());
		if (dirWhich < 0)
			return fPath;
		
		BString found = os.GetPath(dirWhich);
		out.ReplaceFirst(temp.String(), found.String());
	}
	return out;
}


const char *
PkgPath::Path(void)
{
	return fPath.String();
}


void
PkgPath::SetPackageInstallDirectory(const char *path)
{
	sPackageInstallDir.SetTo(path);
}


const char *
PkgPath::GetPackageInstallDirectory(void)
{
	return sPackageInstallDir.Path();
}

