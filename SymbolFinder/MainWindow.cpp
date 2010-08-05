#include "MainWindow.h"

#include <Application.h>
#include <ScrollView.h>
#include <View.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Entry.h>
#include <Path.h>
#include <stdlib.h>
#include <stdio.h>
#include "DPath.h"

enum
{
	M_SEARCH = 'sear'
};

MainWindow::MainWindow(void)
	:	DWindow(BRect(100,100,500,400),"Symbol Locator"),
		fTerminateSem(-1),
		fThreadID(-1)
{
	fTerminateSem = create_sem(1,"abort_sem");
	
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	fGoButton = new BButton(BRect(0,0,1,1),"gobutton","Search",new BMessage(M_SEARCH),
							B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fGoButton->ResizeToPreferred();
	fGoButton->MoveTo(Bounds().right - fGoButton->Bounds().Width() - 10.0, 10.0);
	
	BRect r(10.0,10.0,fGoButton->Frame().left - 10.0, 35);
	fTextBox = new BTextControl(r,"textbox","Symbol to Find:",NULL,new BMessage(M_SEARCH),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fTextBox);
	fTextBox->SetDivider(fTextBox->StringWidth("Symbol to Find:"));
	float w,h;
	fTextBox->GetPreferredSize(&w,&h);
	fTextBox->ResizeTo(fTextBox->Bounds().Width(),h);
	
	top->AddChild(fGoButton);
	fGoButton->MakeDefault(true);
	
	fStatusView = new BStringView(BRect(0,0,1,1),"statusview","SomeTextHere",
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	fStatusView->GetPreferredSize(&w,&h);
	fStatusView->ResizeTo(Bounds().Width(),h);
	fStatusView->MoveTo(10.0,Bounds().Height() - h);
	fStatusView->SetText("");
	top->AddChild(fStatusView);
	
	r = fTextBox->Frame();
	r.OffsetBy(0,r.Height() + 10.0);
	r.bottom = fStatusView->Frame().top - B_H_SCROLL_BAR_HEIGHT - 1.0;
	r.right = Bounds().right - B_V_SCROLL_BAR_WIDTH - 10.0;
	
	fResultList = new BListView(r,"resultlist",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL);
	BScrollView *scroll = new BScrollView("scroller",fResultList,B_FOLLOW_ALL,0,true,true);
	top->AddChild(scroll);

	SetSizeLimits(fGoButton->Frame().Width() + fTextBox->StringWidth("Symbol to Find:") +
					30.0 + 100.0,30000,200,30000);
	
	fTextBox->MakeFocus();
}


MainWindow::~MainWindow(void)
{
	delete_sem(fTerminateSem);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_SEARCH:
		{
//			if (fTextBox->Text() && strlen(fTextBox->Text()) > 0)
//				DoSearch(fTextBox->Text());
			fThreadID = spawn_thread(SearchThread,"searchthread",B_NORMAL_PRIORITY,this);
			if (fThreadID > 0)
				resume_thread(fThreadID);
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
	acquire_sem(fTerminateSem);
	
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::DoSearch(const char *text)
{
	if (!text)
		return;
	
	Lock();
	fGoButton->SetEnabled(false);
	
	for (int32 i = fResultList->CountItems() - 1; i >= 0; i--)
	{
		BStringItem *item = (BStringItem*)fResultList->RemoveItem(i);
		delete item;
	}
	Unlock();
	
	status_t status = B_OK;
	BPath path;
	if (find_directory(B_BEOS_LIB_DIRECTORY,&path) == B_OK)
		status = SearchLibPath(path.Path(),text);
	
	if (status == B_OK && find_directory(B_USER_LIB_DIRECTORY,&path) == B_OK)
		status = SearchLibPath(path.Path(),text);
	
	if (status == B_OK && BEntry("/boot/common").Exists() && find_directory(B_COMMON_LIB_DIRECTORY,&path) == B_OK)
		status = SearchLibPath(path.Path(),text);
	
	Lock();
	fThreadID = -1;
	fGoButton->SetEnabled(true);
	Unlock();
}


int32
MainWindow::SearchThread(void *data)
{
	MainWindow *win = static_cast<MainWindow*>(data);
	if (win->fTextBox->Text() && strlen(win->fTextBox->Text()) > 0)
		win->DoSearch(win->fTextBox->Text());
	return 0;
}


status_t
MainWindow::SearchLibPath(const char *path, const char *text)
{
	BDirectory dir(path);
	if (dir.InitCheck() != B_OK)
		return B_BAD_VALUE;
	
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		sem_info info;
		get_sem_info(fTerminateSem,&info);
		if (info.count <= 0)
		{
			release_sem(fTerminateSem);
			return B_ERROR;
		}
		
		DPath file(ref);
		if (!file.GetExtension() ||
			!(	(strcmp(file.GetExtension(),"so") == 0) ||
				(strcmp(file.GetExtension(),"o") == 0) ||
				(strcmp(file.GetExtension(),"a") == 0)) )
			continue;
		
		BString label = "Searching ";
		label << file.GetFileName();
		Lock();
		fStatusView->SetText(label.String());
		UpdateIfNeeded();
		Unlock();
		
		BString command("nm -CA --defined-only --format=posix '");
		command << file.GetFullPath() << "' | grep '" << text << "'";
		
		FILE *fd = popen(command.String(),"r");
		if (fd)
		{
			BString out;
			char buffer[32];
			while (fgets(buffer,32,fd))
				out += buffer;
			pclose(fd);
			
			if (out.CountChars() > 0)
			{
				char *line = strtok(out.LockBuffer(out.Length()),"\n");
				while (line)
				{
					Lock();
					fResultList->AddItem(new BStringItem(line));
					UpdateIfNeeded();
					Unlock();
					line = strtok(NULL,"\n");
				}			
			}
		}
	}
	
	Lock();
	fStatusView->SetText("");
	UpdateIfNeeded();
	Unlock();
	
	return B_OK;
}
