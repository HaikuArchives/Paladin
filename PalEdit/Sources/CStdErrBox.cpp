/*	$Id: CStdErrBox.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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
#include "CStdErrBox.h"
#include "Utils.h"
#include "HDialog.h"
#include "HDefines.h"
#include "HPreferences.h"
#include "Prefs.h"

const unsigned long
	msg_ChangedPattern = 'ECnm';

class CStdErrItem : public BStringItem
{
  public:
	CStdErrItem(const char *pattern, int file, int line, int msg, int warning)
		: BStringItem(Escape(pattern)), fFile(file), fLine(line), fMsg(msg), fWarning(warning)
	{
	};
	
	const char *Escape(const char *p)
	{
		static char s[1024];	// ooooohhh, how horrible...
		
		const char *t = p;
		char *d = s;
		
		while (true)
		{
			char c = *t++;
			
			if (c == 0)
			{
				*d = 0;
				break;
			}
			
			switch (c)
			{
				case '\r':	*d++ = '\\'; *d++ = 'r'; break;
				case '\n':	*d++ = '\\'; *d++ = 'n'; break;
				case '\t':	*d++ = '\\'; *d++ = 't'; break;
				default:		*d++ = c;
			}
		}
		
		return s;
	}
	
	int fFile, fLine, fMsg, fWarning;
};

#pragma mark -

CStdErrBox::CStdErrBox(BRect r, const char *name)
	: CListBox(r, name)
{
	r = fList->Frame();
	r.bottom = r.bottom - 70 * gFactor;

// resize the scroller of the list

	BView *v = FindView("scroller");
	RemoveChild(v);
	delete v;
	
	fList = new BListView(r, "list", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	fList->SetSelectionMessage(new BMessage('lbSl'));
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true));
	
	r.top = r.bottom + 10 * gFactor;
	r.bottom = r.top + 16 * gFactor;
	
	BTextControl *t;

	AddChild(t = new BTextControl(r, "SEpt", "Pattern:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	t->SetDivider(40 * gFactor);
	t->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	r.OffsetBy(0, 20 * gFactor);
	r.right = r.left + r.Width() / 2 - 10 * gFactor;

	AddChild(t = new BTextControl(r, "SEfl", "File:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	t->SetDivider(40 * gFactor);
	t->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	r.OffsetBy(0, 20 * gFactor);

	AddChild(t = new BTextControl(r, "SEln", "Line:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	t->SetDivider(40 * gFactor);
	t->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	r.OffsetBy(r.Width() + 10 * gFactor, -20 * gFactor);
	r.right += 10 * gFactor;

	AddChild(t = new BTextControl(r, "SEms", "Message:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	t->SetDivider(50 * gFactor);
	t->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	r.OffsetBy(0, 20 * gFactor);

	AddChild(t = new BTextControl(r, "SEwa", "Warning:",
		NULL, new BMessage(msg_ChangedPattern)));
	
	t->SetDivider(50 * gFactor);
	t->SetModificationMessage(new BMessage(msg_ChangedPattern));
	
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	fDelete->SetEnabled(false);
} /* CStdErrBox::CStdErrBox */

CStdErrBox::~CStdErrBox()
{
} /* CStdErrBox::~CStdErrBox */

void CStdErrBox::AttachedToWindow()
{
	BView::AttachedToWindow();
	
	fDialog = dynamic_cast<HDialog*>(Window());
	FailNil(fDialog);

	static_cast<BControl*>(fDialog->FindView("SEpt"))->SetTarget(this);
	static_cast<BControl*>(fDialog->FindView("SEfl"))->SetTarget(this);
	static_cast<BControl*>(fDialog->FindView("SEln"))->SetTarget(this);
	static_cast<BControl*>(fDialog->FindView("SEms"))->SetTarget(this);
	static_cast<BControl*>(fDialog->FindView("SEwa"))->SetTarget(this);
} /* CStdErrBox::AttachedToWindow */

void CStdErrBox::AddClicked()
{
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	fList->AddItem(new CStdErrItem(fDialog->GetText("SEpt"),
		fDialog->GetValue("SEfl"), fDialog->GetValue("SEln"),
		fDialog->GetValue("SEms"), fDialog->GetValue("SEwa")));
	
	ListBoxChanged();
} /* CStdErrBox::AddClicked */

void CStdErrBox::ChangeClicked()
{
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CStdErrItem *i = static_cast<CStdErrItem*>(fList->ItemAt(ix));
		i->SetText(fDialog->GetText("SEpt"));
		i->fFile = fDialog->GetValue("SEfl");
		i->fLine = fDialog->GetValue("SEln");
		i->fMsg = fDialog->GetValue("SEms");
		i->fWarning = fDialog->GetValue("SEwa");
		fList->InvalidateItem(ix);

		ListBoxChanged();
	}
} /* CStdErrBox::ChangeClicked */

void CStdErrBox::DeleteClicked()
{
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CStdErrItem *i = static_cast<CStdErrItem*>(fList->ItemAt(ix));
		fList->RemoveItem(i);
		delete i;

		ListBoxChanged();
	}
} /* CStdErrBox::DeleteClicked */

void CStdErrBox::Selected()
{
	int ix = fList->CurrentSelection();

	fDelete->SetEnabled(ix >= 0);
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	if (ix >= 0)
	{
		CStdErrItem *i = static_cast<CStdErrItem*>(fList->ItemAt(ix));
		
		fDialog->SetText("SEpt", i->Text());
		fDialog->SetValue("SEfl", i->fFile);
		fDialog->SetValue("SEln", i->fLine);
		fDialog->SetValue("SEms", i->fMsg);
		fDialog->SetValue("SEwa", i->fWarning);
	}
} /* CStdErrBox::Selected */

void CStdErrBox::MessageReceived(BMessage *msg)
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
} /* CStdErrBox::MessageReceived */

void CStdErrBox::DoOK()
{
	gPrefs->RemovePref(prf_X_StdErrPattern);
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		CStdErrItem *m = static_cast<CStdErrItem*>(fList->ItemAt(i));
		
		char buf[1024];
		sprintf(buf, "%d;%d;%d;%d;%s", m->fFile, m->fLine, m->fMsg,
			m->fWarning, m->Text());
		
		gPrefs->SetIxPrefString(prf_X_StdErrPattern, i, buf);
	}
} /* CStdErrBox::DoOK */

void CStdErrBox::DoCancel()
{
	StChangeLB changeMe(this);
	
	int i;
	CStdErrItem *m;
	
	for (i = fList->CountItems() - 1; i >= 0; i--)
	{
		m = static_cast<CStdErrItem*>(fList->ItemAt(i));
		fList->RemoveItem(i);
		delete m;
	}

	if (gPrefs->GetPrefInt(prf_I_StdErrInitEd, 0) == 0)
	{
		gPrefs->SetIxPrefString(prf_X_StdErrPattern, 0, "1;2;4;3;^([^:]+):([0-9]+): (warning: )?(.*)\\n");
		gPrefs->SetIxPrefString(prf_X_StdErrPattern, 1, "4;5;2;1;^### [^ ]+ (.*)\\n(#[ \\t](.*\\n))+#-+\\n[ \\t]*File \"([^\"]+)\"; Line ([0-9]+)\\n(#.*\\n)*#-+\\n");
		gPrefs->SetPrefInt(prf_I_StdErrInitEd, 1);
	}

	i = 0;
	const char *pattern;
	int file, line, msg, warning, n;

	do
	{
		pattern = gPrefs->GetIxPrefString(prf_X_StdErrPattern, i);

		if (pattern && sscanf(pattern, "%d;%d;%d;%d;%n", &file, &line, &msg, &warning, &n) == 4)
			fList->AddItem(new CStdErrItem(pattern + n, file, line, msg, warning));
		
		i++;
	}
	while (pattern);
	
} /* CStdErrBox::DoCancel */

