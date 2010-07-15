/*	$Id: PApp.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 29 September, 1998 10:55:45
*/

#ifndef PAPP_H
#define PAPP_H

class CDoc;

class HDialog;
class PDoc;
class CPrefOpener;

class PApp
	: public BApplication
{
friend class CPrefOpener;

public:
			PApp();
virtual	~PApp();

			PDoc* NewWindow(const entry_ref *ref = NULL, bool show = true);
			CDoc* OpenWindow(const entry_ref& doc, bool show = true);

virtual	void ReadyToRun();

virtual	void ArgvReceived(int32 argc, char **argv);
virtual	void RefsReceived(BMessage *msg);
virtual	void MessageReceived(BMessage *msg);
virtual	bool QuitRequested();

			void FindAndOpen(const char *file, const char* fromSource);
			PDoc* OpenWorksheet();

			HDialog* FindDialog();
			HDialog* PrefsDialog();

			bool IsQuitting();

private:
			void SetColor(int colorId, const char* const prefName, uint8 red, uint8 green, uint8 blue);

virtual	BHandler* ResolveSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property);

			BFilePanel *fOpenPanel;
			thread_id fPrefOpener;
			HDialog *fFindDialog;
			HDialog *fPrefsDialog;
			bool fIsQuitting;
};

extern BDirectory gAppDir, gCWD, gPrefsDir;
extern BFile gAppFile;

inline HDialog* PApp::FindDialog() {
	return fFindDialog;
}

inline HDialog* PApp::PrefsDialog() {
	return fPrefsDialog;
}

inline bool PApp::IsQuitting() {
       return fIsQuitting;
}

extern PApp *gApp;

extern bool gAutoIndent, gSyntaxColoring, gBalance, gBlockCursor, gFlashCursor, gSmartBrace;
extern int gSpacesPerTab;
extern bool gPopupIncludes, gPopupProtos, gPopupFuncs, gPopupSorted;
extern bool gRedirectStdErr, gUseWorksheet;
extern bool gRestorePosition, gRestoreFont, gRestoreSelection, gRestoreScrollbar, gRestoreCWD;
extern int gSavedState, gRecentBufferSize;
extern uid_t gUid;
extern gid_t gGid;
extern char gTabChar[4], gReturnChar[4], gSpaceChar[4], gControlChar[4];

#endif // PAPP_H
