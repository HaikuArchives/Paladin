#ifndef PKGINFOWINDOW_H
#define PKGINFOWINDOW_H

#include "DWindow.h"

#include <CheckBox.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>

#include "AutoTextControl.h"

class PackageInfo;

class PkgInfoWindow : public DWindow
{
public:
					PkgInfoWindow(BWindow *owner, PackageInfo *info);
	void			MessageReceived(BMessage *msg);

private:
	PackageInfo		*fInfo;
	
	AutoTextControl	*fName,
					*fAppVersion,
					*fAuthorName,
					*fAuthorEmail,
					*fAuthorURL;
	
	BMenuField		*fInstallLocationField;
	BCheckBox		*fShowChooserBox;
	BWindow			*fOwner;
};


#endif
