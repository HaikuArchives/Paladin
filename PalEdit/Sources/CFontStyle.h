/*	$Id: CFontStyle.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 02-06-02
*/

#ifndef FONTSTYLE_H
#define FONTSTYLE_H

#include <vector>

#include <Font.h>

#include "Benaphore.h"
typedef float f_unit_t;

using std::vector;

struct EscapementEntry {
	int unicode;
	f_unit_t escapement;
};

class CFontStyle
{
public:
static	CFontStyle* Locate(const char *name, const char *style,
								f_unit_t size);
static	void Cleanup();
	
	f_unit_t operator[](const char *s);
	f_unit_t CharWidth(const char *s);
	f_unit_t StringWidth(const char *s, int len);
	
private:
	CFontStyle(const char *name, const char *style, f_unit_t size);
	virtual ~CFontStyle();

	void InitTable();
	void ReHash();
	void PrintToStream();

	EscapementEntry *fEscapements;
	int fEscapementsCount;
	int fCharCount;

	static vector<CFontStyle*> fgFontStyleList;

	BFont fFont;
	CBenaphore fLock;
};

inline f_unit_t CFontStyle::CharWidth(const char *s)
{
	return operator[](s);
} /* CFontStyle::CharWidth */

#endif // FONTSTYLE_H
