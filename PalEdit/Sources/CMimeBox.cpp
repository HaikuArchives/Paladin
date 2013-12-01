/*	$Id: CMimeBox.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/23/97 09:38:06
*/

#include "pe.h"
#include "CListBox.h"
#include "CMimeBox.h"
#include "Utils.h"
#include "HError.h"
#include "HDialog.h"
#include "HDefines.h"
#include "HPreferences.h"
#include "Prefs.h"

const unsigned long
	msg_MimeChanged = 'MChg',
	msg_MimeFind = 'MFnd',
	msg_MimeFound = 'MFon';

class CMimeItem : public BListItem {
public:
		CMimeItem(const char *mime);
		
virtual	void DrawItem(BView *owner, BRect bounds, bool complete = false);

		BMimeType fMime;
		unsigned char fIcon[256], fIconSelected[256];
};

CMimeItem::CMimeItem(const char *mime)
{
	fMime.SetTo(mime);
	
	memset(fIcon, B_TRANSPARENT_8_BIT, 256);
	memset(fIconSelected, B_TRANSPARENT_8_BIT, 256);

	BBitmap bm(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
	if (fMime.GetIcon(&bm, B_MINI_ICON) != B_OK)
	{
		try
		{
			char p[PATH_MAX];
			if (find_directory(B_SYSTEM_TEMP_DIRECTORY, 0, true, p, PATH_MAX) == B_OK)
			{
				BDirectory tmpdir;
				FailOSErr(tmpdir.SetTo(p));

				time_t t;
				time(&t);
				sprintf(p, "tmp.pe_is_looking_for_a_mime_icon:%ld", t);

				BFile f;
				FailOSErr(tmpdir.CreateFile(p, &f));

				BNodeInfo ni;
				FailOSErr(ni.SetTo(&f));
				FailOSErr(ni.SetType(mime));
				FailOSErr(ni.GetTrackerIcon(&bm, B_MINI_ICON));
			}
		}
		catch (HErr& e) { }
	}

	for (int i = 0; i < 16; i++)
	{
		unsigned char *ba = (unsigned char *)((unsigned char *)bm.Bits() + bm.BytesPerRow() * i);
		memcpy(fIcon + i * 16, ba, 16);
		
		for (int j = 0; j < 16; j++)
			if (ba[j] < 255)
				fIconSelected[j + i * 16] = gSelectedMap[ba[j]];
	}
} /* CMimeItem::CMimeItem */

void CMimeItem::DrawItem(BView *owner, BRect bounds, bool)
{
	BBitmap bm(BRect(0, 0, 15, 15), B_COLOR_8_BIT);

	if (IsSelected())
	{
		bm.SetBits(fIconSelected, 256, 0, B_COLOR_8_BIT);
		owner->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	}
	else
	{
		bm.SetBits(fIcon, 256, 0, B_COLOR_8_BIT);
		owner->SetLowColor(kWhite);
	}

	BRect r(bounds);
	owner->FillRect(r, B_SOLID_LOW);
	r.InsetBy(1, 1);

	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	owner->SetDrawingMode(B_OP_OVER);
	owner->DrawBitmap(&bm, BPoint(r.left + 2, r.top));
	owner->SetDrawingMode(B_OP_COPY);
	
	owner->DrawString(fMime.Type(), BPoint(r.left + 22, r.bottom - fh.descent));
	
	owner->SetLowColor(kWhite);
} /* CMimeItem::DrawItem */

CMimeBox::CMimeBox(BRect r, const char *name)
	: CListBox(r, name)
{
	fPanel = NULL;

	r = fList->Frame();
	r.bottom -= 30 * gFactor;

// resize the scroller of the list

	BView *v = FindView("scroller");
	RemoveChild(v);
	delete v;
	
	fList = new BListView(r, "list", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	fList->SetSelectionMessage(new BMessage('lbSl'));
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true));
	
	r.top = r.bottom + 10 * gFactor;
	r.bottom = Bounds().bottom;
	
//	r.right -= 70;
	AddChild(fEdit = new BTextControl(r, "edit mime", "Mimetype:",
		NULL, new BMessage(msg_MimeChanged)));
	
	fEdit->SetDivider(be_plain_font->StringWidth("Mimetype:") + 4);
	fEdit->SetModificationMessage(new BMessage(msg_MimeChanged));
	
	r.left = r.right + 10;
	r.right = r.left + 60;
	r.top = r.bottom - 24 * gFactor;
	AddChild(fGet = new BButton(r, "get", "Find…", new BMessage(msg_MimeFind)));
	
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	fDelete->SetEnabled(false);
	
	AddType("text/plain");
} /* CMimeBox::CMimeBox */

CMimeBox::~CMimeBox()
{
	if (fPanel) delete fPanel;
} /* CMimeBox::~CMimeBox */

void CMimeBox::AttachedToWindow()
{
	BView::AttachedToWindow();
	fEdit->SetTarget(this);
	fGet->SetTarget(this);
} /* CMimeBox::AttachedToWindow */

void CMimeBox::AddClicked()
{
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	AddType(fEdit->Text());
	
	ListBoxChanged();
} /* CMimeBox::AddClicked */

void CMimeBox::ChangeClicked()
{
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CMimeItem *i = static_cast<CMimeItem*>(fList->ItemAt(ix));
		i->fMime.SetTo(fEdit->Text());
		fList->InvalidateItem(ix);

		ListBoxChanged();
	}
} /* CMimeBox::ChangeClicked */

void CMimeBox::DeleteClicked()
{
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CMimeItem *i = static_cast<CMimeItem*>(fList->ItemAt(ix));
		fList->RemoveItem(i);
		delete i;

		ListBoxChanged();
	}
} /* CMimeBox::DeleteClicked */

void CMimeBox::Selected()
{
	fDelete->SetEnabled(true);
	fAdd->SetEnabled(false);
	fChange->SetEnabled(false);
	
	int ix = fList->CurrentSelection();
	if (ix >= 0)
	{
		CMimeItem *i = static_cast<CMimeItem*>(fList->ItemAt(ix));
		fEdit->SetText(i->fMime.Type());
	}
} /* CMimeBox::Selected */

void CMimeBox::MessageReceived(BMessage *msg)
{
	char t[NAME_MAX];
		
	if (msg->WasDropped())
	{
		if (msg->HasString("mimeSignature"))
		{
			const char *m;
			ssize_t s;
			
			msg->FindData("mimeSignature", B_STRING_TYPE, (const void**)&m, &s);
			strncpy(t, m, s);
			t[s] = 0;
			AddType(t);
		}
		else if (msg->HasRef("refs"))
		{
			entry_ref ref;
			int32 i = 0;
			
			while (msg->FindRef("refs", i++, &ref) == B_OK)
			{
				BNode n(&ref);
				BNodeInfo(&n).GetType(t);
				AddType(t);
			}
		}
	}
	
	switch (msg->what)
	{
		case msg_MimeChanged:
		{
			bool valid = BMimeType::IsValid(fEdit->Text());
			fAdd->SetEnabled(valid);
			fChange->SetEnabled(valid);
			break;
		}

		case msg_MimeFind:
			if (fPanel) fPanel->Show();
			else
			{
				fPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this),
					NULL, B_FILE_NODE, true, new BMessage(msg_MimeFound));
				fPanel->SetButtonLabel(B_DEFAULT_BUTTON, "Pick");
				fPanel->Show();
			}
			break;

		case msg_MimeFound:
		{
			delete fPanel;
			fPanel = NULL;
			
			entry_ref ref;
			int32 i = 0;
			
			while (msg->FindRef("refs", i++, &ref) == B_OK)
			{
				BNode n(&ref);
				BNodeInfo(&n).GetType(t);
				AddType(t);
			}
			break;
		}

		default:
			CListBox::MessageReceived(msg);
			break;
	}
} /* CMimeBox::MessageReceived */

void CMimeBox::DoOK()
{
	gPrefs->RemovePref(prf_X_Mimetype);
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		CMimeItem *m = static_cast<CMimeItem*>(fList->ItemAt(i));
		gPrefs->SetIxPrefString(prf_X_Mimetype, i, m->fMime.Type());
	}
} /* CMimeBox::DoOK */

void CMimeBox::DoCancel()
{
	StChangeLB changeMe(this);
	
	int i = 0;
	
	const char *t;
	CMimeItem *m;
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		m = static_cast<CMimeItem*>(fList->ItemAt(i));
		fList->RemoveItem(i);
		delete m;
	}

	do
	{
		t = gPrefs->GetIxPrefString(prf_X_Mimetype, i++);
		if (t) AddType(t);
	}
	while (t);
} /* CMimeBox::DoCancel */

void CMimeBox::AddType(const char *type)
{
	CMimeItem *m;
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		m = static_cast<CMimeItem*>(fList->ItemAt(i));
		if (strcmp(m->fMime.Type(), type) == 0)
			return;
	}
	
	m = new CMimeItem(type);
	fList->AddItem(m);
	m->SetHeight(18);
	fList->InvalidateItem(fList->IndexOf(m));

	ListBoxChanged();
} /* CMimeBox::AddType */
