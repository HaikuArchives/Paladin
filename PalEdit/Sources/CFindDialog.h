/*	$Id: CFindDialog.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 02-06-02
*/

#ifndef CFINDDIALOG_H
#define CFINDDIALOG_H

#include <stack>

#include "HDialog.h"
#include "HDialogViews.h"

#include "CRegex.h"

class PMessageItem;
class PDoc;

using std::stack;

const unsigned long
	msg_QueryCanReplace = 'CRpl',
	msg_ReplyCanReplace = 'RpRp';

class CFindDialog : public HDialog
{
	friend class CDirDropArea;
public:
	enum { sResID = 8 };
		
	CFindDialog(BRect frame, const char *name,
		window_type type, int flags, BWindow *owner);

	virtual void DoFind(unsigned long cmd);
	virtual void MessageReceived(BMessage *msg);
	virtual void FrameResized(float width, float height);
	virtual void UpdateFields();

	void Create();
	void Layout();

	const char* FindString();
	const char* ReplaceString();
	char* RxReplaceString(const char* what, int32 len);

	bool Backward();
	bool IgnoreCase();
	bool Wrap();
	bool Word();
	bool Grep();
	bool IsInMultiFileState() const;
	
	CRegex* Regex();
	
	void GetPatternInfo(BMessage* patternMsg);
	void SetPatternInfo(const BMessage* patternMsg);
	bool PatternInfoDiffersFrom(const BMessage* patternMsg);
		
private:
	virtual bool QuitRequested();

	virtual void WindowActivated(bool active);

	enum MultiReplaceKind { mrNone, mrLeaveOpen, mrSaveToDisk };

	void UpdateSearchDirMenu();
	void AddPathToDirMenu(const char *path, bool select = false, bool addToPrefs = false);
	void AddPathToDirMenu(entry_ref& ref, bool select = false, bool addToPrefs = false);

	bool DoMultiFileFind(const char *dir, bool recursive, bool restart, MultiReplaceKind replace, PDoc **doc);
	bool FindInFile(const entry_ref& ref, vector<PMessageItem*> *lst);
	bool GetRefForPath(entry_ref& ref, const char *path);
	void DoIncludesFind();
	void DoOpenWindows(bool replace);
	
	void ShowBatch(vector<PMessageItem*> *lst, BWindow **w);
	
	void AddCurrentPatternToFindPopup(bool showReplaceText);

	char **fBeIncludes;
	int fBeIncludeCount;
	stack<char*> fDirNameStack;
	stack<DIR*> fDirStack;
	DIR *fCurrentDir;
	BFilePanel *fDirPanel;

	vector<PDoc*> fOpenWindows;
	int fOpenWindowIndex;
	int fCurrentIncludeIndex;
	CRegex fRegex;
	long padding[4];
	
	// Interface
	HButton			*fButFind;
	HButton			*fButRepl;
	HButton			*fButRepF;
	HButton			*fButRepA;
	HStringView		*fLabRepl;
	HTextView		*fEdiFind;
	HScrollView		*fScrFind;
	HTextView		*fEdiRepl;
	HScrollView		*fScrRepl;
	HCheckBox		*fChkCase;
	HCheckBox		*fChkWrap;
	HCheckBox		*fChkBack;
	HCheckBox		*fChkWord;
	HCheckBox		*fChkGrep;
	HCheckBox		*fChkBtch;
	HMenuField		*fMfdPats;
	HCheckBox		*fChkMult;
	HMenuField		*fMfdMeth;
	BMenuItem		*fMitMethDir;
	BMenuItem		*fMitMethWin;
	BMenuItem		*fMitMethInc;
	HCheckBox		*fChkText;
	HCheckBox		*fChkRecu;
	HMenuField		*fMfdSdir;
	BMenuItem		*fMitSdirOth;
	HMenuField		*fMfdNamp;
	BMenuItem		*fMitNampAny;
	BMenuItem		*fMitNampEnd;
	BMenuItem		*fMitNampBeg;
	BMenuItem		*fMitNampCon;
	HTextControl	*fEdiName;
	HBox			*fBoxMult;
	BMenuBar		*fTest;
};

extern CFindDialog* gFindDialog;

inline bool CFindDialog::Backward()
{
	BAutolock lock(this);
	return IsOn("back");
}

inline bool CFindDialog::IgnoreCase()
{
	BAutolock lock(this);
	return IsOn("case");
}

inline bool CFindDialog::Wrap()
{
	BAutolock lock(this);
	return IsOn("wrap");
}

inline bool CFindDialog::Word()
{
	BAutolock lock(this);
	return IsOn("word");
}

inline bool CFindDialog::Grep()
{
	BAutolock lock(this);
	return IsOn("regx");
}

inline bool CFindDialog::IsInMultiFileState() const
{
	return fCurrentDir != NULL || fOpenWindowIndex >= 0;
}

inline CRegex* CFindDialog::Regex()
{
	return &fRegex;
}

#pragma mark - Find

void initskip(const char *p, int skip[], bool ignoreCase);
void initskip_b(const char *p, int skip[], bool ignoreCase);
int mismatchsearch(const char *p, const char *a, int N, int skip[], bool ignoreCase);
int mismatchsearch_b(const char *p, const char *a, int N, int skip[], bool ignoreCase);

int Find(const char *what, const char *buf, int bufSize, bool ignoreCase);
void Offset2Line(const char *buf, int size, int offset, int& line, int& selStart, char** l);

#endif // CFINDDIALOG_H
