/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "ErrorWindow.h"

#include <Alignment.h>
#include <Application.h>
#include <Catalog.h>
#include <Button.h>
#include <CheckBox.h>
#include <Clipboard.h>
#include <Entry.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <ScrollView.h>
#include <String.h>
#include <StringItem.h>
#include <TypeConstants.h>

#include "DListView.h"
#include "MsgDefs.h"
#include "Project.h"
#include "ProjectBuilder.h"
#include "ProjectWindow.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ErrorWindow"

enum
{
	M_TOGGLE_ERRORS = 'tger',
	M_TOGGLE_WARNINGS = 'tgwn',
	M_COPY_ERRORS = 'cper'
};


class ErrorItem : public BStringItem {
public:
								ErrorItem(error_msg* errorMessage);
	virtual	void				DrawItem(BView* owner, BRect frame, bool complete);
	
			error_msg*			GetMessage(void) const { return fMessage; }

private:
			error_msg*			fMessage;
};


//	#pragma mark - ErrorItem


ErrorItem::ErrorItem(error_msg* message)
	:
	BStringItem(""),
	fMessage(message)
{
	if (fMessage != NULL)
		SetText(message->rawdata.String());
}


void
ErrorItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	switch (fMessage->type) {
		case ERROR_ERROR:
		{
			owner->SetLowColor(250, 170, 170);
			break;
		}

		case ERROR_WARNING:
		{
			owner->SetLowColor(250, 250, 170);
			break;
		}

		default:
		{
			owner->SetLowColor(255, 255, 255);
			break;
		}
	}

	owner->SetDrawingMode(B_OP_OVER);
	owner->FillRect(frame, B_SOLID_LOW);
	BStringItem::DrawItem(owner, frame, complete);
}


//	#pragma mark - ErrorWindow


ErrorWindow::ErrorWindow(BRect frame, ProjectWindow* parent, ErrorList* list)
	:
	BWindow(frame, B_TRANSLATE("Errors and warnings"), B_DOCUMENT_WINDOW,
		B_ASYNCHRONOUS_CONTROLS),
	fParent(parent),
	fErrorCount(0),
	fWarningCount(0)
{
	SetSizeLimits(400, 30000, 250, 30000);
	MoveTo(20,200);
	AddShortcut('M', B_COMMAND_KEY, new BMessage(M_MAKE_PROJECT));
	AddShortcut('R', B_COMMAND_KEY, new BMessage(M_RUN_PROJECT));
	AddShortcut('R', B_COMMAND_KEY | B_SHIFT_KEY,
		new BMessage(M_RUN_IN_TERMINAL));
	AddShortcut('R', B_COMMAND_KEY | B_CONTROL_KEY,
		new BMessage(M_RUN_IN_DEBUGGER));
	AddShortcut('R', B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY,
		new BMessage(M_RUN_WITH_ARGS));
	AddShortcut('-', B_COMMAND_KEY, new BMessage(M_FORCE_REBUILD));
	AddShortcut('I', B_COMMAND_KEY, new BMessage(B_QUIT_REQUESTED));
	AddShortcut('W', B_COMMAND_KEY, new BMessage(B_QUIT_REQUESTED));

	if (list != NULL)
		fErrors = *list;

	if (parent != NULL) {
		BString text = B_TRANSLATE("Errors and warnings: ");
		text << fParent->GetProject()->GetName();
		SetTitle(text.String());
	} else
		debugger("BUG: Invalid project for error window");

	BString startingLabel(B_TRANSLATE("Errors"));
	startingLabel << " (10000)";

	fErrorBox = new BCheckBox("errorbox", startingLabel.String(),
		new BMessage(M_TOGGLE_ERRORS));
	fErrorBox->SetLabel(B_TRANSLATE("Errors"));
	fErrorBox->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_VERTICAL_CENTER));

	startingLabel = B_TRANSLATE("Warnings");
	startingLabel << " (10000)";

	fWarningBox = new BCheckBox("warningbox", startingLabel.String(),
		new BMessage(M_TOGGLE_WARNINGS));
	fWarningBox->SetLabel("Warnings");
	fWarningBox->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_VERTICAL_CENTER));

	fCopyButton = new BButton("copy", B_TRANSLATE("Copy to clipboard"),
		new BMessage(M_COPY_ERRORS));

	fErrorList = new DListView("errorlist", B_SINGLE_SELECTION_LIST,
		B_WILL_DRAW | B_NAVIGABLE);
	BScrollView* errorScrollView = fErrorList->MakeScrollView("scroller", true, true);
	errorScrollView->ScrollBar(B_HORIZONTAL)->SetSteps(25, 75);

	BPopUpMenu* contextMenu = new BPopUpMenu("context_menu", false, false);
	contextMenu->AddItem(new BMenuItem(B_TRANSLATE("Copy list to clipboard"),
		new BMessage(M_COPY_ERRORS)));
	contextMenu->SetTargetForItems(this);
	fErrorList->SetContextMenu(contextMenu);

	BView* header = BLayoutBuilder::Group<>(B_HORIZONTAL)
		.Add(fErrorBox)
		.Add(fWarningBox)
		.Add(fCopyButton)
		.AddGlue()
		.SetInsets(B_USE_DEFAULT_SPACING, 0, B_USE_DEFAULT_SPACING, 0)
		.View();
	header->SetName("header");
	header->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(header)
		.Add(errorScrollView)
		.SetInsets(-1.0f)
		.End();

	if (list != NULL)
		RefreshList();

	BRect newframe;
	BNode node(fParent->GetProject()->GetPath().GetFullPath());
	if (node.ReadAttr("error_frame", B_RECT_TYPE, 0, &newframe, sizeof(newframe))) {
		MoveTo(newframe.left, newframe.top);
		ResizeTo(newframe.Width(), newframe.Height());
	}

	fErrorBox->SetValue(B_CONTROL_ON);
	fWarningBox->SetValue(B_CONTROL_ON);

	fErrorList->SetInvocationMessage(new BMessage(M_JUMP_TO_MSG));
	fErrorList->MakeFocus(true);
}


ErrorWindow::~ErrorWindow(void)
{
	Project* project = fParent->GetProject();
	if (project == NULL)
		return;

	BRect frame(Frame());
	BNode node(project->GetPath().GetFullPath());
	node.WriteAttr("error_frame", B_RECT_TYPE, 0, &frame, sizeof(frame));
}


void
ErrorWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_RUN_PROJECT:
		case M_RUN_IN_TERMINAL:
		case M_RUN_IN_DEBUGGER:
		case M_RUN_WITH_ARGS:
		case M_MAKE_PROJECT:
		case M_FORCE_REBUILD:
		{
			fParent->PostMessage(message);
			break;
		}

		case M_TOGGLE_ERRORS:
		case M_TOGGLE_WARNINGS:
		{
			RefreshList();
			break;
		}

		case M_COPY_ERRORS:
		{
			CopyList();
			break;
		}

		case M_CLEAR_ERROR_LIST:
		{
			EmptyList();
		 	fErrors.msglist.MakeEmpty();
			break;
		}

 		case M_BUILD_WARNINGS:
 		case M_BUILD_FAILURE:
 		{
 			ErrorList list;
 			list.Unflatten(*message);
 			AppendToList(list);
 			break;
 		}

 		case M_JUMP_TO_MSG:
 		{
 			int32 selection = fErrorList->CurrentSelection();
 			if (selection >= 0) {
 				ErrorItem* item = (ErrorItem*)fErrorList->ItemAt(selection);
 				error_msg* gcc = item->GetMessage();

 				if (gcc->path.Length() < 1)
 					break;

 				entry_ref ref;
 				BEntry entry(gcc->path.String());
 				entry.GetRef(&ref);
 				message->what = EDIT_OPEN_FILE;
 				message->AddRef("refs", &ref);
 				if (gcc->line > 0)
 					message->AddInt32("line", gcc->line);

 				be_app->PostMessage(message);
 			}
 			break;
 		}

		default:
			BWindow::MessageReceived(message);
	}
}


bool
ErrorWindow::QuitRequested(void)
{
	if (fParent != NULL) {
		BMessage message(M_ERRORWIN_CLOSED);
		message.AddRect("error_frame", Frame());
		fParent->PostMessage(&message);
	}

	return true;
}


void
ErrorWindow::AppendToList(ErrorList& list)
{
	list.Rewind();

	error_msg* message = list.GetNextItem();
	int32 counter = 0;
	while (message != NULL) {
		error_msg* newmessage = new error_msg;
		*newmessage = *message;
		fErrors.msglist.AddItem(newmessage);
		ErrMsgToItem(newmessage);
		if (counter % 5 == 0)
			UpdateIfNeeded();

		counter++;
		message = list.GetNextItem();
	}
}


void
ErrorWindow::RefreshList(void)
{
	EmptyList();

	fErrors.Rewind();
	error_msg* item = fErrors.GetNextItem();
	int32 counter = 0;
	while (item != NULL) {
		ErrMsgToItem(item);
		if (counter % 5 == 0)
			UpdateIfNeeded();

		counter++;
		item = fErrors.GetNextItem();
	}
}



void
ErrorWindow::ErrMsgToItem(error_msg* message)
{
	if (message == NULL)
		return;
	
	if ((message->type == ERROR_ERROR && fErrorBox->Value() == B_CONTROL_ON)
		|| (message->type == ERROR_WARNING && fWarningBox->Value() == B_CONTROL_ON)
		|| message->type == ERROR_MSG) {
		fErrorList->AddItem(new ErrorItem(message));

		BString boxLabel;
		if (message->type == ERROR_ERROR) {
			fErrorCount++;
			boxLabel = B_TRANSLATE("Errors");
			boxLabel << " (" << fErrorCount << ")";
			fErrorBox->SetLabel(boxLabel.String());
		} else if (message->type == ERROR_WARNING) {
			fWarningCount++;
			boxLabel = B_TRANSLATE("Warnings");
			boxLabel << " (" << fWarningCount << ")";
			fWarningBox->SetLabel(boxLabel.String());
		}
	}
}


void
ErrorWindow::EmptyList(void)
{
	for (int32 i = fErrorList->CountItems() - 1; i >= 0; i--) {
		ErrorItem* item = (ErrorItem*)fErrorList->RemoveItem(i);
		delete item;
	}

	fErrorCount = fWarningCount = 0;

	BString label(B_TRANSLATE("Errors"));
	label << " (0)";
	fErrorBox->SetLabel(label.String());
	label = B_TRANSLATE("Warnings");
	label << " (0)";
	fWarningBox->SetLabel(label.String());
}


void
ErrorWindow::CopyList(void)
{
	BString data;
	fErrors.Rewind();
	error_msg* message = fErrors.GetNextItem();
	while (message) {
		data << message->rawdata << "\n";
		message = fErrors.GetNextItem();
	}

	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		BMessage* clip = be_clipboard->Data();
		if (clip) {
			clip->AddData("text/plain", B_MIME_TYPE, data.String(), data.Length());
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
}
