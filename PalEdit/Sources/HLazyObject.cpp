/*	$Id: HLazyObject.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/08/97 09:58:29
*/

#include "pe.h"
#include "HLazyObject.h"
#include <memory>

HLazyObject::HLazyObject(BLooper *looper)
{
	looper->AddHandler(this);
} /* HLazyObject::HLazyObject */

HLazyObject::~HLazyObject()
{
} /* HLazyObject::HLazyObject */

BHandler *HLazyObject::ResolveSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property)
{
	std::auto_ptr<HLazyObject> lets_commit_suicide(this);
	BHandler *result = NULL;

//	if (index > 0)
		result = GetSubSpecifier(msg, index, specifier, form, property);
//	else
	if (result == NULL)
		MustBeMe(property, msg);	
	
	Looper()->RemoveHandler(this);
	return result;
} /* HLazyObject::ResolveSpecifier */

BHandler *HLazyObject::GetSubSpecifier(BMessage */*msg*/, int32 /*index*/,
			BMessage */*specifier*/, int32 /*form*/, const char */*property*/)
{
	return NULL;
} /* HLazyObject::GetSubSpecifier */

void HLazyObject::MustBeMe(const char *property, BMessage *msg)
{
	switch (msg->what)
	{
		case B_GET_PROPERTY:
			GetProperty(property, msg);
			break;
		case B_SET_PROPERTY:
			SetProperty(property, msg);
			break;
		default:
		{
			BMessage reply;
			reply.AddString("Error", "Don't know what to do with this message");
			msg->SendReply(&reply);
			break;
		}
	}
} /* HLazyObject::MustBeMe */

void HLazyObject::GetProperty(const char *property, BMessage *msg)
{
	BMessage reply;
	
	char s[256];
	sprintf(s, "Sorry, but I don't have a property called \"%s\"", property);
	reply.AddString("Error", s);

	msg->SendReply(&reply);
} /* HLazyObject::GetProperty */

void HLazyObject::SetProperty(const char *property, BMessage *msg)
{
	BMessage reply;

	char s[256];
	sprintf(s, "Sorry, but I don't have a property called \"%s\"", property);
	reply.AddString("Error", s);

	msg->SendReply(&reply);
} /* HLazyObject::SetProperty */
