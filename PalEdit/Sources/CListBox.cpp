/*	$Id: CListBox.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/20/97 20:08:37
*/

#include "pe.h"
#include "CListBox.h"
#include "PMessages.h"
#include "Utils.h"
#include "PApp.h"
#include "HDefines.h"
#include "HDialog.h"

const unsigned long
	msg_LBAdd = 'lbAd',
	msg_LBChange = 'lbCn',
	msg_LBDelete = 'lbDl',
	msg_LBSelect = 'lbSl';

CListBox::CListBox(BRect frame, const char *name)
	: BView(frame, name, 0, B_WILL_DRAW)
{
	fSelfChange = true;

	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BRect r(Bounds());
	r.InsetBy(2, 2);
	r.right -= 70 * gFactor + B_V_SCROLL_BAR_WIDTH;
	
	fList = new BListView(r, "list", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	fList->SetSelectionMessage(new BMessage(msg_LBSelect));
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true));
	
	BRect b(Bounds());
	b.left = b.right - 60 * gFactor;
	float dy = 30 * gFactor;
	b.bottom = b.top + 20 * gFactor;
	
	AddChild(fAdd = new BButton(b, "add", "Add", new BMessage(msg_LBAdd)));
	b.OffsetBy(0, dy);
	AddChild(fChange = new BButton(b, "change", "Change", new BMessage(msg_LBChange)));
	b.OffsetBy(0, dy);
	AddChild(fDelete = new BButton(b, "delete", "Delete", new BMessage(msg_LBDelete)));
	
	fSelfChange = false;
} // CListBox::CListBox

void CListBox::AllAttached()
{
	FailOSErr(fAdd->SetTarget(this));
	FailOSErr(fChange->SetTarget(this));
	FailOSErr(fDelete->SetTarget(this));
	FailOSErr(fList->SetTarget(this));
} /* CListBox::AttachedToWindow */
		
void CListBox::MessageReceived(BMessage *msg)
{
	try
	{
		switch (msg->what)
		{
			case msg_LBAdd:
				AddClicked();
				break;
			
			case msg_LBDelete:
				DeleteClicked();
				break;
			
			case msg_LBChange:
				ChangeClicked();
				break;
			
			case msg_LBSelect:
				Selected();
				break;
			
			default:
				BView::MessageReceived(msg);
				break;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* CListBox::MessageReceived */

void CListBox::ListBoxChanged()
{
	if (!fSelfChange)
	{
		BMessage msg(msg_FieldChanged);
		msg.AddPointer("source", this);
		Looper()->PostMessage(&msg);
	}
} /* CListBox::ListBoxChanged */

void CListBox::Selected()
{
} /* CListBox::Selected */

void CListBox::UpdateFields()
{
	int ix = fList->CurrentSelection();
	fAdd->SetEnabled(true);
	fDelete->SetEnabled(ix >= 0);
	fChange->SetEnabled(ix >= 0);
} // CListBox::UpdateFields
