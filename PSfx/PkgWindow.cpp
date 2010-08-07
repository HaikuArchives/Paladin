#include "PkgWindow.h"
#include <Alert.h>
#include <AppFileInfo.h>
#include <Application.h>
#include <Box.h>
#include <Messenger.h>
#include <Path.h>
#include <Roster.h>

#include "App.h"
#include "FileListView.h"
#include "ListItem.h"
#include "PackageInfo.h"

enum
{
	M_NEW_PACKAGE = 'nwpk',
	M_SHOW_OPEN_PACKAGE = 'oppk',
	M_SAVE_PACKAGE = 'svpk',
	M_SAVE_PACKAGE_AS = 'sapk',
	M_ABOUT_REQUESTED = 'abrq',
	M_SET_INSTALL_PATH = 'sinp',
	M_SET_REPLACE_MODE = 'strp',
	M_BUILD_PACKAGE = 'bldp',
	M_ITEM_SELECTED = 'itsl',
	M_SHOW_ADD_ITEM = 'shad',
	M_ADD_ITEM = 'adit',
	M_REMOVE_ITEM = 'rmit'
};

static int32 sUntitled = 0;

PkgWindow::PkgWindow(entry_ref *ref)
	:	DWindow(BRect(100,100,479,500),"PSfx"),
		fOpenPanel(NULL),
		fAddPanel(NULL),
		fSavePanel(NULL),
		fNeedSave(false),
		fQuitAfterSave(false)
{
	MakeCenteredOnShow(true);
	RegisterWindow();
	BView *top = GetBackgroundView();
	
	BuildMenus();
	top->AddChild(fBar);
	
	BRect r(Bounds().InsetByCopy(10,10));
	BBox *box = new BBox(r,NULL,B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	box->SetLabel("Item Settings");
	
	// Create Install Path field
	BMenu *menu = new BMenu("Path");
	menu->SetLabelFromMarked(true);
	
	float w,h;
	
	r.top += 5;
	fInstallField = new BMenuField(r,"installfield","Path: ",menu, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	box->AddChild(fInstallField);
	fInstallField->GetPreferredSize(&w,&h);
	fInstallField->ResizeTo(r.Width(),h);
	fInstallField->SetDivider(fInstallField->StringWidth("Replacement Mode:") + 5.0);
	r = fInstallField->Frame();
	
	// Create Replacement mode field
	menu = new BMenu("Replace Mode");
	menu->SetLabelFromMarked(true);
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fReplaceField = new BMenuField(r,"replacefield","Replacement Mode: ",menu,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	box->AddChild(fReplaceField);
	fReplaceField->SetDivider(fInstallField->Divider());
	
	InitFields();
	
	// Create Groups label
	r.OffsetBy(0.0, r.Height() + 5.0);
	fGroupLabel = new BStringView(r,"grouplabel","Groups: ",
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fGroupLabel->GetPreferredSize(&w,&h);
	fGroupLabel->ResizeTo(r.Width(),h);
	box->AddChild(fGroupLabel);
	r = fGroupLabel->Frame();
	
	// Create Platform field
	menu = new BMenu("Platform");
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fPlatformLabel = new BStringView(r,"platformlabel","Platforms: ",
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	box->AddChild(fPlatformLabel);
	r = fPlatformLabel->Frame();
	
	// create Link field -- adding links to different locations
	menu = new BMenu("Links");
	
	r.OffsetBy(0.0, r.Height() + 5.0);
	fLinkLabel = new BStringView(r,"linklabel","Create Links: ",
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	box->AddChild(fLinkLabel);
	
	box->ResizeTo(box->Frame().Width(),fLinkLabel->Frame().bottom + 10.0);
	box->MoveTo(10.0,Bounds().bottom - box->Frame().Height() - 10.0);
	
	// Set up the item list
	r.left = 10.0;
	r.top = fBar->Frame().bottom + 10;
	r.right = Bounds().Width() - 10.0 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = box->Frame().top - 10.0 - B_H_SCROLL_BAR_HEIGHT;
	fListView = new FileListView(r,"listview",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL);
	BScrollView *sv = fListView->MakeScrollView("listscrollview",true,true);
	fListView->SetDefaultDisplayMode(REFITEM_NAME_ICON);
	fListView->SetSelectionMessage(new BMessage(M_ITEM_SELECTED));
	
	r.OffsetBy(0,r.Height() + B_H_SCROLL_BAR_HEIGHT + 10.0);
	r.bottom = Bounds().bottom - 10.0;
	r.right += B_V_SCROLL_BAR_WIDTH;
	
	top->AddChild(sv);
	top->AddChild(box);
	
	fOpenPanel = new BFilePanel();
	BMessenger msgr(this);
	fAddPanel = new BFilePanel(B_OPEN_PANEL, &msgr, 0, 0, true, new BMessage(M_ADD_ITEM));
	
	if (ref)
		LoadPackage(*ref);
	else
		InitEmptyProject();
}


PkgWindow::~PkgWindow(void)
{
	delete fOpenPanel;
	delete fAddPanel;
	delete fSavePanel;
}


bool
PkgWindow::QuitRequested(void)
{
	if (fNeedSave)
	{
		BAlert *alert = new BAlert("PSfx","Save your changes?",
									"Don't Save", "Cancel", "Save");
		int32 result = alert->Go();
		switch (result)
		{
			case 1:
			{
				return false;
				break;
			}
			case 2:
			{
				bool returnval = true;
				if (fFilePath.IsEmpty())
				{
					returnval = false;
					fQuitAfterSave = true;
				}
				ShowSave(false);
				return returnval;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	DeregisterWindow();
	return true;
}


void
PkgWindow::BuildMenus(void)
{
	BRect r(Bounds());
	r.bottom = 20;
	fBar = new BMenuBar(r,"menubar");
	
	BMenu *menu = new BMenu("Package");
	menu->AddItem(new BMenuItem("New…",new BMessage(M_NEW_PACKAGE),'N'));
	menu->AddItem(new BMenuItem("Open…",new BMessage(M_SHOW_OPEN_PACKAGE),'O'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Save",new BMessage(M_SAVE_PACKAGE),'S'));
	menu->AddItem(new BMenuItem("Save As…",new BMessage(M_SAVE_PACKAGE),'S',
								B_COMMAND_KEY | B_SHIFT_KEY));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("About PSfx…",new BMessage(M_ABOUT_REQUESTED)));
	fBar->AddItem(menu);
	
	menu = new BMenu("Items");
	menu->AddItem(new BMenuItem("Add…", new BMessage(M_ADD_ITEM), 'A',
								B_COMMAND_KEY | B_SHIFT_KEY));
	menu->AddItem(new BMenuItem("Remove",new BMessage(M_REMOVE_ITEM), 'D'));
	fBar->AddItem(menu);
	
	menu = new BMenu("Installation");
	fBar->AddItem(menu);
}


void
PkgWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_NEW_PACKAGE:
		{
			PkgWindow *win = new PkgWindow(NULL);
			win->Show();
			break;
		}
		case M_SHOW_OPEN_PACKAGE:
		{
			fOpenPanel->Show();
			break;
		}
		case M_SAVE_PACKAGE:
		{
			ShowSave(false);
			break;
		}
		case M_SAVE_PACKAGE_AS:
		{
			ShowSave(true);
			break;
		}
		case B_SAVE_REQUESTED:
		{
			entry_ref ref;
			BString name;
			if (msg->FindRef("directory",&ref) == B_OK && 
				msg->FindString("name",&name) == B_OK)
			{
				debugger("");
				fFilePath.SetTo(ref);
				fFilePath << name;
				DoSave();
			}
			
			break;
		}
		case M_ABOUT_REQUESTED:
		{
			ShowAbout();
			break;
		}
		case M_QUIT_PROGRAM:
		{
			be_app->PostMessage(M_QUIT_PROGRAM);
			break;
		}
		case M_ITEM_SELECTED:
		{
			ItemSelected(fListView->CurrentSelection());
			break;
		}
		case M_SHOW_ADD_ITEM:
		{
			fAddPanel->Show();
			break;
		}
		case M_REMOVE_ITEM:
		{
			for (int32 i = fListView->CountItems() - 1; i >= 0; i--)
			{
				FileListItem *item = dynamic_cast<FileListItem*>(fListView->ItemAt(i));
				if (item && item->IsSelected())
				{
					fListView->RemoveItem(item);
					delete item;
				}
			}
			break;
		}
		case M_SET_INSTALL_PATH:
		{
			PathMenuItem *item = (PathMenuItem*)fInstallField->Menu()->FindMarked();
			if (!item)
				break;
			
			SetInstallFolder(item->GetPath());
			fNeedSave = true;
			break;
		}
		case M_SET_REPLACE_MODE:
		{
			BMenuItem *item = fReplaceField->Menu()->FindMarked();
			if (!item)
				break;
			
			SetReplaceMode(fReplaceField->Menu()->IndexOf(item));
			fNeedSave = true;
			break;
		}
		case M_NEW_PACKAGE_ITEM:
		{
			FileListItem *listItem;
			if (msg->FindPointer("item",(void**)&listItem) != B_OK)
				break;
			
			entry_ref ref = listItem->GetRef();
			FileItem *fileItem = new FileItem();
			listItem->SetData(fileItem);
			fileItem->SetName(ref.name);
			
			fPkgInfo.AddFile(fileItem);
			fNeedSave = true;
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
		}
	}
}


void
PkgWindow::ShowAbout(void)
{
	app_info ai;
	version_info vi;
	be_app->GetAppInfo(&ai);
	BFile file(&ai.ref,B_READ_ONLY);
	BAppFileInfo appinfo(&file);
	appinfo.GetVersionInfo(&vi,B_APP_VERSION_KIND);

	BString variety;
	switch(vi.variety)
	{
		case 0:
			variety="d";
			break;
		case 1:
			variety="a";
			break;
		case 2:
			variety="b";
			break;
		case 3:
			variety="g";
			break;
		case 4:
			variety="rc";
			break;
		default:
			variety="Final";
			break;
	}
	
	char version[128];
	if(variety != "Final")
		sprintf(version,"v%lu.%lu %s%lu",vi.major,vi.middle,variety.String(),vi.internal);
	else
		sprintf(version,"v%lu.%lu",vi.major,vi.middle);
	
	BString aboutstr;
	aboutstr << "PSfx " << version << "\n©2010 DarkWyrm";
	BAlert *alert = new BAlert("PSfx",aboutstr.String(),"OK");
	alert->Go();
}


void
PkgWindow::ItemSelected(int32 index)
{
	FileListItem *item = (FileListItem*)fListView->ItemAt(index);
	FileItem *fileItem = item ? item->GetData() : NULL;

	// Get install folder
	BMenuItem *menuItem = fInstallField->Menu()->FindMarked();
	if (menuItem)
		menuItem->SetMarked(false);
	
	if (!item)
		fInstallField->Menu()->ItemAt(0L)->SetMarked(true);
	else
	{
		if (!fileItem)
			return;
		
		switch (item->GetData()->GetPathConstant())
		{
			case M_INSTALL_DIRECTORY:
			{
				fInstallField->Menu()->ItemAt(0L)->SetMarked(true);
				break;
			}
			case M_CUSTOM_DIRECTORY:
			{
				// TODO: Implement
				debugger("");
				break;
			}
			default:
			{
				BMenuItem *menuItem = fInstallField->Menu()->FindItem(
													GetFriendlyPathConstantName(
															fileItem->GetPathConstant()).String());
				if (menuItem)
					menuItem->SetMarked(true);
				break;
			}
		}
	}
	
	// get replace mode
	menuItem = fReplaceField->Menu()->FindMarked();
	if (menuItem)
		menuItem->SetMarked(false);
	if (fileItem)
		fReplaceField->Menu()->ItemAt(fileItem->GetReplaceMode())->SetMarked(true);
	
	// get groups
	BString label = "Groups: ";
	if (fileItem)
		label << fileItem->GroupString().String();
	
	fGroupLabel->SetText(label.String());
	
	// get platforms
	label = "Platforms: ";
	if (fileItem)
		label << fileItem->PlatformString().String();
	
	fPlatformLabel->SetText(label.String());
	
	// get links
	label = "Links: ";
	if (fileItem)
		label << fileItem->LinkString().String();
	
	fLinkLabel->SetText(label.String());
}


void
PkgWindow::InitFields(void)
{
	BMenu *menu = fInstallField->Menu();
	
	BMenuItem *item = new PathMenuItem("Package Install Directory",M_INSTALL_DIRECTORY,
									new BMessage(M_SET_INSTALL_PATH));
	menu->AddItem(item);
	item->SetMarked(true);
	
	menu->AddSeparatorItem();
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_DESKTOP_DIRECTORY).String(),
									B_DESKTOP_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_DESKBAR_DIRECTORY).String(),
									B_USER_DESKBAR_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_APPS_DIRECTORY).String(),
									B_APPS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_PREFERENCES_DIRECTORY).String(),
									B_PREFERENCES_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_UTILITIES_DIRECTORY).String(),
									B_UTILITIES_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddSeparatorItem();
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_DIRECTORY).String(),
									B_USER_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_ADDONS_DIRECTORY).String(),
									B_USER_ADDONS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_BOOT_DIRECTORY).String(),
									B_USER_BOOT_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_CACHE_DIRECTORY).String(),
									B_USER_CACHE_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_CONFIG_DIRECTORY).String(),
									B_USER_CONFIG_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_DATA_DIRECTORY).String(),
									B_USER_DATA_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_FONTS_DIRECTORY).String(),
									B_USER_FONTS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_LIB_DIRECTORY).String(),
									B_USER_LIB_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_MEDIA_NODES_DIRECTORY).String(),
									B_USER_MEDIA_NODES_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_PRINTERS_DIRECTORY).String(),
									B_USER_PRINTERS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_SETTINGS_DIRECTORY).String(),
									B_USER_SETTINGS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_SOUNDS_DIRECTORY).String(),
									B_USER_SOUNDS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_TRANSLATORS_DIRECTORY).String(),
									B_USER_TRANSLATORS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddSeparatorItem();
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DIRECTORY).String(),
									B_COMMON_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_ADDONS_DIRECTORY).String(),
									B_COMMON_ADDONS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_BIN_DIRECTORY).String(),
									B_COMMON_BIN_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_BOOT_DIRECTORY).String(),
									B_COMMON_BOOT_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DATA_DIRECTORY).String(),
									B_COMMON_DATA_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DEVELOP_DIRECTORY).String(),
									B_COMMON_DEVELOP_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DOCUMENTATION_DIRECTORY).String(),
									B_COMMON_DOCUMENTATION_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_ETC_DIRECTORY).String(),
									B_COMMON_ETC_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_FONTS_DIRECTORY).String(),
									B_COMMON_FONTS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_LIB_DIRECTORY).String(),
									B_COMMON_LIB_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_LOG_DIRECTORY).String(),
									B_COMMON_LOG_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_MEDIA_NODES_DIRECTORY).String(),
									B_COMMON_MEDIA_NODES_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SERVERS_DIRECTORY).String(),
									B_COMMON_SERVERS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SETTINGS_DIRECTORY).String(),
									B_COMMON_SETTINGS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SOUNDS_DIRECTORY).String(),
									B_COMMON_SOUNDS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SPOOL_DIRECTORY).String(),
									B_COMMON_SPOOL_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SYSTEM_DIRECTORY).String(),
									B_COMMON_SYSTEM_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_TEMP_DIRECTORY).String(),
									B_COMMON_TEMP_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_TRANSLATORS_DIRECTORY).String(),
									B_COMMON_TRANSLATORS_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_VAR_DIRECTORY).String(),
									B_COMMON_VAR_DIRECTORY,new BMessage(M_SET_INSTALL_PATH)));
	
	
	menu = fReplaceField->Menu();
	menu->AddItem(new BMenuItem("Always Ask User",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Never Replace",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Rename Existing",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Ask user if newer version",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Ask user if newer creation date",
								new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Ask user if newer modification date",
								new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Replace if newer version",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Replace if newer creation date",
								new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Replace if newer modification date",
								new BMessage(M_SET_REPLACE_MODE)));
//	menu->AddItem(new BMenuItem("Merge with existing folder",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Install only if item exists",new BMessage(M_SET_REPLACE_MODE)));
	menu->ItemAt(0)->SetMarked(true);
}


void
PkgWindow::InitEmptyProject(void)
{
	BString title("Untitled ");
	sUntitled++;
	title << sUntitled;
	SetTitle(title.String());
}


void
PkgWindow::LoadPackage(entry_ref ref)
{
	BPath path(&ref);
	if (fPkgInfo.LoadFromFile(path.Path()) != B_OK)
	{
		// TODO: complain that we failed.
		return;
	}
	
	// TODO: Load custom paths into field
	
	fListView->MakeEmpty();
	for (int32 i = 0; i < fPkgInfo.CountFiles(); i++)
	{
		FileItem *fileItem = fPkgInfo.FileAt(i);
		FileListItem *listItem = new FileListItem(entry_ref(), REFITEM_NAME);
		listItem->SetText(fileItem->GetName());
		listItem->SetData(fileItem);
		fListView->AddItem(listItem);
	}
	
	fFilePath = path.Path();
}


void
PkgWindow::SetInstallFolder(const int32 &value, const char *custom)
{
	int32 i = 0; 
	int32 selection = fListView->CurrentSelection(i++);
	while (selection >= 0)
	{
		FileListItem *item = (FileListItem*)fListView->ItemAt(selection);
		if (item->GetData())
		{
			if (custom)
				item->GetData()->SetPath(custom);
			else
				item->GetData()->SetPath(value);
		}
		
		selection = fListView->CurrentSelection(i++);
	}
}


void
PkgWindow::SetReplaceMode(const int32 &value)
{
	int32 i = 0; 
	int32 selection = fListView->CurrentSelection(i++);
	while (selection >= 0)
	{
		FileListItem *item = (FileListItem*)fListView->ItemAt(selection);
		if (item->GetData())
			item->GetData()->SetReplaceMode(value);
		selection = fListView->CurrentSelection(i++);
	}
}


void
PkgWindow::ShowSave(bool force_saveas)
{
	if (fFilePath.IsEmpty() || force_saveas)
	{
		// Execute Save As code
		if (!fSavePanel)
			fSavePanel = new BFilePanel(B_SAVE_PANEL,new BMessenger(NULL, this));
		fSavePanel->Show();
	}
	else
		DoSave();
}


void
PkgWindow::DoSave(void)
{
	fNeedSave = false;
	
	fPkgInfo.SaveToFile(fFilePath.GetFullPath());
	
	if (fQuitAfterSave)
		PostMessage(B_QUIT_REQUESTED);
}


PathMenuItem::PathMenuItem(const char *label, const int32 &constant, BMessage *msg)
	:	BMenuItem(label, msg),
		fPathConstant(constant)
{
}


int32
PathMenuItem::GetPath(void) const
{
	return fPathConstant;
}


void
PathMenuItem::SetPath(const int32 &path)
{
	fPathConstant = path;
}

