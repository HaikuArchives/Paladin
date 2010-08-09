#include "AboutWindow.h"

#include <Application.h>
#include <AppFileInfo.h>
#include <Roster.h>
#include <String.h>
#include <stdio.h>
#include <Screen.h>
#include <TranslationUtils.h>

AboutWindow::AboutWindow(void)
 : BWindow(BRect(100,100,500,400),"Capital Be", B_MODAL_WINDOW_LOOK,
 	B_MODAL_APP_WINDOW_FEEL,
 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	BScreen screen;
	BRect screenrect(screen.Frame());
	
	AboutView *aboutview = new AboutView(Bounds());
	AddChild(aboutview);
	aboutview->MakeFocus(true);
	
	MoveTo( (screenrect.Width()-Frame().Width())/2, (screenrect.Height()-Frame().Height())/2 );
}

AboutView::AboutView(BRect frame)
 : BView (frame, "AboutView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(126,126,190);
	
	fLogo = BTranslationUtils::GetBitmap('JPEG',"PSfxAbout.jpg");
	
	app_info ai;
	version_info vi;
	be_app->GetAppInfo(&ai);
	BFile file(&ai.ref,B_READ_ONLY);
	BAppFileInfo appinfo(&file);
	appinfo.GetVersionInfo(&vi,B_APP_VERSION_KIND);

	BString variety;
	switch(vi.variety)
	{
		case 0:
			variety="d";
			break;
		case 1:
			variety="a";
			break;
		case 2:
			variety="b";
			break;
		case 3:
			variety="g";
			break;
		case 4:
			variety="rc";
			break;
		default:
			variety="Final";
			break;
	}
	
	if(variety != "Final")
		sprintf(version,"%s %lu.%lu %s%lu","Version",vi.major,vi.middle,
				variety.String(),vi.internal);
	else
		sprintf(version,"%s %lu.%lu","Version",vi.major,vi.middle);
	
	font_height height;
	be_plain_font->GetHeight(&height);
	
	versionpos.y = fLogo->Bounds().bottom - 5;
	versionpos.x = fLogo->Bounds().left + 5;
	
	SetDrawingMode(B_OP_OVER);
	SetFont(be_bold_font);
}

AboutView::~AboutView(void)
{
	delete fLogo;
}

void AboutView::MouseDown(BPoint pt)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

void AboutView::AttachedToWindow(void)
{
	Window()->ResizeTo(fLogo->Bounds().Width(),fLogo->Bounds().Height());
}

void AboutView::Draw(BRect update)
{
	DrawBitmap(fLogo, BPoint(0,0));
	DrawString(version,versionpos);
}

void AboutView::KeyUp(const char *bytes, int32 numbytes)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

