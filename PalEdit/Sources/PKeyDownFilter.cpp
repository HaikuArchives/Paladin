/*	$Id: PKeyDownFilter.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/07/97 21:53:17
*/

#include "pe.h"
#include "PText.h"
#include "PMessages.h"
#include "PApp.h"
#include "KeyBindings.h"
#include "CKeyMapper.h"
#include "PKeyDownFilter.h"

PKeyDownFilter::PKeyDownFilter()
	: BMessageFilter(B_KEY_DOWN)
{
} /* PKeyDownFilter::PKeyDownFilter */

filter_result PKeyDownFilter::Filter(BMessage *msg, BHandler **target)
{
	int32 key, modifiers, rawchar;
	filter_result result = B_DISPATCH_MESSAGE;
	PText *txt = dynamic_cast<PText*>(*target);

	if (msg->FindInt32("modifiers", &modifiers) == B_OK &&
		msg->FindInt32("raw_char", &rawchar) == B_OK &&
		msg->FindInt32("key", &key) == B_OK)
	{
		int cmd = CKeyMapper::Instance().GetCmd(txt, modifiers, rawchar, key);
		
		switch (cmd)
		{
			case 0:
				break;

			case msg_New:
			case msg_Open:
			case msg_NewGroup:
			case msg_Quit:
			case msg_Preferences:
			case msg_Worksheet:
				be_app_messenger.SendMessage(cmd);
				result = B_SKIP_MESSAGE;
				break;
			
			default:
				if (txt)	// only translate keybindings for text view
					msg->what = cmd;
				else		// else send them to the app, maybe it can do something useful...
					be_app_messenger.SendMessage(cmd);
				break;
		}
	}
	
	return result;
} /* PKeyDownFilter::Filter */
