/*	$Id: CLineObject.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 11/24/97 13:49:32 by Maarten Hekkelman
*/

#include "pe.h"
#include "PDoc.h"
#include "PText.h"
#include "HLazyObject.h"
#include "CLineObject.h"
#include "CCharObject.h"
#include "PMessages.h"
#include <memory>

CLineObject::CLineObject(PText *txt, int index, int range)
	: HLazyObject(txt->Doc())
{
	fText = txt;
	fIndex = index;
	fRange  = range;
} /* CLineObject::CLineObject */
		
BHandler *CLineObject::GetSubSpecifier(BMessage *msg, int32 index,
	BMessage *specifier, int32 form, const char *property)
{
	if ((strcmp(property, "Char") == 0 || strcmp(property, "Character") == 0))
	{
		long ix, rng;
		
		if (form == B_INDEX_SPECIFIER)
		{
			if (specifier->FindInt32("index", &ix) == B_OK)
			{
				msg->PopSpecifier();
				ix += fText->LineStart(fIndex);
				return new CCharObject(fText, ix - 1, 1);
			}
		}
		else if (form == B_RANGE_SPECIFIER)
		{
			if (specifier->FindInt32("index", &ix) == B_OK &&
				specifier->FindInt32("range", &rng) == B_OK)
			{
				msg->PopSpecifier();
				ix += fText->LineStart(fIndex);
				return new CCharObject(fText, ix - 1, rng); 
			}
		}
	}
	
	return HLazyObject::GetSubSpecifier(msg, index, specifier, form, property);
} /* CLineObject::GetSubSpecifier */

void CLineObject::MessageReceived(BMessage *msg)
{
	std::auto_ptr<CLineObject> destroy(this);

	switch (msg->what)
	{
		case msg_Select:
		{
			BMessage m(msg_SelectLines);
			m.AddInt32("from", fIndex);
			m.AddInt32("to", fIndex + fRange);
			
			bool activate = false;
			if (msg->HasBool("activate"))
				msg->FindBool("activate", &activate);
			
			m.AddBool("activate", activate);
			BMessenger(fText).SendMessage(&m);
			break;
		}
		default:
			HLazyObject::MessageReceived(msg);
			break;
	}
} /* CLineObject::MustBeMe */

