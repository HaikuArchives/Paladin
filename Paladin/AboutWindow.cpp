/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "AboutWindow.h"

#include <Application.h>
#include <AppFileInfo.h>
#include <Roster.h>
#include <String.h>
#include <stdio.h>
#include <Screen.h>
#include <TranslationUtils.h>


class AboutView : public BView {
public:
								AboutView(BRect frame);
	virtual						~AboutView(void);

	virtual	void				AttachedToWindow(void);
	virtual	void				Draw(BRect update);
	virtual	void				MouseDown(BPoint where);
	virtual	void				KeyUp(const char* bytes, int32 numbytes);

private:
			BBitmap*			fLogo;
			char				fVersion[256];
			BPoint				fVersionPosition;

			uint8				fAboutFlags;
			int32				fEntryCount;
};


//	#pragma mark - AboutView


AboutView::AboutView(BRect frame)
	:
	BView(frame, "AboutView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(126, 126, 190);

	fLogo = BTranslationUtils::GetBitmap('JPG ',"AboutPaladin.jpg");

	app_info ai;
	version_info vi;
	be_app->GetAppInfo(&ai);
	BFile file(&ai.ref, B_READ_ONLY);
	BAppFileInfo appinfo(&file);
	appinfo.GetVersionInfo(&vi, B_APP_VERSION_KIND);

	BString variety;
	switch(vi.variety) {
		case 0:
			variety = "d";
			break;

		case 1:
			variety = "a";
			break;

		case 2:
			variety = "b";
			break;

		case 3:
			variety = "g";
			break;

		case 4:
			variety = "rc";
			break;

		default:
			variety = "Final";
	}

	if (variety == "Final")
		sprintf(fVersion,"%s %" B_PRIu32 ".%" B_PRIu32,"Version", vi.major, vi.middle);
	else {
		sprintf(fVersion, "%s %" B_PRIu32 ".%" B_PRIu32 " %s%" B_PRIu32, 
		        "Version", vi.major, vi.middle, variety.String(), vi.internal);
	}

	font_height height;
	be_plain_font->GetHeight(&height);

	fVersionPosition.y = fLogo->Bounds().bottom - 5;
	fVersionPosition.x = fLogo->Bounds().right - StringWidth(fVersion) - 10;

	SetDrawingMode(B_OP_OVER);
	SetFont(be_bold_font);
}


AboutView::~AboutView(void)
{
	delete fLogo;
}


void
AboutView::MouseDown(BPoint pt)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}


void
AboutView::AttachedToWindow(void)
{
	Window()->ResizeTo(fLogo->Bounds().Width(), fLogo->Bounds().Height());
}


void
AboutView::Draw(BRect update)
{
	DrawBitmap(fLogo, BPoint(0, 0));
	DrawString(fVersion, fVersionPosition);
}


void
AboutView::KeyUp(const char* bytes, int32 numbytes)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}


//	#pragma mark - AboutWindow


AboutWindow::AboutWindow(void)
	:
	BWindow(BRect(100, 100, 500, 400), "", B_MODAL_WINDOW_LOOK,
 		B_MODAL_APP_WINDOW_FEEL,
 		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	BScreen screen;
	BRect screenRect(screen.Frame());

	AboutView* aboutView = new AboutView(Bounds());
	AddChild(aboutView);
	aboutView->MakeFocus(true);

	MoveTo((screenRect.Width() - Frame().Width()) / 2,
		(screenRect.Height() - Frame().Height()) / 2);
}
