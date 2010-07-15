/*	$Id: PText.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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
*/

#ifndef PTEXT_H
#define PTEXT_H

typedef float g_unit_t;

#include <Messenger.h>
#include <View.h>

#include "PTextBuffer.h"
#include "CFontStyle.h"

#include <stack>

class BPopUpMenu;
class PCmd;
class PStatus;
class PExec;
class CLanguageInterface;
class CFunctionScanHandler;
class PSplitter;
class PDoc;

using std::stack;

struct LineInfo {
	unsigned long start;
	unsigned char state		: 8;
	unsigned char dirty		: 1;
	unsigned char nl		: 1;
	
	LineInfo() { start = state = dirty = nl = 0; };
	LineInfo(int nStart, int nState, bool nNl = true)
		: start(nStart), state(nState), dirty(1), nl(nNl) {};
};

struct WordInfo {
	short version;
	short classCnt;
	unsigned char classMap[256];
	short forwardCnt;
	unsigned char forwardTable[1]; 
};

typedef vector<LineInfo> VLineInfo;

class PText
	: public BView
{

//friend class PFontTabsCmd;	// lets keep life simple

enum {
	kNormalFont			= 0,
	kAltFont			= 1,
	kIndividualFont		= 2,
};

public:
			PText(BRect frame, PTextBuffer& txt, BScrollBar *bars[], 
				  const char *ext);
			~PText();

			void ReInit();

			void SetStatus(PStatus *status);
		
virtual		void AttachedToWindow();

virtual		void Draw(BRect updateRect);
virtual		void MouseDown(BPoint where);
			void TrackDrag(BPoint where);
virtual		void KeyDown(const char *bytes, int32 numBytes);
			bool DoKeyCommand(BMessage *msg);
virtual		void MouseMoved(BPoint where, uint32 code, const BMessage *a_message);
virtual		void MessageReceived(BMessage *msg);
virtual		void WindowActivated(bool active);
			void MenusBeginning();

virtual		void FrameMoved(BPoint newPosition);
virtual		void FrameResized(float w, float h);

			void Pulse();

			void HandleDrop(BMessage *msg);
			
			void ProcessCommand(unsigned long what, void *param);

			void SetText(const char *utf8Text, size_t size);
			const char* Text();
			int Size() const;
			void SetDefaultLanguageByExtension(const char *ext);
			void SetLanguage(int indx);
			int Language() const;

			void CollectSettings(BMessage& msg);
			void ApplySettings(const BMessage& msg);
			void GetSettingsMW(BPositionIO& set);
			void SetSettingsMW(BPositionIO& set);

			void SetFontKind(int kind);
			int FontKind() const;
			void FontChanged(bool reInit = true);

			void ChangedInfo(BMessage *msg);
			
			void GetSelectedText(char*& s, bool extend = false);
			void CopyBlock(char*& s, int from, int to);

			bool WaitMouseMoved(BPoint where);
			void ShowContextualMenu(BPoint where);
			void ShowFunctionMenu(BPoint where, int which);
			
			void SetDirty(bool dirty);
			bool IsDirty();
		
			void AdjustScrollBars();
			void ScrollBarChanged(BScrollBar *bar, g_unit_t newValue);
			void ScrollToCaret(bool keepContext);
			void ScrollToOffset(int offset, int part, bool keepContext);
			void ScrollToSelection(bool keepContext, bool centerVertically);
	virtual	void ScrollTo(BPoint p);

			void ShowTabStops(bool show);
	
			void DrawLine(int lineNr, g_unit_t y, bool buffer);
//			void DrawString(BView *v, const char *buf, int size, float hv, float& x, float y);
			void RedrawDirtyLines();
			void ShiftLines(int first, int dy, int part);
			void HiliteSelection();
			void ToggleCaret();
			void DrawCaret();
			void HideCaret();
			void ShowCaret();
			
			status_t PageSetup();
			status_t Print();

			int FindWord(int key, bool subWord = false);
			int FindWord(int indx, int key, bool mouseSelect = false, bool subWord = false);
			int FindNextWord(int offset);
			void RecalculateLineBreaks();
			void InsertLinebreaks();
			void RemoveLinebreaks();
			int FindLineBreak(int offset, bool hard = false);
			int RewrapLines(int from, int to, bool hard = false);
			void Kiss(int ch);
			int FindTheOther(int offset, int ch);
			void AutoIndent(int offset);
			void SmartBrace();
			void ScanForFunctions(CFunctionScanHandler& handler);
			void HashLines(vector<int>& hv, bool ignoreCase, bool ignoreWhite);
			
			int Offset2Line(int offset);
			BPoint Offset2Position(int offset, int part = -1);
			int LinePosition2Offset(int line, g_unit_t position);
			int Position2Offset(BPoint where, int part = -1);
			int LineStart(int line) const;
			int LineState(int line) const;
			int LineCount() const;
			bool SoftStart(int line) const;
			void OffsetLineBreaks(int bytes, int line);
			int DeleteLineBreaks(int from, int to);
			int Offset2Column(int offset);
			int Column2Offset(int lineNr, int column);
			int RealLine2Line(int lineNr);
			
			// block select mode:
			bool IsFixedFont() const;
			bool IsBlockSelect() const				{ return fBlockSelect; }
			void SetBlockSelect(bool block)			{ fBlockSelect = block; }
			void BlockOffsetsForLine(int lineNr, int& startOffset, int& endOffset);
			
			void Selection2Region(BRegion& rgn, int part = -1);
			void ChangeSelection(int newAnchor, int newCaret, bool block = false);
			void SelectLine(int lineNr);
			void SelectParagraph();
			void InvertRegion(BRegion& rgn);
	
			void CharKeyDown(const char *bytes, int numBytes);
			void BackspaceKeyDown();
			void DeleteKeyDown();
			
			g_unit_t TextWidth(int offset, int len) const;
			g_unit_t StringWidth(const char *buf, int len) const;
			
			void Insert(const char *text, int textLen, int offset);
			void Delete(int from, int to);
			void RestyleDirtyLines(int from);
			void TextChanged(int from, int to);
			void TypeString(const char *string);
			
			void Cut(int append = 0);
			void Copy(int append = 0);
			void Paste();
			void Clear();
			
			void TouchLine(int lineNr);
			void TouchLines(int from, int to = -1);
		
			int Anchor();
			int Caret();
			void SetCaret(int newCaret);
			void SetMark(int offset);
			void Select(int anchor, int caret, bool update, bool block);
			void CenterSelection();
			
			void RegisterCommand(PCmd *cmd);
			void Undo();
			void Redo();
			void ResetUndo();
			void FlushUndo();
			
			void Find(unsigned long msg, void *args);
			bool CanReplace(const char *s, bool ignoreCase, bool regx);
			bool FindNext(const char *what, int& offset, bool ignoreCase,
					bool wrap, bool backward, bool entireWord, bool regx,
					bool scroll = true, int* foundLen = NULL);
			bool IsIncSearching() const;
			void DoIncSearch(bool forward);
			void IncSearchKey(const char *bytes, int numBytes);
			
			void JumpToFunction(const char *func, int offset);
			void FindNextError(bool backward);
			
			void ExecuteSelection();
			void KillCurrentJob();
			void PrepareForOutput();
			void SetCWD(const char *cwd);
			const char *GetCWD() const;
			void SendTextToJob(const char *txt);
			
			void SetSplitter(PSplitter *splitter);
			void TrackSplitter(BPoint where);
			void SplitterMoved(g_unit_t dy);
			void DoneMovingSplitter();
			void SplitWindow();
			void UnsplitWindow();
			void SwitchPart(int toPart);
		
			const PTextBuffer& TextBuffer() const;
			const char* FindString() const;
			const char* ReplaceString() const;
			bool IsExecuting() const;
			const BFont& Font() const;
			int TabStops() const;
			bool SyntaxColoring() const;
			bool ShowInvisibles() const;
			bool SoftWrap() const;
			int WrapType() const;
			int WrapWidth() const;
			PDoc* Doc() const;

private:
			void VerticallyScrollToSelection(int startOffset,
											 int endOffset,
											 bool keepContext,
											 bool centered,
											 int part = -1);
			void HorizontallyScrollToSelection(int startOffset,
											   int endOffset,
											   bool keepContext);
			BRect CursorFrame(int caret);
			void SetupBitmap();
			BRect PartBounds(int part);
			bool OffsetIsOutsideOfContextArea(int32 offset);
			void NavigateOverFunctions(char direction);
			
			BBitmap *fLineMap;
			BView	*fLineView;
			BFont fFont;
			font_height fFH;
			BPopUpMenu *fMainPopUp;

			bigtime_t fNextCaret;
			bigtime_t fLastMouseTime;
			BScrollBar *fHScrollBar, *fVScrollBar1, *fVScrollBar2;
			stack<PCmd*> fDoneCmds, fUndoneCmds;
			int fMouseClicks;

			g_unit_t fLineHeight, fTabWidth;
			g_unit_t fWalkOffset;

			bool fUsingDefaultLanguage;
			CLanguageInterface *fLangIntf;
			BRect fBounds;
			PTextBuffer& fText;
			VLineInfo fLineInfo;
			CFontStyle *fMetrics;
			int fAnchor, fCaret, fStoredCaret;
			bool fBlockSelect;
			bool fCaretVisible;	// logically visible
			bool fCaretDrawn;	// actually drawn on screen
			long fCaretShielded;
			PStatus *fStatus;
			PCmd* fLastCommand;
			PExec *fExec;
			bool fWindowActive;
			bool fSyntaxColoring;
			bool fShowInvisibles;
			int fSavedCaret, fSavedAnchor;
			int fDragStart;
			int fDragButtons;
			PSplitter *fSplitter;
			g_unit_t fSplitAt;
			bool fSplitCursorShown;
			int fActivePart, fOPCaret, fOPAnchor, fOPMark;
			BPoint fLastMouseDown;
			int fIncSearch;
			char fIncPat[256];
			int fIncCaret;
			int fMark;
			int fNrArgument;
			int fShift;
			int fLastKillPoint;
			bool fAppendNextCut;
			PCmd* fLastSavedStateCmd;
			float fDefaultCharWidth;
			BMessage *fPrintSettings;
		
static		PText *sfDragSource;

			rgb_color fLowColor, fTextColor;
			rgb_color fKeywordColor, fStringColor, fCommentColor;

// saveable settings
			int fTabStops;
			char *fCWD;
			bool fSoftWrap;
			int fWrapType;
			int fWrapWidth;
			int fFontKind;
};

inline void PText::HideCaret() {
	if (fCaretVisible) ToggleCaret();
	fNextCaret = system_time() + 500000;
}

inline void PText::ShowCaret() {
	if (!fCaretVisible) ToggleCaret();
	fNextCaret = system_time() + 500000;
}

inline int PText::Anchor() {
	return fAnchor;
}

inline int PText::Caret() {
	return fCaret;
}

inline const PTextBuffer& PText::TextBuffer() const {
	return fText;
}

inline bool PText::IsExecuting() const {
	return fExec != NULL;
}

inline int PText::TabStops() const {
	return fTabStops;
}

inline bool PText::SyntaxColoring() const {
	return fSyntaxColoring;
}

inline bool PText::ShowInvisibles() const {
	return fShowInvisibles;
}

inline int PText::LineCount() const {
	return fLineInfo.size();
}

inline int PText::LineStart(int line) const {
	if (line >= 0 && line < LineCount())
		return fLineInfo[line].start;
	else if (line < 0)
		return 0;
	else
	{
		ASSERT(line < LineCount());
		return fText.Size();
	}
}

inline int PText::LineState(int line) const {
	if (line >= 0 && line < LineCount())
		return fLineInfo[line].state;
	else
	{
#if DEBUG
		ASSERT(line >= 0 && line < LineCount());
#endif
		return 0;
	}
}

inline g_unit_t PText::StringWidth(const char *buf, int len) const
{
	return fMetrics->StringWidth(buf, len);
} /* PText::StringWidth */

inline bool PText::IsIncSearching() const
{
	return fIncSearch != 0;
} /* PText::IsIncSearching */

inline bool PText::SoftWrap() const
{
	return fSoftWrap;
} /* PText::SoftWrap */

inline int PText::WrapType() const
{
	return fWrapType;
} /* PText::WrapType */

inline int PText::WrapWidth() const
{
	return fWrapWidth;
} /* PText::WrapWidth */

inline bool PText::SoftStart(int line) const
{
	return ! fLineInfo[line].nl;
} /* PText::SoftStart */

inline const char* PText::GetCWD() const
{
	return fCWD;
} /* PText::GetCWD */

inline const BFont& PText::Font() const
{
	return fFont;
} // PText::Font

inline bool PText::IsFixedFont() const
{
	return fFont.IsFixed();
} // PText::IsFixedFont

inline void PText::ProcessCommand(unsigned long what, void *param)
{
	BMessage msg(what); 
	msg.AddPointer("param", param); 
	BMessenger(this).SendMessage(&msg);
}

#endif // PTEXT_H
