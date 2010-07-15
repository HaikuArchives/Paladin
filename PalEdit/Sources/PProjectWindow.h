/*	$Id: PProjectWindow.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/18/98 21:23:57
*/

#ifndef PPROJECTWINDOW_H
#define PPROJECTWINDOW_H

#include "CDocWindow.h"

const ulong
	msg_PProjectItemInvoked = 'IvkP',
	msg_PProjectItemSelected = 'SelP',
	msg_PRemove			 = 'Rmve',
	msg_PAdd				 = 'Add ';

class PGroupStatus;
class PEntryItem;
class PToolBar;
class HButtonBar;

class CProjectFile;
class CProjectItem;

class BListItem;

class PProjectWindow : public CDocWindow
{
	typedef CDocWindow inherited;
public:
	static PProjectWindow* Create(const entry_ref *doc, const char* mimetype);
	~PProjectWindow();

	virtual void SetupSizeAndLayout();
	virtual const char* DocWindowType();

	virtual void MessageReceived(BMessage *msg);
	virtual void SetDirty(bool dirty);
	virtual void WindowActivated(bool active);

	virtual status_t InitCheck() const;
	virtual	const char* ErrorMsg() const;

protected:	
	virtual	void GetText(BString &docText) const;
	virtual	void SetText(const BString& docText);
	virtual	const char* DefaultName() const;
	virtual	void NameChanged();

	virtual void Save();
	virtual	void ReadAttr(BFile& file, BMessage& settingsMsg);
	virtual	void WriteAttr(BFile& file, const BMessage& settingsMsg);
		
private:
	PProjectWindow(const entry_ref *doc, const char* mimetype);

	void SelectionChanged(void);
	void AddItemsToList(CProjectItem* item, BListItem* parentListItem);
	
	void OpenItem();
	void AddRefs(BMessage *msg);
	void AddRef(const entry_ref& ref);
	void AddFiles();
	void RemoveSelected();
	
	const char *AddGroup(const char *e, BDirectory& d);
	
	int fLookahead, fTokenLength;
	
	CProjectFile* fPrjFile;
	BOutlineListView *fList;
	PGroupStatus *fStatus;
	PToolBar *fToolBar;
	HButtonBar *fButtonBar;
	BFilePanel *fPanel;
};

#endif // PPROJECTWINDOW_H
