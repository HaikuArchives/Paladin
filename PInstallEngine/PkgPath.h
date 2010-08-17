#ifndef PKGPATH_H
#define PKGPATH_H

#include <FindDirectory.h>
#include <String.h>
#include <Volume.h>

#include "OSPath.h"

enum
{
	M_INSTALL_DIRECTORY = 9998,
	M_CUSTOM_DIRECTORY = 9999
};

class PkgPath : public OSPath
{
public:
				PkgPath(void);
				PkgPath(const char *stringpath);
				PkgPath(const PkgPath &from);
	PkgPath &	operator=(const PkgPath &from);
		
	void		SetTo(const char *custom);
	void		SetTo(int32 pathid, BVolume *vol = NULL);
	
	int32		ResolveToConstant(void);
	BString		ResolveToString(void);
	const char *Path(void);
	
	static	void		SetPackageInstallDirectory(const char *path);
	static	const char *GetPackageInstallDirectory(void);
	
private:
	BString		fPath;
};


#endif
