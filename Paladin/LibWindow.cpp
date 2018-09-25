/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "LibWindow.h"

#include <Catalog.h>
#include <CheckBox.h>
#include <Directory.h>
#include <Entry.h>
#include <Locale.h>
#include <Path.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>

#include <LayoutBuilder.h>

#include "Globals.h"
#include "Project.h"
#include "Settings.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "LibWindow"

#define B_USER_DEVELOP_DIRECTORY ((directory_which)3028)


enum {
	M_TOGGLE_LIB = 'tglb'
};


LibraryWindow::LibraryWindow(BRect frame, const BMessenger& parent,
	Project* project)
	:
	DWindow(frame, NULL, B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS
		| B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE),
	fParent(parent),
	fProject(project)
{
	if (project != NULL) {
		BString title(B_TRANSLATE("Libraries: "));
		title << project->GetName();
		SetTitle(title.String());
	}

	BStringView* label = new BStringView("label",
		B_TRANSLATE("Choose the system libraries for your project:"));
	label->SetText(B_TRANSLATE("Scanning libraries" B_UTF8_ELLIPSIS));

	fCheckList = new BView("checklist", B_WILL_DRAW);

	BScrollView* scrollView = new BScrollView("scrollView", fCheckList,
		0, false, true);
	
	fScanThread = spawn_thread(ScanThread, "libscanthread", B_LOW_PRIORITY,
		this);
	resume_thread(fScanThread);
	
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(label)
		.Add(scrollView)
	.End();

	fCheckList->MakeFocus(true);
}


LibraryWindow::~LibraryWindow(void)
{
	gSettings.Lock();
	if (gSettings.ReplaceRect("libwin_frame",Frame()) != B_OK)
		gSettings.AddRect("libwin_frame",Frame());

	gSettings.Unlock();

	if (fScanThread > 0)
		kill_thread(fScanThread);
}


bool
LibraryWindow::QuitRequested(void)
{
	if (fProject != NULL)
		fProject->Save();

	fParent.SendMessage(M_LIBWIN_CLOSED);

	return true;
}


void
LibraryWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_TOGGLE_LIB:
		{
			if (fProject == NULL)
				break;

			entry_ref ref;
			BString pathString;
			if (message->FindRef("ref", &ref) != B_OK
				|| message->FindString("path", &pathString) != B_OK) {
				break;
			}

			// We're not using the ref's path to preserve symlinked folder
			// names in the /boot/system/develop hierarchy.
			// Both Haiku and Zeta do this.
			BPath path(pathString.String());
			path.Append(ref.name);
			if (fProject->HasLibrary(path.Path()))
				fProject->RemoveLibrary(path.Path());
			else
				fProject->AddLibrary(path.Path());

			break;
		}

		default:
			DWindow::MessageReceived(message);
			break;
	}
}


BView*
LibraryWindow::AddHeader(BPoint location, const char* label)
{
	Lock();

	BRect r(location.x, location.y, location.x + 1, location.y + 1);
	BStringView* header = new BStringView(r, "header", label);
	fCheckList->AddChild(header);
	header->SetFont(be_bold_font);
	header->SetHighColor(0, 0 ,255);
	header->ResizeToPreferred();

	Unlock();

	return header;
}


BRect
LibraryWindow::ScanFolder(BPoint location, const char* path, float* maxWidth)
{
	BDirectory directory(path);
	if (directory.InitCheck() != B_OK)
		return BRect(0, 0, -1, -1);

	float width = 0.0;
	directory.Rewind();
	entry_ref ref;
	BRect r(location.x, location.y, location.x + 1, location.y + 1);

	while (directory.GetNextRef(&ref) == B_OK) {
		BString refString(ref.name);

		int32 soPosition = refString.FindLast(".so");
		int32 aPosition = refString.FindLast(".a");

		if (soPosition < 0 && aPosition < 0)
			continue;

		if (soPosition >= 0 && soPosition < refString.CountChars() - 3)
			continue;

		if (aPosition >= 0 && aPosition < refString.CountChars() - 2)
			continue;

		Lock();
		BMessage* message = new BMessage(M_TOGGLE_LIB);
		message->AddRef("ref", &ref);
		message->AddString("path", path);
		BCheckBox* checkBox = new BCheckBox(r, ref.name, ref.name, message);
		checkBox->ResizeToPreferred();
		r = checkBox->Frame();
		fCheckList->AddChild(checkBox);
		fSystemLibs.AddItem(checkBox);

		// make sure that there isn't any shortcut normalization
		BPath directoryPath(path);
		directoryPath.Append(ref.name);

		if (fProject != NULL && fProject->HasLibrary(directoryPath.Path()))
			checkBox->SetValue(B_CONTROL_ON);
		else
			checkBox->SetValue(B_CONTROL_OFF);

		Unlock();

		width = MAX(r.right, width);
		r.OffsetBy(0, r.Height());
	}

	if (r.Height() == 1)
		r.Set(0, 0, -1, -1);

	*maxWidth = width;

	return r;
}


int32
LibraryWindow::ScanThread(void* data)
{
	// ToDo: convert to using find_paths() once we drop R1/A4 support

	LibraryWindow* window = (LibraryWindow*)data;

	float maxwidth;
	BRect r(5, 5, 105, 20);

	BView* systemHeader = window->AddHeader(r.LeftTop(), B_TRANSLATE("System libraries:"));

	window->Lock();
	r = systemHeader->Frame();
	window->Unlock();
	maxwidth = r.right;

	r.OffsetBy(0, r.Height() + 10);

	DPath sysPath = GetSystemPath(B_SYSTEM_LIB_DIRECTORY);
	BRect out = window->ScanFolder(r.LeftTop(), sysPath.GetFullPath(), &maxwidth);
	if (out != BRect(0, 0, -1, -1)) {
		r = out;
		r.OffsetBy(0, 10);
	}

	if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4) {
		// ToDo: remove this once we drop R1/A4 support
		BView* commonHeader = window->AddHeader(r.LeftTop(),
			B_TRANSLATE("Common libraries:"));
		window->Lock();
		r = commonHeader->Frame();
		window->Unlock();
		maxwidth = MAX(r.right, maxwidth);

		r.OffsetBy(0.0f, r.Height() + 10.0f);

		out = window->ScanFolder(r.LeftTop(),
			GetSystemPath(B_USER_LIB_DIRECTORY).GetFullPath(), &maxwidth);
		if (out != BRect(0, 0, -1, -1)) {
			r = out;
			r.OffsetBy(0.0f, 10.0f);
		}
	}

	BView* userHeader = window->AddHeader(r.LeftTop(), B_TRANSLATE("User libraries:"));
	window->Lock();
	r = userHeader->Frame();
	window->Unlock();
	maxwidth = MAX(r.right, maxwidth);

	r.OffsetBy(0, r.Height() + 10);

	DPath userPath = GetSystemPath(B_USER_LIB_DIRECTORY);
	out = window->ScanFolder(r.LeftTop(), userPath.GetFullPath(), &maxwidth);
	if (out.IsValid()) {
		r = out;
		r.OffsetBy(0,10);
	}

	window->Lock();
	BView* top = window->GetBackgroundView();
	BScrollView* scrollView = (BScrollView*)top->FindView("scrollView");

	BScrollBar* vbar = scrollView->ScrollBar(B_VERTICAL);
	vbar->SetRange(0, r.bottom - scrollView->Bounds().Height());
	vbar->SetSteps(r.Height() * 2.0,r.Height() * 8.0);
	gSettings.Lock();
	BRect savedframe;
	if (gSettings.FindRect("libwin_frame", &savedframe) == B_OK)
		window->ResizeTo(savedframe.Width(), savedframe.Height());

	gSettings.Unlock();

	BStringView* label = (BStringView*)top->FindView("label");
	label->SetText(B_TRANSLATE("Choose the system libraries for your project:"));
	float minw = label->Frame().right + 10;
	window->SetSizeLimits(minw, 30000, 200, 30000);
	if (window->Bounds().Width() < minw)
		window->ResizeTo(minw, window->Bounds().Height());

	window->fScanThread = -1;
	window->Unlock();

	return 0;
}
