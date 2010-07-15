/*	$Id: CFindDialog.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:
	   
	    This product includes software developed by Hekkelman Programmatuur B.V.
	
	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	

	Created: long ago
*/

#include "pe.h"

#include <vector>

#include "PText.h"
#include "PDoc.h"
#include "CFindDialog.h"
#include "PMessages.h"
#include "PApp.h"
#include "PMessageWindow.h"
#include "Utils.h"
#include "HError.h"
#include "HPreferences.h"
#include "PLongAction.h"
#include "MAlert.h"
#include "HDialogViews.h"
#include "HDefines.h"
#include "Prefs.h"

const unsigned long
	msg_SelectDirectory	= 'SelO',
	msg_SelectedDir		= 'SelD',
	msg_YASD			= 'SYAD',
	msg_ChangedMFKind	= 'MFKn',
	msg_FindPopup		= 'FnPp',
	msg_Collapse		= 'clps',
	msg_FlagBackward	= 'FnBk',
	msg_FlagRegex		= 'FnRx';

enum {
	METHOD_IDX_DIR	= 0,
	METHOD_IDX_WIN	= 1,
	METHOD_IDX_INC	= 2,
	//
	FNAME_IDX_ANY	= 0,
	FNAME_IDX_END	= 1,
	FNAME_IDX_BEG	= 2,
	FNAME_IDX_CON	= 3
};

CFindDialog* gFindDialog;

bool FileContains(const char *path, const char *what, bool ignoreCase, 
				  bool word, vector<PMessageItem*> *lst = NULL);
bool BufferContains(const char *buf, int size, const char *path, 
					const char *what, bool ignoreCase, bool word, 
					vector<PMessageItem*> *lst = NULL);

bool FileContainsEx(const char *path, CRegex* regex, 
					vector<PMessageItem*> *lst = NULL);
bool BufferContainsEx(const char *buf, int size, const char *path, 
					  CRegex* regex, vector<PMessageItem*> *lst = NULL);

#pragma mark - CRecentPatternController

class CRecentPatternController
{
public:
	CRecentPatternController();
	~CRecentPatternController();
	bool AddCurrentPattern();
	void Next();
	void Previous();
	void GoToIndex(uint32 index);
	static CRecentPatternController* ActiveController();
private:
	vector<BMessage*> fPatternVect;
	uint32 fCurrIdx;
	BMessage fSavedPattern;
};

static CRecentPatternController sgRecentPatternController;

CRecentPatternController::CRecentPatternController()
{
	fCurrIdx = 0;
}

CRecentPatternController::~CRecentPatternController()
{
	for(uint32 i=0; i<fPatternVect.size(); ++i) 
		delete fPatternVect[i];
	fPatternVect.clear();
}

CRecentPatternController* CRecentPatternController::ActiveController()
{
	// TODO: later, we should double-dispatch this to the project roster,
	// such that each project could have its own set of recent patterns.
	return &sgRecentPatternController;
}

bool CRecentPatternController::AddCurrentPattern()
{
	BMessage* lastPatternMsg 
		= fPatternVect.empty() ? NULL : fPatternVect.back();
	if (!lastPatternMsg || gFindDialog->PatternInfoDiffersFrom(lastPatternMsg))
	{	// current pattern is different than last, we add it:
		BMessage* patternMsg = new BMessage();
		gFindDialog->GetPatternInfo(patternMsg);
		fPatternVect.push_back(patternMsg);
		fCurrIdx = fPatternVect.size()-1;
			// let index point to the pattern added just now
		return true;
	}
	return false;
}

void CRecentPatternController::Next()
{
	if (fSavedPattern.IsEmpty())
		return;
	if (fCurrIdx == fPatternVect.size()-1)
	{	// restore the saved pattern edited by user:
		gFindDialog->SetPatternInfo(&fSavedPattern);
	} 
	else if (fCurrIdx < fPatternVect.size()-1)
	{	// navigate downwards:
		fCurrIdx++;
		gFindDialog->SetPatternInfo(fPatternVect[fCurrIdx]);
	}
}

void CRecentPatternController::Previous()
{
	if (fCurrIdx == fPatternVect.size()-1 
	&& gFindDialog->PatternInfoDiffersFrom(fPatternVect.back()))
	{	// save pattern edited by user before navigating upwards, such
		// that the user can get back to this pattern when navigating 
		// back downwards:
		fSavedPattern.MakeEmpty();
		gFindDialog->GetPatternInfo(&fSavedPattern);
		gFindDialog->SetPatternInfo(fPatternVect[fCurrIdx]);
	}
	else if (fCurrIdx > 0)
	{
		// navigate upwards:
		fCurrIdx--;
		gFindDialog->SetPatternInfo(fPatternVect[fCurrIdx]);
	}
}

void CRecentPatternController::GoToIndex(uint32 index)
{
	if (index < 0 || index >= fPatternVect.size())
		return;
	fCurrIdx = index;
	gFindDialog->SetPatternInfo(fPatternVect[fCurrIdx]);
}

#pragma mark - CFindDialogKeyFilter

class CFindDialogKeyFilter : public BMessageFilter
{
  public:
	CFindDialogKeyFilter();

	virtual filter_result Filter(BMessage *msg, BHandler **target);
};

CFindDialogKeyFilter::CFindDialogKeyFilter()
	: BMessageFilter(B_KEY_DOWN)
{
}

filter_result CFindDialogKeyFilter::Filter(BMessage *msg, BHandler **target)
{
	int32 keychar, modifiers;
	if (*target && msg->FindInt32("modifiers", &modifiers) == B_OK 
		&& msg->FindInt32("raw_char", &keychar) == B_OK)
	{
		// if the user pressed return the given target will already
		// be set to the default button, so we have to find out the
		// focus view by accessing the preferred handler: 
		BHandler* focusHandler 
			= (*target)->Looper()->PreferredHandler();
		BTextView* textview 
			= dynamic_cast<BTextView*>(focusHandler);
		if (!textview)
			// we only want to filter keys pressed in a textview
			return B_DISPATCH_MESSAGE;
		switch(keychar)
		{
			case B_RETURN: 
				if (modifiers & (B_CONTROL_KEY | B_OPTION_KEY))
				{
					// insert a newline into the focused textview:
					char newline = '\n';
					textview->Insert(&newline, 1);
					textview->ScrollToSelection();
					return B_SKIP_MESSAGE;
				}
				break;
			case B_UP_ARROW:
				if (modifiers & (B_CONTROL_KEY | B_OPTION_KEY)) 
				{
					CRecentPatternController::ActiveController()->Previous();
					return B_SKIP_MESSAGE;
				}
				break;
			case B_DOWN_ARROW:
				if (modifiers & (B_CONTROL_KEY | B_OPTION_KEY))
				{
					CRecentPatternController::ActiveController()->Next();
					return B_SKIP_MESSAGE;
				}
				break;
		}
	}
	return B_DISPATCH_MESSAGE;
}

#pragma mark - CFindDialog

CFindDialog::CFindDialog(BRect frame, const char *name,
		window_type type, int flags, BWindow *owner)
	: HDialog(frame, name, type, flags, owner, NULL)
{
	gFindDialog = this;
	fCurrentDir = NULL;
	
	char *bi = strdup(getenv("BEINCLUDES"));
	
	int i = 1;
	const char *p = bi;
	while ((p = strchr(p + 1, ';')) != NULL)
		i++;

	fBeIncludes = (char **)malloc(i * sizeof(char *));
	FailNil(fBeIncludes);
	
	p = bi;
	i = 0;
	
	do
	{
		char *ep = strchr(p, ';');
		if (ep) *ep++ = 0;
		
		fBeIncludes[i] = strdup(p);
		FailNil(fBeIncludes[i++]);
		p = ep;
	}
	while (p);
	
	free(bi);
	fBeIncludeCount = i;
	
	fDirPanel = NULL;
	
	Create();
	Layout();

	UpdateSearchDirMenu();

	fOpenWindowIndex = -1;
	
	UpdateFields();
} /* CFindDialog::CFindDialog */

void CFindDialog::Create(void)
{
	fMainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// Add Buttons
	fButFind	= new HButton(fMainView, "bfnd", 'Find', H_FOLLOW_RIGHT_TOP);
	fButRepl	= new HButton(fMainView, "brpl", 'Rplc', H_FOLLOW_RIGHT_TOP);
	fButRepF	= new HButton(fMainView, "br&f", 'RpFn', H_FOLLOW_RIGHT_TOP);
	fButRepA	= new HButton(fMainView, "ball", 'RpAl', H_FOLLOW_RIGHT_TOP);
	fButFind->MakeDefault(true);

	// Add Search and Replace fields
	fMfdPats = new HMenuField(fMainView, "pats");
	fEdiFind = new HTextView(NULL, "find", B_FOLLOW_ALL);
	fEdiFind->SetWordWrap(false);
	fScrFind = new HScrollView(fMainView, "findScr", fEdiFind);

	fLabRepl = new HStringView(fMainView);
	fEdiRepl = new HTextView(NULL, "repl", B_FOLLOW_ALL);
	fEdiRepl->SetWordWrap(false);
	fScrRepl = new HScrollView(fMainView, "replScr", fEdiRepl);

	// Add Settings
	fChkCase = new HCheckBox(fMainView, "case", NULL, H_FOLLOW_LEFT_BOTTOM);
	fChkWrap = new HCheckBox(fMainView, "wrap", NULL, H_FOLLOW_LEFT_BOTTOM);
	fChkBack = new HCheckBox(fMainView, "back", new BMessage(msg_FlagBackward), 
		H_FOLLOW_LEFT_BOTTOM);
	fChkWord = new HCheckBox(fMainView, "word", NULL, H_FOLLOW_LEFT_BOTTOM);
	fChkGrep = new HCheckBox(fMainView, "regx", new BMessage(msg_FlagRegex), 
		H_FOLLOW_LEFT_BOTTOM);
	fChkBtch = new HCheckBox(fMainView, "btch", NULL, H_FOLLOW_LEFT_BOTTOM);
	fChkCase->SetOn(gPrefs->GetPrefInt(prf_I_SearchIgnoreCase, 1));
	fChkWrap->SetOn(gPrefs->GetPrefInt(prf_I_SearchWrap, 1));
	fChkBack->SetOn(gPrefs->GetPrefInt(prf_I_SearchBackwards, 0));
	fChkWord->SetOn(gPrefs->GetPrefInt(prf_I_SearchEntireWord, 0));
	fChkBtch->SetOn(gPrefs->GetPrefInt(prf_I_SearchBatch, 0));
	fChkGrep->SetOn(gPrefs->GetPrefInt(prf_I_SearchWithGrep, 0));

	// Add Multifile Search
	fBoxMult = new HBox(fMainView, "", H_FOLLOW_LEFT_RIGHT_BOTTOM);
	fChkMult = new HCheckBox(fMainView, "mult", NULL, H_FOLLOW_LEFT_BOTTOM);
	//
	fMfdMeth = new HMenuField(fMainView, "meth", H_FOLLOW_LEFT_BOTTOM, false, 1000);
	fMfdMeth->Menu()->SetLabelFromMarked(true);
	fMitMethDir = fMfdMeth->AddMenuItem(msg_ChangedMFKind, METHOD_IDX_DIR);
	fMitMethWin = fMfdMeth->AddMenuItem(msg_ChangedMFKind, METHOD_IDX_WIN);
	fMitMethInc = fMfdMeth->AddMenuItem(msg_ChangedMFKind, METHOD_IDX_INC);
	BMenuItem *item = fMfdMeth->Menu()->ItemAt(gPrefs->GetPrefInt(prf_I_SearchMultikind, 0));
	(item ? item : fMfdMeth->Menu()->ItemAt(METHOD_IDX_DIR))->SetMarked(true);
	//
	fChkText = new HCheckBox(fMainView, "text", NULL, H_FOLLOW_LEFT_BOTTOM);
	fChkRecu = new HCheckBox(fMainView, "recu", NULL, H_FOLLOW_LEFT_BOTTOM);
	fChkRecu->SetOn(gPrefs->GetPrefInt(prf_I_SearchRecursive, 1));
	fChkText->SetOn(gPrefs->GetPrefInt(prf_I_SearchTextFilesOnly, 1));
	//
	fMfdSdir = new HMenuField(fMainView, "sdir", H_FOLLOW_LEFT_BOTTOM, false, 1000);
	fMfdSdir->Menu()->SetLabelFromMarked(true);
	fMfdSdir->Menu()->AddItem(fMitSdirOth = new BMenuItem("", new BMessage(msg_SelectDirectory)));
	//
	fMfdNamp = new HMenuField(fMainView, "namp", H_FOLLOW_LEFT_BOTTOM, false, 1000);
	fMfdNamp->Menu()->SetLabelFromMarked(true);
	fMitNampAny = fMfdNamp->AddMenuItem(msg_ChangedMFKind, FNAME_IDX_ANY);
	fMitNampEnd = fMfdNamp->AddMenuItem(msg_ChangedMFKind, FNAME_IDX_END);
	fMitNampBeg = fMfdNamp->AddMenuItem(msg_ChangedMFKind, FNAME_IDX_BEG);
	fMitNampCon = fMfdNamp->AddMenuItem(msg_ChangedMFKind, FNAME_IDX_CON);
	fMfdNamp->Menu()->ItemAt(FNAME_IDX_ANY)->SetMarked(true);

	fEdiName = new HTextControl(fMainView, "name", H_FOLLOW_LEFT_BOTTOM);

	AddCommonFilter(new CFindDialogKeyFilter());
} // CFindDialog::Create

void CFindDialog::Layout(void) {
	fButFind	->ResizeLocalized("Find");
	fButRepl	->ResizeLocalized("Replace");
	fButRepF	->ResizeLocalized("Replace & Find");
	fButRepA	->ResizeLocalized("Replace All");
	fMfdPats	->ResizeLocalized(NULL, "Find:");
	fLabRepl	->ResizeLocalized("Replace:");
	fChkCase	->ResizeLocalized("Ignore Case");
	fChkWrap	->ResizeLocalized("Wrap Around");
	fChkBack	->ResizeLocalized("Backwards");
	fChkWord	->ResizeLocalized("Entire Word");
	fChkGrep	->ResizeLocalized("Regex");
	fChkBtch	->ResizeLocalized("Batch");
	fChkMult	->ResizeLocalized("Multi-File:");
	fMfdMeth	->ResizeLocalized();
	fMitMethDir	->SetLabel("Directory Scan");
	fMitMethWin	->SetLabel("Open Windows");
	fMitMethInc	->SetLabel("$BEINCLUDES");
	fChkText	->ResizeLocalized("Text Files Only");
	fChkRecu	->ResizeLocalized("Recursive");
	fMfdSdir	->ResizeLocalized("Search In:");
	fMitSdirOth	->SetLabel("Other…");
	fMfdNamp	->ResizeLocalized("File Name:");
	fMitNampAny	->SetLabel("Any");
	fMitNampEnd	->SetLabel("Ends with");
	fMitNampBeg	->SetLabel("Begins with");
	fMitNampCon	->SetLabel("Contains");
	fEdiName	->ResizeLocalized();

	// Hack: Somehow the Label isn't set (SetLabelFromMarked()) if we don't do this
	fMfdNamp->Menu()->FindMarked()->SetMarked(true);
	fMfdMeth->Menu()->FindMarked()->SetMarked(true);

	// Widths
	float w, h, wm, wd;
	float dx = fMainView->StringWidth("m");
	float dy = fMainView->StringWidth("n");
	BRect r = fMainView->Bounds();

	// Position Buttons
	w = std::max(fButFind->Width(), fButRepl->Width());
	w = std::max(w, fButRepF->Width());
	w = std::max(w, fButRepA->Width());

	fButFind->MoveTo(fMainView->Right()-w-dx, dy);
	fButFind->SetWidth(w);
	fButRepl->MoveTo(fButFind->Left(), fButFind->Bottom()+dy);
	fButRepl->SetWidth(w);
	fButRepF->MoveTo(fButRepl->Left(), fButRepl->Bottom()+dy);
	fButRepF->SetWidth(w);
	fButRepA->MoveTo(fButRepF->Left(), fButRepF->Bottom()+dy);
	fButRepA->SetWidth(w);

	// Position Inputs+Labels
	w = fButFind->Left()-2*dx;

	fMfdPats->MoveTo(dx, dy);

	float lineHeight = fEdiFind->LineHeight();

	fScrFind->MoveTo(dx, fMfdPats->Bottom()+1);
	fScrFind->ResizeTo(w, lineHeight*2+6);

	fLabRepl->MoveTo(dx, fScrFind->Bottom()+dy);

	fScrRepl->MoveTo(dx, fLabRepl->Bottom()+1);
	fScrRepl->ResizeTo(w, lineHeight*2+6);

	// Multi File Search
	w = std::max(fChkText->Width(), fChkRecu->Width());
	wd = std::max(fMfdNamp->Divider(), fMfdSdir->Divider());
	fMfdSdir->SetDivider(wd);
	fMfdNamp->SetDivider(wd);

	fChkText->MoveTo(dx, r.bottom-fChkText->Height()-dy);
	fChkRecu->MoveAbove(fChkText, 2);

	wm = std::max(fMfdNamp->StringWidth(fMitNampAny->Label()), fMfdNamp->StringWidth(fMitNampEnd->Label()));
	wm = std::max(wm, fMfdNamp->StringWidth(fMitNampBeg->Label()));
	wm = std::max(wm, fMfdNamp->StringWidth(fMitNampCon->Label()));
	wm += fMfdNamp->StringWidth("WW");

	fMfdNamp->MoveTo(2*dx+w, fChkText->Top()-2);
	fEdiName->MoveTo(fMfdNamp->Left()+wd+wm, fMfdNamp->Top()+2);

	fMfdSdir->MoveTo(2*dx+w, fChkRecu->Top()-2);
	fMfdSdir->MenuBar()->SetMaxContentWidth(r.right-fMfdSdir->Left()-fMfdSdir->Divider()-2*dx-30);

	wm = std::max(fMfdMeth->StringWidth(fMitMethDir->Label()), fMfdMeth->StringWidth(fMitMethWin->Label()));
	wm = std::max(wm, fMfdMeth->StringWidth(fMitMethInc->Label()));
	wm += fMfdMeth->StringWidth("WW");

	fChkMult->MoveAbove(fChkRecu, dy);
	fMfdMeth->MoveTo(fChkMult->Right()+dy, fChkMult->Top()-2);
	fBoxMult->MoveTo(fChkMult->Right()+dx+wm, fChkMult->Top()+fChkMult->Height()/2);
	fBoxMult->ResizeTo(r.Width()-fChkMult->Right()-2*dx-wm, 1);

	// Checkboxes
	fChkWrap->MoveAbove(fChkMult, dy);
	fChkCase->MoveAbove(fChkWrap);

	fChkWord->MoveTo(std::max(fChkCase->Right(), fChkWrap->Right())+dx, fChkWrap->Top());
	fChkBack->MoveAbove(fChkWord);

	fChkBtch->MoveTo(std::max(fChkBack->Right(), fChkWord->Right())+dx, fChkWrap->Top());
	fChkGrep->MoveAbove(fChkBtch);

	// set minimum height to fit one line in textviews...
	h = fScrRepl->Bottom()+r.bottom-fChkCase->Top()+2;
	w = 2*dx+std::max(fChkGrep->Right(), fChkBtch->Right())+fButRepA->Width();
	ResizeToLimits(w, 99999, h-2*lineHeight, 99999);
	// ...but default to two lines in textviews:
	ResizeTo(w, h);
} // CFindDialog::Layout

bool CFindDialog::QuitRequested()
{
	while (!IsHidden())
		Hide();

	gPrefs->SetPrefInt(prf_I_SearchBackwards,		fChkBack->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchEntireWord,		fChkWord->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchIgnoreCase,		fChkCase->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchWrap,			fChkWrap->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchTextFilesOnly,	fChkText->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchWithGrep,		fChkGrep->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchBatch,			fChkBtch->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchRecursive,		fChkRecu->IsOn());
	gPrefs->SetPrefInt(prf_I_SearchMultikind,		fMfdMeth->FindMarkedIndex());

	return CDoc::CountDocs() == 0;
} /* CFindDialog::QuitRequested */

void CFindDialog::DoFind(unsigned long cmd)
{
	if (!strlen(fEdiFind->Text()))
		return;

	// add this pattern to the list of recently used patterns:
	if (CRecentPatternController::ActiveController()->AddCurrentPattern())
		AddCurrentPatternToFindPopup(cmd != msg_Find);

	if (fChkGrep->IsOn() && (cmd == msg_Find || cmd == msg_ReplaceAll))
	{
		status_t res = fRegex.SetTo(fEdiFind->Text(), fChkCase->IsOn(), 
									fChkWord->IsOn());
		if (res != B_OK)
		{
			MWarningAlert a(fRegex.ErrorStr().String());
			a.Go();
			return;
		}
	}
	
	if (fChkMult->IsOn())
	{
		switch (fMfdMeth->FindMarkedIndex())
		{
			case METHOD_IDX_DIR:
			{
				PDoc *w = NULL;
				
				MultiReplaceKind all = mrNone;
				
				if (cmd == msg_ReplaceAll)
				{
					switch (MAlert("Replace All on multiple files", "Cancel", "Leave Open", "Save To Disk").Go())
					{
						case 1:	return;
						case 2:	all = mrLeaveOpen; break;
						case 3:	all = mrSaveToDisk; break;
					}
				}
				
				if (!DoMultiFileFind(fMfdSdir->Menu()->FindMarked()->Label(),
					fChkRecu->IsOn(), true, all, &w))
					beep();
				else if (w)
					w->Show();
				break;

			}

			case METHOD_IDX_WIN:
				fOpenWindowIndex = -1;
				DoOpenWindows(cmd == msg_ReplaceAll);
				break;

			case METHOD_IDX_INC:
				fCurrentIncludeIndex = -1;
				DoIncludesFind();
				break;
		}
	}
	else if (fChkBtch->IsOn())
	{
		PDoc *doc = PDoc::TopWindow();
		if (doc)
		{
			vector<PMessageItem*>* lst = new vector<PMessageItem*>;
			FindInFile(*doc->EntryRef(), lst);
			BWindow* w = NULL;
			ShowBatch(lst, &w);
			delete lst;
		}
	}
	else
	{
		BMessage msg(cmd);
		
		msg.AddString("what", fEdiFind->Text());
		msg.AddString("with", fEdiRepl->Text());
		msg.AddBool  ("wrap", fChkWrap->IsOn());
		msg.AddBool  ("case", fChkCase->IsOn());
		msg.AddBool  ("word", fChkWord->IsOn());
		msg.AddBool  ("back", fChkBack->IsOn());
		msg.AddBool  ("regx", fChkGrep->IsOn());

		PDoc *w = PDoc::TopWindow();

		if (w)
			w->PostMessage(&msg, w->TextView(), this);
	}
} /* CFindDialog::DoFind */

void CFindDialog::WindowActivated(bool active)
{
	HDialog::WindowActivated(active);
	UpdateFields();
	fEdiFind->MakeFocus();
} /* CFindDialog::WindowActivated */

void CFindDialog::UpdateFields()
{
	PDoc *w = PDoc::TopWindow();

	if (w && strlen(fEdiFind->Text()))
	{
		BMessage query(msg_QueryCanReplace);
		query.AddString("what", fEdiFind->Text());
		query.AddBool("case", fChkCase->IsOn());
		query.AddBool("regx", fChkGrep->IsOn());
		w->PostMessage(&query, w->TextView(), this);
	}
	else
	{
		fButRepl->SetEnabled(false);
		fButRepF->SetEnabled(false);
	}

	if (fChkMult->IsOn())
	{
		int meth = fMfdMeth->FindMarkedIndex();

		fMfdMeth->SetEnabled(true);
		fMfdSdir->SetEnabled(meth == METHOD_IDX_DIR);
		fEdiName->SetEnabled(meth == METHOD_IDX_DIR);
		fMfdNamp->SetEnabled(meth == METHOD_IDX_DIR);
		fChkText->SetEnabled(meth == METHOD_IDX_DIR);
		fChkRecu->SetEnabled(meth == METHOD_IDX_DIR || meth == METHOD_IDX_INC);
		
		if (meth == METHOD_IDX_INC)
			fChkText->SetOn(true);

		int nmKind = fMfdNamp->FindMarkedIndex();
		fEdiName->SetEnabled(nmKind != FNAME_IDX_ANY);
	}
	else
	{
		fMfdMeth->SetEnabled(false);
		fMfdSdir->SetEnabled(false);
		fChkText->SetEnabled(false);
		fEdiName->SetEnabled(false);
		fMfdNamp->SetEnabled(false);
		fChkRecu->SetEnabled(false);
		
		fCurrentDir = NULL;
		while (fDirStack.size())
		{
			closedir(fDirStack.top());
			fDirStack.pop();
			
			char *p = fDirNameStack.top();
			free(p);
			fDirNameStack.pop();
		}

		fCurrentIncludeIndex = -1;
		fOpenWindowIndex = -1;
	}

	fButRepA->SetEnabled(!(fChkMult->IsOn() && fMfdMeth->FindMarkedIndex() == METHOD_IDX_INC));
} /* CFindDialog::UpdateFields */

void CFindDialog::MessageReceived(BMessage *msg)
{
	try
	{
		long what = msg->what;

		if (msg->WasDropped() && msg->HasRef("refs"))
		{
			entry_ref ref;
			FailOSErr(msg->FindRef("refs", &ref));
			BEntry e(&ref);
			
			if (!e.IsDirectory())
				BEntry(&ref).GetParent(&e); // nasty huh?

			fMfdMeth->Menu()->ItemAt(METHOD_IDX_DIR)->SetMarked(true);

			fChkMult->SetOn(true);
			BPath path;
			e.GetPath(&path);
			
			AddPathToDirMenu(path.Path(), true);
			UpdateFields();
		}
		else switch (what)
		{
			case msg_Find:
			case msg_Replace:
			case msg_ReplaceAndFind:
			case msg_ReplaceAll:
				DoFind(what);
				PostMessage(B_QUIT_REQUESTED);
				break;
	
			case msg_ReplyCanReplace:
			{
				bool canReplace;
				if (msg->FindBool("canreplace", &canReplace) == B_NO_ERROR)
				{
					fButRepl->SetEnabled(canReplace);
					fButRepF->SetEnabled(canReplace);
				}
				break;
			}
			
			case msg_EnterSearchString:
			case msg_FindSelection:
			case msg_FindSelectionBackward:
			{
				fChkMult->SetOn(false);
				fChkGrep->SetOn(false);
				UpdateFields();
				const char *s;
				if (msg->FindString("string", &s) == B_NO_ERROR)
					fEdiFind->SetText(s);
				if (what == msg_FindSelection)
					DoFind(msg_Find);
				else if (what == msg_FindSelectionBackward)
					DoFind(msg_FindAgainBackward);
				break;
			}
				
			case msg_EnterReplaceString:
			{
				fChkMult->SetOn(false);
				fChkGrep->SetOn(false);
				UpdateFields();
				const char *s;
				if (msg->FindString("string", &s) == B_NO_ERROR)
					fEdiRepl->SetText(s);
				break;
			}
			
			case msg_FindInNextFile:
			{
				switch (fMfdMeth->FindMarkedIndex())
				{
					case METHOD_IDX_DIR:
					{
						PDoc *w = NULL;
						if (!DoMultiFileFind(fMfdSdir->Menu()->FindMarked()->Label(),
							fChkRecu->IsOn(), false, mrNone, &w))
							beep();
						else if (w)
							w->Show();
						break;
					}
					
					case METHOD_IDX_WIN:
						DoOpenWindows(false);
						break;
					
					case METHOD_IDX_INC:
						DoIncludesFind();
						break;
				}
				break;
			}
			case msg_SelectDirectory:
				if (!fDirPanel)
				{
					BMessage msg(msg_SelectedDir);
					fDirPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
						B_DIRECTORY_NODE, false, &msg);
				}
				fDirPanel->Show();
				break;
			
			case msg_SelectedDir:
			{
				entry_ref ref;
				if (msg->FindRef("refs", &ref) == B_NO_ERROR)
					AddPathToDirMenu(ref, true, true);
				break;
			}
			case msg_YASD:
				gPrefs->SetPrefInt(prf_I_SearchWhichDir, fMfdSdir->FindMarkedIndex());
			case msg_ChangedMFKind:
				fChkMult->SetOn(true);
				UpdateFields();
				break;
			
			case msg_FindPopup:
			{
				long ix;
				FailOSErr(msg->FindInt32("index", &ix));
				
				CRecentPatternController::ActiveController()->GoToIndex(ix);
				break;
			}

			case msg_PrefsChanged:
				UpdateSearchDirMenu();
				break;

			case msg_FlagBackward:
			{
				fChkGrep->SetOn(false);
				UpdateFields();
				break;
			}

			case msg_FlagRegex:
			{
				fChkBack->SetOn(false);
				UpdateFields();
				break;
			}

			default:
				HDialog::MessageReceived(msg);
				break;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* CFindDialog::MessageReceived */

void CFindDialog::FrameResized(float width, float height)
{
	HDialog::FrameResized(width, height);
	float dx = fMainView->StringWidth("m");
	float dy = fMainView->StringWidth("n");
	float h = (fChkCase->Top()-fMfdPats->Bottom()-fLabRepl->Height()-dy-4)/2;
	float w = fButFind->Left()-dx-fScrFind->Left();

	fScrFind->ResizeTo(w,h);
	fScrRepl->ResizeTo(w,h);

	fLabRepl->MoveBelow(fScrFind, dy);
	fScrRepl->MoveBelow(fLabRepl, 1);

	fMfdSdir->MenuBar()->SetMaxContentWidth(Bounds().right-fMfdSdir->Left()-fMfdSdir->Divider()-30);
} /* CFindDialog::FrameResized */

void CFindDialog::UpdateSearchDirMenu()
{
	// remove old directory entries

	for (int i = fMfdSdir->Menu()->CountItems(); i-- > 1;)
	{
		delete fMfdSdir->Menu()->RemoveItem(i);
	}

	// add new ones...

	const char *path;
	int i = 0;
	while ((path = gPrefs->GetIxPrefString(prf_X_SearchPath, i++)) != NULL)
		AddPathToDirMenu(path, false);

	// ... and (re)select the current entry

	BMenuItem *item = fMfdSdir->Menu()->ItemAt(gPrefs->GetPrefInt(prf_I_SearchWhichDir, 0));
	if (item == NULL)
		item = fMfdSdir->Menu()->ItemAt(0);

	item->SetMarked(true);

} /* CFindDialog::UpdateSearchDirMenu */

void CFindDialog::AddPathToDirMenu(const char *path, bool select, bool addToPrefs)
{
	int i;

	for (i = 2; i < fMfdSdir->Menu()->CountItems(); i++)
	{
		if (!strcmp(fMfdSdir->Menu()->ItemAt(i)->Label(), path))
		{
			if (select) fMfdSdir->Menu()->ItemAt(i)->SetMarked(true);
			return;
		}
	}

	if (fMfdSdir->Menu()->CountItems() == 1)
		fMfdSdir->Menu()->AddSeparatorItem();

	fMfdSdir->Menu()->AddItem(new BMenuItem(path, new BMessage(msg_YASD)));

	i = fMfdSdir->Menu()->CountItems() - 1;

	if (select)
		fMfdSdir->Menu()->ItemAt(i)->SetMarked(true);

	if (addToPrefs && i >= 2)
		gPrefs->SetIxPrefString(prf_X_SearchPath, i - 2, path);
} /* CFindDialog::AddPathToDirMenu */

void CFindDialog::AddPathToDirMenu(entry_ref& ref, bool select, bool addToPrefs)
{
	BEntry e;
	FailOSErr(e.SetTo(&ref));
	BPath p;
	FailOSErr(e.GetPath(&p));
	
	AddPathToDirMenu(p.Path(), select, addToPrefs);
} /* CFindDialog::AddPathToDirMenu */

const char* CFindDialog::FindString()
{
	BAutolock lock(this);
	return fEdiFind->Text();
} /* CFindDialog::FindString */

const char* CFindDialog::ReplaceString()
{
	BAutolock lock(this);
	return fEdiRepl->Text();
} /* CFindDialog::FindString */

char* CFindDialog::RxReplaceString(const char* what, int32 len)
{
	return fRegex.ReplaceString(what, len, ReplaceString());
}

#pragma mark - Pattern Info

static const char* patternFlags[] = {
	"back",
	"btch",
	"case",
	"mult",
	"recu",
	"regx",
	"text",
	"word",
	"wrap",
	NULL
};

void CFindDialog::GetPatternInfo(BMessage* patternMsg)
{
	patternMsg->AddString("find", fEdiFind->Text());
	patternMsg->AddString("repl", fEdiRepl->Text());
	for( int f=0; patternFlags[f]; ++f)
		patternMsg->AddBool(patternFlags[f], IsOn( patternFlags[f]));
	patternMsg->AddInt32("meth", fMfdMeth->FindMarkedIndex());
	patternMsg->AddString("sdir", fMfdSdir->Menu()->FindMarked()->Label());
	patternMsg->AddInt32("namp", fMfdNamp->FindMarkedIndex());
	patternMsg->AddString("name", fEdiName->Text());
}

void CFindDialog::SetPatternInfo(const BMessage* patternMsg)
{
	fEdiFind->SetText(patternMsg->FindString( "find"));
	fEdiRepl->SetText(patternMsg->FindString( "repl"));
	for( int f=0; patternFlags[f]; ++f)
		SetOn(patternFlags[f], patternMsg->FindBool(patternFlags[f]));
	BMenuItem* item = fMfdMeth->Menu()->ItemAt(patternMsg->FindInt32("meth"));
	if (item)
		item->SetMarked(true);
	item = fMfdSdir->Menu()->FindItem(patternMsg->FindString( "sdir"));
	if (item)
		item->SetMarked(true);
	item = fMfdNamp->Menu()->ItemAt(patternMsg->FindInt32( "namp"));
	if (item)
		item->SetMarked(true);
	fEdiName->SetText(patternMsg->FindString( "name"));
	UpdateFields();
}

bool CFindDialog::PatternInfoDiffersFrom(const BMessage* patternMsg)
{
	bool res = false;
	if (strcmp(fEdiFind->Text(), patternMsg->FindString( "find")) != 0
	|| strcmp(fEdiRepl->Text(), patternMsg->FindString( "repl")) != 0)
		res = true;
	for( int f=0; !res && patternFlags[f]; ++f)
		if (IsOn(patternFlags[f]) != patternMsg->FindBool(patternFlags[f]))
			res = true;
	return res;
}

void CFindDialog::AddCurrentPatternToFindPopup(bool showReplaceText)
{
	BString label = fEdiFind->Text();
	label.ReplaceAll("\n", "\\n");
	if (label.Length() > 60) {
		label.Truncate(60);
		label << B_UTF8_ELLIPSIS;
	}
	if (showReplaceText)
	{
		BString repl = fEdiRepl->Text();
		repl.ReplaceAll("\n", "\\n");
		label << " ---> " << repl;
		if (label.Length() > 80) {
			label.Truncate(80);
			label << B_UTF8_ELLIPSIS;
		}
	}
	BMenuItem* item 
		= new BMenuItem(label.String(), new BMessage(msg_FindPopup));
	fMfdPats->Menu()->AddItem(item);
}

#pragma mark - Multi file

bool CFindDialog::DoMultiFileFind(const char *dir, bool recursive, bool restart, MultiReplaceKind replace, PDoc **w)
{
	char path[PATH_MAX];
	bool batch = fChkBtch->IsOn();
	vector<PMessageItem*> *lst = NULL;
	
	if (batch && replace == mrNone)
		lst = new vector<PMessageItem*>;

	if (restart)
	{
		while (fDirStack.size())
		{
			closedir(fDirStack.top());
			fDirStack.pop();

			free(fDirNameStack.top());
			fDirNameStack.pop();
		}
		fCurrentDir = opendir(dir);
		if (!fCurrentDir) FailOSErr(errno);
		fDirStack.push(fCurrentDir);
		fDirNameStack.push(strdup(dir));
	}

	if (!fCurrentDir) THROW(("Directory not found: %s", dir));
	
	struct dirent *dent;
	struct stat stbuf;
	
	PLongAction la;
	bool cancel = false;
	
	while (fDirStack.size() && ! cancel)
	{
		fCurrentDir = fDirStack.top();
		dir = fDirNameStack.top();
		
		while ((dent = readdir(fCurrentDir)) != NULL && ! cancel)
		{
			strcpy(path, dir);
			strcat(path, "/");
			strcat(path, dent->d_name);
			status_t err = stat(path, &stbuf);
			
			if (err || !(strcmp(dent->d_name, ".") && strcmp(dent->d_name, "..")))
				continue;
			
			if (S_ISREG(stbuf.st_mode))
			{
				entry_ref ref;
				if (GetRefForPath(ref, path) && FindInFile(ref, lst))
				{
					if (replace == mrLeaveOpen)
					{
						PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref));
						if (doc)
						{
							BAutolock lock(doc);
		
							doc->TextView()->Find(msg_ReplaceAll, NULL);
						}
					}
					else if (replace == mrSaveToDisk)
					{
						PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref, false));
						if (doc)
						{
							doc->Lock();
		
							doc->TextView()->Find(msg_ReplaceAll, NULL);
							
							if (doc->IsHidden())
							{
								doc->Save();
								doc->Quit();
							}
							else
								doc->Unlock();
						}
					}
					else if (replace == mrNone && !batch)
					{
						PDoc *doc = dynamic_cast<PDoc*>(gApp->OpenWindow(ref));
						if (doc)
						{
							BAutolock lock(doc);
		
							int offset = 0;
							
							doc->TextView()->FindNext(fEdiFind->Text(),
								offset, fChkCase->IsOn(), false, false, false, 
								fChkGrep->IsOn(), true);
							return true;
						}
					}
				}
			}
			else if (S_ISDIR(stbuf.st_mode) && recursive)
			{
				DIR *r = opendir(path);
				if (!r) FailOSErr(errno);
				fDirStack.push(r);
				fDirNameStack.push(strdup(path));
				fCurrentDir = NULL;
				break;
			}
			
			cancel = la.Tick();
		}

		if (fCurrentDir)
		{
			closedir(fDirStack.top());
			fDirStack.pop();
			fCurrentDir = NULL;
			
			free(fDirNameStack.top());
			fDirNameStack.pop();
			dir = NULL;
		}
	}
	
	// not found... or batch of course!
	if (lst)
	{
		ShowBatch(lst, (BWindow**)w);
		delete lst;
	}
	
	return false;
} /* CFindDialog::DoMultiFileFind */

bool CFindDialog::GetRefForPath(entry_ref& ref, const char *path)
{
	bool result = true;
	FailOSErr(get_ref_for_path(path, &ref));

	if (fChkText->IsOn())
	{
		BNode n;
		
		FailOSErr(n.SetTo(&ref));

		char mime[64];
		BNodeInfo(&n).GetType(mime);

		if (strncmp(mime, "text/", 5))
			result = false;
	}
	
	int fnam = fMfdNamp->FindMarkedIndex();
	if (result && fnam != FNAME_IDX_ANY)
	{
		const char *pat = fEdiName->Text();
		char *file = strrchr(path, '/') + 1;
	
		switch (fnam)
		{
			case 1:	result = strcmp(file + strlen(file) - strlen(pat), pat) == 0; break;
			case 2: result = strncmp(file, pat, strlen(pat)) == 0; break;
			case 3: result = strstr(file, pat) != NULL; break;
		}
	}

	return result;
} /* CFindDialog::GetRefForPath */

bool CFindDialog::FindInFile(const entry_ref& ref, vector<PMessageItem*> *lst)
{
	bool found;
	bool word = fChkWord->IsOn();
	const char *what = fEdiFind->Text();
	PDoc *doc;

	BEntry e;
	FailOSErr(e.SetTo(&ref));
	BPath p;
	FailOSErr(e.GetPath(&p));
	const char *path = p.Path();
	
	doc = dynamic_cast<PDoc*>(CDoc::FindDoc(ref));

	if (doc)
	{
		BAutolock lock(doc);

		PText *txt = doc->TextView();
		if (fChkGrep->IsOn())
			found = BufferContainsEx(txt->Text(), txt->Size(), path,
									 &fRegex, lst);
		else
			found = BufferContains(txt->Text(), txt->Size(), path, what, 
								   fChkCase->IsOn(), word, lst);
	}
	else if (fChkGrep->IsOn())
		found = FileContainsEx(path, &fRegex, lst);
	else
		found = FileContains(path, what, fChkCase->IsOn(), word, lst);

	return found;
} /* CFindDialog::FindInFile */

void CFindDialog::DoIncludesFind()
{
	PDoc *w = NULL;
	
	if (fCurrentIncludeIndex > -1)
	{
		if (fCurrentDir && DoMultiFileFind(fBeIncludes[fCurrentIncludeIndex], false, false, mrNone, &w))
			return;
		else
			fCurrentIncludeIndex++;
	}
	else
		fCurrentIncludeIndex = 0;
		
	do
	{
		if (DoMultiFileFind(fBeIncludes[fCurrentIncludeIndex], false, true, mrNone, &w))
			return;
	}
	while (++fCurrentIncludeIndex < fBeIncludeCount);

	if (w)
		w->Show();
		
	fCurrentIncludeIndex = -1;
	beep();
} /* CFindDialog::DoBeIncludesFind */

void CFindDialog::DoOpenWindows(bool replace)
{
	if (fChkBtch->IsOn())
	{
		int i = be_app->CountWindows();
		vector<PMessageItem*>* lst = new vector<PMessageItem*>;
		while (i--)
		{
			PDoc *doc = dynamic_cast<PDoc*>(be_app->WindowAt(i));
			if (doc)
				FindInFile(*doc->EntryRef(), lst);
		}

		BWindow *w = NULL;
		ShowBatch(lst, &w);
		delete lst;
	}
	else
	{
		if (fOpenWindowIndex == -1)
		{
			fOpenWindows.clear();
	
			int i = be_app->CountWindows();
			
			while (i--)
			{
				PDoc *doc = dynamic_cast<PDoc*>(be_app->WindowAt(i));
				if (doc)
					fOpenWindows.push_back(doc);
			}
			
			fOpenWindowIndex = fOpenWindows.size();
		}
	
		while (--fOpenWindowIndex >= 0)
		{
			PDoc* doc = fOpenWindows[fOpenWindowIndex];

			const entry_ref& file = *doc->EntryRef();

			if (doc && FindInFile(file, NULL))
			{
				BAutolock lock(doc);
				
				if (!lock.IsLocked())
					continue;
				doc->Activate();
				if (replace)
					doc->TextView()->Find(msg_ReplaceAll, NULL);
				else
				{
					int offset = 0;
					doc->TextView()->FindNext(fEdiFind->Text(), offset,
						fChkCase->IsOn(), false, false, fChkWord->IsOn(), 
						fChkGrep->IsOn(), true);
					return;
				}
			}
		}
		beep();
	}
	
} /* CFindDialog::DoOpenWindows */

void CFindDialog::ShowBatch(vector<PMessageItem*> *lst, BWindow** w)
{
	if (lst && lst->size())
	{
		PMessageWindow *m;
		
		if (w && *w)
			m = static_cast<PMessageWindow*>(*w);
		else
			m = new PMessageWindow("Search Results");

		BAutolock lock(m);
		
		if (lock.IsLocked())
		{
			if (w) *w = m;
	
			for (int i = 0; i < lst->size(); i++)
				m->AddMessage(static_cast<PMessageItem*>((*lst)[i]));
	
			if (w && *w)
				m->Show();
		}
	}
} /* CFindDialog::ShowBatch */

#pragma mark - Find


void initskip(const char *p, int skip[], bool ignoreCase)
{
	for (int i = 0; i < 255; i++)
		skip[i] = 1;
		
	// [zooey]: temporary fix, as the current code isn't able to deal
	//          with utf-8 chars correctly.

#if 0
	int M = strlen((char *)p), i;
	
	for (i = 0; i < 255; i++)
		skip[i] = M;
	
	if (ignoreCase)
	{
		for (i = 0; i < M; i++)
			skip[toupper(p[i])] = M - i - 1;
	}
	else
	{
		for (i = 0; i < M; i++)
			skip[p[i]] = M - i - 1;
	}
#endif
} /* initskip */

int mismatchsearch(const char *p, const char *a, int N, int skip[], bool ignoreCase)
{
	ASSERT(p);
	ASSERT(a);
	ASSERT(skip);
	int i, j, t, M = strlen(p);
	unsigned char c;

	if (ignoreCase)
	{
		for (i = M - 1, j = M - 1; j >= 0; i--, j--)
		{
			while (toupper(a[i]) != toupper(p[j]))
			{
				c = toupper(a[i]);
				t = skip[c];
				i += (M - j > t) ? M - j : t;
				if (i >= N)
					return N;
				j = M - 1;
			}
		}
	}
	else
	{
		for (i = M - 1, j = M - 1; j >= 0; i--, j--)
		{
			while (a[i] != p[j])
			{
				c = a[i];
				t = skip[c];
				i += (M - j > t) ? M - j : t;
				if (i >= N)
					return N;
				j = M - 1;
			}
		}
	}
	return i;
} /* mismatchsearch */

void initskip_b(const char*p, int skip[], bool ignoreCase)
{
	for (int i = 0; i < 255; i++)
		skip[i] = 1;
		
	// [zooey]: temporary fix, as the current code isn't able to deal
	//          with utf-8 chars correctly.

#if 0
	int M = strlen((char *)p), i;
	
	for (i = 0; i < 255; i++)
		skip[i] = M;
	
	if (ignoreCase)
	{
		for (i = M - 1; i >= 0; i--)
			skip[toupper(p[i])] = i;
	}
	else
	{
		for (i = M - 1; i >= 0; i--)
			skip[p[i]] = i;
	}
#endif
} /* initskip_b */

int mismatchsearch_b(const char *p, const char *a, int N, int skip[], bool ignoreCase)
{
	ASSERT(p);
	ASSERT(a);
	ASSERT(skip);
	int i, j, t, M = strlen((char *)p);
	unsigned char c;

	if (ignoreCase)
	{
		for (i = N - M, j = 0; j < M; i++, j++)
		{
			while (toupper(a[i]) != toupper(p[j]))
			{
				c = toupper(a[i]);
				t = skip[c];
				i -= (j + 1 > t) ? j + 1 : t;
				if (i < 0)
					return -1;
				j = 0;
			}
		}
	}
	else
	{
		for (i = N - M, j = 0; j < M; i++, j++)
		{
			while (a[i] != p[j])
			{
				c = a[i];
				t = skip[c];
				i -= (j + 1 > t) ? j + 1 : t;
				if (i < 0)
					return -1;
				j = 0;
			}
		}
	}
	return i - M;
} /* mismatchsearch_b */

int Find(const char *what, const char *buf, int bufSize, bool ignoreCase)
{
	int skip[256];
	int offset = 0;

	initskip(what, skip, ignoreCase);
	offset = mismatchsearch(what, buf, bufSize, skip, ignoreCase);

	return offset;
} /* Find */

static bool IsWord(const char *buf, int size, int start, int len)
{
	bool result;
	
	result = isalnum(buf[start]);
	if (result) result = isalnum(buf[start + len - 1]);
	if (result) result = (start == 0 || (!isalnum(buf[start - 1]) && buf[start - 1] != '_'));
	if (result) result = (start + len == size || (!isalnum(buf[start + len]) && buf[start + len] != '_'));
	
	return result;
} /* IsWord */

void Offset2Line(const char *buf, int size, int offset, int& line, int& selStart, char** l)
{
	line = 1;
	int i = 0, ls = 0;
	
	while (i < offset)
	{
		if (buf[i] == '\n')
		{
			line++;
			ls = i + 1;
		}
		i++;
	}
	
	selStart = offset - ls + 2;

	i = ls + 1;
	while (buf[i] != '\n' && i < size)
		i++;

	*l = (char *)malloc(i - ls + 2);
	FailNil(*l);
	strncpy(*l, buf + ls, i - ls + 1);
	(*l)[i - ls + 1] = 0;

	for (i = 0; (*l)[i]; i++)
		if (isspace((*l)[i])) (*l)[i] = ' ';
} /* Offset2Line */

bool FileContains(const char *path, const char *what, bool ignoreCase, bool word, vector<PMessageItem*> *lst)
{
	bool result = false;

	FILE *f = fopen(path, "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		char *buf = (char *)malloc(size + 1);
		if (buf)
		{
			fread(buf, 1, size, f);
			buf[size] = 0;
			result = BufferContains(buf, size, path, what, ignoreCase, word, lst);			
			free(buf);
		}
		fclose(f);
	}

	return result;
} /* FileContains */

bool BufferContains(const char *buf, int size, const char *path, const char *what, bool ignoreCase,
	bool word, vector<PMessageItem*> *lst)
{
	bool result = false;
	int skip[256];
	int offset = 0;

	initskip(what, skip, ignoreCase);
	
	do
	{
		offset += mismatchsearch(what, buf + offset, size - offset, skip, ignoreCase);
		result = offset < size;
		if (result && word)
			result = IsWord(buf, size, offset + 1, strlen(what));
		if (!result) offset += strlen(what);
	}
	while (!result && offset < size);

	if (result && lst)
	{
		do
		{
			if (!word || IsWord(buf, size, offset + 1, strlen(what)))
			{
				PMessageItem *i = new PMessageItem;
				
				char *l;
				int line, start;
				
				Offset2Line(buf, size, offset, line, start, &l);
				
				i->SetError(l);
				i->SetFile(path);
				i->SetLine(line);
				i->SetSel(start, strlen(what));
				i->SetKind(3);
				
				lst->push_back(i);
				
				free(l);
			}
			
			offset += strlen(what);
			offset += mismatchsearch(what, buf + offset,
				size - offset, skip, ignoreCase);
		}
		while (offset < size);
	}
				
	return result;
} /* BufferContains */

bool FileContainsEx(const char *path, CRegex* regex, vector<PMessageItem*> *lst)
{
	bool result = false;

	FILE *f = fopen(path, "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		char *buf = (char *)malloc(size + 1);
		if (buf)
		{
			fread(buf, 1, size, f);
			buf[size] = 0;
			result = BufferContainsEx(buf, size, path, regex, lst);
			free(buf);
		}
		
		fclose(f);
	}

	return result;
} /* FileContainsEx */

bool BufferContainsEx(const char *buf, int size, const char *path, 
					  CRegex* regex, vector<PMessageItem*> *lst)
{
	int offset, e = 0, r;
	
	offset = e;
	r = regex->Match(buf, size, offset);
	offset = regex->MatchStart();
	e = offset + regex->MatchLen();

	if (r == 0 && lst)
	{
		do
		{
			PMessageItem *i = new PMessageItem;
				
			char *l;
			int line, start;
				
			Offset2Line(buf, size, offset, line, start, &l);
				
			i->SetError(l);
			i->SetFile(path);
			i->SetLine(line);
			i->SetSel(start - 1, e - offset);
			i->SetKind(3);
				
			lst->push_back(i);
				
			free(l);
			
			offset = std::max(e, offset+1);
			r = regex->Match(buf, size, offset);
			offset = regex->MatchStart();
			e = offset + regex->MatchLen();
		}
		while (r == 0);
	}
				
	return (r == 0);
} /* BufferContainsEx */
