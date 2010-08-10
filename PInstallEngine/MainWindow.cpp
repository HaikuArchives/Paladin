#include "MainWindow.h"
#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <Menu.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Path.h>
#include <VolumeRoster.h>
#include <Volume.h>

#include "BitmapMenuItem.h"
#include "Globals.h"
#include "InstallWindow.h"

PackageInfo	gPkgInfo;

enum
{
	M_SET_VOLUME = 'stvl',
	M_SET_GROUP = 'stgr',
	M_SHOW_SET_FOLDER = 'shfl',
	M_SET_FOLDER = 'stfl',
	M_START_INSTALL,
	M_SET_CUSTOM = 'stcs',
	M_TOGGLE_LINKS_TARGETING = 'tglt'
};

MainWindow::MainWindow(void)
	:	DWindow(BRect(100,100,500,400),"Package Installer",
				B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
		fVolumeField(NULL),
		fFolderField(NULL),
		fGroupField(NULL),
		fCustomItem(NULL),
		fLastFolderItem(NULL),
		fFilePanel(NULL)
{
	MakeCenteredOnShow(true);
	
	gPkgInfo.LoadFromResources();
	
	BRect r(Bounds());
	r.bottom = 200;
	BView *top = GetBackgroundView();
	fText = new BTextView(r,"textview",r.InsetByCopy(10,10),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fText);
	fText->SetViewColor(top->ViewColor());
	fText->MakeEditable(false);
	fText->SetStylable(true);
	
	BString text;
	text << gPkgInfo.GetName() << " "
		<< gPkgInfo.GetAppVersion() << "\n";
	
	fText->SetFontAndColor(be_bold_font);	
	fText->Insert(text.String());
	
	text = "";
	text << gPkgInfo.GetPrettyReleaseDate("%Ex") << "\n";
	
	fText->SetFontAndColor(be_plain_font);	
	fText->Insert(text.String());
	
	text = "Welcome to the installer for ";
	text << gPkgInfo.GetName() << ". To begin installation, click the \"Begin\" button.\n";
	
	if (gPkgInfo.GetAuthorEmail() && strlen(gPkgInfo.GetAuthorEmail()) > 0)
		text << "\nFor assistance with installation, contact the author at "
			<< gPkgInfo.GetAuthorEmail() << ".";
	fText->Insert(text.String());
	
	int32 lineCount = fText->CountLines();
	float totalTextHeight = 0.0;
	for (int32 i = 0; i < lineCount; i++)
		totalTextHeight += fText->LineHeight(i);
	fText->ResizeTo(fText->Bounds().Width(), totalTextHeight + 20.0);
	
	r.left = 10.0;
	r.top = fText->Frame().bottom + 10.0;
	r.right = Bounds().right - 10.0;
	r.bottom = r.top + 25;
	InitGroupField(r);
	if (fGroupField)
	{
		top->AddChild(fGroupField);
		r.bottom = fGroupField->Frame().bottom;
		r.OffsetBy(0,r.Height() + 10.0);
	}
	
	r.right = Bounds().right / 3.0;
	r.bottom = r.top + 25.0;
	InitVolumeField(r);
	if (fVolumeField)
	{
		top->AddChild(fVolumeField);
		r.bottom = fVolumeField->Frame().bottom;
		r.right = fVolumeField->Frame().right;
		r.OffsetBy(0, r.Height() + 10.0);
	}
	
	InitFolderField(r);
	if (fFolderField)
	{
		top->AddChild(fFolderField);
		r = fFolderField->Frame();
	}
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fLinksOnTargetVolume = new BCheckBox(r, "linksontarget", "Create Links on Target Volume",
										new BMessage(M_TOGGLE_LINKS_TARGETING));
	fLinksOnTargetVolume->ResizeToPreferred();
	top->AddChild(fLinksOnTargetVolume);
	fLinksOnTargetVolume->SetEnabled(false);
	
	BButton *begin = new BButton(BRect(0,0,1,1),"begin","Begin",new BMessage(M_START_INSTALL));
	begin->ResizeToPreferred();
	
	begin->MoveTo(Bounds().right - 10.0 - begin->Frame().Width(),
				fLinksOnTargetVolume->Frame().top);
	top->AddChild(begin);
	begin->MakeDefault(true);
	
	ResizeTo(Frame().Width(), begin->Frame().bottom + 10.0);
}


MainWindow::~MainWindow(void)
{
	delete fFilePanel;
}


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_START_INSTALL:
		{
			InstallWindow *win = new InstallWindow();
			win->Show();
			Hide();
			break;
		}
		case M_SET_VOLUME:
		{
			dev_t dev;
			if (msg->FindInt32("device",(int32*)&dev) == B_OK)
			{
				gNonBootInstall = (dev != gBootVolumeID);
				gPkgInfo.SetInstallVolume(dev);
				
				fLinksOnTargetVolume->SetEnabled(gNonBootInstall);
			}
			break;
		}
		case M_SET_FOLDER:
		{
			fLastFolderItem = (BitmapMenuItem*)(fFolderField->Menu()->FindMarked());
			BString str;
			if (msg->FindString("which", &str) == B_OK)
			{
				if (str == "apps")
					gPkgInfo.SetInstallPath(B_APPS_DIRECTORY);
				else if (str == "home")
					gPkgInfo.SetInstallPath(B_USER_DIRECTORY);
			}
			break;
		}
		case M_SET_CUSTOM:
		{
			fLastFolderItem = fCustomItem;
			entry_ref ref;
			if (msg->FindRef("refs",&ref) != B_OK)
				break;
			
			BPath path(&ref);
			BMessage *itemmsg = new BMessage(M_SHOW_SET_FOLDER);
			itemmsg->AddString("where",path.Path());
			fCustomItem->SetMessage(itemmsg);
			fCustomItem->SetLabel(path.Leaf());
			
			// This is to make sure the menu field label is updated properly
			fCustomItem->SetMarked(true);
			
			gPkgInfo.SetInstallPath(path.Path());
			break;
		}
		case M_TOGGLE_LINKS_TARGETING:
		{
			gLinksOnTargetVolume = (fLinksOnTargetVolume->Value() == B_CONTROL_ON);
			break;
		}	
		case M_SHOW_SET_FOLDER:
		{
			if (!fFilePanel)
			{
				BMessenger msgr(NULL,this);
				fFilePanel = new BFilePanel(B_OPEN_PANEL,&msgr,NULL,
											B_DIRECTORY_NODE,false,new BMessage(M_SET_CUSTOM));
			}
			fFilePanel->Show();
			break;
		}
		case B_CANCEL:
		{
			fLastFolderItem->SetMarked(true);
			break;
		}
		case M_SET_GROUP:
		{
			BMenu *groupmenu = fGroupField->Menu();
			if (groupmenu->FindMarked())
				gPkgInfo.SetGroup(groupmenu->FindMarked()->Label());
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
MainWindow::InitVolumeField(BRect frame)
{
	BVolumeRoster roster;
	BVolume bootVol, vol;
	roster.GetBootVolume(&bootVol);
	
	BMenu *menu = new BMenu("Disk");
	
	BBitmap *icon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
	
	char volName[B_FILE_NAME_LENGTH];
	bootVol.GetName(volName);
	
	BMessage *menumsg = new BMessage(M_SET_VOLUME);
	menumsg->AddInt32("device",bootVol.Device());
	
	BitmapMenuItem *item = NULL;
	if (bootVol.GetIcon(icon,B_MINI_ICON) != B_OK)
	{
		delete icon;
		icon = NULL;
	}
	item = new BitmapMenuItem(volName,menumsg,icon);
	menu->AddItem(item);
	item->SetMarked(true);
	
	// Get the other volumes besides the boot volume
	while (roster.GetNextVolume(&vol) == B_OK)
	{
		if (vol.InitCheck() != B_OK ||
			vol.Device() == bootVol.Device() ||
			vol.IsReadOnly() ||
			!vol.KnowsMime() ||
			!vol.KnowsAttr() ||
			!vol.KnowsQuery() ||
			vol.GetName(volName) != B_OK)
			continue;
		
		icon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
		BMessage *menumsg = new BMessage(M_SET_VOLUME);
		menumsg->AddInt32("device",vol.Device());
		
		if (bootVol.GetIcon(icon,B_MINI_ICON) != B_OK)
		{
			delete icon;
			icon = NULL;
		}
		menu->AddItem(new BitmapMenuItem(volName,menumsg,icon));
	}
		
	menu->SetLabelFromMarked(true);
	fVolumeField = new BMenuField(frame,"disks", "Disk", menu);
	fVolumeField->SetDivider(fVolumeField->StringWidth("Install Type") + 5.0);
	fVolumeField->ResizeToPreferred();
}


void
MainWindow::InitFolderField(BRect frame)
{
	if (!gPkgInfo.GetShowChooser())
		return;
		
	BMenu *menu = new BMenu("Folder");
	
	BPath path;
	find_directory(B_APPS_DIRECTORY,&path);
	BNode node(path.Path());
	BNodeInfo nodeInfo(&node);
	BBitmap *icon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
	nodeInfo.GetTrackerIcon(icon,B_MINI_ICON);
	
	BMessage *menumsg = new BMessage(M_SET_FOLDER);
	menumsg->AddString("where",path.Path());
	menumsg->AddString("which","apps");
	menu->AddItem(new BitmapMenuItem("Programs Folder",menumsg,icon));

	find_directory(B_USER_DIRECTORY,&path);
	
	node.SetTo(path.Path());
	nodeInfo.SetTo(&node);
	icon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
	nodeInfo.GetTrackerIcon(icon,B_MINI_ICON);
	
	menumsg = new BMessage(M_SET_FOLDER);
	menumsg->AddString("where",path.Path());
	menumsg->AddString("which","home");
	menu->AddItem(new BitmapMenuItem("Home Folder",menumsg,icon));

	fCustomItem = new BitmapMenuItem("Other Folder…",new BMessage(M_SHOW_SET_FOLDER),NULL);
	menu->AddItem(fCustomItem);
	
	fLastFolderItem = (BitmapMenuItem*)menu->ItemAt(0L);
	fLastFolderItem->SetMarked(true);
	menu->SetLabelFromMarked(true);
	
	fFolderField = new BMenuField(frame,"folder", "Folder", menu);
	fFolderField->SetDivider(fFolderField->StringWidth("Install Type") + 5.0);
	fFolderField->ResizeToPreferred();
	
	// This is the default install folder. The ability to choose the install path is available
	// to the user, so now we choose a sensible default so that if a user just hits 'Begin' --
	// pretty likely for a novice user -- the package is installed into a sensible location
	gPkgInfo.SetInstallPath(B_APPS_DIRECTORY);
}


void
MainWindow::InitGroupField(BRect frame)
{
	if (gPkgInfo.CountGroups() < 2)
		return;
	
	BMenu *menu = new BMenu("Group");
	
	for (int32 i = 0; i < gPkgInfo.CountGroups(); i++)
	{
		if (i == 1)
			menu->AddSeparatorItem();
		BMessage *menumsg = new BMessage(M_SET_GROUP);
		menumsg->AddString("name",gPkgInfo.GroupAt(i));
		menu->AddItem(new BMenuItem(gPkgInfo.GroupAt(i),menumsg));
	}
	
	menu->ItemAt(0L)->SetMarked(true);
	menu->SetLabelFromMarked(true);
	
	fGroupField = new BMenuField(frame,"groups", "Install Type", menu);
	fGroupField->SetDivider(fGroupField->StringWidth("Install Type") + 5.0);
	fGroupField->ResizeToPreferred();
}

