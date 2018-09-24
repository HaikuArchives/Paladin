/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "StartWindow.h"

#include <Alert.h>
#include <Entry.h>
#include <FilePanel.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <TranslationUtils.h>

#include "BitmapButton.h"
#include "ClickableStringView.h"
#include "EscapeCancelFilter.h"
#include "Globals.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"
#include "SCMImportWindow.h"
#include "Settings.h"
#include "TemplateWindow.h"
#include "TypedRefFilter.h"
#include "PaladinFileFilter.h"


enum
{
	M_OPEN_SELECTION = 'opsl',
	M_REMOVE_FROM_LIST = 'rmls',
	M_SHOW_IMPORT = 'shim',
	M_ONLINE_IMPORT = 'olim',
	M_OPEN_FROM_LIST = 'ofls'
};


class RefStringItem : public BStringItem {
public:
	RefStringItem(entry_ref fileref);
	
	entry_ref ref;
};


RefStringItem::RefStringItem(entry_ref fileref)
	:
	BStringItem(fileref.name)
{
	ref = fileref;
	BString name(ref.name);
	name.RemoveLast(".pld");
	SetText(name.String());
}


class RecentProjectsList : public BListView {
public:
								RecentProjectsList();
	virtual	void				MouseDown(BPoint where);
};


static BitmapButton*
make_button(const char* name, const char* up, const char* down,
	int32 command)
{
	BBitmap* upBitmap = BTranslationUtils::GetBitmap('PNG ', up);
	BBitmap* downBitmap = BTranslationUtils::GetBitmap('PNG ', down);
	BitmapButton* button = new BitmapButton(name, upBitmap, downBitmap,
		new BMessage(command));
	button->SetExplicitSize(BSize(31, 31));

	return button;
}


static ClickableStringView*
make_label(BitmapButton* button, const char* label)
{
	if (button == NULL)
		return NULL;
	
	BString labelName = button->Name();
	labelName << " label";

	return new ClickableStringView(labelName.String(), label);
}


//	#pragma mark - RecentProjectsList


RecentProjectsList::RecentProjectsList()
	:
	BListView("listview")
{
}


void
RecentProjectsList::MouseDown(BPoint where)
{
	uint32 buttons;
	BPoint point;
	GetMouse(&point, &buttons);

	if ((buttons & B_SECONDARY_MOUSE_BUTTON) != 0) {
		BMessage* message = Window()->CurrentMessage();
		int32 clicks;
		message->FindInt32("clicks", &clicks);
		if (clicks > 1) {
			clicks = 1;
			message->ReplaceInt32("clicks", clicks);
		}
	}
	BListView::MouseDown(where);

	if ((buttons & B_SECONDARY_MOUSE_BUTTON) != 0) {
		BPoint screenPoint(where);
		ConvertToScreen(&screenPoint);
		screenPoint.x -= 5;
		screenPoint.y -= 5;

		BStringItem* stringItem = (BStringItem*)ItemAt(IndexOf(where));
		if (stringItem == NULL)
			return;

		BPopUpMenu menu("context");

		menu.AddItem(new BMenuItem(TR("Remove from recent list"),
			new BMessage(M_REMOVE_FROM_LIST)));
		menu.SetTargetForItems(Window());
		menu.Go(screenPoint, true, false);
	}
}


//	#pragma mark - StartWindow


StartWindow::StartWindow(void)
	:
	BWindow(BRect(0, 0, -1, -1), "Paladin", B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	RegisterWindow();

	AddShortcut('O', B_COMMAND_KEY, new BMessage(M_SHOW_OPEN_PROJECT));
	AddShortcut('N', B_COMMAND_KEY, new BMessage(M_NEW_PROJECT));
	AddShortcut('I', B_COMMAND_KEY, new BMessage(M_SHOW_IMPORT));

	AddCommonFilter(new EscapeCancelFilter());

	// new button and label

	fNewButton = make_button("new", "NewProjectButtonUp.png",
		"NewProjectButtonDown.png", M_NEW_PROJECT);

	ClickableStringView* newLabel = make_label(fNewButton, TR("Create a new project"));
	newLabel->SetMessage(new BMessage(M_NEW_PROJECT));

	// open button and label

	fOpenButton = make_button("open", "OpenProjectButtonUp.png",
		"OpenProjectButtonDown.png", M_SHOW_OPEN_PROJECT);

	ClickableStringView* openLabel = make_label(fOpenButton, "Open a project");
	openLabel->SetMessage(new BMessage(M_SHOW_OPEN_PROJECT));

	// open recent button and label

	fOpenRecentButton = make_button("openrecent", "OpenRecentButtonUp.png",
		"OpenRecentButtonDown.png", M_OPEN_SELECTION);
	SetToolTip(fOpenRecentButton,
		TR("Open a project in the list on the right. You "
		   "can also press Command + a number key."));

	ClickableStringView* openRecentLabel = make_label(fOpenRecentButton,
		TR("Open the selected project"));
	openRecentLabel->SetMessage(new BMessage(M_OPEN_SELECTION));

	// quick import button and label

	fQuickImportButton = make_button("quickimport", "QuickImportButtonUp.png",
		"QuickImportButtonDown.png", M_SHOW_IMPORT);
	SetToolTip(fQuickImportButton,
		TR("Quickly make a project by importing all source files and resource files.\n"
		   "You can also import a BeIDE project."));

	ClickableStringView* quickImportLabel = make_label(fQuickImportButton,
		"Import an existing project");
	quickImportLabel->SetMessage(new BMessage(M_SHOW_IMPORT));
	SetToolTip(quickImportLabel,
		TR("Quickly make a project by importing all source files "
		   "and resource files.\n You can also import a BeIDE project."));

	// online import button and label

	fOnlineImportButton = make_button("onlineimport", "OnlineImportButtonUp.png",
		"OnlineImportButtonDown.png", M_ONLINE_IMPORT);
	SetToolTip(fQuickImportButton,
		TR("Import a project from an online repository"));

	ClickableStringView* onlineImportLabel = make_label(fOnlineImportButton,
		"Import a project from online");
	onlineImportLabel->SetMessage(new BMessage(M_ONLINE_IMPORT));
	SetToolTip(onlineImportLabel, TR("Import a project from an online repository"));

	// recent projects list view and scroller

	fRecentProjectsListView = new RecentProjectsList();
	// set the minimum width to 16em
	float minWidth = fRecentProjectsListView->StringWidth("M") * 16;
	fRecentProjectsListView->SetExplicitMinSize(BSize(minWidth, B_SIZE_UNSET));
	fRecentProjectsListView->SetInvocationMessage(new BMessage(M_OPEN_SELECTION));
	SetToolTip(fRecentProjectsListView,
		"Open a recent project. You can also press Command + a number key.");

	BLayoutBuilder::Group<>(this, B_HORIZONTAL)
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.Add(fNewButton, 0, 0)
			.Add(newLabel, 1, 0)

			.Add(fOpenButton, 0, 1)
			.Add(openLabel, 1, 1)

			.Add(fOpenRecentButton, 0, 2)
			.Add(openRecentLabel, 1, 2)

			.Add(fQuickImportButton, 0, 3)
			.Add(quickImportLabel, 1, 3)

			.Add(fOnlineImportButton, 0, 4)
			.Add(onlineImportLabel, 1, 4)
			.End()
		.AddStrut(20)
		.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
			.Add(new BStringView("recentProjectsLabel", TR("Recent projects:")))
			.Add(new BScrollView("recentProjectsScroller", fRecentProjectsListView, 0,
				false, true))
			.End()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.End();

#ifdef DISABLE_ONLINE_IMPORT
	fOnlineImportButton->Hide();
#endif

	BMessenger messager(this);
	BEntry entry(gProjectPath.GetFullPath());
	entry_ref ref;
	entry.GetRef(&ref);
	fOpenPanel = new BFilePanel(B_OPEN_PANEL, &messager, &ref, B_FILE_NODE, false,
		new BMessage(M_OPEN_PROJECT),new PaladinFileFilter());
	BString titleString(TR("Open project"));
	titleString.Prepend("Paladin: ");
	fOpenPanel->Window()->SetTitle(titleString.String());

	fImportPanel = new BFilePanel(B_OPEN_PANEL, &messager, &ref, B_DIRECTORY_NODE,
		true, new BMessage(M_QUICK_IMPORT));
	titleString = TR("Choose project folder");
	titleString.Prepend("Paladin: ");
	fImportPanel->Window()->SetTitle(titleString.String());

	gSettings.Lock();
	int32 index = 0;
	while (gSettings.FindRef("recentitems", index++, &ref) == B_OK) {
		if (!BEntry(&ref).Exists()) {
			index--;
			gSettings.RemoveData("recentitems",index);
		} else
			fRecentProjectsListView->AddItem(new RefStringItem(ref),0);
	}
	gSettings.Unlock();

	// Alt + number opens that number project from the list
	int32 count = (fRecentProjectsListView->CountItems() > 9) ? 9 : fRecentProjectsListView->CountItems();
	for (int32 i = 0; i < count; i++) {
		BMessage* listMessage = new BMessage(M_OPEN_FROM_LIST);
		listMessage->AddInt32("index", i);
		AddShortcut('1' + i, B_COMMAND_KEY, listMessage);
	}

	fNewButton->MakeFocus(true);

	CenterOnScreen();
}


StartWindow::~StartWindow(void)
{
	delete fOpenPanel;
	delete fImportPanel;
}


bool
StartWindow::QuitRequested(void)
{
	DeregisterWindow();
	return true;
}


void
StartWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_NEW_PROJECT:
		{
			TemplateWindow* window = new TemplateWindow(BRect(0, 0, 300, 200));
			window->Show();
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case M_SHOW_OPEN_PROJECT:
		{
			fOpenPanel->Show();
			break;
		}

		case M_OPEN_SELECTION:
		{
			RefStringItem* item = (RefStringItem*)fRecentProjectsListView->ItemAt(
				fRecentProjectsListView->CurrentSelection());
			if (item == NULL)
				break;

			message->AddRef("refs",&item->ref);
		}
		// fall through to M_OPEN_PROJECT
		case M_OPEN_PROJECT:
		{
			// gQuitOnZeroWindows is a special flag which we set to 0 so that when
			// this window quits and deregisters itself with the app Paladin
			// doesn't close.
			Hide();
			atomic_add(&gQuitOnZeroWindows,-1);
			message->what = B_REFS_RECEIVED;
			be_app->PostMessage(message);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case M_SHOW_IMPORT:
		{
			fImportPanel->Show();
			break;
		}

		case M_ONLINE_IMPORT:
		{
			if (!gHgAvailable && !gGitAvailable && !gSvnAvailable) {
				ShowAlert(TR("Online import requires Mercurial, "
					"Git, and/or Subversion to be installed, "
					"but Paladin can't find them, sorry."));
			} else {
				SCMImportWindow* window = new SCMImportWindow();
				window->Show();
			}
			break;
		}

		case M_OPEN_FROM_LIST:
		{
			int32 index;
			if (message->FindInt32("index", &index) == B_OK) {
				fRecentProjectsListView->Select(index);
				PostMessage(M_OPEN_SELECTION);
			}
			break;
		}

		case M_QUICK_IMPORT:
		{
			Hide();
			be_app->PostMessage(message);
			atomic_add(&gQuitOnZeroWindows,-1);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case M_REMOVE_FROM_LIST:
		{
			RefStringItem* item = (RefStringItem*)fRecentProjectsListView->ItemAt(
				fRecentProjectsListView->CurrentSelection());
			if (item == NULL)
				break;
			
			fRecentProjectsListView->RemoveItem(item);
			
			gSettings.Lock();
			
			entry_ref ref;
			int32 index = 0;
			while (gSettings.FindRef("recentitems",index++,&ref) == B_OK) {	
				if (ref == item->ref) {
					gSettings.RemoveData("recentitems",index - 1);
					break;
				}
			}
			gSettings.Unlock();
			delete item;
			break;
		}

		default:
			BWindow::MessageReceived(message);
	}
}
