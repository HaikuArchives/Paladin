#include "AboutWindow.h"

#include <Application.h>
#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Font.h>
#include <Roster.h>
#include <String.h>
#include <stdio.h>
#include <TranslationUtils.h>
#include <View.h>

class AboutView : public BView
{
public:
			AboutView(BRect frame, const char *imageName,
					const int32 &imageType);
			~AboutView(void);
	void	AttachedToWindow(void);
	void	Draw(BRect update);
	void	MouseDown(BPoint pt);
	void	KeyUp(const char *bytes, int32 numbytes);
	
	BBitmap	*fLogo;
	
	char	version[256];
	BPoint	versionpos;
	
	int32	fEntryCount;
};


AboutWindow::AboutWindow(const char *imageName, const int32 &imageType)
	:	DWindow(BRect(100,100,500,400), "", B_MODAL_WINDOW,
			 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	MakeCenteredOnShow(true);
	
	AboutView *aboutview = new AboutView(Bounds(), imageName, imageType);
	AddChild(aboutview);
	
	aboutview->MakeFocus(true);
}


AboutView::AboutView(BRect frame, const char *imageName, const int32 &imageType)
 : BView (frame, "AboutView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(126,126,190);
	
	fLogo = BTranslationUtils::GetBitmap(imageType, imageName);
	
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
	
	BRect r(fLogo ? fLogo->Bounds() : Bounds());
	versionpos.y = r.bottom - 5;
	versionpos.x = r.right - StringWidth(version) - 7;
	
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
	if (fLogo)
		Window()->ResizeTo(fLogo->Bounds().Width(),fLogo->Bounds().Height());
}


void
AboutView::Draw(BRect update)
{
	if (fLogo)
		DrawBitmap(fLogo, BPoint(0,0));
	DrawString(version,versionpos);
}


void
AboutView::KeyUp(const char *bytes, int32 numbytes)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

