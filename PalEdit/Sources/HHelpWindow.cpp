/*	$Id: HHelpWindow.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 1/8/98
*/

#include "pe.h"
#include "HHelpWindow.h"

HHelpView::HHelpView(BRect r, const char *helptext)
	: BView(r, "help view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	fHelpText = strdup(helptext);
	SetViewColor(255, 255, 203);
	SetLowColor(ViewColor());
	SetFont(be_plain_font);
} /* HHelpView::HHelpView */

HHelpView::~HHelpView()
{
	free(fHelpText);
} /* HHelpView::~HHelpView */
			
void HHelpView::Draw(BRect update)
{
	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	DrawString(fHelpText, BPoint(2, fh.ascent - 1));
} /* HHelpView::Draw */
			
HHelpWindow::HHelpWindow(BRect r, const char *msg)
	: BWindow(r, "", B_BORDERED_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL, B_AVOID_FOCUS | B_AVOID_FRONT)
{
	r.OffsetTo(0, 0);
	AddChild(new HHelpView(r, msg));
	Show();
} /* HHelpWindow::HHelpWindow */

