#include "AboutWindow.h"

#include <Bitmap.h>
#include <Screen.h>
#include <String.h>
#include <View.h>
#include <Window.h>
#include <TranslationUtils.h>
#include <TranslatorFormats.h>

#include "DelayedMessenger.h"

class AboutView : public BView
{
public:
			AboutView(BBitmap *bitmap);
			~AboutView(void);
	void	Draw(BRect update);
	void	MouseDown(BPoint pt);
	void	KeyUp(const char *bytes, int32 numbytes);

private:	
			BBitmap *fLogo;
};

class AboutWindow : public BWindow
{
public:
			AboutWindow(BBitmap *bitmap);

private:
			DelayedMessenger	fMessenger;
};


void
ShowSplash(void)
{
	BBitmap *logo = BTranslationUtils::GetBitmap(B_JPEG_FORMAT,"Splash.jpg");
	if (!logo)
		logo = BTranslationUtils::GetBitmap(B_PNG_FORMAT,"Splash.png");
	
	if (logo)
	{
		AboutWindow *win = new AboutWindow(logo);
		win->Show();
	}
}


AboutWindow::AboutWindow(BBitmap *bitmap)
	:	BWindow(BRect(100,100,500,400),"", B_MODAL_WINDOW_LOOK,
 		B_MODAL_APP_WINDOW_FEEL,
	 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK),
	 	fMessenger(this)
{
	BRect screenrect(BScreen().Frame());
	
	AboutView *av = new AboutView(bitmap);
	
	ResizeTo(av->Frame().Width(),av->Frame().Height());
	
	AddChild(av);
	av->MakeFocus(true);
	
	MoveTo((screenrect.Width() - Frame().Width()) / 2.0,
			(screenrect.Height() - Frame().Height()) / 2.0);
	fMessenger.SendMessage(BMessage(B_QUIT_REQUESTED),1);
}


AboutView::AboutView(BBitmap *bitmap)
	:	BView (BRect(0,0,1,1), "AboutView", B_FOLLOW_ALL, B_WILL_DRAW),
		fLogo(NULL)
{
	if (!bitmap || !bitmap->IsValid())
		ResizeTo(400,300);
	else
	{
		fLogo = bitmap;
		BRect r(fLogo->Bounds());
		ResizeTo(r.Width(),r.Height());
	}
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
AboutView::Draw(BRect update)
{
	if (fLogo)
		DrawBitmap(fLogo, BPoint(0,0));
	else
	{
		BPoint pt;
		BString msg("Splash picture unavailable");
		pt.y = Bounds().Height() / 2.0;
		pt.x = (Bounds().Width() - StringWidth(msg.String())) / 2.0;
		DrawString(msg.String(),pt);
	}
}


void
AboutView::KeyUp(const char *bytes, int32 numbytes)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

