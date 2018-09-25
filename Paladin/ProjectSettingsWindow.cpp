/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "ProjectSettingsWindow.h"

#include <Box.h>
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <Size.h>
#include <StringView.h>

#include "DListView.h"
#include "EscapeCancelFilter.h"
#include "Globals.h"
#include "Project.h"
#include "TypedRefFilter.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ProjectSettingsWindow"

enum {
	M_TOGGLE_DEBUG			= 'tgdb',
	M_TOGGLE_PROFILE		= 'tgpf',
	M_TOGGLE_OPSIZE			= 'tgsi',
	M_SET_OP_VALUE			= 'sopv',
	M_SET_TARGET_TYPE		= 'stgt',
	M_TARGET_NAME_CHANGED	= 'tgnc',
	M_CCOPTS_CHANGED		= 'ccoc',
	M_LDOPTS_CHANGED		= 'ldoc',
	M_SHOW_ADD_PATH			= 'shap',
	M_DROP_PATH				= 'drpt',
	M_ADD_PATH				= 'adpt',
	M_REMOVE_PATH			= 'rmpt'
};


class IncludeList : public RefListView {
public:
			IncludeList(const char* projectPath);
	void	RefDropped(entry_ref ref);

private:
	BString	fProjectPath;
};


//	#pragma mark - IncludeList


IncludeList::IncludeList(const char* projectPath)
	:
	RefListView("includelist", B_MULTIPLE_SELECTION_LIST),
	fProjectPath(projectPath)
{
	if (projectPath && fProjectPath[fProjectPath.CountChars() - 1] != '/')
		fProjectPath << "/";
}


void
IncludeList::RefDropped(entry_ref ref)
{
	RefListItem* item = new RefListItem(ref, REFITEM_OTHER);
	BString text(BPath(&ref).Path());
	text.RemoveFirst(fProjectPath);
	AddItem(item);
	item->SetText(text.String());

	BMessenger messenger(Parent());
	BMessage message(M_DROP_PATH);
	message.AddString("path", text);
	messenger.SendMessage(&message);
}


//	#pragma mark - ProjectSettingsWindow


ProjectSettingsWindow::ProjectSettingsWindow(BRect frame, Project* project)
	:
	BWindow(frame, B_TRANSLATE("Project settings"), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fProject(project),
	fDirty(false)
{
	if (fProject == NULL)
		debugger("Bad project given to Project Settings window");

	fRefFilter = new TypedRefFilter(NULL, B_DIRECTORY_NODE);

	BMessenger messenger(this);
	entry_ref projectFolderRef;
	BEntry(fProject->GetPath().GetFolder()).GetRef(&projectFolderRef);
	fFilePanel = new BFilePanel(B_OPEN_PANEL, &messenger, &projectFolderRef,
		B_DIRECTORY_NODE, true, new BMessage(M_ADD_PATH), fRefFilter);

	fAutolock = new BAutolock(fProject);

	AddCommonFilter(new EscapeCancelFilter());

	fTargetText = new AutoTextControl("targetname", B_TRANSLATE("Target name:"),
		fProject->GetTargetName(), new BMessage(M_TARGET_NAME_CHANGED));

	BPopUpMenu* targetTypeMenu = new BPopUpMenu(B_TRANSLATE("Target type"));
	targetTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Application"),
		new BMessage(M_SET_TARGET_TYPE)));
	targetTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Shared library"),
		new BMessage(M_SET_TARGET_TYPE)));
	targetTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Static library"),
		new BMessage(M_SET_TARGET_TYPE)));
	targetTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Device driver"),
		new BMessage(M_SET_TARGET_TYPE)));

	fTypeField = new BMenuField("type", B_TRANSLATE("Target type:"), targetTypeMenu);
	SetToolTip(fTypeField, B_TRANSLATE("The kind of program you want to build"));

	BMenuItem* item = targetTypeMenu->ItemAt(fProject->TargetType());
	if (item != NULL)
		item->SetMarked(true);

	fIncludeList = new IncludeList(fProject->GetPath().GetFolder());
	SetToolTip(fIncludeList,
		B_TRANSLATE("The folders you want Paladin to search for header files"));

	BScrollView* includeScrollView = new BScrollView("includescrollview",
		fIncludeList, B_WILL_DRAW, true, true);
	includeScrollView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	includeScrollView->SetExplicitMinSize(
		BSize(be_plain_font->StringWidth("M") * 34.0f,
			be_plain_font->StringWidth("M") * 12.0f));

	for (int32 i = 0; i < fProject->CountLocalIncludes(); i++) {
		fIncludeList->AddItem(new BStringItem(
			fProject->LocalIncludeAt(i).Relative().String()));
	}

	float buttonWidth = be_plain_font->StringWidth("+") * 2.0f + 3.0f;

	BButton* addButton = new BButton("addbutton", B_TRANSLATE("+"),
		new BMessage(M_SHOW_ADD_PATH));
	addButton->SetExplicitSize(BSize(buttonWidth, buttonWidth));
	addButton->SetToolTip(B_TRANSLATE("Add a file to the include path list"));

	BButton* removeButton = new BButton("removebutton", B_TRANSLATE("−"),
		new BMessage(M_REMOVE_PATH));
	removeButton->SetExplicitSize(BSize(buttonWidth, buttonWidth));
	removeButton->SetToolTip(B_TRANSLATE("Remove the selected path"));

	// general tab

	fGeneralView = new BView(B_TRANSLATE("General"), B_WILL_DRAW);
	fGeneralView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BLayoutBuilder::Group<>(fGeneralView, B_VERTICAL, 0)
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.Add(fTargetText->CreateLabelLayoutItem(), 0, 0)
			.Add(fTargetText->CreateTextViewLayoutItem(), 1, 0)

			.Add(fTypeField->CreateLabelLayoutItem(), 0, 1)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING, 1, 1)
				.Add(fTypeField->CreateMenuBarLayoutItem())
				.AddGlue()
				.End()
			.End()
		.AddStrut(B_USE_DEFAULT_SPACING)
		.AddGroup(B_VERTICAL, 2.0f)
			.Add(new BStringView("include paths", B_TRANSLATE("Include paths:")))
			.AddGroup(B_HORIZONTAL, B_USE_SMALL_SPACING)
				.Add(includeScrollView)
				.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
					.Add(addButton)
					.Add(removeButton)
					.AddGlue()
					.End()
				.End()
			.End()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.End();

	BPopUpMenu* optimizationMenu = new BPopUpMenu(B_TRANSLATE("Optimization"));
	optimizationMenu->AddItem(new BMenuItem(B_TRANSLATE("None"),
		new BMessage(M_SET_OP_VALUE)));
	optimizationMenu->AddItem(new BMenuItem(B_TRANSLATE("Some"),
		new BMessage(M_SET_OP_VALUE)));
	optimizationMenu->AddItem(new BMenuItem(B_TRANSLATE("More"),
		new BMessage(M_SET_OP_VALUE)));
	optimizationMenu->AddItem(new BMenuItem(B_TRANSLATE("Full"),
		new BMessage(M_SET_OP_VALUE)));

	fOpField = new BMenuField("optimize", B_TRANSLATE("Optimize:"), optimizationMenu);
	SetToolTip(fOpField, B_TRANSLATE("Compiler optimization level. "
		"Disabled when debugging info is checked."));

	item = optimizationMenu->ItemAt(fProject->OpLevel());
	if (item != NULL)
		item->SetMarked(true);

	fOpSizeBox = new BCheckBox("opsizebox", B_TRANSLATE("Optimize for size over speed"),
		new BMessage(M_TOGGLE_OPSIZE));

	if (fProject->OpForSize())
		fOpSizeBox->SetValue(B_CONTROL_ON);
	
	if (fProject->Debug()) {
		fOpField->SetEnabled(false);
		fOpSizeBox->SetEnabled(false);
	}

	fDebugBox = new BCheckBox("debugbox", B_TRANSLATE("Build debugging information"),
		new BMessage(M_TOGGLE_DEBUG));
	SetToolTip(fDebugBox,
		B_TRANSLATE("Check this if you want to use your program in a debugger "
		   "during development. You'll want to rebuild your project "
		   "after change this."));

	if (fProject->Debug())
		fDebugBox->SetValue(B_CONTROL_ON);

	fProfileBox = new BCheckBox("profilebox", B_TRANSLATE("Build profiling information"),
		new BMessage(M_TOGGLE_PROFILE));
	SetToolTip(fProfileBox,
		B_TRANSLATE("Check this if you want to use your program "
		   "with gprof or bprof for profiling."));

	if (fProject->Profiling())
		fProfileBox->SetValue(B_CONTROL_ON);

	fCompileText = new AutoTextControl("extracc", "Extra compiler options:",
		fProject->ExtraCompilerOptions(), new BMessage(M_CCOPTS_CHANGED));
	SetToolTip(fCompileText,
		B_TRANSLATE("Extra GCC flags you wish included when each file is compiled."));

	fLinkText = new AutoTextControl("extrald" ,B_TRANSLATE("Extra linker options:"),
		fProject->ExtraLinkerOptions(), new BMessage(M_LDOPTS_CHANGED));
	SetToolTip(fLinkText,
		B_TRANSLATE("Extra GCC linker flags you wish included when your project "
		   "is linked."));

	// build tab

	fBuildView = new BView("Build", B_WILL_DRAW);
	fBuildView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BLayoutBuilder::Group<>(fBuildView, B_VERTICAL)
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
			.Add(fOpField->CreateLabelLayoutItem(), 0, 0)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING, 1, 0)
				.Add(fOpField->CreateMenuBarLayoutItem())
				.AddGlue()
				.End()
			.AddGroup(B_VERTICAL, 0.0f, 1, 2)
				.Add(fOpSizeBox)
				.AddStrut(B_USE_SMALL_SPACING)
				.Add(fDebugBox)
				.Add(fProfileBox)
				.End()
			.End()
		.AddGlue()
		.AddGroup(B_VERTICAL, 0)
			.Add(fCompileText->CreateLabelLayoutItem())
			.Add(fCompileText->CreateTextViewLayoutItem())
			.End()
		.AddGroup(B_VERTICAL, 0)
			.Add(fLinkText->CreateLabelLayoutItem())
			.Add(fLinkText->CreateTextViewLayoutItem())
			.End()
		.SetInsets(B_USE_DEFAULT_SPACING)
		.End();

	fTabView = new BTabView("tabview", B_WIDTH_FROM_LABEL);
	fTabView->SetBorder(B_NO_BORDER);
	fTabView->AddTab(fGeneralView);
	fTabView->AddTab(fBuildView);
	fTabView->Select(0L);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.AddStrut(B_USE_SMALL_SPACING)
		.Add(fTabView)
		.End();

	targetTypeMenu->SetTargetForItems(this);
	optimizationMenu->SetTargetForItems(this);

	fIncludeList->Select(0);
	fTargetText->MakeFocus(true);
}


ProjectSettingsWindow::~ProjectSettingsWindow(void)
{
	delete fFilePanel;
	delete fRefFilter;
}


void
ProjectSettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_SHOW_ADD_PATH:
		{
			fFilePanel->Show();
			break;
		}

		case M_DROP_PATH:
		{
			BString path;
			if (message->FindString("path",&path) == B_OK)
				fProject->AddLocalInclude(path.String());
			break;
		}

		case M_ADD_PATH:
		{
			entry_ref ref;
			int32 i = 0;
			while (message->FindRef("refs", i++, &ref) == B_OK) {
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
			
			for (int32 i = fIncludeList->CountItems() - 1; i >= 0; i--) {
				BStringItem* item = (BStringItem*)fIncludeList->ItemAt(i);
				if (item->IsSelected()) {
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
			if (fDebugBox->Value() == B_CONTROL_ON) {
				fProject->SetDebug(true);
				fOpField->SetEnabled(false);
				fOpSizeBox->SetEnabled(false);
			} else {
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
			BWindow::MessageReceived(message);
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
ProjectSettingsWindow::AddInclude(const entry_ref& ref)
{
	BString path(BPath(&ref).Path());

	// Make any subfolders of the project's parent folder into relative paths
	if (path[0] == '/') {
		BString projpath = fProject->GetPath().GetFolder();
		projpath << '/';
		if (path.FindFirst(projpath) == 0)
			path.RemoveFirst(projpath);
	}

	if (!fProject->HasLocalInclude(path.String())) {
		fProject->AddLocalInclude(path.String());
		fIncludeList->AddItem(new BStringItem(path.String()));
	}
}
