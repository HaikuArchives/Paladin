#include "ErrorWindow.h"
#include <Application.h>
#include <Clipboard.h>
#include <Entry.h>
#include <ListItem.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <TypeConstants.h>

#include "DListView.h"
#include "MsgDefs.h"
#include "PLocale.h"
#include "Project.h"
#include "ProjectBuilder.h"
#include "ProjectWindow.h"

enum
{
	M_TOGGLE_ERRORS = 'tger',
	M_TOGGLE_WARNINGS = 'tgwn',
	M_COPY_ERRORS = 'cper'
};

ErrorWindow::ErrorWindow(BRect frame, ProjectWindow *parent, ErrorList *list)
	:	DWindow(frame,TR("Errors and Warnings"),B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS),
		fParent(parent),
		fErrorCount(0),
		fWarningCount(0)
{
	AddShortcut('M',B_COMMAND_KEY, new BMessage(M_MAKE_PROJECT));
	AddShortcut('R',B_COMMAND_KEY, new BMessage(M_RUN_PROJECT));
	AddShortcut('R',B_COMMAND_KEY | B_SHIFT_KEY, new BMessage(M_RUN_IN_TERMINAL));
	AddShortcut('R',B_COMMAND_KEY | B_CONTROL_KEY, new BMessage(M_RUN_IN_DEBUGGER));
	AddShortcut('R',B_COMMAND_KEY | B_CONTROL_KEY | B_SHIFT_KEY, new BMessage(M_RUN_WITH_ARGS));
	AddShortcut('-',B_COMMAND_KEY, new BMessage(M_FORCE_REBUILD));
	AddShortcut('I',B_COMMAND_KEY, new BMessage(B_QUIT_REQUESTED));
	AddShortcut('W',B_COMMAND_KEY, new BMessage(B_QUIT_REQUESTED));
	
	BView *top = GetBackgroundView();
	
	if (list)
		fErrors = *list;
	
	if (parent)
	{
		BString text = TR("Errors and Warnings: ");
		text << fParent->GetProject()->GetName();
		SetTitle(text.String());
	}
	else
		debugger("BUG: Invalid project for error window");
	
	BRect bounds(Bounds());
	BRect r(bounds);
	
	BString startingLabel(TR("Errors"));
	startingLabel << " (10000)";
	fErrorBox = new BCheckBox(r,"errorbox",startingLabel.String(),
							new BMessage(M_TOGGLE_ERRORS));
	fErrorBox->ResizeToPreferred();
	fErrorBox->SetLabel(TR("Errors"));
	
	startingLabel = TR("Warnings");
	startingLabel << " (10000)";
	fWarningBox = new BCheckBox(r,"warningbox","Warnings (10000)",
								new BMessage(M_TOGGLE_WARNINGS));
	fWarningBox->ResizeToPreferred();
	fWarningBox->MoveTo(fErrorBox->Frame().right + 10, 0);
	fWarningBox->SetLabel("Warnings");
	
	fCopyButton = new BButton(fWarningBox->Frame(),"copy",TR("Copy to Clipboard"),
							new BMessage(M_COPY_ERRORS), B_FOLLOW_LEFT | B_FOLLOW_TOP);
	fCopyButton->MoveBy(fCopyButton->Bounds().Width() + 10.0,0);
	fCopyButton->ResizeToPreferred();
	
	r = Bounds();
	r.bottom = fCopyButton->Bounds().bottom + 1.0;
	BView *header = new BView(r,"header",B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
							B_WILL_DRAW);
	header->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	top->AddChild(header);
	header->AddChild(fErrorBox);
	header->AddChild(fWarningBox);
	header->AddChild(fCopyButton);
	fErrorBox->SetValue(B_CONTROL_ON);
	fWarningBox->SetValue(B_CONTROL_ON);
	
	r.OffsetBy(0,r.Height() + 1.0);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom = bounds.bottom - B_H_SCROLL_BAR_HEIGHT;
	fErrorList = new DListView(r,"errorlist",B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL);
	BScrollView *scrollView = fErrorList->MakeScrollView("scroller",true,true);
	top->AddChild(scrollView);
	BScrollBar *hbar = scrollView->ScrollBar(B_HORIZONTAL);
	hbar->SetSteps(25,75);
	
	BPopUpMenu *popUp = new BPopUpMenu("context_menu",false,false);
	popUp->AddItem(new BMenuItem(TR("Copy List to Clipboard"),new BMessage(M_COPY_ERRORS)));
	popUp->SetTargetForItems(this);
	fErrorList->SetContextMenu(popUp);
	
	if (list)
		RefreshList();
	
	BNode node(fParent->GetProject()->GetPath().GetFullPath());
	if (node.ReadAttr("error_frame",B_RECT_TYPE,0,&r,sizeof(BRect)) == sizeof(BRect))
	{
		MoveTo(r.left,r.top);
		ResizeTo(r.Width(),r.Height());
	}
	
	fErrorList->SetInvocationMessage(new BMessage(M_JUMP_TO_MSG));
	fErrorList->MakeFocus(true);
}


ErrorWindow::~ErrorWindow(void)
{
	Project *proj = fParent->GetProject();
	if (!proj)
		return;
	
	BNode node(proj->GetPath().GetFullPath());
	BRect r(Frame());
	node.WriteAttr("error_frame",B_RECT_TYPE,0,&r,sizeof(BRect));
}


void
ErrorWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
 	{
		case M_RUN_PROJECT:
		case M_RUN_IN_TERMINAL:
		case M_RUN_IN_DEBUGGER:
		case M_RUN_WITH_ARGS:
		case M_MAKE_PROJECT:
		case M_FORCE_REBUILD:
		{
			fParent->PostMessage(msg);
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
 			list.Unflatten(*msg);
 			AppendToList(list);
 			break;
 		}
 		case M_JUMP_TO_MSG:
 		{
 			int32 selection = fErrorList->CurrentSelection();
 			if (selection >= 0)
 			{
 				ErrorItem *item = (ErrorItem*) fErrorList->ItemAt(selection);
 				error_msg *gcc = item->GetMessage();
 				
 				if (gcc->path.Length() < 1)
 					break;
 				
 				entry_ref ref;
 				BEntry entry(gcc->path.String());
 				entry.GetRef(&ref);
 				msg->what = PALEDIT_OPEN_FILE;
 				msg->AddRef("refs",&ref);
 				if (gcc->line > 0)
 					msg->AddInt32("line",gcc->line);
 				be_app->PostMessage(msg);
 			}
 			break;
 		}
		default:
			DWindow::MessageReceived(msg);
	}
}


bool
ErrorWindow::QuitRequested(void)
{
	if (fParent)
	{
		BMessage msg(M_ERRORWIN_CLOSED);
		msg.AddRect("error_frame",Frame());
		fParent->PostMessage(&msg);
	}
	return true;
}


void
ErrorWindow::AppendToList(ErrorList &list)
{
	list.Rewind();
	
	error_msg *msg = list.GetNextItem();
	int32 counter = 0;
	while (msg)
	{
		error_msg *newmsg = new error_msg;
		*newmsg = *msg;
		fErrors.msglist.AddItem(newmsg);
		ErrMsgToItem(newmsg);
		if (counter % 5 == 0)
			UpdateIfNeeded();
		counter++;
		msg = list.GetNextItem();
	}
}


void
ErrorWindow::RefreshList(void)
{
	EmptyList();
	
	fErrors.Rewind();
	error_msg *item = fErrors.GetNextItem();
	int32 counter = 0;
	while (item)
	{
		ErrMsgToItem(item);
		if (counter % 5 == 0)
			UpdateIfNeeded();
		counter++;
		item = fErrors.GetNextItem();
	}
}



void
ErrorWindow::ErrMsgToItem(error_msg *msg)
{
	if (!msg)
		return;
	
	if ((msg->type == ERROR_ERROR && fErrorBox->Value() == B_CONTROL_ON) ||
		(msg->type == ERROR_WARNING && fWarningBox->Value() == B_CONTROL_ON) ||
		msg->type == ERROR_MSG)
	{
		fErrorList->AddItem(new ErrorItem(msg));
		
		BString boxLabel;
		if (msg->type == ERROR_ERROR)
		{
			fErrorCount++;
			boxLabel = TR("Errors");
			boxLabel << " (" << fErrorCount << ")";
			fErrorBox->SetLabel(boxLabel.String());
		}
		else if (msg->type == ERROR_WARNING)
		{
			fWarningCount++;
			boxLabel = TR("Warnings");
			boxLabel << " (" << fWarningCount << ")";
			fWarningBox->SetLabel(boxLabel.String());
		}
	}
}


void
ErrorWindow::EmptyList(void)
{
	for (int32 i = fErrorList->CountItems() - 1; i >= 0; i--)
	{
		ErrorItem *item = (ErrorItem*)fErrorList->RemoveItem(i);
		delete item;
	}
	
	fErrorCount = fWarningCount = 0;
	
	BString label(TR("Errors"));
	label << " (0)";
	fErrorBox->SetLabel(label.String());
	label = TR("Warnings");
	label << " (0)";
	fWarningBox->SetLabel(label.String());
}


void
ErrorWindow::CopyList(void)
{
	BString data;
	fErrors.Rewind();
	error_msg *msg = fErrors.GetNextItem();
	while (msg)
	{
		data << msg->rawdata << "\n";
		msg = fErrors.GetNextItem();
	}
	
	if (be_clipboard->Lock())
	{
		be_clipboard->Clear();
		BMessage *clip = be_clipboard->Data();
		if (clip)
		{
			clip->AddData("text/plain",B_MIME_TYPE, data.String(), data.Length());
			be_clipboard->Commit();
		}
	}
	be_clipboard->Unlock();
}


ErrorItem::ErrorItem(error_msg *msg)
	:	BStringItem(""),
		fMessage(msg)
{
	if (fMessage)
		SetText(msg->rawdata.String());
}


void
ErrorItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	switch (fMessage->type)
	{
		case ERROR_ERROR:
		{
			owner->SetLowColor(250,170,170);
			break;
		}
		case ERROR_WARNING:
		{
			owner->SetLowColor(250,250,170);
			break;
		}
		default:
		{
			owner->SetLowColor(255,255,255);
			break;
		}
	}
	owner->FillRect(frame,B_SOLID_LOW);
	BStringItem::DrawItem(owner,frame,complete);
}


