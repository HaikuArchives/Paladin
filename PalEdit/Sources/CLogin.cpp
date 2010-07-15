/*	$Id: CLogin.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 03/30/98 22:58:03
*/

#include "pe.h"
#include "CLogin.h"

string sfUsername, sfPassword;

CLogin::CLogin(BRect frame, const char *name, window_type type, int flags,
				BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	SetText("username", sfUsername.c_str());
	dynamic_cast<BTextControl*>(FindView("password"))->TextView()->HideTyping(true);
	SetText("password", sfPassword.c_str());
	if (sfUsername.length())
		FindView("password")->MakeFocus(true);
	else
		FindView("username")->MakeFocus(true);
} /* CLogin::CLogin */

CLogin::~CLogin()
{
} /* CLogin::~CLogin */

bool CLogin::OkClicked()
{
	sfUsername = GetText("username");
	sfPassword = GetText("password");
	*fOK = true;

	return true;
} /* CLogin::OkClicked */

bool CLogin::CancelClicked()
{
	*fOK = false;
	return true;
} /* CLogin::CancelClicked */

void CLogin::Connect(const char *server, string& username, string& password, bool *ok)
{
	char name[NAME_MAX];
	BStringView *cap = dynamic_cast<BStringView*>(FindView("server"));
	if (cap)
	{
		sprintf(name, cap->Text(), server);
		cap->SetText(name);
	}

	fOK = ok;

	Show();

	long l;
	wait_for_thread(Thread(), &l);
	username = sfUsername;
	password = sfPassword;
} /* CLogin::Connect */
