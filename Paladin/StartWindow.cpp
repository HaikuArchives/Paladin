#include "StartWindow.h"

#include <Alert.h>
#include <Entry.h>
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

enum
{
	M_OPEN_SELECTION = 'opsl',
	M_REMOVE_FROM_LIST = 'rmls',
	M_SHOW_IMPORT = 'shim',
	M_ONLINE_IMPORT = 'olim',
	M_OPEN_FROM_LIST = 'ofls'
};

class RefStringItem : public BStringItem
{
public:
	RefStringItem(entry_ref fileref);
	
	entry_ref ref;
};

RefStringItem::RefStringItem(entry_ref fileref)
	:	BStringItem(fileref.name)
{
	ref = fileref;
	BString name(ref.name);
	name.RemoveLast(".pld");
	SetText(name.String());
}

class StartWindowList : public BListView
{
public:
							StartWindowList(const BRect &frame);
		void				MouseDown(BPoint pt);
};


StartWindow::StartWindow(void)
	:	DWindow(BRect(0,0,400,300),"Paladin",B_DOCUMENT_WINDOW, B_NOT_ZOOMABLE)
{
	RegisterWindow();
	AddShortcut('O',B_COMMAND_KEY, new BMessage(M_SHOW_OPEN_PROJECT));
	AddShortcut('N',B_COMMAND_KEY, new BMessage(M_NEW_PROJECT));
	AddShortcut('I',B_COMMAND_KEY, new BMessage(M_SHOW_IMPORT));
	
	AddCommonFilter(new EscapeCancelFilter());
	
	MakeCenteredOnShow(true);
	
	BView *top = GetBackgroundView();
	
	fNewButton = MakeButton("new","NewProjectButtonUp.png","NewProjectButtonDown.png",
							M_NEW_PROJECT);
	top->AddChild(fNewButton);
	fNewButton->MoveTo(10,10);
		
	ClickableStringView *label = MakeLabel(fNewButton,TR("Create a new project"));
	top->AddChild(label);
	label->SetMessage(new BMessage(M_NEW_PROJECT));
	
	fOpenButton = MakeButton("open","OpenProjectButtonUp.png",
							"OpenProjectButtonDown.png", M_SHOW_OPEN_PROJECT);
	top->AddChild(fOpenButton);
	fOpenButton->MoveTo(10,fNewButton->Frame().bottom + 10.0);
	
	label = MakeLabel(fOpenButton,"Open a project");
	top->AddChild(label);
	label->SetMessage(new BMessage(M_SHOW_OPEN_PROJECT));
	
	fOpenRecentButton = MakeButton("openrecent","OpenRecentButtonUp.png",
									"OpenRecentButtonDown.png", M_OPEN_SELECTION);
	top->AddChild(fOpenRecentButton);
	fOpenRecentButton->MoveTo(10,fOpenButton->Frame().bottom + 10.0);
	SetToolTip(fOpenRecentButton,TR("Open a project in the list on the right. You "
									"can also press Command + a number key."));
	
	label = MakeLabel(fOpenRecentButton,TR("Open the selected project"));
	top->AddChild(label);
	label->SetMessage(new BMessage(M_OPEN_SELECTION));
	
	BRect r(Bounds());
	r.left = label->Frame().right + 20.0;
	r.right -= B_V_SCROLL_BAR_WIDTH + 10.0;
	r.top += 10.0;
	r.bottom -= 10.0;
	
	fListView = new StartWindowList(r);
	BScrollView *scrollView = new BScrollView("scroller",fListView,0,
											B_FOLLOW_ALL, false, true);
	top->AddChild(scrollView);
	fListView->SetInvocationMessage(new BMessage(M_OPEN_SELECTION));
	SetToolTip(fListView, "Open a recent project. You can also press Command + a number key.");
	
	fQuickImportButton = MakeButton("quickimport","QuickImportButtonUp.png",
									"QuickImportButtonDown.png",M_SHOW_IMPORT);
	top->AddChild(fQuickImportButton);
	fQuickImportButton->MoveTo(10,fOpenRecentButton->Frame().bottom + 10.0);
	
	label = MakeLabel(fQuickImportButton,"Import an existing project");
	top->AddChild(label);
	label->SetMessage(new BMessage(M_SHOW_IMPORT));
	SetToolTip(label,TR("Quickly make a project by importing all source files and resource files.\n"
						"You can also import a BeIDE project."));
	SetToolTip(fQuickImportButton,
				TR("Quickly make a project by importing all source files and resource files.\n"
					"You can also import a BeIDE project."));	
	fOnlineImportButton = MakeButton("onlineimport","OnlineImportButtonUp.png",
									"OnlineImportButtonDown.png",M_ONLINE_IMPORT);
	top->AddChild(fOnlineImportButton);
	fOnlineImportButton->MoveTo(10,fQuickImportButton->Frame().bottom + 10.0);
	
	label = MakeLabel(fOnlineImportButton,"Import a project from online");
	top->AddChild(label);
	label->SetMessage(new BMessage(M_ONLINE_IMPORT));
	SetToolTip(label,TR("Import a project from an online repository"));
	SetToolTip(fQuickImportButton,
				TR("Import a project from an online repository"));

	
	#ifdef DISABLE_ONLINE_IMPORT
	
	fOnlineImportButton->Hide();
	SetSizeLimits(scrollView->Frame().left + 110.0,30000,
				fQuickImportButton->Frame().bottom + 10.0,30000);
	ResizeTo(scrollView->Frame().left + 160.0,fQuickImportButton->Frame().bottom + 10.0);
	
	#else
	
	SetSizeLimits(scrollView->Frame().left + 110.0,30000,
				fOnlineImportButton->Frame().bottom + 10.0,30000);
	ResizeTo(scrollView->Frame().left + 160.0,fOnlineImportButton->Frame().bottom + 10.0);
	
	#endif
	
	BMessenger msgr(this);
	BEntry entry(gProjectPath.GetFullPath());
	entry_ref ref;
	entry.GetRef(&ref);
	fOpenPanel = new BFilePanel(B_OPEN_PANEL,&msgr,&ref,B_FILE_NODE,true,
								new BMessage(M_OPEN_PROJECT));
	BString titleString(TR("Open Project"));
	titleString.Prepend("Paladin: ");
	fOpenPanel->Window()->SetTitle(titleString.String());
	
	fImportPanel = new BFilePanel(B_OPEN_PANEL,&msgr,&ref,B_DIRECTORY_NODE,true,
								new BMessage(M_QUICK_IMPORT));
	titleString = TR("Choose Project Folder");
	titleString.Prepend("Paladin: ");
	fImportPanel->Window()->SetTitle(titleString.String());
	
	gSettings.Lock();
	int32 index = 0;
	while (gSettings.FindRef("recentitems",index++,&ref) == B_OK)
	{
		if (!BEntry(&ref).Exists())
		{
			index--;
			gSettings.RemoveData("recentitems",index);
		}
		else
			fListView->AddItem(new RefStringItem(ref),0);
	}
	gSettings.Unlock();
	
	// Alt + number opens that number project from the list
	int32 count = (fListView->CountItems() > 9) ? 9 : fListView->CountItems();
	for (int32 i = 0; i < count; i++)
	{
		BMessage *listMsg = new BMessage(M_OPEN_FROM_LIST);
		listMsg->AddInt32("index", i);
		AddShortcut('1' + i, B_COMMAND_KEY, listMsg);
	}
	
	fNewButton->MakeFocus(true);
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
StartWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_NEW_PROJECT:
		{
			TemplateWindow *twin = new TemplateWindow(BRect(0,0,400,300));
			twin->Show();
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
			RefStringItem *item = (RefStringItem*)fListView->ItemAt(fListView->CurrentSelection());
			if (!item)
				break;
			
			msg->AddRef("refs",&item->ref);
			
			// fall through to M_OPEN_PROJECT
		}
		case M_OPEN_PROJECT:
		{
			// gQuitOnZeroWindows is a special flag which we set to 0 so that when this window
			// quits and deregisters itself with the app, Paladin doesn't close.
			Hide();
			atomic_add(&gQuitOnZeroWindows,-1);
			msg->what = B_REFS_RECEIVED;
			be_app->PostMessage(msg);
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
			if (!gHgAvailable && !gGitAvailable && !gSvnAvailable)
			{
				ShowAlert("Online import requires Mercurial, "
							"Git, and/or Subversion to be installed, "
							"but Paladin can't find them. Sorry.");
			}
			else
			{
				SCMImportWindow *win = new SCMImportWindow();
				win->Show();
			}
			break;
		}
		case M_OPEN_FROM_LIST:
		{
			int32 index;
			if (msg->FindInt32("index", &index) == B_OK)
			{
				fListView->Select(index);
				PostMessage(M_OPEN_SELECTION);
			}
			break;
		}
		case M_QUICK_IMPORT:
		{
			Hide();
			be_app->PostMessage(msg);
			atomic_add(&gQuitOnZeroWindows,-1);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case M_REMOVE_FROM_LIST:
		{
			RefStringItem *item = (RefStringItem*)fListView->ItemAt(fListView->CurrentSelection());
			if (!item)
				break;
			
			fListView->RemoveItem(item);
			
			gSettings.Lock();
			
			entry_ref ref;
			int32 index = 0;
			while (gSettings.FindRef("recentitems",index++,&ref) == B_OK)
			{
				
				if (ref == item->ref)
				{
					gSettings.RemoveData("recentitems",index - 1);
					break;
				}
			}
			
			gSettings.Unlock();
			
			delete item;
			
			break;
		}
		default:
			DWindow::MessageReceived(msg);
	}
}


BitmapButton *
StartWindow::MakeButton(const char *name, const char *up, const char *down,
						int32 command)
{
	BBitmap *upbmp = BTranslationUtils::GetBitmap('PNG ',up);
	BBitmap *downbmp = BTranslationUtils::GetBitmap('PNG ',down);
	BitmapButton *button = new BitmapButton(BRect(0,0,31,31),name,upbmp,downbmp,
											new BMessage(command));
	return button;
}


ClickableStringView *
StartWindow::MakeLabel(BitmapButton *button, const char *label)
{
	if (!button)
		return NULL;
	
	BString labelName = button->Name();
	labelName << " label";
	ClickableStringView *labelView = new ClickableStringView(BRect(0,0,1,1),labelName.String(), label);
	labelView->ResizeToPreferred();
	
	BRect r(button->Frame());
	labelView->MoveTo(r.right + 10.0,r.top + 
									((r.Height() - labelView->Bounds().Height()) / 2.0));
	return labelView;
}


StartWindowList::StartWindowList(const BRect &frame)
	:	BListView(frame,"listview",B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL)
{
}


void
StartWindowList::MouseDown(BPoint pt)
{
	uint32 buttons;
	BPoint point;
	GetMouse(&point,&buttons);
	if (buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		BMessage *msg = Window()->CurrentMessage();
		int32 clicks;
		msg->FindInt32("clicks",&clicks);
		if (clicks > 1)
		{
			clicks = 1;
			msg->ReplaceInt32("clicks",clicks);
		}
	}
	BListView::MouseDown(pt);
	
	if (buttons & B_SECONDARY_MOUSE_BUTTON)
	{
		BPoint screenpt(pt);
		ConvertToScreen(&screenpt);
		screenpt.x -= 5;
		screenpt.y -= 5;
		
		BStringItem *stringItem = (BStringItem*)ItemAt(IndexOf(pt));
		if (!stringItem)
			return;
		
		BPopUpMenu menu("context");
		
		menu.AddItem(new BMenuItem(TR("Remove from Recent List"),
									new BMessage(M_REMOVE_FROM_LIST)));
		menu.SetTargetForItems(Window());
		menu.Go(screenpt,true,false);
	}
}

