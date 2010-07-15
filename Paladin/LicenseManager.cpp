#include "LicenseManager.h"

#include <Button.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>
#include <Screen.h>
#include <ScrollView.h>
#include <StringView.h>

#include "DPath.h"
#include "EscapeCancelFilter.h"
#include "Globals.h"
#include "PLocale.h"

enum
{
	M_LICENSE_SELECTED = 'lcsl',
	M_LICENSE_CHOSEN = 'lcch'
};

LicenseManager::LicenseManager(const char *targetpath)
	:	DWindow(BRect(0,0,500,400),TR("Add Project License"),B_TITLED_WINDOW, B_NOT_ZOOMABLE
																		| B_NOT_RESIZABLE),
		fTargetPath(targetpath),
		fPathList(20,true)
{
	AddCommonFilter(new EscapeCancelFilter());
	
	BRect bounds(Bounds());
	BView *top = GetBackgroundView();
	
	BButton *addButton = new BButton(BRect(0,0,1,1),"addbutton",TR("Set License"),
									new BMessage(M_LICENSE_CHOSEN),B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	addButton->ResizeToPreferred();
	addButton->MoveTo(bounds.right - 10 - addButton->Frame().Width(),
						bounds.bottom - 10 - addButton->Frame().Height());
	top->AddChild(addButton);
	addButton->MakeDefault(true);
	
	BStringView *licenseLabel = new BStringView(BRect(0,0,1,1),"licenselabel",TR("License:"));
	licenseLabel->ResizeToPreferred();
	licenseLabel->MoveTo(10.0,5.0);
	top->AddChild(licenseLabel);
	
	BRect r(bounds);
	r.InsetBy(10,10);
	r.top = licenseLabel->Frame().bottom + 1.0;
	r.right = r.left + 100 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = addButton->Frame().top - 10;
	fLicenseList = new BListView(r,"licenselist",B_SINGLE_SELECTION_LIST,
								B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT);
	BScrollView *scrollView = new BScrollView("scrollView",fLicenseList,
											B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM,
											0,false,true);
	top->AddChild(scrollView);
	fLicenseList->SetSelectionMessage(new BMessage(M_LICENSE_SELECTED));
	
	BStringView *shortLabel = new BStringView(BRect(0,0,1,1),"shortlabel",TR("Summary:"));
	shortLabel->ResizeToPreferred();
	shortLabel->MoveTo(scrollView->Frame().right + 10.0,5.0);
	top->AddChild(shortLabel);
	
	
	r.left = scrollView->Frame().right + 10.0;
	r.top = shortLabel->Frame().bottom + 1.0;
	r.right = bounds.right - 10.0 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = ((addButton->Frame().bottom - 10) / 2.0) - 5.0;
	
	BRect textrect = r;
	textrect.OffsetTo(5,5);
	textrect.bottom -= 5.0;
	textrect.right -= 5.0;
	
	fLicenseShort = new BTextView(r,"shortlicense",textrect, B_FOLLOW_ALL);
	BScrollView *shortScrollView = new BScrollView("scrollView",fLicenseShort,
											B_FOLLOW_ALL, 0 ,false,true);
	top->AddChild(shortScrollView);
	fLicenseShort->MakeEditable(false);
	
	BStringView *longLabel = new BStringView(BRect(0,0,1,1),"longlabel",TR("Full Version:"));
	longLabel->ResizeToPreferred();
	longLabel->MoveTo(scrollView->Frame().right + 10.0,shortScrollView->Frame().bottom + 5.0);
	top->AddChild(longLabel);
	
	r.top = longLabel->Frame().bottom + 1.0;
	r.bottom = addButton->Frame().top - 10;
	
	textrect = r;
	textrect.OffsetTo(5,5);
	textrect.bottom -= 5.0;
	textrect.right -= 5.0;
	
	fLicenseLong = new BTextView(r,"longlicense",textrect,B_FOLLOW_ALL | B_FRAME_EVENTS);
	BScrollView *longScrollView = new BScrollView("scrollView",fLicenseLong,
											B_FOLLOW_ALL, B_FRAME_EVENTS,false,true);
	top->AddChild(longScrollView);
	fLicenseLong->MakeEditable(false);
	
	r = Frame();
	BRect screen(BScreen().Frame());
	MoveTo( (screen.Width() - r.Width()) / 2.0, (screen.Height() - r.Height()) / 2.0);
	
	ScanLicenses();
}


void
LicenseManager::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
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
			DWindow::MessageReceived(msg);
	}
}


void
LicenseManager::FrameResized(float w, float h)
{
	
}


void
LicenseManager::ScanLicenses(void)
{
	DPath licensepath(gAppPath.GetFolder());
	licensepath << "Licenses";
	
	BDirectory dir(licensepath.GetFullPath());
	
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		BPath path(&ref);
		BString pathstr(path.Path());
		if (pathstr.FindLast(".summary") >= 0)
			continue;
		
		fPathList.AddItem(new BString(path.Path()));
		fLicenseList->AddItem(new BStringItem(ref.name));
	}
}


void
LicenseManager::LoadLicense(const char *mainpath)
{
	BString path(mainpath);
	BFile file(path.String(),B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;
	
	off_t size;
	file.GetSize(&size);
	
	BString filedata;
	char *filedataptr = filedata.LockBuffer(size + 1);
	file.Read(filedataptr,size);
	filedata.UnlockBuffer();
	fLicenseLong->SetText(filedata.String());
	
	path << ".summary";
	file.SetTo(path.String(),B_READ_ONLY);
	if (file.InitCheck() != B_OK)
	{
		fLicenseShort->SetText("A summary of this license is not available.");
		return;
	}
	
	filedata = "";
	file.GetSize(&size);
	filedataptr = filedata.LockBuffer(size + 1);
	file.Read(filedataptr,size);
	filedataptr[size] = '\0';
	filedata.UnlockBuffer();
	fLicenseShort->SetText(filedata.String());
}


void
LicenseManager::CopyLicense(const char *mainpath)
{
	BString path(mainpath);
	BFile file(path.String(),B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;
	
	off_t size;
	file.GetSize(&size);
	
	BString filedata;
	char *filedataptr = filedata.LockBuffer(size + 1);
	file.Read(filedataptr,size);
	filedata.UnlockBuffer();
	
	BPath target(fTargetPath.String());
	target.Append("LICENSE");
	
	file.SetTo(target.Path(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return;
	file.Write(filedata.String(),filedata.Length());
}
