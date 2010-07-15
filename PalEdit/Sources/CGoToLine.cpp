/*	$Id: CGoToLine.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 10/10/97 20:44:22
*/

#include "pe.h"
#include "PDoc.h"
#include "PText.h"
#include "CGoToLine.h"
#include "PMessages.h"

CGoToLine::CGoToLine(BRect frame, const char *name, window_type type, int flags,
	BWindow *owner)
	: HDialog(frame, name, type, flags | B_NOT_RESIZABLE, owner)
{
	Create();
	Layout();

	SetDefaultButton(fOkButton);
	fLine->MakeFocus();
	Show();
} /* CGoToLine::CGoToLine */

bool CGoToLine::OkClicked()
{
	double line;
	if (GetDouble("line", line) && line == (int)line)
	{
		BMessage m(msg_DoGoToLine);
		m.AddInt32("line", (int)line);
		fOwner->PostMessage(&m,	static_cast<PDoc*>(fOwner)->TextView());
	}

	return true;
} /* CGoToLine::OkClicked */

void CGoToLine::Create(void)
{
	fMainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fLine = new HTextControl(fMainView, "line");
	// Buttons
	fCancelButton = new HButton(fMainView, "cncl", 'cncl', 
								B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
	fOkButton = new HButton(fMainView, "ok", 'ok  ', 
							B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM);
}

void CGoToLine::Layout(void)
{
	fLine->ResizeLocalized("Line:");
	fCancelButton->ResizeLocalized("Cancel");
	fOkButton->ResizeLocalized("Ok");

	float div = fLine->StringWidth("Line:")+20;
	fLine->SetDivider(div);
	fLine->SetWidth(div+fLine->TextView()->StringWidth("1234567")+10);

	//### Layout
	float dx = fMainView->StringWidth("m");
	float dy = fMainView->StringWidth("n");

	fLine->MoveTo(dx, dy);

	fOkButton->MoveTo(fMainView->Right()-fOkButton->Width()-dx, 
					  fMainView->Bottom()-dy-fOkButton->Height());
	fCancelButton->MoveTo(fOkButton->Left()-fCancelButton->Width()-dx, 
					 	  fMainView->Bottom()-dy-fCancelButton->Height());

	float minW = fOkButton->Width()+fCancelButton->Width()+3*dx;
	float minH = fLine->Bottom()+fOkButton->Height()+4*dy;
	ResizeToLimits(minW, minW, minH, minH);
}
