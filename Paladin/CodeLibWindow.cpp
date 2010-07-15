#include "CodeLibWindow.h"

#include <Alert.h>
#include <OS.h>
#include <MenuField.h>
#include <Path.h>
#include <Roster.h>
#include <ScrollView.h>
#include <StringView.h>
#include <stdio.h>
#include <TextView.h>

#include "DListView.h"
#include "DPath.h"
#include "Globals.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"
#include "SourceFile.h"
#include "StringInputWindow.h"

enum
{
	M_MODULE_SELECTED = 'slmd',
	M_SHOW_CREATE_MODULE = 'shcm',
	M_CREATE_MODULE = 'crmd',
	M_DELETE_MODULE = 'dlmd',
	M_EXPORT_TO_PROJECT = 'expj',
	M_SHOW_ADD_FILES = 'shad',
	M_SHOW_ADD_PROJECT_FILES = 'shap',
	M_SHOW_FILES = 'shfl',
	M_BACKUP_LIBRARY = 'bklb',
	M_HIDE_ADD_FILES = 'hiad',
	M_DESCRIPTION_CHANGED = 'dsch',
	M_PROJECT_SELECTED = 'pjsl'
};

static CodeLibWindow *sCodeLibWindowInstance = NULL;

class AddFileView : public BView
{
public:
					AddFileView(Project *proj, BRect frame, const char *name, int32 resize,
								int32 flags);
		void		MessageReceived(BMessage *msg);
		void		AttachedToWindow(void);
		void		MakeFocus(bool value=true);
		
		Project		*fProject;
private:
		RefListView	*fFileList;
		BButton		*fAddButton;
};

CodeLibWindow *
CodeLibWindow::GetInstance(BRect frame)
{
	if (!sCodeLibWindowInstance)
		sCodeLibWindowInstance = new CodeLibWindow(frame);
	
	return sCodeLibWindowInstance;
}


CodeLibWindow::CodeLibWindow(BRect frame)
	:	DWindow(frame,TR("Code Library"), B_TITLED_WINDOW),
		fCurrentModule(NULL),
		fCurrentProject(NULL),
		fFilePanel(NULL)
{
	SetSizeLimits(400,30000,250,30000);
	BView *top = GetBackgroundView();
	
	fMainView = new BView(Bounds(),"mainview",B_FOLLOW_ALL,B_WILL_DRAW);
	top->AddChild(fMainView);
	fMainView->SetViewColor(top->ViewColor());
	
	BRect r(Bounds());
	r.bottom = 20;
	fBar = new BMenuBar(r,"bar");
	fMainView->AddChild(fBar);
	
	SetupMenus();
	
	r.left = 10.0;
	r.top = fBar->Frame().bottom + 5.0, 
	r.bottom = r.top + 25.0;
	if (gProjectList->CountItems() > 1)
	{
		r.right = Bounds().right - 10.0;
		fProjectMenu = new BMenu(TR("Projects"));
		fProjectMenu->SetLabelFromMarked(true);
		
		BMenuField *field = new BMenuField(r,"projectfield",TR("Selected Project:"),fProjectMenu);
		
		float pw,ph;
		field->GetPreferredSize(&pw,&ph);
		field->ResizeTo(r.Width(),ph);
		field->SetDivider(field->StringWidth(TR("Selected Project:")) + 10.0);
		fMainView->AddChild(field);
		
		gProjectList->Lock();
		for (int32 i = 0; i < gProjectList->CountItems(); i++)
		{
			Project *proj = gProjectList->ItemAt(i);
			BMessage *msg = new BMessage(M_PROJECT_SELECTED);
			msg->AddPointer("project",gProjectList->ItemAt(i));
			fProjectMenu->AddItem(new BMenuItem(proj->GetName(),msg));
		}
		gProjectList->Unlock();
		
		fProjectMenu->ItemAt(0L)->SetMarked(true);
		r = field->Frame();
		r.OffsetBy(0,r.Height() + 5.0);
	}
	fCurrentProject = gProjectList->ItemAt(0L);
	r.right = r.left + 25.0;
	
	
	BStringView *modLabel = new BStringView(BRect(0,0,1,1),"modlabel",TR("Code Modules:"));
	modLabel->SetFont(be_bold_font);
	modLabel->ResizeToPreferred();
	modLabel->MoveTo(10.0,r.top);
	fMainView->AddChild(modLabel);
	
	r = Bounds().InsetByCopy(10,10);
	fDescription = new AutoTextControl(r,"description",TR("Description:"),"",
										new BMessage(M_DESCRIPTION_CHANGED),
										B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	float pw, ph;
	fDescription->GetPreferredSize(&pw, &ph);
	fDescription->ResizeTo(r.Width(),ph);
	fDescription->MoveTo(10.0,Bounds().bottom - 10.0 - ph);
	fDescription->SetDivider(fDescription->StringWidth(fDescription->Label()));
	fMainView->AddChild(fDescription);
	
	r.top = modLabel->Frame().bottom + 5.0;
	r.left = 10.0;
	r.right = (Bounds().Width() / 2.0) - 10.0 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = fDescription->Frame().top - 10.0;
	fModList = new DListView(r,"modlist",B_SINGLE_SELECTION_LIST,
							B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);
	fModList->SetSelectionMessage(new BMessage(M_MODULE_SELECTED));
	fModList->SetInvocationMessage(new BMessage(M_EXPORT_TO_PROJECT));
	fModList->SetAcceptDrops(false);
	
	BScrollView *sv = fModList->MakeScrollView("modscroll",false,true);
	fMainView->AddChild(sv);
	
	
	r.OffsetBy(r.Width() + 20.0 + B_V_SCROLL_BAR_WIDTH,0);
	fFileList = new RefListView(r,"filelist", B_MULTIPLE_SELECTION_LIST,
							B_FOLLOW_ALL);
	fFileList->SetDefaultDisplayMode(REFITEM_NAME);
	fFileList->SetInvocationMessage(new BMessage(M_SHOW_FILES));
	fFileList->SetDropMessage(new BMessage(M_ADD_FILES));
	
	sv = fFileList->MakeScrollView("filescroll",false,true);
	fMainView->AddChild(sv);
	
	BStringView *fileLabel = new BStringView(BRect(0,0,1,1),"filelabel",TR("Files in Module:"));
	fileLabel->SetFont(be_bold_font);
	fileLabel->ResizeToPreferred();
	fileLabel->MoveTo(r.left,modLabel->Frame().top);
	fMainView->AddChild(fileLabel);
	
	ScanModules();
	fModList->MakeFocus(true);
	if (fModList->CountItems() > 0)
		fModList->Select(0L);
	
	SetupContextMenus();
	
	fAddView = new AddFileView(fCurrentProject, Bounds(), "addview", B_FOLLOW_ALL, B_WILL_DRAW);
	top->AddChild(fAddView);
	fAddView->Hide();
	
	gProjectList->Lock();
	fCurrentProject = gProjectList->ItemAt(0L);
	gProjectList->Unlock();
}


CodeLibWindow::~CodeLibWindow(void)
{
	if (fCurrentModule)
		fCurrentModule->SaveInfo();

	if (sCodeLibWindowInstance)
		sCodeLibWindowInstance = NULL;
}


void
CodeLibWindow::MessageReceived(BMessage *msg)
{
	CodeModule *mod = NULL;
	int32 index = 0;
	int32 sel = 0;
	BStringItem *item = NULL;
	
	switch (msg->what)
	{
		case M_MODULE_SELECTED:
		{
			if (fCurrentModule)
				fCurrentModule->SaveInfo();
			int32 sel = fModList->CurrentSelection();
			if (sel < 0)
				ShowModule(NULL);
			item = (BStringItem*)fModList->ItemAt(sel);
			if (item)
				ShowModule(item->Text());
			
			break;
		}
		case M_SHOW_CREATE_MODULE:
		{
			StringInputWindow *inwin = new StringInputWindow("Create Module",
														TR("Choose the name for the new module:"),
														BMessage(M_CREATE_MODULE),
														BMessenger(NULL,this));
			inwin->Show();
			break;
		}
		case M_CREATE_MODULE:
		{
			BString name;
			if (msg->FindString("string",&name) == B_OK)
			{
				if (gCodeLib.CreateModule(name.String()) == B_OK)
				{
					int32 properindex = fModList->AddItemSorted(new BStringItem(name.String()));
					mod = gCodeLib.ModuleAt(gCodeLib.CountModules() - 1);
					gCodeLib.RemoveModule(mod);
					gCodeLib.AddModule(mod,properindex);
					fCurrentModule = mod;
					
					if (fModList->CountItems() == 1)
						fModList->Select(0L);
				}
			}
			break;
		}
		case M_DELETE_MODULE:
		{
			sel = fModList->CurrentSelection();
			if (sel < 0)
				break;
			item = (BStringItem*)fModList->ItemAt(sel);
			if (item)
			{
				BString alertmsg = TR("This cannot be undone. Delete module?");
				BAlert *alert = new BAlert("ModMan",alertmsg.String(),TR("Cancel"),TR("Delete"));
				if (alert->Go())
				{
					gCodeLib.DeleteModule(item->Text());
					fCurrentModule = NULL;
					ScanModules();
				}
			}
			break;
		}
		case M_EXPORT_TO_PROJECT:
		{
			if (!fCurrentModule)
				break;
			
			ExportModule();
			break;
		}
		case M_SHOW_ADD_FILES:
		{
			if (!fCurrentModule)
				break;
			
			if (!fFilePanel)
			{
				BMessenger msgr(NULL,this);
				
				entry_ref openref;
				BEntry entry(fCurrentProject->GetPath().GetFolder());
				entry.GetRef(&openref);
				
				fFilePanel = new BFilePanel(B_OPEN_PANEL, &msgr, &openref, 0, true,
											new BMessage(M_ADD_FILES));
			}
			fFilePanel->Show();
			break;
		}
		case M_SHOW_ADD_PROJECT_FILES:
		{
			if (!fCurrentModule)
				break;
			
			fMainView->Hide();
			fAddView->Show();
			fAddView->MakeFocus();
			break;
		}
		case M_ADD_FILES:
		{
			if (!fCurrentModule)
				break;
			
			entry_ref ref;
			int32 index = 0;
			while (msg->FindRef("refs",index++,&ref) == B_OK)
				fCurrentModule->AddFile(BPath(&ref).Path());
			ShowModule(fCurrentModule->GetName());
			break;
		}
		case M_REMOVE_FILES:
		{
			if (!fCurrentModule)
				break;
			
			sel = fFileList->CurrentSelection(index++);
			item = (BStringItem*)fModList->ItemAt(fModList->CurrentSelection());
			if (sel < 0 || !item)
				break;
			
			mod = gCodeLib.FindModule(item->Text());
			
			if (!mod)
				break;
			
			BAlert *alert = new BAlert("ModMan",
									TR("These files (except libraries) will be permanently deleted. Remove them?"),
									TR("Cancel"),TR("Remove"));
			if (!alert->Go())
				break;
			
			for (int32 i = fFileList->CountItems() - 1; i >= 0; i--)
			{
				item = (BStringItem*)fFileList->ItemAt(sel);
				if (!item || !item->IsSelected())
					continue;
				
				RefListItem *refitem = dynamic_cast<RefListItem*>(fFileList->ItemAt(sel));
				if (refitem)
				{
					mod->RemoveFile(refitem->GetRef().name);
					fFileList->RemoveItem(refitem);
					delete refitem;
				}
				else
				{
					// This entry is a library, so we'll just remove it and be done with things
					mod->RemoveLibrary(item->Text());
					fFileList->RemoveItem(item);
					delete item;
					
					// We have to update the MODINFO any time we change any library entries
					mod->SaveInfo();
				}
			}
			break;
		}
		case M_SHOW_FILES:
		{
			sel = fFileList->CurrentSelection(index++);
			while (sel >= 0)
			{
				RefListItem *refitem = (RefListItem*)fFileList->ItemAt(sel);
				entry_ref ref = refitem->GetRef();
				be_roster->Launch(&ref);
				sel = fFileList->CurrentSelection(index++);
			}
			break;
		}
		case M_HIDE_ADD_FILES:
		{
			fAddView->Hide();
			fMainView->Show();
			break;
		}
		case M_DESCRIPTION_CHANGED:
		{
			if (!fCurrentModule)
				break;
			
			fCurrentModule->SetDescription(fDescription->Text());
//			fCurrentModule->SaveInfo();
			break;
		}
		case M_PROJECT_SELECTED:
		{
			Project *proj = NULL;
			if (msg->FindPointer("project",(void**)&proj) == B_OK)
				fCurrentProject = proj;
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}


void
CodeLibWindow::SetupMenus(void)
{
	BMenu *menu = new BMenu(TR("Actions"));
	menu->AddItem(new BMenuItem(TR("Create New Module…"),new BMessage(M_SHOW_CREATE_MODULE),'N'));
	menu->AddItem(new BMenuItem(TR("Delete Current Module"),new BMessage(M_DELETE_MODULE),'D'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(TR("Add Files to Module…"),new BMessage(M_SHOW_ADD_FILES),
								'A',B_COMMAND_KEY | B_SHIFT_KEY));
	menu->AddItem(new BMenuItem(TR("Add Project Files to Module…"),
								new BMessage(M_SHOW_ADD_PROJECT_FILES),'A'));
	menu->AddItem(new BMenuItem(TR("Remove Files from Module"),new BMessage(M_REMOVE_FILES)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(TR("Export Module to Project"),new BMessage(M_EXPORT_TO_PROJECT),'E'));
	fBar->AddItem(menu);
}


void
CodeLibWindow::SetupContextMenus(void)
{
	BPopUpMenu *menu = new BPopUpMenu("Modules",false,false);
	menu->AddItem(new BMenuItem("Create New Module…",new BMessage(M_SHOW_CREATE_MODULE)));
	menu->AddItem(new BMenuItem("Delete Current Module",new BMessage(M_DELETE_MODULE)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Export Module to Project",new BMessage(M_EXPORT_TO_PROJECT)));
	menu->SetTargetForItems(this);
	fModList->SetContextMenu(menu);
	
	menu = new BPopUpMenu("Module Files",false,false);
	menu->AddItem(new BMenuItem("Add Files to Module…",new BMessage(M_SHOW_ADD_FILES)));
	menu->AddItem(new BMenuItem("Add Project Files to Module…",
								new BMessage(M_SHOW_ADD_PROJECT_FILES)));
	menu->AddItem(new BMenuItem("Remove Files from Module",new BMessage(M_REMOVE_FILES)));
	menu->SetTargetForItems(this);
	fFileList->SetContextMenu(menu);
}


void
CodeLibWindow::ScanModules(void)
{
	for (int32 i = fFileList->CountItems() - 1; i >= 0; i--)
	{
		BStringItem *item = (BStringItem*)fFileList->RemoveItem(i);
		delete item;
	}
	
	for (int32 i = fModList->CountItems() - 1; i >= 0; i--)
	{
		BStringItem *item = (BStringItem*)fModList->RemoveItem(i);
		delete item;
	}
	
	gCodeLib.ScanFolders();
	for (int32 i = 0; i < gCodeLib.CountModules(); i++)
	{
		CodeModule *mod = gCodeLib.ModuleAt(i);
		fModList->AddItem(new BStringItem(mod->GetName()));
	}
}


void
CodeLibWindow::ShowModule(const char *name)
{
	for (int32 i = fFileList->CountItems() - 1; i >= 0; i--)
	{
		BStringItem *item = (BStringItem*)fFileList->RemoveItem(i);
		delete item;
	}
	fDescription->SetText("");
	
	fCurrentModule = gCodeLib.FindModule(name);
	if (!fCurrentModule)
		return;
	
	for (int32 i = 0; i < fCurrentModule->CountFiles(); i++)
	{
		ModFile *file = fCurrentModule->FileAt(i);
		BEntry entry(file->path.GetFullPath());
		entry_ref ref;
		entry.GetRef(&ref);
		fFileList->AddItem(new RefListItem(ref));
	}
	
	for (int32 i = 0; i < fCurrentModule->CountFiles(); i++)
		fFileList->AddItem(new BStringItem(fCurrentModule->LibraryAt(i)));
	
	if (fCurrentModule->GetDescription())
		fDescription->SetText(fCurrentModule->GetDescription());
	else
		fDescription->SetText("");
}


void
CodeLibWindow::ExportModule(void)
{
	BWindow *win = WindowForProject(fCurrentProject);
	if (!win)
		return;
	
	DPath exportFolder(fCurrentProject->GetPath().GetFolder());
	exportFolder << "ProjectModules";
	gCodeLib.ExportModule(fCurrentModule->GetName(),exportFolder.GetFullPath());
	
	BDirectory dir(exportFolder.GetFullPath());
	entry_ref ref;
	BMessage msg(M_ADD_FILES);
	while (dir.GetNextRef(&ref) == B_OK)
	{
		DPath refPath(ref);
		if (refPath.GetExtension() && strcmp(refPath.GetExtension(),"h") == 0)
			continue;
		
		msg.AddRef("refs",&ref);
	}
	win->PostMessage(&msg);
}


AddFileView::AddFileView(Project *proj, BRect frame, const char *name, int32 resize,
						int32 flags)
	:	BView(frame,name,resize,flags),
		fProject(proj)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	BRect r = Bounds().InsetByCopy(10,10);
	
	r.bottom = r.top + 10;
	BRect textRect = r.OffsetToCopy(0,0);
	BTextView *textView = new BTextView(r,"paneltext",textRect,B_FOLLOW_LEFT | B_FOLLOW_TOP);
	AddChild(textView);
	textView->MakeEditable(false);
	textView->SetViewColor(ViewColor());
	
	BString labelmsg(TR("Choose the project files you wish to import. "));
	labelmsg << TR("You can select multiple files by clicking on items while holding the XXXXX.");
	
	if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4)
		labelmsg.ReplaceFirst("XXXXX",TR("Command key (usually Alt)."));
	else if (gPlatform == PLATFORM_ZETA)
		labelmsg.ReplaceFirst("XXXXX",TR("Control key."));
	else
		labelmsg.ReplaceFirst("XXXXX",TR("Shift key"));
	textView->SetText(labelmsg.String());
	textView->ResizeTo(r.Width(), 20.0 + textView->TextHeight(0,textView->TextLength()));
	
	r = textView->Frame();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	// Init to wrong label so that it gets resized to match the Cancel button
	fAddButton = new BButton(BRect(0,0,1,1),"addbutton",TR("Cancel"),new BMessage(M_ADD_FILES),
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	fAddButton->ResizeToPreferred();
	fAddButton->SetLabel(TR("Add"));
	fAddButton->MoveTo(Bounds().right - 10.0 - fAddButton->Frame().Width(),
				Bounds().bottom - 10.0 - fAddButton->Frame().Height());
	fAddButton->SetTarget(this);
	
	BButton *cancel = new BButton(BRect(0,0,1,1),"cancelbutton","Cancel",
								new BMessage(M_HIDE_ADD_FILES),
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	cancel->ResizeToPreferred();
	cancel->MoveTo(fAddButton->Frame().left - 10.0 - cancel->Frame().Width(), fAddButton->Frame().top);
	
	r.OffsetBy(0,r.Height());
	r.bottom = fAddButton->Frame().top - 15.0;
	fFileList = new RefListView(r,"filelist",B_MULTIPLE_SELECTION_LIST, B_FOLLOW_ALL);
	BScrollView *sv = fFileList->MakeScrollView("filescroll",false,true);
	AddChild(sv);
	fFileList->SetDefaultDisplayMode(REFITEM_NAME);
	fFileList->SetInvocationMessage(new BMessage(M_ADD_FILES));
	
	AddChild(cancel);
	AddChild(fAddButton);
	fAddButton->MakeDefault(true);
	
	
	for (int32 i = 0; i < fProject->CountGroups(); i++)
	{
		SourceGroup *group = fProject->GroupAt(i);
		for (int32 j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			BEntry entry(file->GetPath().GetFullPath());
			entry_ref ref;
			entry.GetRef(&ref);
			fFileList->AddItem(new RefListItem(ref,REFITEM_NAME));
		}
	}
}


void
AddFileView::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_ADD_FILES:
		{
			int32 index = 0;
			int32 selection = fFileList->CurrentSelection(index++);
			while (selection >= 0)
			{
				RefListItem *item = (RefListItem*)fFileList->ItemAt(selection);
				entry_ref ref = item->GetRef();
				msg->AddRef("refs",&ref);
				selection = fFileList->CurrentSelection(index++);
			}
			Window()->PostMessage(M_HIDE_ADD_FILES);
			Window()->PostMessage(msg);
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}


void
AddFileView::AttachedToWindow(void)
{
	fAddButton->SetTarget(this);
	fFileList->SetTarget(this);
}


void
AddFileView::MakeFocus(bool value)
{
	if (value)
	{
		fFileList->MakeFocus();
		fFileList->Select(0L);
	}
}


