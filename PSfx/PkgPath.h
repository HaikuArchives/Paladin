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

class PkgPath
{
public:
				PkgPath(void);
				PkgPath(const char *custom);
				PkgPath(int32 pathid);
				PkgPath(const PkgPath &from);
	PkgPath &	operator=(const PkgPath &from);
		
	void		SetTo(const char *custom);
	void		SetTo(int32 pathid, BVolume *vol = NULL);
	void		SetVolume(const BVolume &vol);
	
	int32		AsConstant(void) const;
	const char *AsString(void) const;

	void		ConvertFromString(const char *string);
	
private:
	OSPath	fOSPath;
	BVolume	fVolume;
	
	BString	fPath;
	int32	fPathConstant;
};


#endif
