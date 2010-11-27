#include "App.h"
#include "FloaterBroker.h"
#include "MainWindow.h"
#include "MsgDefs.h"
#include "PObjectBroker.h"

App::App(void)
	:	BApplication("application/x-vnd.dw-PDesigner")
{
	InitObjectSystem();
	InitFloaterSystem();
	
	fMainWin = new MainWindow();
	fMainWin->Show();
}


bool
App::QuitRequested(void)
{
	ShutdownFloaterSystem();
	ShutdownObjectSystem();
	return true;
}


void
App::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_ACTIVATE_OBJECT:
		case M_UPDATE_PROPERTY_EDITOR:
		{
			fMainWin->PostMessage(msg);
			break;
		}
		default:
			BApplication::MessageReceived(msg);
	}
}


int
main(void)
{
	App *app = new App();
	app->Run();
	delete app;
	return 0;
}
