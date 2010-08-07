#include "App.h"

#include <Locker.h>

#include "CommandLine.h"
#include "Globals.h"
#include "PkgWindow.h"

static int32 sWindowCount = 0;
static BLocker sWindowLocker;

void
RegisterWindow(void)
{
	sWindowLocker.Lock();
	sWindowCount++;
	sWindowLocker.Unlock();
}


void
DeregisterWindow(void)
{
	bool quit = false;
	sWindowLocker.Lock();
	sWindowCount--;
	
	if (sWindowCount == 0)
		quit = true;
	
	sWindowLocker.Unlock();
	if (quit)
		be_app->PostMessage(B_QUIT_REQUESTED);
}


int32
CountRegisteredWindows(void)
{
	int32 count;
	sWindowLocker.Lock();
	count = sWindowCount;
	sWindowLocker.Unlock();
	return count;
}


App::App(void)
	:	BApplication("application/x-vnd.dw-PSfx")
{
	if (gCommandLineMode)
	{
		DoCommandLine();
		PostMessage(B_QUIT_REQUESTED);
	}
}


void
App::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		default:
		{
			BApplication::MessageReceived(msg);
		}
	}
}


void
App::RefsReceived(BMessage *msg)
{
	int32 i = 0;
	entry_ref ref;
	while (msg->FindRef("refs",i++,&ref) == B_OK)
	{
		PkgWindow *win = new PkgWindow(&ref);
		win->Show();
	}
}


void
App::ReadyToRun(void)
{
	if (CountRegisteredWindows() < 1)
	{
		PkgWindow *win = new PkgWindow(NULL);
		win->Show();
	}
}


int
main(int argc, char **argv)
{
	if (!ProcessArgs(argc,argv))
		return -1;
	
	App app;
	app.Run();
	
	return gReturnValue;
}


