/*	$Id: PCmd.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 27 October, 1998 15:19:58
*/

#ifndef PCMD_H
#define PCMD_H


#include <Path.h>
#include <list>
#include <vector>


class PText;
class PTextBuffer;

using std::list;
using std::vector;


class PCmd {
public:
		PCmd(const char *str, PText *txt);
virtual	~PCmd();
		
virtual void Do() = 0;
virtual void Undo() = 0;
virtual void Redo();

		void Update();
		void Redraw();
		
		const char* Desc() const;
		bool IsUndoable() const;
		
protected:
		char		*fStr;
		PText		*fText;
		bool		fUndoable;
};

inline const char* PCmd::Desc() const
{
	return fStr;
}

inline bool PCmd::IsUndoable() const
{
	return fUndoable;
}

class PCutCmd : public PCmd {
public:
		PCutCmd(PText *txt, int append = 0);
		~PCutCmd();
		
virtual	void Do();
virtual	void Undo();

private:
		int fAnchor, fCaret;
		char *fSavedTxt;
		int fWhere;
		int fAppend;
};

class PClearCmd : public PCmd {
public:
		PClearCmd(PText *txt);
		~PClearCmd();
		
virtual	void Do();
virtual	void Undo();

private:
		int fAnchor, fCaret;
		char *fSavedTxt;
		int fWhere;
};

class PPasteCmd : public PCmd {
public:
		PPasteCmd(PText *txt);
		~PPasteCmd();
		
virtual	void Do();
virtual void Undo();
virtual void Redo();

private:
		char *fSavedTxt;
		char *fPasted;
		int fWhere, fTo;
};

class PTypingCmd : public PCmd {
friend class PText;
public:
		PTypingCmd(PText *txt);
		~PTypingCmd();
		
virtual	void Do();
virtual	void Undo();
virtual void Redo();

protected:
		int fDeletedIndx, fDeletedLen, fInsertedLen;
		char *fDeleted;
};

//class PFontTabsCmd : public PCmd {
//public:
//		PFontTabsCmd(PText *txt, BMessage *config);
//
//virtual void Do();
//virtual void Undo();
//	
//private:
//		font_family fNewFamily;
//		font_style fNewStyle;
//		float fNewSize;
//		int fNewTabs, fNewLB;
//		bool fSyntaxColoring, fShowTabs, fShowInvisibles;
//		int fEncoding;
//};

class PDropCmd : public PCmd {
public:
		PDropCmd(PText *txt, const char *data, ssize_t dataLen, int srcLoc, int dstLoc);
		~PDropCmd();
		
virtual void Do();
virtual void Undo();

private:
		char *fData;
		ssize_t fDataLen;
		int fSrcLoc;
		int fDstLoc;
};

#include <list>

enum {
	eaInsert,
	eaDelete
};

struct ExtAction {
	int aType;
	int aOffset;
	char *aText;
	
	ExtAction();
	ExtAction(int type, int offset, char *text);
	~ExtAction();
	
	bool operator<(const ExtAction& a);
	bool operator==(const ExtAction& a);
};

class PExtCmd : public PCmd {
public:
		PExtCmd(PText *txt, const char *ext);
		~PExtCmd();

virtual	void Do();
virtual void Undo();
virtual void Redo();

		list<ExtAction>& Actions();

private:
		list<ExtAction> fActions;
		int fAnchor, fCaret;
};

inline list<ExtAction>& PExtCmd::Actions()
{
	return fActions;
}

class PReplaceCmd : public PCmd {
public:
		PReplaceCmd(PText *txt, int offset, int size, bool findNext, bool backward);
		~PReplaceCmd();
	
virtual	void Do();
virtual	void Undo();

private:
		int fOffset, fSize;
		bool fFindNext, fWrap, fIgnoreCase, fBackward, fEntireWord, fGrep;
		char *fWhat, *fWith, *fExpr;
};

class PReplaceAllCmd : public PCmd {
public:
		PReplaceAllCmd(PText *txt);
		~PReplaceAllCmd();

virtual	void Do();
virtual	void Undo();
virtual	void Redo();

private:
		char *fFind, *fRepl;
		bool fIgnoreCase, fEntireWord, fGrep;
		BPath fBackingStore;
};

#include "PCmd.h"

class PScriptCmd : public PCmd {
public:
			PScriptCmd(PText *txt, const char *script);
			~PScriptCmd();
		
virtual	void Do();
virtual	void Undo();
virtual	void Redo();

			int Exec();

static		long Piper(void *data);

private:
			int fPid, fFD;
			char *fScript;
			char *fOldText;
			int fOldTextSize;
			int fAnchor, fCaret;
};

class PShiftCmd : public PCmd {
public:
		PShiftCmd(const char *name, PText *txt);

		int FirstLine() const;
		int LastLine() const;

protected:
		int fFrom;			// start offset
		int fTo;			// end offset (excl.)
};

class PShiftLeftCmd : public PShiftCmd {
public:
		PShiftLeftCmd(PText *txt);
		~PShiftLeftCmd();

virtual	void Do();
virtual	void Undo();

		bool IsNoOp();

private:
		int fLineCount;		// hard lines
		char *fFirstChars;	// first char of each hard line
		bool fNoOp;
};

class PShiftRightCmd : public PShiftCmd {
public:
		PShiftRightCmd(PText *txt);

virtual	void Do();
virtual	void Undo();
};

class PTwiddleCmd : public PCmd {
public:
		PTwiddleCmd(PText *txt);

virtual void Do();
virtual void Undo();

private:
		int fFrom, fTo;
};

class PCommentCmd : public PCmd {
public:
		PCommentCmd(PText *txt, bool comment,
			const char *before, const char *after);
	
virtual void Do();
virtual void Undo();

private:
		void CommentLine(int line);
		bool UncommentLine(int line);
		
		bool fComment;
		int fFrom, fTo;
		char fBefore[8], fAfter[8];
		vector<int> fLines;
};

class PWrapCmd : public PCmd {
public:
			PWrapCmd(PText *txt);
			~PWrapCmd();

virtual	void Do();
virtual	void Undo();

private:
			int fCaret, fAnchor, fNewLen;
			char *fSavedText;
};

class PUnwrapCmd : public PCmd {
public:
			PUnwrapCmd(PText *txt);
			~PUnwrapCmd();

virtual	void Do();
virtual	void Undo();

private:
			int fCaret, fAnchor, fNewLen;
			char *fSavedText;
};

class PJustifyCmd : public PCmd {
public:
			PJustifyCmd(PText *txt);
			~PJustifyCmd();
			
virtual	void Do();
virtual	void Undo();
virtual	void Redo();

private:
			int fAnchor, fOldLen, fNewLen;
			char *fSavedText;
};

class PChangeCaseCmd : public PCmd {
public:
		PChangeCaseCmd(PText *txt, int newCase);
		~PChangeCaseCmd();

virtual	void Do();
virtual	void Undo();

private:

		void ChangeCase(int newCase);

		char *fSavedText;
		int fAnchor, fCaret;
};

class PEncodingCmd : public PCmd {
public:
		PEncodingCmd(PText *txt, int from, int to);
		~PEncodingCmd();

virtual void Do();
virtual void Undo();

private:
		int fSourceEncoding, fDestEncoding, fPrevEncoding;
		char *fSaved;
		long fAnchor, fCaret, fSrcLen, fDstLen;
};




#endif // PCMD_H
