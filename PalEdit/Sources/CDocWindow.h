/*	$Id: CDocWindow.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#ifndef CDOC_WINDOW_H
#define CDOC_WINDOW_H

#include <Window.h>

#include "CDoc.h"

class HDialog;

class CDocWindow : public BWindow, public CDoc
{
            typedef BWindow inheritedWindow;
            typedef CDoc inheritedDoc;

public:
			CDocWindow(const entry_ref *ref = NULL);
virtual		~CDocWindow();
		
virtual		void SetupSizeAndLayout();
virtual		const char* DocWindowType() = 0;

virtual		void Show();
virtual		bool QuitRequested();
virtual		void Quit();
virtual 	void MessageReceived(BMessage *msg);

			BRect NextPosition(bool inc = true);

			void AddDialog(HDialog *dlog, bool isModal);
			void RemoveDialog(HDialog *dlog);
			void MakeModal(HDialog *dlog);
	
protected:

virtual 	void CollectSettings(BMessage& settingsMsg) const;
virtual 	void ApplySettings(const BMessage& settingsMsg);
			status_t WriteState();

virtual                void SetDirty(bool dirty);
virtual		void NameChanged();

			vector<HDialog*> fDialogs;

            bool fCloseWinAfterSave;
            bool fCloseAppAfterSave;

private:
			HDialog *fWindowModal;

			BRect fLastStoredFrame;
			BRect fInitialFrame;
protected:
static		int sfNewCount;
};

#endif // CDOC_WINDOW_H
