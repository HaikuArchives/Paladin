/*	$Id: CPathsBox.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/20/97 21:53:02
*/

#include "pe.h"
#include "CListBox.h"
#include "CPathsBox.h"
#include "PMessages.h"
#include "Utils.h"
#include "PApp.h"
#include "HError.h"
#include "HDefines.h"
#include "HPreferences.h"

	typedef BListItem CListItem;
	typedef BView LView;

class CPathItem : public CListItem
{
  public:
	CPathItem(const char *path);
	~CPathItem();

	const char *Path() const		{ return fPath; }
	void SetPath(const char *path);
	virtual void DrawItem(LView *owner, BRect bounds, bool complete);

  private:
	char *fPath;
};

CPathItem::CPathItem(const char *path)
{
	fPath = NULL;
	SetPath(path);
} // CPathItem::CPathItem

CPathItem::~CPathItem()
{
	free(fPath);
} // CPathItem::~CPathItem

void CPathItem::SetPath(const char *path)
{
	if (fPath) free(fPath);
	
	int len = strlen(path);
	
	fPath = (char *)malloc(len + 2);
	FailNil(fPath);

	strcpy(fPath, path);
	if (path[len - 1] != kDirectorySeparator)
	{
		fPath[len] = kDirectorySeparator;
		fPath[len + 1] = 0;
	}
} // CPathItem::SetPath

void CPathItem::DrawItem(LView *owner, BRect bounds, bool /*complete*/)
{
	char buf[256];
	strncpy(buf, fPath, 255);
	buf[strlen(fPath)] = 0;

	TruncPathString(buf, 133, (int)Width());
	
	if (IsSelected())
		owner->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	else
		owner->SetLowColor(kWhite);
	
	owner->FillRect(bounds, B_SOLID_LOW);
	
	font_height fi;
	be_plain_font->GetHeight(&fi);
	
	owner->DrawString(buf, BPoint(bounds.left + 4, bounds.bottom - fi.descent));
	owner->SetLowColor(kWhite);
} // CPathItem::DrawItem

const unsigned long
	msg_AddPath = 'NewP',
	msg_ChangePath = 'ChgP';

CPathsBox::CPathsBox(BRect r, const char *name)
	: CListBox(r, name)
{
	fName = NULL;
	fPanel = NULL;
} /* CPathsBox::CPathsBox */

CPathsBox::~CPathsBox()
{
	if (fName) free(fName);
} /* CPathsBox::~CPathsBox */

void CPathsBox::SetPrefName(const char *name)
{
	fName = strdup(name);
} /* CPathsBox::SetPrefName */

void CPathsBox::DoOK()
{
// First remove the older prefs
	gPrefs->RemovePref(fName);

	for (int i = 0; i < fList->CountItems(); i++)
		gPrefs->SetIxPrefString(fName, i, static_cast<CPathItem*>(fList->ItemAt(i))->Path());
} /* CPathsBox::DoOK */

void CPathsBox::DoCancel()
{
	StChangeLB changeSelf(this);

//	for (int i = fList->CountItems(); i >= 0; i--)
//		DeleteItem(i);
	fList->MakeEmpty();
	
	const char *p ;
	int i = 0;

	while ((p = gPrefs->GetIxPrefString(fName, i++)) != NULL)
		fList->AddItem(new CPathItem(p));
} /* CPathsBox::DoCancel */

void CPathsBox::MessageReceived(BMessage *msg)
{
	try
	{
		entry_ref ref;
		BEntry e;
		BPath p;
		
		if (msg->WasDropped() && msg->HasRef("refs"))
		{
			FailOSErr(msg->FindRef("refs", &ref));
			FailOSErr(e.SetTo(&ref));
			FailOSErr(e.GetPath(&p));
			
			fList->AddItem(new BStringItem(p.Path()));
			
			ListBoxChanged();
		}
		else switch (msg->what)
		{
			case msg_AddPath:
			case msg_ChangePath:
				FailOSErr(msg->FindRef("refs", &ref));
				FailOSErr(e.SetTo(&ref));
				FailOSErr(e.GetPath(&p));
				
				if (msg->what == msg_AddPath)
					fList->AddItem(new BStringItem(p.Path()));
				else
				{
					BStringItem *item;
					FailOSErr(msg->FindPointer("item", (void**)&item));
					if (fList->IndexOf(item) < 0)
						THROW(("item was removed from list!"));
					item->SetText(p.Path());
				}
				
				delete fPanel;
				fPanel = NULL;
				
				ListBoxChanged();
				break;
				
			default:
				CListBox::MessageReceived(msg);
				break;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* CPathsBox::MessageReceived */

void CPathsBox::AddClicked()
{
	if (fPanel)
		delete fPanel;

	fPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this),
		NULL, B_DIRECTORY_NODE, false, new BMessage(msg_AddPath));
	FailNil(fPanel);
	
	fPanel->Window()->SetTitle("Add Directory");
	fPanel->Show();
} /* CPathsBox::AddClicked */

void CPathsBox::ChangeClicked()
{
	if (fList->CurrentSelection() < 0)
		return;
	
	if (fPanel)
		delete fPanel;

	BStringItem *item = static_cast<BStringItem*>
		(fList->ItemAt(fList->CurrentSelection()));

	entry_ref ref;
	get_ref_for_path(item->Text(), &ref); // may fail
	
	BMessage *msg = new BMessage(msg_ChangePath);
	msg->AddPointer("item", item);
			
	fPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this),
		&ref, B_DIRECTORY_NODE, false, msg);
	FailNil(fPanel);
	
	fPanel->Window()->SetTitle("Change Directory");
	fPanel->Show();
} /* CPathsBox::ChangeClicked */

void CPathsBox::DeleteClicked()
{
	delete fList->RemoveItem(fList->CurrentSelection());
	ListBoxChanged();
} /* CPathsBox::DeleteClicked */

