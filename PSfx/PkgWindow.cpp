#include "PkgWindow.h"

#include <Alert.h>
#include <AppFileInfo.h>
#include <Application.h>
#include <Box.h>
#include <File.h>
#include <fstream>
#include <iostream>
#include <Messenger.h>
#include <Path.h>
#include <Resources.h>
#include <Roster.h>
#include <stdlib.h>
#include <string>

#include "AboutWindow.h"
#include "App.h"
#include "CommandLine.h"
#include "FileListView.h"
#include "Globals.h"
#include "ListItem.h"
#include "PackageInfo.h"
#include "PkgInfoWindow.h"

using namespace std;

enum
{
	M_NEW_PACKAGE = 'nwpk',
	M_SHOW_OPEN_PACKAGE = 'oppk',
	M_SAVE_PACKAGE = 'svpk',
	M_SAVE_PACKAGE_AS = 'sapk',
	M_SHOW_PACKAGE_INFO = 'shpi',
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
		fQuitAfterSave(false),
		fBuildAfterSave(false),
		fBuildType(OS_HAIKU)
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
	r.right -= 15;
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
	fListView = new FileListView(r,"listview",B_MULTIPLE_SELECTION_LIST,B_FOLLOW_ALL);
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
		LoadProject(*ref);
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
	menu->AddItem(new BMenuItem("Add…", new BMessage(M_SHOW_ADD_ITEM), 'A',
								B_COMMAND_KEY | B_SHIFT_KEY));
	menu->AddItem(new BMenuItem("Remove",new BMessage(M_REMOVE_ITEM), 'D'));
	fBar->AddItem(menu);
	
	
	menu = new BMenu("Installation");
	
	menu->AddItem(new BMenuItem("Package Settings…", new BMessage(M_SHOW_PACKAGE_INFO),
				','));
	menu->AddSeparatorItem();
	BMenu *submenu = new BMenu("Build Package");
	menu->AddItem(submenu);
	
	BMessage *msg = new BMessage(M_BUILD_PACKAGE);
	msg->AddInt32("type", OS_HAIKU);
	submenu->AddItem(new BMenuItem("Haiku", msg, 'B'));
	
	msg = new BMessage(M_BUILD_PACKAGE);
	msg->AddInt32("type", OS_HAIKU_GCC4);
	submenu->AddItem(new BMenuItem("Haiku GCC4", msg, 'B', B_COMMAND_KEY | B_SHIFT_KEY));
	
	msg = new BMessage(M_BUILD_PACKAGE);
	msg->AddInt32("type", OS_ZETA);
	submenu->AddItem(new BMenuItem("Zeta", msg));
	
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
				DPath tempPath(ref);
				tempPath << name;
				if (!tempPath.HasExtension())
					tempPath.SetExtension("pfx");
				fFilePath.SetTo(tempPath);
				DoSave();
			}
			
			break;
		}
		case M_SHOW_PACKAGE_INFO:
		{
			PkgInfoWindow *win = new PkgInfoWindow(this, &fPkgInfo);
			win->Show();
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
		case M_ADD_ITEM:
		{
			int32 index = 0;
			entry_ref ref;
			while (msg->FindRef("refs", index++, &ref) == B_OK)
			{
				FileListItem *item = new FileListItem(ref, fListView->GetDefaultDisplayMode());
				fListView->AddItem(item);
			}
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
		case M_BUILD_PACKAGE:
		{
			int32 type;
			if (msg->FindInt32("type",&type) == B_OK)
			{
				if (fFilePath.IsEmpty())
				{
					fBuildAfterSave = true;
					fBuildType = (ostype_t)type;
					ShowSave(true);
					break;
				}
				
				Build((ostype_t)type);
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
	AboutWindow *win = new AboutWindow();
	win->Show();
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
		
		switch (item->GetData()->GetPath().ResolveToConstant())
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
												fileItem->GetPath().ResolveToConstant()).String());
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
	{
		BMenuItem *newReplaceItem = fReplaceField->Menu()->ItemAt(fileItem->GetReplaceMode());
		if (newReplaceItem)
			newReplaceItem->SetMarked(true);
		else
			fReplaceField->Menu()->ItemAt(0L)->SetMarked(true);
	}
	
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
	
	GeneratePathMenu(menu, M_SET_INSTALL_PATH);
	
	menu = fReplaceField->Menu();
	menu->AddItem(new BMenuItem("Always Ask User",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Never Replace",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Rename Existing",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Ask user if newer version",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Replace if newer version",new BMessage(M_SET_REPLACE_MODE)));
	menu->AddItem(new BMenuItem("Install only if item exists",new BMessage(M_SET_REPLACE_MODE)));
	menu->ItemAt(0)->SetMarked(true);
}


void
PkgWindow::InitEmptyProject(void)
{
	BString title("PSfx: Untitled ");
	sUntitled++;
	title << sUntitled;
	SetTitle(title.String());
}


void
PkgWindow::LoadProject(entry_ref ref)
{
	DPath path(ref);
	ifstream file(path.GetFullPath(), ifstream::in);
	
	FileItem *item = NULL;
	FileListItem *listItem = NULL;
	while (file.good())
	{
		string line;
		getline(file, line);
		
		size_t pos = line.find("=");
		if (pos == string::npos)
			continue;
		
		string key = line.substr(0, pos);
		string value = line.substr(pos + 1, string::npos);
		
		if (key == "ITEMFILEPATH")
		{
			listItem = new FileListItem(value.c_str(),
										fListView->GetDefaultDisplayMode());
			item = new FileItem();
			listItem->SetData(item);
			fListView->AddItem(listItem);
		}
		else
		if (key == "ITEMNAME")
		{
			if (listItem)
				listItem->SetText(value.c_str());
			if (item)
				item->SetName(value.c_str());
		}
		else
		if (key == "ITEMINSTALLEDNAME")
			item->SetInstalledName(value.c_str());
		else
		if (key == "ITEMREPLACEMODE")
			item->SetReplaceMode(atoi(value.c_str()));
		else
		if (key == "ITEMPATH")
			item->SetPath(value.c_str());
		else
		if (key == "ITEMCONSTPATH")
			item->SetPath(value.c_str());
		else
		if (key == "ITEMGROUP")
			item->AddGroup(value.c_str());
		else
		if (key == "ITEMPLATFORM")
			item->AddGroup(value.c_str());
		else
		if (key == "ITEMLINK")
			item->AddLink(value.c_str());
		else
		if (key == "ITEMCATEGORY=")
			item->SetCategory(value.c_str());
		else
		if (key == "PKGVERSION")
			fPkgInfo.SetPackageVersion(value.c_str());
		else
		if (key == "PKGNAME")
			fPkgInfo.SetName(value.c_str());
		else
		if (key == "AUTHORNAME")
			fPkgInfo.SetAuthorName(value.c_str());
		else
		if (key == "INSTALLFOLDER")
			fPkgInfo.SetPath(value.c_str());
		else
		if (key == "INSTALLCONSTFOLDER")
			fPkgInfo.SetPath(value.c_str());
		else
		if (key == "CONTACT")
			fPkgInfo.SetAuthorEmail(value.c_str());
		else
		if (key == "URL")
			fPkgInfo.SetAuthorURL(value.c_str());
		else
		if (key == "RELEASEDATE")
			fPkgInfo.SetReleaseDate(atol(value.c_str()));
		else
		if (key == "APPVERSION")
			fPkgInfo.SetAppVersion(value.c_str());
		
	}
	file.close();
	
	fFilePath.SetTo(ref);
	BString title("PSfx: ");
	title << fFilePath.GetFileName();
	SetTitle(title.String());
}


void
PkgWindow::SaveProject(const char *path)
{
	BString out;
	
	out << "PKGVERSION=" << fPkgInfo.GetPackageVersion() << "\n";
	
	DPath filePath(path);
	if (!fPkgInfo.GetName() || strlen(fPkgInfo.GetName()) < 1)
		fPkgInfo.SetName(filePath.GetBaseName());
	
	out	<< "PFXPROJECT=Always first\n"
		<< "PKGNAME=" << fPkgInfo.GetName()
		<< "\nTYPE=SelfExtract\n"
		<< "INSTALLFOLDER=" << fPkgInfo.GetPath().Path() << "\n";
	
	if (fPkgInfo.GetAuthorName() && strlen(fPkgInfo.GetAuthorName()) > 0)
		out << "AUTHORNAME=" << fPkgInfo.GetAuthorName() << "\n";
	if (fPkgInfo.GetAuthorEmail() && strlen(fPkgInfo.GetAuthorEmail()) > 0)
		out << "CONTACT=" << fPkgInfo.GetAuthorEmail() << "\n";
	if (fPkgInfo.GetAuthorURL() && strlen(fPkgInfo.GetAuthorURL()) > 0)
		out << "URL=" << fPkgInfo.GetAuthorURL() << "\n";
	out << "RELEASEDATE=" << fPkgInfo.GetReleaseDate() << "\n";
	
	out << "APPVERSION=";
	if (fPkgInfo.GetAppVersion() || strlen(fPkgInfo.GetAppVersion()) < 1)
		out << "0.0.1\n";
	else
		out << fPkgInfo.GetAppVersion() << "\n";
	
	
	for (int32 i = 0; i < fListView->CountItems(); i++)
	{
		FileListItem *fileListItem = dynamic_cast<FileListItem*>(fListView->ItemAt(i));
		if (!fileListItem)
			continue;
		
		FileItem *fileItem = fileListItem->GetData();
		
		if (!fileItem)
		{
			printf("Skipping NULL fileItem for fileListItem %s\n", fileListItem->Text());
			continue;
		}
		
		entry_ref ref = fileListItem->GetRef();
		BPath refPath(&ref);
		
		out << "ITEMFILEPATH=" << refPath.Path() << "\n";
		out << "ITEMNAME=" << fileItem->GetName() << "\n";
		if (fileItem->GetInstalledName() && strlen(fileItem->GetInstalledName()) > 0)
			out << "ITEMINSTALLEDNAME=" << fileItem->GetInstalledName() << "\n";
			
		if (fileItem->GetReplaceMode() > 0)
			out << "ITEMREPLACEMODE=" << fileItem->GetReplaceMode() << "\n";
				
		if (strcmp(fileItem->GetPath().Path(), "M_INSTALL_DIRECTORY") != 0)
			 out << "ITEMPATH=" << fileItem->GetPath().Path() << "\n";
		
		for (int32 i = 0; i < fileItem->CountGroups(); i++)
			out << "ITEMGROUP=" << fileItem->GroupAt(i) << "\n";
		
		for (int32 i = 0; i < fileItem->CountPlatforms(); i++)
			out << "ITEMPLATFORM=" << (int32) fileItem->PlatformAt(i) << "\n";
		
		for (int32 i = 0; i < fileItem->CountLinks(); i++)
			out << "ITEMLINK=" << fileItem->LinkAt(i) << "\n";
		
		if (fileItem->CountLinks() > 0)
			out << "ITEMCATEGORY=" << fileItem->GetCategory() << "\n";
	}
	
	BFile file(path, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	file.Write(out.String(), out.Length());

	BNodeInfo nodeInfo(&file);
	nodeInfo.SetType(PFX_MIME_TYPE);
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
			{
				OSPath osPath;
				item->GetData()->SetPath(osPath.DirToString(value));
			}
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
	
	SaveProject(fFilePath.GetFullPath());
	
	if (fQuitAfterSave)
		PostMessage(B_QUIT_REQUESTED);
	
	if (fBuildAfterSave)
	{
		fBuildAfterSave = false;
		Build(fBuildType);
	}
}


void
PkgWindow::Build(ostype_t platform)
{
	BString pkgPath;
	pkgPath << fFilePath.GetFolder() << "/" << fFilePath.GetBaseName() << ".sfx";
	
	BuildPackage(fFilePath.GetFullPath(), pkgPath.String(),
					OSTypeToString(platform).String());
}

