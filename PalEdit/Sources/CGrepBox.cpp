/*	$Id: CGrepBox.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 02/22/98 22:12:11
*/

#include "pe.h"
#include "CListBox.h"
#include "CGrepBox.h"
#include "Utils.h"
#include "HPreferences.h"
#include "HDialog.h"
#include "Prefs.h"

const unsigned long
	msg_ChangedPattern = 'GCnm';

class CGrepItem : public BStringItem {
public:
		CGrepItem(const char *name, const char *find, const char *repl)
			: BStringItem(name)
		{
			fFind = strdup(find);
			fRepl = strdup(repl);
		};

		~CGrepItem()
		{
			free(fFind);
			free(fRepl);
		};
		
		void Change(const char *find, const char *repl)
		{
			free(fFind); fFind = strdup(find);
			free(fRepl); fRepl = strdup(repl);
		};

		char *fFind, *fRepl;
};

#pragma mark -

CGrepBox::CGrepBox(BRect r, const char *name)
	: CListBox(r, name)
{
	r = fList->Frame();
	r.bottom = r.top + 100 * gFactor;

// resize the scroller of the list

	BView *v = FindView("scroller");
	RemoveChild(v);
	delete v;
	
	fList = new BListView(r, "list", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	fList->SetSelectionMessage(new BMessage('lbSl'));
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true));
	
	r.top = r.bottom + 10 * gFactor;
	r.bottom = r.top + 16 * gFactor;
	
	AddChild(fName = new BTextControl(r, "edit mime", "Name:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	fName->SetDivider(40 * gFactor);
	fName->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	r.OffsetBy(0, 20 * gFactor);

	AddChild(fFind = new BTextControl(r, "edit mime", "Find:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	fFind->SetDivider(40 * gFactor);
	fFind->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	r.OffsetBy(0, 20 * gFactor);

	AddChild(fReplace = new BTextControl(r, "edit mime", "Replace:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	fReplace->SetDivider(40 * gFactor);
	fReplace->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	fDelete->SetEnabled(false);
} /* CGrepBox::CGrepBox */

CGrepBox::~CGrepBox()
{
} /* CGrepBox::~CGrepBox */

void CGrepBox::AttachedToWindow()
{
	BView::AttachedToWindow();
	fName->SetTarget(this);
	fFind->SetTarget(this);
	fReplace->SetTarget(this);
} /* CGrepBox::AttachedToWindow */

void CGrepBox::AddClicked()
{
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	fList->AddItem(new CGrepItem(fName->Text(), fFind->Text(), fReplace->Text()));
	
	ListBoxChanged();
} /* CGrepBox::AddClicked */

void CGrepBox::ChangeClicked()
{
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CGrepItem *i = static_cast<CGrepItem*>(fList->ItemAt(ix));
		i->Change(fFind->Text(), fReplace->Text());
		i->SetText(fName->Text());
		fList->InvalidateItem(ix);

		ListBoxChanged();
	}
} /* CGrepBox::ChangeClicked */

void CGrepBox::DeleteClicked()
{
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CGrepItem *i = static_cast<CGrepItem*>(fList->ItemAt(ix));
		fList->RemoveItem(i);
		delete i;

		ListBoxChanged();
	}
} /* CGrepBox::DeleteClicked */

void CGrepBox::Selected()
{
	fDelete->SetEnabled(true);
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CGrepItem *i = static_cast<CGrepItem*>(fList->ItemAt(ix));
		fName->SetText(i->Text());
		fFind->SetText(i->fFind);
		fReplace->SetText(i->fRepl);
	}
} /* CGrepBox::Selected */

void CGrepBox::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case msg_ChangedPattern:
		{
			fAdd->SetEnabled(true);
			fChange->SetEnabled(true);
			break;
		}

		default:
			CListBox::MessageReceived(msg);
			break;
	}
} /* CGrepBox::MessageReceived */

void CGrepBox::DoOK()
{
	gPrefs->RemovePref(prf_X_GrepPatName);
	gPrefs->RemovePref(prf_X_GrepPatFind);
	gPrefs->RemovePref(prf_X_GrepPatRepl);
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		CGrepItem *m = static_cast<CGrepItem*>(fList->ItemAt(i));
		gPrefs->SetIxPrefString(prf_X_GrepPatName, i, m->Text());
		gPrefs->SetIxPrefString(prf_X_GrepPatFind, i, m->fFind);
		gPrefs->SetIxPrefString(prf_X_GrepPatRepl, i, m->fRepl);
	}
} /* CGrepBox::DoOK */

void CGrepBox::DoCancel()
{
	StChangeLB changeMe(this);
	
	int i;
	CGrepItem *m;
	
	for (i = fList->CountItems() - 1; i >= 0; i--)
	{
		m = static_cast<CGrepItem*>(fList->ItemAt(i));
		fList->RemoveItem(i);
		delete m;
	}

	i = 0;
	const char *name, *find, *repl;

	do
	{
		name = gPrefs->GetIxPrefString(prf_X_GrepPatName, i);
		find = gPrefs->GetIxPrefString(prf_X_GrepPatFind, i);
		repl = gPrefs->GetIxPrefString(prf_X_GrepPatRepl, i);

		if (name && find && repl)
			fList->AddItem(new CGrepItem(name, find, repl));
		
		i++;
	}
	while (name);
} /* CGrepBox::DoCancel */

