/*	$Id: PTypeAHeadList.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/18/98 21:25:47
*/

#include "pe.h"
#include "PGroupWindow.h"
#include "PTypeAHeadList.h"
#include "HDefines.h"
#include "HError.h"
#include "Utils.h"
#include "utf-support.h"
#include <algorithm>

PGroupStatus::PGroupStatus(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW)
{
	fPath = NULL;
	fText = NULL;
} /* PGroupStatus::PGroupStatus */

PGroupStatus::~PGroupStatus()
{
	if (fPath) free(fPath);
	if (fText) free(fText);
} /* PGroupStatus::~PGroupStatus */

void PGroupStatus::Draw(BRect /*update*/)
{
	BRect b(Bounds());
	
	FillRect(b, B_SOLID_LOW);

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeLine(b.LeftTop(), b.RightTop());
	b.top++;

	font_height fh;
	be_plain_font->GetHeight(&fh);
	float l = b.top + fh.ascent;

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	StrokeLine(b.RightBottom(), b.LeftBottom());
//	StrokeLine(b.RightTop(), b.RightBottom());
	
	SetHighColor(kWhite);
	StrokeLine(b.LeftTop(), b.RightTop());
	StrokeLine(b.LeftTop(), b.LeftBottom());

	SetHighColor(kBlack);
	MovePenTo(3, l);
	
	if (fText)
		DrawString(fText);
	else if (fOwner)
	{
		PGroupWindow* gw = dynamic_cast<PGroupWindow*>(fOwner);
		if (gw) {
			char s[32];
			sprintf(s, "%d items", gw->CountItems());
			DrawString(s);
		}
	}
} /* PGroupStatus::Draw */

void PGroupStatus::MouseDown(BPoint where)
{
	if (fPath)
	{
		BPopUpMenu popup("no title");
		popup.SetFont(be_plain_font);
		
		char *s = strdup(fPath), *d;
	
		d = strrchr(s, '/');
		if (d) *d = 0;
		
		d = strtok(s, "/");
		while (d)
		{
			popup.AddItem(new BMenuItem(d, NULL), 0);
			d = strtok(NULL, "/");
		}
		
		where.y = Bounds().bottom + 1;
		BMenuItem *i = popup.Go(ConvertToScreen(where), true, false, ConvertToScreen(Bounds()));
		
		if (i)
		{
			free(s);
			s = strdup(fPath);
			d = strchr(s, '/');
			FailNil(d);
	
			int ix = popup.CountItems() - popup.IndexOf(i);
			
			while (ix--)
				d = strchr(d + 1, '/');
			
			FailNil(d);
			*d = 0;
	
			entry_ref ref;
			FailOSErr(get_ref_for_path(s, &ref));
			OpenInTracker(ref);
		}
		
		free(s);
	}
	else
		beep();
} /* PGroupStatus::MouseDown */

void PGroupStatus::AttachedToWindow()
{
	fOwner = Window();

	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
} /* PGroupStatus::AttachedToWindow */

void PGroupStatus::SetPath(const char *path)
{
	if (fPath) free(fPath);
	fPath = strdup(path);
	Draw(Bounds());
} /* PGroupStatus::SetPath */

void PGroupStatus::SetTypeahead(const char *text)
{
	if (fText) free(fText);
	fText = text ? strdup(text) : NULL;
	Draw(Bounds());
} // PGroupStatus::SetText



#pragma mark -

class IsLess
{
  public:
	IsLess() {}
	bool operator () (BStringItem*& item, const string& t)
		{ return strcasecmp(item->Text(), t.c_str()) < 0; }
};

class IsLessItem
{
  public:
	IsLessItem() {}
	bool operator () (const BStringItem* const & a, const BStringItem* const& b) const
		{ return strcasecmp(a->Text(), b->Text()) < 0; }
};

void PTypeAHeadList::KeyDown(const char *bytes, int32 numBytes)
{
	if (!iscntrl(*bytes) || (*bytes == B_BACKSPACE && fTyped.length()))
	{
		if (*bytes == B_BACKSPACE)
		{
			int l = mprevcharlen(fTyped.c_str() + fTyped.length());
			fTyped.erase(fTyped.length() - l, fTyped.length());
		}
		else if (fLastKeyDown == 0)
			fTyped.assign(bytes, numBytes);
		else
			fTyped.append(bytes, numBytes);
	
		fStatus->SetTypeahead(fTyped.c_str());
		fLastKeyDown = system_time();

		BStringItem **start = (BStringItem **)Items();

		vector<BStringItem*> items(start, start + CountItems());
		stable_sort(items.begin(), items.end(), IsLessItem());
		
		vector<BStringItem*>::iterator found;
		found = lower_bound(items.begin(), items.end(), fTyped, IsLess());
		
		if (found != items.end())
			Select(IndexOf(*found));
/*	[zooey] let the selection move only if anything has been found:
		else
			Select(-1);
*/		
		ScrollToSelection();
	}
	else if (*bytes == B_TAB)
	{
		int32 modifiers;
		Looper()->CurrentMessage()->FindInt32("modifiers", &modifiers);
		HandleTab(modifiers & B_SHIFT_KEY);
	}
	else
		BListView::KeyDown(bytes, numBytes);
} // PTypeAHeadList::KeyDown

void PTypeAHeadList::Pulse()
{
	if (fLastKeyDown && fLastKeyDown < system_time() - 1000*1000)
	{
		fStatus->SetTypeahead(NULL);
		fLastKeyDown = 0;
	}
} // PTypeAHeadList::Pulse

void PTypeAHeadList::HandleTab(bool backward)
{
	BStringItem **start = (BStringItem **)Items(), *item;

	item = static_cast<BStringItem*>(ItemAt(CurrentSelection()));
	if (item == NULL)
		return;

	vector<BStringItem*> items(start, start + CountItems());
	stable_sort(items.begin(), items.end(), IsLessItem());
	
	vector<BStringItem*>::iterator found = find(items.begin(), items.end(), item);

	if (backward)
		--found;
	else
		++found;
	
	if (found != items.end())
		Select(IndexOf(*found));
	else
		Select(-1);
	
	ScrollToSelection();
} // PTypeAHeadList::HandleTab
