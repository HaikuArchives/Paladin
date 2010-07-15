/*	$Id: PLongAction.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 04/20/98 16:46:09
*/

#include "pe.h"
#include "PLongAction.h"
#include "CKeyMapper.h"
#include "PMessages.h"

PLongAction::PLongAction(BView *vw)
{
	fView = vw;
	fLastTick = 0;
} /* PLongAction::PLongAction */

PLongAction::PLongAction()
{
	fView = NULL;
	fLastTick = 0;
} /* PLongAction::PLongAction */

PLongAction::~PLongAction()
{
	
} /* PLongAction::~PLongAction */
	
bool PLongAction::PrivateTick()
{
	key_info ki;

	if (get_key_info(&ki) == B_OK)
	{
		int i, j, k;

		k = 0;
		
		for (i = 0; i < 16; i++)
		{
			for (j = 7; j >= 0; j--)
			{
				if (ki.key_states[i] & (1 << j))
				{
					int cmd = CKeyMapper::Instance().GetCmd(NULL, ki.modifiers, 0, k);
					if (cmd == msg_CancelCommand)
						return true;
				}
				k++;
			}
		}
	}

	return false;
} /* PLongAction::PrivateTick */
