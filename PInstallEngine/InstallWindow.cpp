#include "InstallWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Mime.h>
#include <NodeInfo.h>
#include <Path.h>
#include <ScrollView.h>
#include <stdlib.h>
#include <VolumeRoster.h>
#include <Volume.h>

/*
#include "DepItem.h"
#include "DPath.h"
#include "Globals.h"
#include "InstallEngine.h"
#include "PackageInfo.h"
#include "ResData.h"

extern PackageInfo		gPkgInfo;
*/
#include "Globals.h"

InstallWindow::InstallWindow(void)
	:	DWindow(BRect(100,100,500,400),"Installing Package",B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE | B_NOT_CLOSABLE)
//				B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE | B_NOT_CLOSABLE),
//		fInstallThread(-1),
//		fQuitFlag(0)
{
	gInstallEngine.SetTarget(BMessenger(NULL,this));
	
	MakeCenteredOnShow(true);
	
	BView *top = GetBackgroundView();
	
	fQuit = new BButton(BRect(0,0,1,1),"quit","Quit",new BMessage(B_QUIT_REQUESTED),
						B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	fQuit->ResizeToPreferred();
	fQuit->MoveTo(Bounds().right - fQuit->Frame().Width() - 10.0,
				Bounds().bottom - fQuit->Frame().Height() - 10.0);
	fQuit->SetEnabled(false);
	fQuit->MakeDefault(true);
	
	BRect r(Bounds());
	r.InsetBy(10,10);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom = fQuit->Frame().top - 10.0;
	BRect textRect(r.OffsetToCopy(0,0));
	textRect.InsetBy(5,5);
	
	fText = new BTextView(r,"logview",textRect,B_FOLLOW_ALL);
	fText->MakeEditable(false);
	fText->MakeFocus(true);
	
	BScrollView *sv = new BScrollView("scroller",fText,B_FOLLOW_ALL,0,false,true);
	top->AddChild(sv);
	
	top->AddChild(fQuit);
	
//	fInstallThread = spawn_thread(InstallThread,"installthread",B_NORMAL_PRIORITY,this);
//	if (fInstallThread >= 0)
//		resume_thread(fInstallThread);
	gInstallEngine.StartInstall();
}


InstallWindow::~InstallWindow(void)
{
	gInstallEngine.StopInstall();
/*
	if (fInstallThread >= 0)
	{
		atomic_add(&fQuitFlag, 1);
		while (fQuitFlag)
			snooze(10000);
	}
*/
}


bool
InstallWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
InstallWindow::FrameResized(float w, float h)
{
	BRect r(fText->Bounds());
	r.InsetBy(5,5);
	fText->SetTextRect(r);
}


void
InstallWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_LOG_MESSAGE:
		{
			BString str;
			if (msg->FindString("message",&str) == B_OK)
			{
				Lock();
				fText->Insert(str.String());
				Unlock();
			}
			break;
		}
		case M_INSTALL_ABORT:
		case M_INSTALL_FAILURE:
		case M_INSTALL_SUCCESS:
		{
			fQuit->SetEnabled(true);
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}


/*
int32
InstallWindow::InstallThread(void *data)
{
	((InstallWindow*)data)->DoInstall();
	return 0;
}
*/

