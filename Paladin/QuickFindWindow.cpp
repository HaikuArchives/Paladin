/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include "QuickFindWindow.h"

#include <string>

#include <Application.h>
#include <Button.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <Locale.h>
#include <Message.h>
#include <Messenger.h>
#include <Path.h>
#include <Size.h>
#include <StringItem.h>
#include <View.h>
#include <TextControl.h>
#include <Font.h>
#include <interface/GraphicsDefs.h>

#include "AutoTextControl.h"
#include "DebugTools.h"
#include "EscapeCancelFilter.h"
#include "Globals.h"
#include "MsgDefs.h"
#include "Project.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "QuickFindWindow"

namespace BPrivate {

_BTextQueryEntry_::_BTextQueryEntry_(BRect frame, BRect textRect,
								uint32 resizeMask,
								uint32 flags)
	:
	BTextView(frame,"_textqueryentry_",textRect,resizeMask,flags)
{
	MakeResizable(true);							
}

_BTextQueryEntry_::~_BTextQueryEntry_()
{
	// free things
}
							
void _BTextQueryEntry_::KeyDown(const char *bytes, int32 numBytes)
{
	BMessage* keym = NULL;
	switch (*bytes) {
		case B_ENTER:
		{
			// fire selection execution event
			STRACE(1,("Enter key intercepted\n"));
			BMessage* keym = new BMessage(M_QUICK_FIND_SELECT);
			TextQueryList()->MessageReceived(keym);
			break;
		}
		case B_TAB:
		{
			BView::KeyDown(bytes, numBytes);
			break;
		}
		case B_UP_ARROW:
		{
			STRACE(1,("Up arrow key intercepted\n"));
			keym = new BMessage(M_QUICK_FIND_SELECT_PREVIOUS);
			TextQueryList()->MessageReceived(keym);
			break;
		}
		case B_DOWN_ARROW:
		{
			STRACE(1,("Down arrow key intercepted\n"));
			BMessage* keym = new BMessage(M_QUICK_FIND_SELECT_NEXT);
			TextQueryList()->MessageReceived(keym);
			break;
		}
		default:
		{
			BTextView::KeyDown(bytes, numBytes);
			// Send message to parent
			BMessage* keym = new BMessage(M_QUICK_FIND_TEXT_UPDATED);
			keym->AddString("query",Text());
			TextQueryList()->MessageReceived(keym);
			break;
		}
	}
}
	
BTextQueryList*	_BTextQueryEntry_::TextQueryList()
{
	BTextQueryList* textControl = NULL;
	if (Parent() != NULL)
		textControl = dynamic_cast<BTextQueryList*>(Parent());

	if (textControl == NULL)
		debugger("_BTextQueryEntry_ should have a BTextQueryList as parent");

	return textControl;
}

} // end namespace





_BTextQueryIFace_::_BTextQueryIFace_()
{
}

_BTextQueryIFace_::~_BTextQueryIFace_()
{
}




namespace BPrivate {


_BTextQueryLooper_::_BTextQueryLooper_(_BTextQueryIFace_* iface)
	:
	fIFace(iface)
{
	Run();
}

_BTextQueryLooper_::~_BTextQueryLooper_()
{
}

void
_BTextQueryLooper_::MessageReceived(BMessage* message)
{
	STRACE(1,("Message Received in _BTextQueryLooper_\n"));
	if (NULL == message) 
	{
		STRACE(1,("Message is null somehow...\n"));
		return;
	}
	if (message->what == M_QUICK_FIND_QUERY_REPLY)
	{
		Lock();
		if (NULL == fIFace)
		{
			STRACE(1,("fIFace is null!!!!! HOW!?!\n"));
		}
		STRACE(1,("Calling clear options\n"));
		fIFace->ClearOptions();
		// update query results
		type_code type;
		int32 cnt;
		STRACE(1,("Looping over options in message\n"));
		status_t gotIt = message->GetInfo("option",&type,&cnt);
		if (B_OK == gotIt)
		{
			for (int i = 0;i < cnt;i++)
			{
				STRACE(1,("Calling add option\n"));
				fIFace->AddOption(message->GetString("option",i,""));
			}
		}
		STRACE(1,("Calling all options added\n"));
		fIFace->AllOptionsAdded();
		Unlock();
	} else {
		BLooper::MessageReceived(message);
	}
}




} // end private namespace














BTextQueryList::BTextQueryList(BHandler* messageHandler, BRect frame,const char* name, 
	const char* label, const char* text, BMessage* queryMessage, 
	BMessage* selectionMessage,uint32 resizeMask, uint32 flags)
	:
	BControl(frame, name, label, selectionMessage, resizeMask, flags | B_FRAME_EVENTS),
	fText(NULL),
	fResults(NULL),
	fHandler(messageHandler),
	fTextQueryLooper(NULL),
	fMessenger(new BMessenger(fHandler)),
	fQueryMessage(queryMessage),
	fSelectionMessage(selectionMessage)
{
	_InitText(text);
	_InitResults();
	
	fTextQueryLooper = new _BTextQueryLooper_(this);
}

BTextQueryList::~BTextQueryList()
{
}

void
BTextQueryList::Draw(BRect updateRect)
{
}

void
BTextQueryList::MessageReceived(BMessage* message)
{
	STRACE(1,("Message Received in BTextQueryList\n"));
	switch (message->what)
	{
		case M_QUICK_FIND_TEXT_UPDATED:
		{
			// was BControl::MessageReceived(message);
			// , but we want to use a separate thread in case search takes a long time
			fResults->MakeEmpty();
			BMessage qm(*fQueryMessage);
			BString qry;
			if (B_OK == message->FindString("query",&qry))
			{
				qm.AddString("query",qry);
			}
			fMessenger->SendMessage(&qm, fTextQueryLooper); // reply to handler interface
			break;
		}
		case M_QUICK_FIND_SELECT:
		{
			_FireSelect();
			break;
		}
		case M_QUICK_FIND_SELECT_NEXT:
		{
			if (fResults->CountItems() > 0)
			{
				int idx = fResults->CurrentSelection() + 1;
				if (idx >= 0 && idx < fResults->CountItems())
				{
					fResults->Select(idx);
				}
			}
			break;
		}
		case M_QUICK_FIND_SELECT_PREVIOUS:
		{
			if (fResults->CountItems() > 0)
			{
				int idx = fResults->CurrentSelection() - 1;
				if (idx >= 0 && idx < fResults->CountItems())
				{
					fResults->Select(idx);
				}
			}
			break;
		}
		default:
		{
			STRACE(1,("Message what: \n"));
			message->PrintToStream();
			BControl::MessageReceived(message);
		}
	}
}

void
BTextQueryList::_FireSelect()
{
	BMessage message(*fSelectionMessage);
	if (fResults->CountItems() > 0)
	{
		//int idx = fResults->CurrentSelection();
		//const char* selText = ((BStringItem*)fResults->ItemAt(idx))->Text();
		//STRACE(1,("Selected: %s\n",selText));
		//message.AddString("selected",selText);
		// Listener will use GetSelectionOption() to get selected text
		//   Needed as if done by mouse, the selection text isn't sent
		fMessenger->SendMessage(&message);
		fText->SelectAll();
	}
}

void
BTextQueryList::SelectText()
{
	if (NULL != fText)
	{
		fText->SelectAll();
	}
}

const char*
BTextQueryList::GetSelectedOption()
{
	int idx = fResults->CurrentSelection();
	const char* selText = ((BStringItem*)fResults->ItemAt(idx))->Text();
	return selText;
}



void
BTextQueryList::ClearOptions()
{
	BLooper* ptr = NULL;
	fMessenger->Target(&ptr);
	ptr->Lock();
	fResults->MakeEmpty();
	fResults->Invalidate();
	ptr->Unlock();
}

void
BTextQueryList::AddOption(const char* option)
{
	BLooper* ptr = NULL;
	fMessenger->Target(&ptr);
	ptr->Lock();
	fResults->AddItem(new BStringItem(option));
	fResults->Invalidate();
	ptr->Unlock();
}

void BTextQueryList::AllOptionsAdded()
{
	BLooper* ptr = NULL;
	fMessenger->Target(&ptr);
	ptr->Lock();
	fResults->Select(0,false);
	fResults->Invalidate();
	ptr->Unlock();
}

void
BTextQueryList::SetText(const char* text)
{
	fText->SetText(text);
}

const char*
BTextQueryList::Text() const
{
	return fText->Text();
}

int32
BTextQueryList::TextLength() const
{
	return fText->TextLength();
}

BTextView*
BTextQueryList::TextView() const
{
	return fText;
}


void
BTextQueryList::MakeFocus(bool state)
{
	STRACE(1,("MakeFocus called\n"));
	if (!fText) return;
	if (state != fText->IsFocus()) {
		fText->MakeFocus(state);

		if (state)
			fText->SelectAll();
	}
}


void
BTextQueryList::SetEnabled(bool enable)
{
	if (IsEnabled() == enable)
		return;

	if (Window() != NULL) {
		fText->MakeEditable(enable);
		if (enable)
			fText->SetFlags(fText->Flags() | B_NAVIGABLE);
		else
			fText->SetFlags(fText->Flags() & ~B_NAVIGABLE);

		fText->Invalidate();
		Window()->UpdateIfNeeded();
	}

	BControl::SetEnabled(enable);
}

void
BTextQueryList::_InitText(const char* initialText, const BMessage* archive)
{
	if (archive)
		fText = static_cast<BPrivate::_BTextQueryEntry_*>(FindView("_textqueryentry_"));

	if (fText == NULL) {
		STRACE(1,("Creating fText entry field (_BTextQueryEntry_)\n"));
		BRect bounds(Bounds());
		bounds.bottom = bounds.top + 30;
		BRect frame(bounds.left, bounds.top, bounds.right, bounds.bottom);
		// we are stroking the frame around the text view, which
		// is 2 pixels wide
		frame.InsetBy(1, 1);
		BRect textRect(frame.OffsetToCopy(B_ORIGIN));

		fText = new BPrivate::_BTextQueryEntry_(frame, textRect,
			B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS
			| (Flags() & B_NAVIGABLE));
		AddChild(fText);

		SetText(initialText);
		fText->SetAlignment(B_ALIGN_LEFT);
		//fText->AlignTextRect();
		STRACE(1,("Text query entry added\n"));
	}

	// Although this is not strictly initializing the text view,
	// it cannot be done while fText is NULL, so it resides here.
	// TODO add label back eventually
	if (archive) {
		//int32 labelAlignment = B_ALIGN_LEFT;
		int32 textAlignment = B_ALIGN_LEFT;

		status_t err = B_OK;
		if (err == B_OK && archive->HasInt32("_a_text"))
			err = archive->FindInt32("_a_text", &textAlignment);

		//SetAlignment((alignment)textAlignment);
	}

	uint32 navigableFlags = Flags() & B_NAVIGABLE;
	if (navigableFlags != 0)
		BView::SetFlags(Flags() & ~B_NAVIGABLE);
}

void
BTextQueryList::_InitResults()
{
	BRect r = Bounds();
	r.top = r.top + 32.0;
	fResults = new BListView(r,"results",B_SINGLE_SELECTION_LIST,B_WILL_DRAW);
	//fResults->SetSelectionMessage(fSelectionMessage);
	// Don't attach this as it bypasses our selection handler, and is for single click
	//   not double click
	fResults->SetInvocationMessage(fSelectionMessage);
	AddChild(fResults);
	
	fResults->AddItem(new BStringItem(B_TRANSLATE("Type to search")));
}








QuickFindWindow::QuickFindWindow(const char* panelText)
	:
	BWindow(BRect(100,200,700,500), B_TRANSLATE("Quick find"), B_BORDERED_WINDOW, 
		B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE)
{
	fSelectedPath = NULL;
	
	// Set up UI here
	BRect r = Bounds().InsetByCopy(12,12);
	fList = new BTextQueryList(this, r, "_querylist_", B_TRANSLATE("Find files: "),"",
		new BMessage(M_QUICK_FIND_TEXT_UPDATED),new BMessage(M_QUICK_FIND_SELECT));
	
	AddChild(fList);
	fList->MakeFocus(true);
	
	// Center and display
	CenterOnScreen();
}

QuickFindWindow::~QuickFindWindow()
{
	
}

void
QuickFindWindow::SetProject(Project* project)
{
	fProject = project;
}

void
QuickFindWindow::DoSearch(BMessage* queryMessage)
{
	printf(B_TRANSLATE("DoSearch started\n"));
	const char* text;
	text = queryMessage->GetString("query","");
	fSelectedPath = NULL;
	if (0 == strlen(text))
		return;
		
	entry_ref folderref = DPath(fProject->GetPath().GetFolder()).GetRef();
	
	printf(B_TRANSLATE("Searching for file matching: %s\n"), text);
	
	BMessage* reply = new BMessage(M_QUICK_FIND_QUERY_REPLY);
	
	BEntry folderEntry(&folderref);
	DoSearchDirectory(text, reply, folderEntry);
	
	// send reply message
	queryMessage->SendReply(reply);
	
	printf("DoSearch finished\n");
}

void
QuickFindWindow::DoSearchDirectory(const char* text, BMessage* reply,BEntry& dirEntry)
{
	BDirectory dir(&dirEntry);
	if (B_OK != dir.InitCheck())
	{
		printf(B_TRANSLATE("Directory InitCheck failed\n"));
		return;
	}
	dir.Rewind();
	
	BEntry entry;
	
	while (B_OK == dir.GetNextEntry(&entry, true))
	{
		if (entry.IsDirectory())
		{
			// recursive search
			STRACE(1,("Directory found\n"));
			DoSearchDirectory(text, reply, entry);
		}
		else
		{
			DoSearchFile(text, reply,entry);
		}
	}
}

void
QuickFindWindow::DoSearchFile(const char* text, BMessage* reply,BEntry& entry)
{
	char* entryName = new char[1024];
	BPath entryPath;
	char* s;
			// qfw -> QuickFindWindow.* style search
			// TODO case insensitive
			
			// Check to see if the file name matches the search string
			if (B_OK == entry.GetName(entryName))
			{
				bool matches = false;
				s = strstr(entryName,text);
				if (NULL != s)
				{
					matches = true;
				} else if (strlen(text) < 10) {
					// Grab filename capitals, lowercase them, and see if it matches
					char caps[20];
					int idx = 0;
					for (int sIdx = 0;sIdx < strlen(entryName) && idx < 20;sIdx++)
					{
						if (isupper(entryName[sIdx]))
						{
							caps[idx++] = entryName[sIdx];
						}
					}
					caps[idx] = '\0';
					char textCaps[strlen(text)];
					for (int tci = 0;tci < strlen(text);tci++)
					{
						textCaps[tci] = toupper(text[tci]);
					}
					textCaps[strlen(text)] = '\0';
					STRACE(1,("Caps follows\n"));
					STRACE(1,(caps));
					STRACE(1,("Search text caps follows\n"));
					STRACE(1,(textCaps));
					if (idx > 0)
					{
						s = strstr(caps,textCaps); //std::toupper(text,loc)); B_BAD_CAST!?!
						if (NULL != s)
						{
							matches = true;
						}
					}
				}
				if (matches)
				{
					// matches
					if (B_OK == entry.GetPath(&entryPath))
					{
						printf(B_TRANSLATE("Found: %s\n"), entryPath.Path());
						reply->AddString("option",entryPath.Path());
					}
				}
			}
}

void
QuickFindWindow::MessageReceived(BMessage* message)
{
	const char* option = NULL;
	switch (message->what) {
		case M_QUICK_FIND_TEXT_UPDATED:
			STRACE(1,("Query event intercepted in QuickFindWindow\n"));
			DoSearch(message);
			break;
		case M_QUICK_FIND_SELECT:
			STRACE(1,("Find selection message received\n"));
			// go do something now (open the file)
			option = fList->GetSelectedOption();
			if (NULL != option)
			{
				// Hide quick find window (me)
				Hide();
				// Fire open file event
				entry_ref fileref;
				if (B_OK == get_ref_for_path(option, &fileref))
				{
					BMessage refMessage(B_REFS_RECEIVED);
					refMessage.AddRef("refs",&fileref);
					be_app->PostMessage(&refMessage);
				}
			}
			break;
		default:
			STRACE(1,("Observed unhandled: %i\n",message->what));
			BWindow::MessageReceived(message);
	}
}

void
QuickFindWindow::WindowActivated(bool active)
{
	STRACE(1,("QuickFindWindow::WindowActivated\n"));
	BWindow::WindowActivated(active);
	if (!active)
	{
		STRACE(1,("QuickFindWindow inactive - hiding if necessary\n"));
		// Note: Hide and Show are cumulative - don't call if hidden
		if (!IsHidden())
			Hide();
		fList->SelectText();
	} else {
		fList->MakeFocus(true);
	}
}

bool
QuickFindWindow::QuitRequested(void)
{
	STRACE(1,("QuickFindWindow::QuitRequested\n"));
	if (!IsHidden())
		Hide();
	//fList->SetText(""); // Clear for the next search
	// the above is redundant - select all done by text query list instead
	return false;
}
