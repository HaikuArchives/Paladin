#include <Application.h>
#include <FilePanel.h>


#include <stdio.h>

class App : public BApplication
{
public:

	App(void)
		:	BApplication("application/x-vnd.OpenFileDemo")
	{
		/* Empty */
	}

	void MessageReceived(BMessage *msg)
	{
		switch (msg->what)
		{
			case B_REFS_RECEIVED:
				/* User selected "Open" */
				/* CONTINUE DOWN... */
			case B_SAVE_REQUESTED:
				/* User selected "Save" */
				/* CONTINUE DOWN... */
			case B_CANCEL:
				/* User selected "Cancel" */
				/* CONTINUE DOWN... */
			default:
				/* No matter what message what received, just quit the demo. */
				be_app->Quit();
				break;
		}
	}
};



int
main(void)
{
	App *app = new App();

	BFilePanel panel(B_OPEN_PANEL);
	panel.Show();

	app->Run();

	delete app;
	return 0;
}
