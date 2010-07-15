/*	$Id: PTypeAHeadList.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/18/98 21:25:40
*/

#ifndef PTYPEAHEADLIST_H
#define PTYPEAHEADLIST_H

class PGroupStatus : public BView {
public:
			PGroupStatus(BRect frame, const char *name);
			~PGroupStatus();
		
virtual	void Draw(BRect update);
virtual	void MouseDown(BPoint where);
virtual	void AttachedToWindow();

			void SetPath(const char *path);
			void SetTypeahead(const char *text);

private:
			BWindow *fOwner;
			char *fPath, *fText;
};

class CProjectItem;
class PProjectItem : public BStringItem
{
  public:
	PProjectItem(const char* name, int level = 0, CProjectItem* modelItem=NULL)
		: BStringItem(name, level), fModelItem(modelItem) {}
	
	CProjectItem* ModelItem()				{ return fModelItem; }
  private:
   CProjectItem* fModelItem;
};

class PEntryItem : public PProjectItem
{
  public:
	PEntryItem(const entry_ref& ref, int level = 0, CProjectItem* modelItem=NULL)
		: PProjectItem(ref.name, level, modelItem), fEntry(ref) {}

	const entry_ref& Ref() const
		{ return fEntry; }

	bool operator < (const PEntryItem& item) const
		{ return strcasecmp(Text(), item.Text()) < 0; }

	static int Compare(const void *a, const void *b)
	{
		return strcasecmp(
			(*((PEntryItem **)(a)))->fEntry.name,
			(*((PEntryItem **)(b)))->fEntry.name);
	} /* PEntryItem::Compare */

  private:
	entry_ref fEntry;
};

class PTypeAHeadList : public BOutlineListView
{
  public:
	PTypeAHeadList(BRect frame, const char *name, PGroupStatus *status)
		: BOutlineListView(frame, name, B_MULTIPLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES,
			B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE | B_PULSE_NEEDED)
		, fLastKeyDown(0), fStatus(status) {}
	
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void Pulse();

  private:
  
  	void HandleTab(bool backward);
  	
 	bigtime_t fLastKeyDown;
 	string fTyped;
 	PGroupStatus *fStatus;
};

#endif // PTYPEAHEADLIST_H
