#include "ProjectSettingsWindow.h"

#include <Box.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <Path.h>
#include <ScrollView.h>
#include <StringView.h>

#include "DListView.h"
#include "EscapeCancelFilter.h"
#include "Globals.h"
#include "PLocale.h"
#include "Project.h"
#include "TypedRefFilter.h"

enum
{
	M_TOGGLE_DEBUG = 'tgdb',
	M_TOGGLE_PROFILE = 'tgpf',
	M_TOGGLE_OPSIZE = 'tgsi',
	M_SET_OP_VALUE = 'sopv',
	M_SET_TARGET_TYPE = 'stgt',
	M_TARGET_NAME_CHANGED = 'tgnc',
	M_CCOPTS_CHANGED = 'ccoc',
	M_LDOPTS_CHANGED = 'ldoc',
	M_SHOW_ADD_PATH = 'shap',
	M_DROP_PATH = 'drpt',
	M_ADD_PATH = 'adpt',
	M_REMOVE_PATH = 'rmpt'
};

class IncludeList : public RefListView
{
public:
			IncludeList(BRect frame, const char *projectPath);
	void	RefDropped(entry_ref ref);

private:
	BString	fProjectPath;
};

ProjectSettingsWindow::ProjectSettingsWindow(BRect frame, Project *proj)
	:	DWindow(frame,TR("Project Settings"),B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE | B_NOT_H_RESIZABLE),
		fProject(proj),
		fDirty(false)
	
{
	if (!fProject)
		debugger("Bad project given to Project Settings window");
	
	fRefFilter = new TypedRefFilter(NULL,B_DIRECTORY_NODE);
	
	BMessenger msgr(this);
	entry_ref projfolder_ref;
	BEntry(fProject->GetPath().GetFolder()).GetRef(&projfolder_ref);
	fFilePanel = new BFilePanel(B_OPEN_PANEL,&msgr,&projfolder_ref,B_DIRECTORY_NODE,
								true, new BMessage(M_ADD_PATH),fRefFilter);
	
	fAutolock = new BAutolock(fProject);
	
	AddCommonFilter(new EscapeCancelFilter());
	
	BView *top = GetBackgroundView();
	
	BRect r(Bounds());
	fTabView = new BTabView(r,"tabview");
	top->AddChild(fTabView);
	
	r.InsetBy(5,5);
	r.bottom -= fTabView->TabHeight();
	
	BRect bounds = r.OffsetToCopy(0,0);
	
	fGeneralView = new BView(r,TR("General"),B_FOLLOW_ALL,B_WILL_DRAW);
	fGeneralView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fTabView->AddTab(fGeneralView);
	
	r.right -= 10.0;
	fTargetText = new AutoTextControl(r,"targetname",TR("Target Name:"),
										fProject->GetTargetName(),
										new BMessage(M_TARGET_NAME_CHANGED),
										B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fGeneralView->AddChild(fTargetText);
	
	float pwidth, pheight;
	fTargetText->GetPreferredSize(&pwidth, &pheight);
	fTargetText->ResizeTo(r.Width(),pheight);
	r.bottom = r.top + pheight;
	fTargetText->SetDivider(fTargetText->StringWidth(TR("Target Name:")) + 5.0);
	
	r = fTargetText->Frame();
	r.OffsetBy(0,r.Height() + 10.0);
	
	BMenu *menu = new BMenu(TR("Target Type"));
	menu->AddItem(new BMenuItem(TR("Application"),new BMessage(M_SET_TARGET_TYPE)));
	menu->AddItem(new BMenuItem(TR("Shared Library"),new BMessage(M_SET_TARGET_TYPE)));
	menu->AddItem(new BMenuItem(TR("Static Library"),new BMessage(M_SET_TARGET_TYPE)));
	menu->AddItem(new BMenuItem(TR("Device Driver"),new BMessage(M_SET_TARGET_TYPE)));
	
	r.right = (bounds.right - 5.0) / 2.0;
	r.bottom = r.top + 25;
	fTypeField = new BMenuField(r,"type",TR("Target Type:"),menu);
	fGeneralView->AddChild(fTypeField);
	fTypeField->SetDivider(fTypeField->StringWidth(TR("Target Type:")) + 5.0);
	
	SetToolTip(fTypeField,TR("The kind of program you want to build"));
	
	menu->SetTargetForItems(this);
	menu->SetLabelFromMarked(true);
	
	BMenuItem *item = menu->ItemAt(fProject->TargetType());
	if (item)
		item->SetMarked(true);
	
	r.OffsetBy(0,r.Height() + 10.0);
	
	BStringView *label = new BStringView(r,"label",TR("Include Paths:"));
	label->ResizeToPreferred();
	fGeneralView->AddChild(label);
	
	r = label->Frame();
	r.OffsetBy(0,r.Height() + 5.0);
	
	// We create a button now so that the list expands to fill the entire window
	// while leaving space for the two buttons at the bottom. Note that we do not
	// actually add the button to the window until later to preserve proper
	// keyboard navigation order
	BButton *add = new BButton(BRect(0,0,1,1),"addbutton",TR("Add…"),
								new BMessage(M_SHOW_ADD_PATH),
								B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	add->ResizeToPreferred();
	add->MoveTo(5,fGeneralView->Bounds().bottom - 10.0 - add->Frame().Height());
	
	r.right = bounds.right - 10.0 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = add->Frame().top - 10.0 - B_H_SCROLL_BAR_HEIGHT;
	fIncludeList = new IncludeList(r,fProject->GetPath().GetFolder());
	BScrollView *scrollView = new BScrollView("scrollview",fIncludeList,
											B_FOLLOW_ALL,0, true, true);
	scrollView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fGeneralView->AddChild(scrollView);
	
	float width = 0.0;
	for (int32 i = 0; i < fProject->CountLocalIncludes(); i++)
	{
		BStringItem *item = new BStringItem(fProject->LocalIncludeAt(i).Relative().String());
		float strwidth = fIncludeList->StringWidth(item->Text());
		width = MAX(width, strwidth);
		fIncludeList->AddItem(item);
	}
	
	if (width > fIncludeList->Bounds().Width())
	{
		BScrollBar *hbar = scrollView->ScrollBar(B_HORIZONTAL);
		hbar->SetRange(0.0, width - fIncludeList->Bounds().Width());
	}
	
	SetToolTip(fIncludeList,TR("The folders you want Paladin to search for header files"));
	
	fGeneralView->AddChild(add);
	
	BButton *remove = new BButton(BRect(0,0,1,1),"removebutton",TR("Remove"),
								new BMessage(M_REMOVE_PATH), 
								B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	remove->ResizeToPreferred();
	remove->MoveTo(add->Frame().right + 10.0, add->Frame().top);
	fGeneralView->AddChild(remove);
	
	r = bounds;
	fBuildView = new BView(bounds.OffsetByCopy(5,5),TR("Build"),B_FOLLOW_ALL,B_WILL_DRAW);
	fBuildView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fTabView->AddTab(fBuildView);
	
	menu = new BMenu(TR("Optimization"));
	menu->AddItem(new BMenuItem(TR("None"),new BMessage(M_SET_OP_VALUE)));
	menu->AddItem(new BMenuItem(TR("Some"),new BMessage(M_SET_OP_VALUE)));
	menu->AddItem(new BMenuItem(TR("More"),new BMessage(M_SET_OP_VALUE)));
	menu->AddItem(new BMenuItem(TR("Full"),new BMessage(M_SET_OP_VALUE)));
	
	r.right = (bounds.right - 5.0) / 2.0;
	r.bottom = r.top + 25;
	fOpField = new BMenuField(r,"optimize",TR("Optimize"),menu);
	fBuildView->AddChild(fOpField);
	fOpField->SetDivider(fOpField->StringWidth(TR("Optimize")) + 5.0);
	
	SetToolTip(fOpField,TR("Compiler optimization level. Disabled when debugging info is checked."));
	
	menu->SetTargetForItems(this);
	menu->SetLabelFromMarked(true);
	
	item = menu->ItemAt(fProject->OpLevel());
	if (item)
		item->SetMarked(true);
	
	r.right = bounds.right - 10.0;
	r.OffsetTo(5, fOpField->Frame().bottom + 5);
	fOpSizeBox = new BCheckBox(r,"opsizebox",TR("Optimize for size over speed"),
								new BMessage(M_TOGGLE_OPSIZE));
	fOpSizeBox->ResizeToPreferred();
	fBuildView->AddChild(fOpSizeBox);
	r = fOpSizeBox->Frame();
	if (fProject->OpForSize())
		fOpSizeBox->SetValue(B_CONTROL_ON);
	
	if (fProject->Debug())
	{
		fOpField->SetEnabled(false);
		fOpSizeBox->SetEnabled(false);
	}
	
	r.OffsetBy(0, r.Height() + 10);
	fDebugBox = new BCheckBox(r,"debugbox",TR("Build debugging information"),
								new BMessage(M_TOGGLE_DEBUG));
	fDebugBox->ResizeToPreferred();
	if (fProject->Debug())
		fDebugBox->SetValue(B_CONTROL_ON);
	fBuildView->AddChild(fDebugBox);
	SetToolTip(fDebugBox,TR("Check this if you want to use your program in a debugger during development."
							"You'll want to rebuild your project after change this."));
	
	r.OffsetBy(0, r.Height());
	fProfileBox = new BCheckBox(r,"profilebox",TR("Build profiling information"),
								new BMessage(M_TOGGLE_PROFILE));
	fProfileBox->ResizeToPreferred();
	if (fProject->Profiling())
		fProfileBox->SetValue(B_CONTROL_ON);
	fBuildView->AddChild(fProfileBox);
	SetToolTip(fProfileBox,TR("Check this if you want to use your program "
								"with gprof or bprof for profiling."));
	
	
	r.OffsetBy(0, r.Height() + 10.0);
	r.right = bounds.right - 10.0;
	fCompileText = new AutoTextControl(r,"extracc","Extra Compiler Options:",
										fProject->ExtraCompilerOptions(),
										new BMessage(M_CCOPTS_CHANGED),
										B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fBuildView->AddChild(fCompileText);
	fCompileText->SetDivider(fCompileText->StringWidth(fCompileText->Label()) + 5.0);
	SetToolTip(fCompileText,TR("Extra GCC flags you wish included when each file is compiled."));
	
	r = fCompileText->Frame();
	r.OffsetBy(0,r.Height() + 10);
	fLinkText = new AutoTextControl(r,"extrald",TR("Extra Linker Options:"),
										fProject->ExtraLinkerOptions(),
										new BMessage(M_LDOPTS_CHANGED),
										B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fBuildView->AddChild(fLinkText);
	fLinkText->SetDivider(fCompileText->Divider());
	SetToolTip(fLinkText,TR("Extra GCC linker flags you wish included when your project is linked."));
	
	fTabView->Select(0L);
	
	fBuildView->ResizeTo(fGeneralView->Bounds().Width(),fGeneralView->Bounds().Height());

	fTargetText->MakeFocus(true);
}


ProjectSettingsWindow::~ProjectSettingsWindow(void)
{
	delete fFilePanel;
	delete fRefFilter;
}


void
ProjectSettingsWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_SHOW_ADD_PATH:
		{
			fFilePanel->Show();
			break;
		}
		case M_DROP_PATH:
		{
			BString path;
			if (msg->FindString("path",&path) == B_OK)
				fProject->AddLocalInclude(path.String());
			break;
		}
		case M_ADD_PATH:
		{
			entry_ref ref;
			int32 i = 0;
			while (msg->FindRef("refs",i++,&ref) == B_OK)
			{
				fDirty = true;
				AddInclude(ref);
			}
			break;
		}
		case M_REMOVE_PATH:
		{
			int32 selection = fIncludeList->CurrentSelection();
			if (selection < 0)
				break;
			
			fDirty = true;
			
			for (int32 i = fIncludeList->CountItems() - 1; i >= 0; i--)
			{
				BStringItem *item = (BStringItem*)fIncludeList->ItemAt(i);
				if (item->IsSelected())
				{
					fIncludeList->RemoveItem(item);
					fProject->RemoveLocalInclude(item->Text());
					delete item;
				}
			}
			break;
		}
		case M_TARGET_NAME_CHANGED:
		{
			if (fTargetText->Text() && strlen(fTargetText->Text()) > 0)
				fProject->SetTargetName(fTargetText->Text());
			fDirty = true;
		}
		case M_TOGGLE_DEBUG:
		{
			if (fDebugBox->Value() == B_CONTROL_ON)
			{
				fProject->SetDebug(true);
				fOpField->SetEnabled(false);
				fOpSizeBox->SetEnabled(false);
			}
			else
			{
				fProject->SetDebug(false);
				fOpField->SetEnabled(true);
				fOpSizeBox->SetEnabled(true);
			}
			fDirty = true;
			break;
		}
		case M_TOGGLE_PROFILE:
		{
			if (fProfileBox->Value() == B_CONTROL_ON)
				fProject->SetProfiling(true);
			else
				fProject->SetProfiling(false);
			fDirty = true;
			break;
		}
		case M_TOGGLE_OPSIZE:
		{
			if (fOpSizeBox->Value() == B_CONTROL_ON)
				fProject->SetOpForSize(true);
			else
				fProject->SetOpForSize(false);
			fDirty = true;
			break;
		}
		case M_SET_OP_VALUE:
		{
			BMenuItem *item = fOpField->Menu()->FindMarked();
			if (item)
				fProject->SetOpLevel(fOpField->Menu()->IndexOf(item));
			fDirty = true;
			break;
		}
		case M_SET_TARGET_TYPE:
		{
			BMenuItem *item = fTypeField->Menu()->FindMarked();
			if (item)
				fProject->SetTargetType(fTypeField->Menu()->IndexOf(item));
			fDirty = true;
			break;
		}
		case M_CCOPTS_CHANGED:
		{
			fProject->SetExtraCompilerOptions(fCompileText->Text());
			fDirty = true;
			break;
		}
		case M_LDOPTS_CHANGED:
		{
			fProject->SetExtraLinkerOptions(fLinkText->Text());
			fDirty = true;
			break;
		}
		default:
			DWindow::MessageReceived(msg);
	}
}


bool
ProjectSettingsWindow::QuitRequested(void)
{
	if (fDirty)
		fProject->Save();
	
	delete fAutolock;
	return true;
}


void
ProjectSettingsWindow::AddInclude(const entry_ref &ref)
{
	BString path(BPath(&ref).Path());
	
	// Make any subfolders of the project's parent folder into relative paths
	if (path[0] == '/')
	{
		BString projpath = fProject->GetPath().GetFolder();
		projpath << '/';
		if (path.FindFirst(projpath) == 0)
			path.RemoveFirst(projpath);
	}
	if (!fProject->HasLocalInclude(path.String()))
	{
		fProject->AddLocalInclude(path.String());
		fIncludeList->AddItem(new BStringItem(path.String()));
	}
}


IncludeList::IncludeList(BRect frame, const char *projectPath)
	:	RefListView(frame,"includelist", B_MULTIPLE_SELECTION_LIST, B_FOLLOW_ALL),
		fProjectPath(projectPath)
{
	if (projectPath && fProjectPath[fProjectPath.CountChars() - 1] != '/')
		fProjectPath << "/";
}


void
IncludeList::RefDropped(entry_ref ref)
{
	RefListItem *item = new RefListItem(ref,REFITEM_OTHER);
	BString text(BPath(&ref).Path());
	text.RemoveFirst(fProjectPath);
	AddItem(item);
	item->SetText(text.String());
	
	BMessenger msgr(Parent());
	BMessage msg(M_DROP_PATH);
	msg.AddString("path",text);
	msgr.SendMessage(&msg);
}

