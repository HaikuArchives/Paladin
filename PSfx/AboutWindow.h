#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Button.h>
#include <Messenger.h>
#include <Font.h>
#include <StatusBar.h>

enum
{
	ABOUT_STARTUP=0,
	ABOUT_OK
};

enum
{
	M_RESET_STATUS='mrst',
	M_SET_STATUS='msst',
	M_UPDATE_STATUS='mups',
	M_SHOW_ABOUT='msha'
};

class AboutView : public BView
{
public:
	AboutView(BRect frame);
	~AboutView(void);
	void AttachedToWindow(void);
	void Draw(BRect update);
	void MouseDown(BPoint pt);
	void KeyUp(const char *bytes, int32 numbytes);
	
	BBitmap *fLogo;
	
	char version[256];
	BPoint versionpos;
	
	uint8 fAboutFlags;
	int32 fEntryCount;
};

class AboutWindow : public BWindow
{
public:
	AboutWindow(void);
};

#endif
