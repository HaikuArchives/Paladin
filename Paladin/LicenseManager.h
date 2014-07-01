/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _LICENSE_MANAGER_H
#define _LICENSE_MANAGER_H


#include <ObjectList.h>
#include <String.h>
#include <Window.h>


class BListView;
class BTextView;

class LicenseManager : public BWindow {
public:
									LicenseManager(const char* targetPath);
			void					MessageReceived(BMessage* message);

private:
			void					ScanLicenses();
			void					LoadLicense(const char* mainPath);
			void					CopyLicense(const char* mainPath);

			BString					fTargetPath;
			BListView*				fLicenseList;
			BTextView*				fLicenseShort;
			BTextView*				fLicenseLong;
			BObjectList<BString>	fPathList;
};


#endif // _LICENSE_MANAGER_H
