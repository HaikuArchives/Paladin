#ifndef LICENSE_MANAGER_H
#define LICENSE_MANAGER_H

#include "ObjectList.h"
#include <ListItem.h>
#include <ListView.h>
#include <String.h>
#include <TextView.h>
#include "DWindow.h"

class LicenseManager : public DWindow
{
public:
			LicenseManager(const char *targetpath);
	void	MessageReceived(BMessage *msg);
	void	FrameResized(float w, float h);
	
private:
	void	ScanLicenses(void);
	void	LoadLicense(const char *path);
	void	CopyLicense(const char *path);
	
	BString		fTargetPath;
	BListView	*fLicenseList;
	BTextView	*fLicenseShort,
				*fLicenseLong;
	BObjectList<BString>	fPathList;
};

#endif
