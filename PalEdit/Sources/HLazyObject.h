/*	$Id: HLazyObject.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/08/97 09:58:39
	
	To understand what this object does, see the article
	"Better Apple Event coding through objects" written by
	Eric Berdahl in Apple's technical journal Develop nr 13.
	
	In short, an abstract thing like a cell is not implemented
	as an object but must be referenced like one in scripting.
	This means we have to create a specifier every time the
	scripting architecture asks for e.g. a cell.
	This object has to be destroyed afterwards to avoid leaking.
*/

#ifndef HLAZYOBJECT_H
#define HLAZYOBJECT_H

#include <Handler.h>

#include "HLibHekkel.h"

class IMPEXP_LIBHEKKEL HLazyObject : public BHandler {
public:
		HLazyObject(BLooper *looper);
		~HLazyObject();

virtual	BHandler *GetSubSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property);

virtual	void MustBeMe(const char *property, BMessage *msg);

virtual	void GetProperty(const char *property, BMessage *msg);
virtual	void SetProperty(const char *property, BMessage *msg);

private:
					// never touch this one, please
virtual	BHandler *ResolveSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property);
};

#endif // HLAZYOBJECT_H
