#include "MainWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Button.h>
#include <Directory.h>
#include <Path.h>
#include <Roster.h>
#include <ScrollView.h>
#include <StringView.h>
#include <String.h>
#include <View.h>

#include "DListView.h"

enum
{
	M_RUN_ADDON = 'rnad',
	M_SELECT_ALL = 'slal'
};

extern "C" {

	void process_refs(entry_ref dir_ref, BMessage *msg, void *reserved);

}

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,350,400),"Tracker Addon Tester",B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS)
{
	AddShortcut('A', B_COMMAND_KEY, new BMessage(M_SELECT_ALL));
	
	BView *back = new BView(Bounds(), "back", B_FOLLOW_ALL, B_WILL_DRAW);
	back->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(back);
	
	BStringView *refLabel = new BStringView(BRect(10.0, 10.0, -1.0, -1.0),
											"reflabel", "Test Files:");
	refLabel->ResizeToPreferred();
	back->AddChild(refLabel);
	
	BRect r(Bounds().InsetByCopy(10.0, 10.0));
	r.top = refLabel->Frame().bottom + 5.0;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	fRefList = new RefListView(r, "reflist", B_MULTIPLE_SELECTION_LIST,
										B_FOLLOW_ALL);
	fRefList->SetDefaultDisplayMode(REFITEM_NAME);
	BScrollView *scrollView = fRefList->MakeScrollView("scrollview", false, true);
	back->AddChild(scrollView);
	
	BButton *run = new BButton(BRect(0.0, 0.0, 1.0, 1.0), "runbutton",
								"Run Addon", new BMessage(M_RUN_ADDON),
								B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	run->ResizeToPreferred();
	run->MoveTo((Bounds().Width() - run->Bounds().Width()) / 2.0,
				Bounds().bottom - 10.0 - run->Bounds().Height());
	back->AddChild(run);
	run->MakeDefault(true);
	
	scrollView->ResizeBy(0.0, -run->Bounds().Height() - 10.0);
	
	app_info info;
	be_app->GetAppInfo(&info);
	BPath path(&info.ref);
	path.GetParent(&path);
	path.Append("testfiles");
	
	fTestFolderRef = info.ref;
	
	PopulateRefList(path.Path());
	
	fRefList->Select(0, fRefList->CountItems() - 1);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_SELECT_ALL:
		{
			fRefList->Select(0, fRefList->CountItems() - 1);
			break;
		}
		case M_RUN_ADDON:
		{
			thread_id id = spawn_thread(AddonThread, "process_refs",
										B_NORMAL_PRIORITY, this);
			if (id >= 0)
				resume_thread(id);
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::PopulateRefList(const char *path)
{
	BDirectory dir(path);
	if (dir.InitCheck() != B_OK)
	{
		BString msg;
		msg << "Couldn't read the directory '" << path << "'.";
		BAlert *alert = new BAlert("Tracker Addon Tester", msg.String(), "OK");
		alert->Go();
		return;
	}
	
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		struct stat statData;
		BPath bpath(&ref);
		stat(bpath.Path(),&statData);
		
		// Is a directory?
		if (S_ISDIR(statData.st_mode))
			PopulateRefList(bpath.Path());
		else
			fRefList->AddItem(new RefListItem(ref, REFITEM_NAME));
	}
}


void
MainWindow::RunAddon(void)
{
	Lock();
	BMessage refmsg;
	for (int32 i = 0; i < fRefList->CountItems(); i++)
	{
		RefListItem *item = (RefListItem*)fRefList->ItemAt(i);
		if (item->IsSelected())
		{
			entry_ref ref = item->GetRef();
			refmsg.AddRef("refs", &ref);
		}
	}
	Unlock();
	
	process_refs(fTestFolderRef, &refmsg, NULL);
}


int32
MainWindow::AddonThread(void *data)
{
	MainWindow *win = (MainWindow*)data;
	win->RunAddon();
	
	return 0;
}


