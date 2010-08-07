#include "PkgPath.h"

#include <OS.h>
#include <Path.h>

#include "PackageInfo.h"

extern PackageInfo gPkgInfo;

PkgPath::PkgPath(void)
{
	SetTo(B_APPS_DIRECTORY);
}


PkgPath::PkgPath(const char *custom)
{
	SetTo(custom);
}


PkgPath::PkgPath(int32 pathid)
{
	SetTo(pathid);
}


PkgPath::PkgPath(const PkgPath &from)
{
	*this = from;
}


PkgPath &
PkgPath::operator=(const PkgPath &from)
{
	fOSPath = from.fOSPath;
	fPath = from.fPath;
	fPathConstant = from.fPathConstant;
	return *this;
}


void
PkgPath::SetTo(const char *custom)
{
	fPathConstant = M_CUSTOM_DIRECTORY;
	fPath = custom;
}


void
PkgPath::SetTo(int32 pathid)
{
	if (pathid == M_CUSTOM_DIRECTORY)
	{
		// Setting custom paths is done with the other call
		debugger("BUG: Setting a custom path is done with SetInstallPath(const char *)");
		return;
	}
	
	fPathConstant = pathid;
	
	if (pathid == M_INSTALL_DIRECTORY)
		return;
	
	// Perform the directory resolution here and then all that is required is to make
	// one call to get a string path. :)
	BVolume installVol(gPkgInfo.GetInstallVolume());
	fOSPath.SetVolume(installVol);
	fPath = fOSPath.GetPath(pathid);
	if (fPath.CountChars() < 1)
		fPath = fOSPath.GetPath(B_APPS_DIRECTORY);
}


int32
PkgPath::AsConstant(void) const
{
	return fPathConstant == M_INSTALL_DIRECTORY ? gPkgInfo.GetPathConstant() : fPathConstant;
}


const char *
PkgPath::AsString(void) const
{
	return fPathConstant == M_INSTALL_DIRECTORY ? gPkgInfo.GetResolvedPath() : fPath.String();
}


void
PkgPath::ConvertFromString(const char *string)
{
	if (!string)
		return;
	
	BString value(string);
	
	if (value.ICompare("INSTALL_FOLDER") == 0 || value.ICompare("INSTALLFOLDER") == 0)
		SetTo(M_INSTALL_DIRECTORY);
	else
	{
		int32 dir = fOSPath.StringToDir(value.String());
		if (dir >= 0)
			SetTo(dir);
		else
			SetTo(value.String());
	}

}
