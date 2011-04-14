#include "FindResultsWindow.h"

#include <Font.h>
#include <StringView.h>

#include "DListView.h"
#include "DTextView.h"

enum
{
	M_FIND = 'find',
	M_REPLACE = 'repl',
	M_REPLACE_ALL = 'rpla',
	M_TOGGLE_REGEX = 'tgrx',
	M_TOGGLE_CASE_INSENSITIVE = 'tgci',
	M_TOGGLE_MATCH_WORD = 'tgmw'
};

enum
{
	THREAD_FIND = 0,
	THREAD_REPLACE,
	THREAD_REPLACE_ALL
};


FindResultsWindow::FindResultsWindow(void)
	:	DWindow(BRect(100,100,600,500), "Find Results"),
		fIsRegEx(false),
		fMatchCase(false),
		fMatchWord(false),
		fThreadID(-1),
		fThreadMode(0),
		fThreadQuitFlag(0)
{
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	BRect r(Bounds());
	r.bottom = 20.0;
	fMenuBar = new BMenuBar(r, "menubar");
	top->AddChild(fMenuBar);
	
	fFindButton = new BButton(BRect(0,0,1,1), "findbutton", "Replace All",
								new BMessage(M_FIND), B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fFindButton->ResizeToPreferred();
	fFindButton->SetLabel("Find");
	fFindButton->MoveTo(Bounds().right - fFindButton->Bounds().Width() - 10.0, 30.0);
	fFindButton->SetEnabled(false);
	
	font_height fh;
	be_plain_font->GetHeight(&fh);
	float lineHeight = fh.ascent + fh.descent + fh.leading;
	
	r = fFindButton->Frame();
	r.left = 10.0;
	r.right = fFindButton->Frame().left - 10.0 - B_V_SCROLL_BAR_WIDTH;
	r.bottom = r.top + (lineHeight * 2.0) + 10.0 + B_H_SCROLL_BAR_HEIGHT;
	fFindBox = new DTextView(r, "findbox", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	BScrollView *scroll = fFindBox->MakeScrollView("findscroll", true, true);
	top->AddChild(scroll);
	
	top->AddChild(fFindButton);
	
	r.OffsetTo(10.0, fFindBox->Parent()->Frame().bottom + 10.0);
	fReplaceBox = new DTextView(r, "replacebox", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	scroll = fReplaceBox->MakeScrollView("replacescroll", true, true);
	top->AddChild(scroll);
	
	fReplaceButton = new BButton(fFindButton->Bounds(), "replacebutton", "Replace",
								new BMessage(M_REPLACE), B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fReplaceButton->MoveTo(scroll->Frame().right + 10.0, scroll->Frame().top);
	top->AddChild(fReplaceButton);
	fReplaceButton->SetEnabled(false);
	
	fReplaceAllButton = new BButton(fReplaceButton->Frame(), "replaceallbutton", "Replace All",
								new BMessage(M_REPLACE_ALL), B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fReplaceAllButton->MoveBy(0.0, fReplaceAllButton->Frame().Height() + 10.0);
	top->AddChild(fReplaceAllButton);
	fReplaceAllButton->SetEnabled(false);
	
	BStringView *resultLabel = new BStringView(BRect(0,0,1,1), "resultlabel", "Results:");
	resultLabel->ResizeToPreferred();
	resultLabel->MoveTo(10.0, scroll->Frame().bottom + 5.0);
	top->AddChild(resultLabel);
	
	r = Bounds().InsetByCopy(10.0, 10.0);
	r.top = resultLabel->Frame().bottom + 5.0;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fResultList = new DListView(r, "resultlist", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL);
	scroll = fResultList->MakeScrollView("resultscroll", true, true);
	top->AddChild(scroll);
	
	BMenu *menu = new BMenu("Search");
	menu->AddItem(new BMenuItem("Find", new BMessage(M_FIND), 'F', B_COMMAND_KEY));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Replace", new BMessage(M_REPLACE), 'R', B_COMMAND_KEY));
	menu->AddItem(new BMenuItem("Replace All", new BMessage(M_REPLACE_ALL), 'R',
								B_COMMAND_KEY | B_SHIFT_KEY));
	fMenuBar->AddItem(menu);
	
	menu = new BMenu("Options");
	menu->AddItem(new BMenuItem("Regular Expression", new BMessage(M_TOGGLE_REGEX)));
	menu->AddItem(new BMenuItem("Ignore Case", new BMessage(M_TOGGLE_CASE_INSENSITIVE)));
	menu->AddItem(new BMenuItem("Match Whole Word", new BMessage(M_TOGGLE_MATCH_WORD)));
	fMenuBar->AddItem(menu);
	
	fFindBox->MakeFocus(true);
}


void
FindResultsWindow::MessageReceived(BMessage *msg)
{
	BMenuItem *item = NULL;
	switch (msg->what)
	{
		case M_FIND:
		{
			SpawnThread(THREAD_FIND);
			break;
		}
		case M_REPLACE:
		{
			SpawnThread(THREAD_REPLACE);
			break;
		}
		case M_REPLACE_ALL:
		{
			SpawnThread(THREAD_REPLACE_ALL);
			break;
		}
		case M_TOGGLE_REGEX:
		{
			fIsRegEx = !fIsRegEx;
			item = fMenuBar->FindItem("Regular Expression");
			item->SetMarked(fIsRegEx);
			break;
		}
		case M_TOGGLE_CASE_INSENSITIVE:
		{
			fMatchCase = !fMatchCase;
			item = fMenuBar->FindItem("Ignore Case");
			item->SetMarked(!fMatchCase);
			break;
		}
		case M_TOGGLE_MATCH_WORD:
		{
			fMatchWord = !fMatchWord;
			item = fMenuBar->FindItem("Match Whole Word");
			item->SetMarked(fMatchWord);
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
FindResultsWindow::SpawnThread(int8 findMode)
{
	AbortThread();
	
	fThreadID = spawn_thread(FinderThread, "search_thread", B_NORMAL_PRIORITY, this);
	if (fThreadID > 0)
	{
		fThreadMode = findMode;
		resume_thread(fThreadID);
	}
	else
		fThreadID = -1;
}


void
FindResultsWindow::AbortThread(void)
{
	if (fThreadID > 0)
	{
		atomic_add(&fThreadQuitFlag, 1);
		int32 out;
		wait_for_thread(fThreadID, &out);
		
		// fThreadQuitID and fThreadID are reset by the extra thread
	}
}


int32
FindResultsWindow::FinderThread(void *data)
{
	FindResultsWindow *win = static_cast<FindResultsWindow*>(data);
	if (win)
	{
		win->Lock();
		int8 mode = win->fThreadMode;
		win->Unlock();
		
		switch (mode)
		{
			case THREAD_FIND:
			{
				win->FindResults();
				break;
			}
			case THREAD_REPLACE:
			{
				win->Replace();
				break;
			}
			case THREAD_REPLACE_ALL:
			{
				win->ReplaceAll();
				break;
			}
			default:
				break;
		}
	}
	
	win->Lock();
	win->fThreadID = -1;
	win->fThreadQuitFlag = 0;
	win->Unlock();
	
	return 0;
}


void
FindResultsWindow::FindResults(void)
{
	// This function is called from the FinderThread function, so locking is
	// required when accessing any member variables.
	
	
}

void
FindResultsWindow::Replace(void)
{
	// This function is called from the FinderThread function, so locking is
	// required when accessing any member variables.
	
	
}

void
FindResultsWindow::ReplaceAll(void)
{
	// This function is called from the FinderThread function, so locking is
	// required when accessing any member variables.
	
	
}

