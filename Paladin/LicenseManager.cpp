/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "LicenseManager.h"

#include <Button.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <LayoutBuilder.h>
#include <ListItem.h>
#include <ListView.h>
#include <Path.h>
#include <Screen.h>
#include <ScrollView.h>
#include <Size.h>
#include <StringView.h>
#include <TextView.h>

#include "DPath.h"
#include "EscapeCancelFilter.h"
#include "Globals.h"
#include "PLocale.h"


enum {
	M_LICENSE_SELECTED = 'lcsl',
	M_LICENSE_CHOSEN = 'lcch'
};


LicenseManager::LicenseManager(const char* targetPath)
	:
	BWindow(BRect(0.0f, 0.0f, 640.0f, 480.0f), TR("Add license to project"),
		B_TITLED_WINDOW, 0),
	fTargetPath(targetPath),
	fPathList(20, true)
{
	AddCommonFilter(new EscapeCancelFilter());

	fLicenseList = new BListView("licenseList");
	fLicenseList->SetExplicitSize(BSize(fLicenseList->StringWidth("M") * 16,
		B_SIZE_UNSET));
	BScrollView* listScrollView = new BScrollView("listScrollView", fLicenseList,
		B_WILL_DRAW, false, true);
	fLicenseList->SetSelectionMessage(new BMessage(M_LICENSE_SELECTED));

	fLicenseShort = new BTextView("shortLicense");
	fLicenseShort->MakeEditable(false);
	fLicenseShort->SetExplicitPreferredSize(BSize(B_SIZE_UNLIMITED,
		B_SIZE_UNLIMITED));
	BScrollView* shortScrollView = new BScrollView("shortScrollView",
		fLicenseShort, B_WILL_DRAW, false, true);

	fLicenseLong = new BTextView("longLicense");
	fLicenseLong->MakeEditable(false);
	fLicenseLong->SetExplicitPreferredSize(BSize(B_SIZE_UNLIMITED,
		B_SIZE_UNLIMITED));
	BScrollView* longScrollView = new BScrollView("longScrollView",
		fLicenseLong, B_WILL_DRAW | B_FRAME_EVENTS, false, true);

	BButton* addButton = new BButton("addButton", TR("Set License"),
		new BMessage(M_LICENSE_CHOSEN));

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.AddGroup(B_VERTICAL, 0.0f)
				.Add(new BStringView("licenseLabel", TR("License:")))
				.Add(listScrollView)
				.End()
			.AddGroup(B_VERTICAL)
				.AddGroup(B_VERTICAL, 0)
					.Add(new BStringView("shortLabel", TR("Summary:")))
					.Add(shortScrollView)
					.End()
				.AddGroup(B_VERTICAL, 0)
					.Add(new BStringView("longLabel", TR("Full Version:")))
					.Add(longScrollView)
					.End()
				.End()
			.End()
		.AddGroup(B_HORIZONTAL, 0)
			.AddGlue()
			.Add(addButton)
			.End()
		.SetInsets(B_USE_WINDOW_INSETS)
		.End();

	addButton->MakeDefault(true);
	ScanLicenses();
	CenterOnScreen();
}


void
LicenseManager::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_LICENSE_SELECTED:
		{
			int32 selection = fLicenseList->CurrentSelection();
			if (selection < 0)
				break;

			LoadLicense(fPathList.ItemAt(selection)->String());
			break;
		}

		case M_LICENSE_CHOSEN:
		{
			int32 selection = fLicenseList->CurrentSelection();
			if (selection < 0)
				break;
			
			CopyLicense(fPathList.ItemAt(selection)->String());
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		default:
			BWindow::MessageReceived(message);
			break;
	}
}


void
LicenseManager::ScanLicenses()
{
	DPath licensepath(gAppPath.GetFolder());
	licensepath << "Licenses";

	BDirectory directory(licensepath.GetFullPath());

	entry_ref ref;
	while (directory.GetNextRef(&ref) == B_OK) {
		BPath path(&ref);
		BString pathString(path.Path());
		if (pathString.FindLast(".summary") >= 0)
			continue;

		fPathList.AddItem(new BString(path.Path()));
		fLicenseList->AddItem(new BStringItem(ref.name));
	}
}


void
LicenseManager::LoadLicense(const char* mainPath)
{
	BString path(mainPath);
	BFile file(path.String(),B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	off_t size;
	file.GetSize(&size);

	BString filedata;
	char* fileData = filedata.LockBuffer(size + 1);
	file.Read(fileData,size);
	filedata.UnlockBuffer();
	fLicenseLong->SetText(filedata.String());

	path << ".summary";
	file.SetTo(path.String(),B_READ_ONLY);
	if (file.InitCheck() != B_OK) {
		fLicenseShort->SetText("A summary of this license is not available.");
		return;
	}

	filedata = "";
	file.GetSize(&size);
	fileData = filedata.LockBuffer(size + 1);
	file.Read(fileData,size);
	fileData[size] = '\0';
	filedata.UnlockBuffer();
	fLicenseShort->SetText(filedata.String());
}


void
LicenseManager::CopyLicense(const char* mainPath)
{
	BString path(mainPath);
	BFile file(path.String(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	off_t size;
	file.GetSize(&size);

	BString filedata;
	char* fileData = filedata.LockBuffer(size + 1);
	file.Read(fileData, size);
	filedata.UnlockBuffer();

	BPath target(fTargetPath.String());
	target.Append("LICENSE");

	file.SetTo(target.Path(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return;

	file.Write(filedata.String(), filedata.Length());
}
