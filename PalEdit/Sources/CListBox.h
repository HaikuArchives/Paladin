/*	$Id: CListBox.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/20/97 20:05:41
*/

#ifndef CLISTBOX_H
#define CLISTBOX_H

class BListView;

class CListBox
	: public BView
{
friend class StChangeLB;
public:
		CListBox(BRect frame, const char *name);
		
virtual void MessageReceived(BMessage *msg);
virtual void AllAttached();

virtual	void AddClicked() = 0;
virtual	void ChangeClicked() = 0;
virtual	void DeleteClicked() = 0;
virtual void Selected();

virtual void DoOK() = 0;
virtual void DoCancel() = 0;
virtual void UpdateFields();
		
		void ListBoxChanged();

protected:

		BListView	*fList;
		BButton		*fAdd, *fChange, *fDelete;
		bool		fSelfChange;
};

class StChangeLB {
public:
		StChangeLB(CListBox *lb)	{ fLB = lb; fLB->fSelfChange = true; };
		~StChangeLB()				{ fLB->fSelfChange = false; };
		CListBox *fLB;
};

#endif // CLISTBOX_H
