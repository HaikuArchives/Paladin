#ifndef PKGINFOWINDOW_H
#define PKGINFOWINDOW_H

#include "DWindow.h"

class PackageInfo;

class PkgInfoWindow : public DWindow
{
public:
					PkgInfoWindow(PackageInfo *info);

private:
	PackageInfo		*fInfo;
};


#endif
