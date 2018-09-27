/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "MainWindow.h"

#include <stdio.h>
#include <stdlib.h>

#include <Alignment.h>
#include <Application.h>
#include <Button.h>
#include <Entry.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <LayoutItem.h>
#include <ListView.h>
#include <Path.h>
#include <ScrollView.h>
#include <StringView.h>
#include <TextControl.h>

#include "DPath.h"


enum
{
	M_SEARCH = 'sear'
};


MainWindow::MainWindow(void)
	:
	BWindow(BRect(0.0f, 0.0f, 640.0f, 480.0f), "Symbol locator",
		B_TITLED_WINDOW, 0),
	fTerminateSem(-1),
	fThreadID(-1)
{
	fTerminateSem = create_sem(1, "abort_sem");

	fTextBox = new BTextControl("textbox", "Symbol to find:", "",
		new BMessage(M_SEARCH));
	BLayoutItem* labelItem = fTextBox->CreateLabelLayoutItem();
	labelItem->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_VERTICAL_CENTER));
	BLayoutItem* textItem = fTextBox->CreateTextViewLayoutItem();
	textItem->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_VERTICAL_CENTER));

	fGoButton = new BButton("goButton", "Search", new BMessage(M_SEARCH));

	fStatusView = new BStringView("statusView", "");

	fResultList = new BListView("resultList");
	BScrollView* listScroller = new BScrollView("listScroller",
		fResultList, B_WILL_DRAW, true, true);

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.Add(labelItem)
			.Add(textItem)
			.Add(fGoButton)
			.End()
		.Add(fStatusView)
		.Add(listScroller)
		.SetInsets(B_USE_WINDOW_INSETS)
		.End();

	fTextBox->MakeFocus();
	fGoButton->MakeDefault(true);
	fStatusView->Hide();

	CenterOnScreen();
}


MainWindow::~MainWindow(void)
{
	delete_sem(fTerminateSem);
}


void
MainWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_SEARCH:
		{
			fThreadID = spawn_thread(SearchThread, "searchthread",
				B_NORMAL_PRIORITY, this);
			if (fThreadID > 0)
				resume_thread(fThreadID);
			break;
		}

		default:
			BWindow::MessageReceived(message);
			break;
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
MainWindow::DoSearch(const char* text)
{
	if (text == NULL)
		return;

	Lock();

	fStatusView->Show();
	fGoButton->SetEnabled(false);
	for (int32 i = fResultList->CountItems() - 1; i >= 0; i--) {
		BStringItem* item = (BStringItem*)fResultList->RemoveItem(i);
		delete item;
	}

	Unlock();

	status_t result = B_OK;
	BPath path;
	if (find_directory(B_BEOS_LIB_DIRECTORY, &path) == B_OK)
		result = SearchLibPath(path.Path(),text);

	if (result == B_OK && find_directory(B_USER_LIB_DIRECTORY, &path) == B_OK)
		result = SearchLibPath(path.Path(), text);

	if (result == B_OK && BEntry("/boot/system").Exists()
		&& find_directory(B_SYSTEM_LIB_DIRECTORY, &path) == B_OK) {
		result = SearchLibPath(path.Path(),text);
	}

	Lock();

	fThreadID = -1;
	fStatusView->Hide();
	fGoButton->SetEnabled(true);

	Unlock();
}


int32
MainWindow::SearchThread(void* data)
{
	MainWindow* window = static_cast<MainWindow*>(data);
	if (window->fTextBox->Text() != NULL && *window->fTextBox->Text() != '\0')
		window->DoSearch(window->fTextBox->Text());

	return 0;
}


status_t
MainWindow::SearchLibPath(const char* path, const char* text)
{
	BDirectory dir(path);
	if (dir.InitCheck() != B_OK)
		return B_BAD_VALUE;

	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK) {
		sem_info info;
		get_sem_info(fTerminateSem,&info);
		if (info.count <= 0) {
			release_sem(fTerminateSem);
			return B_ERROR;
		}

		DPath file(ref);
		if (!file.GetExtension() || !((strcmp(file.GetExtension(),"so") == 0)
				|| (strcmp(file.GetExtension(),"o") == 0)
				|| (strcmp(file.GetExtension(),"a") == 0))) {
			continue;
		}
		
		BString label = "Searching ";
		label << file.GetFileName();

		Lock();

		fStatusView->SetText(label.String());
		UpdateIfNeeded();

		Unlock();

		BString command("nm -CA --defined-only --format=posix '");
		command << file.GetFullPath() << "' | grep '" << text << "'";

		FILE* fd = popen(command.String(),"r");
		if (fd != NULL) {
			BString out;
			char buffer[32];
			while (fgets(buffer, 32, fd)) {
				if (!ferror(fd)) {
					out += buffer;
				}
			}

			int status = pclose(fd);
			if (0 != status) {
				//STRACE(2,("pclose returned non zero (error) code: %i",status));
			}

			if (out.CountChars() > 0) {
				char* line = strtok(out.LockBuffer(out.Length()), "\n");
				while (line != NULL) {
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
