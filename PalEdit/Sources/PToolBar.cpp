/*	$Id: PToolBar.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 02/21/98 20:48:00
*/

#include "pe.h"
#include "PToolBar.h"
#include "PText.h"
#include "HDefines.h"

PToolBar::PToolBar(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	fShowTabs = false;
	fHOffset = 3;
} /* PToolBar::PToolBar */

void PToolBar::Draw(BRect /*update*/)
{
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	BRect b(Bounds());
	b.bottom -= 1;
		
	FillRect(b, B_SOLID_LOW);

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeLine(b.LeftBottom(), b.RightBottom());
	
	SetHighColor(kBlack);
	
	if (fShowTabs)
	{
		BRect r(b);
		r.top = r.bottom - kTabStopHeight;
		
//		SetHighColor(kWhite);
//		StrokeLine(r.LeftTop(), r.LeftBottom());
//		StrokeLine(r.LeftTop(), r.RightTop());
		
		if (fTabWidth > 1)
		{
			float x;
			
			SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	
			for (x = fHOffset; x < r.right; x += fTabWidth)
				StrokeLine(BPoint(x, r.bottom - kTabStopHeight + 1), BPoint(x, r.bottom - 1));
			
//			x = fHScroll + f80Columns;
//			
//			SetHighColor(kBlack);
//			StrokeLine(BPoint(x, b.bottom - kTabStopHeight + 1), BPoint(x, b.bottom));
		}
	}
} /* PToolBar::Draw */

void PToolBar::MouseMoved(BPoint where, uint32 code, const BMessage *a_message)
{
	if (Bounds().Contains(where))
		be_app->SetCursor(B_HAND_CURSOR);
} /* PToolBar::MouseMoved */

void PToolBar::SetShowsTabs(bool show)
{
	fShowTabs = show;
	Invalidate(Bounds());
} /* PToolBar::SetShowsTabs */

void PToolBar::SetHOffset(float x)
{
	fHOffset = x;
	Draw(Bounds());
} /* PToolBar::SetHOffset */

void PToolBar::SetTabWidth(float tabWidth, int tabCount)
{
	fTabWidth = tabWidth;
	fTabCount = tabCount;
} /* PToolBar::SetTabWidth */

