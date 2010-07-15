/*	$Id: PGroupWindow.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#ifndef PGROUPWINDOW_H
#define PGROUPWINDOW_H

#include "CDocWindow.h"

const ulong
	msg_GroupItemInvoked = 'IvkG',
	msg_Remove			 = 'Rmve',
	msg_Add				 = 'Add ';

class PIconFinder;
class PToolBar;
class PGroupWindow;
class HButtonBar;
class PGroupStatus;
class PEntryItem;

class PGroupWindow : public CDocWindow {
			typedef CDocWindow inherited;
			friend class PIconFinder;
public:
			PGroupWindow(const entry_ref *doc = NULL);
			~PGroupWindow();

virtual	void SetupSizeAndLayout();
virtual	const char* DocWindowType();

virtual	bool QuitRequested();

virtual	void MessageReceived(BMessage *msg);
		
			PEntryItem* AddRef(entry_ref& ref);
			
			int CountItems() const;

protected:
virtual	void SetDirty(bool dirty);
			
virtual	void GetText(BString &docText) const;
virtual	void SetText(const BString& docText);
virtual	const char* DefaultName() const;
virtual	void NameChanged();

virtual	void ReadAttr(BFile& file, BMessage& settingsMsg);
virtual	void WriteAttr(BFile& file, const BMessage& settingsMsg);
		
private:
			void OpenItem();
			void AddRefs(BMessage *msg);
			void AddFiles();
			void RemoveSelected();
			
			BListView *fList;
			PGroupStatus *fStatus;
			PToolBar *fToolBar;
			HButtonBar *fButtonBar;
			BFilePanel *fPanel;
			PIconFinder *fIconFinder;
			vector<PEntryItem*> *fNewItems;
};

inline int PGroupWindow::CountItems() const
{
	return fList->CountItems();
} /* PGroupWindow::CountItems */

#endif // PGROUPWINDOW_H
