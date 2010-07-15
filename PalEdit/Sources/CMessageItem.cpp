/*	$Id: CMessageItem.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/11/98 22:11:49
*/

#include "pe.h"
#include "CMessageItem.h"
#include "PMessages.h"
#include "PApp.h"
#include "HAppResFile.h"
#include "HError.h"
#include "HColorUtils.h"

unsigned char *CMessageItem::fInfoIcon, *CMessageItem::fWarningIcon, *CMessageItem::fErrorIcon;

CMessageItem::CMessageItem(const char *msg, int msg_size, EMessageKind kind,
	entry_ref *file, int line)
{
	if (fInfoIcon == NULL)
	{
		fInfoIcon = (uchar *)HResources::GetResource('MICN', 502);
		fWarningIcon = (uchar *)HResources::GetResource('MICN', 500);
		fErrorIcon = (uchar *)HResources::GetResource('MICN', 501);
	}
	
	fMsg = (char *)malloc(msg_size + 1);
	FailNil(fMsg);
	memcpy(fMsg, msg, msg_size);
	fMsg[msg_size] = 0;
	
	if (file)
		fFile = new entry_ref(*file);
	else
		fFile = NULL;
	fLine = line;
	fKind = kind;
} // CMessageItem::CMessageItem

CMessageItem::~CMessageItem()
{
	free(fMsg);
	delete fFile;
} // CMessageItem::~CMessageItem

void CMessageItem::DrawItem(BView *owner, BRect bounds, bool /*complete*/)
{
	if (IsSelected())
		owner->SetLowColor(gColor[kColorSelection]);
	else
		owner->SetLowColor(gColor[kColorLow]);
	
	owner->FillRect(bounds, B_SOLID_LOW);
	
	owner->SetHighColor(gColor[kColorText]);
	
	font_height fi;

	be_plain_font->GetHeight(&fi);
	owner->SetFont(be_plain_font);
	
	BBitmap bm(BRect(0, 0, 11, 11), B_COLOR_8_BIT);
	switch (fKind)
	{
		case msgInfo:		bm.SetBits(fInfoIcon, 144, 0, B_COLOR_8_BIT); break;
		case msgWarning:	bm.SetBits(fWarningIcon, 144, 0, B_COLOR_8_BIT); break;
		case msgError:		bm.SetBits(fErrorIcon, 144, 0, B_COLOR_8_BIT); break;
	}

	owner->SetDrawingMode(B_OP_OVER);
	owner->DrawBitmap(&bm, BPoint(bounds.left + 1, bounds.top + 1));
	owner->SetDrawingMode(B_OP_COPY);
	
	owner->DrawString(fMsg, BPoint(bounds.left + 15, bounds.bottom - 1 - fi.descent));
	owner->StrokeLine(bounds.LeftBottom(), bounds.RightBottom(), B_MIXED_COLORS);
} // CMessageItem::DrawItem

void CMessageItem::ShowMessage()
{
	if (fFile)
	{
		BPath p;
		BEntry(fFile, true).GetPath(&p);
		
		BMessage msg(msg_Select);
//		msg.AddSpecifier("Char", fSelStart, fSelLen);
		msg.AddSpecifier("Line", fLine);
		msg.AddSpecifier("Window", p.Path());
		msg.AddBool("activate", true);
//		msg.AddBool("skipspaces", true);
		be_app_messenger.SendMessage(&msg);
	}
	else
		beep();
} // CMessageItem::PrintToStream
