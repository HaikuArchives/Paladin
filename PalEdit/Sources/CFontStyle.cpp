/*	$Id: CFontStyle.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

#include "pe.h"
#include "CFontStyle.h"
#include "utf-support.h"
#include "HError.h"

const char kDefChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	"~!@#$%^&*()_+=-[]{}\\|;:<>'\"";

vector<CFontStyle*> CFontStyle::fgFontStyleList;

CFontStyle* CFontStyle::Locate(const char *name, const char *style,
	f_unit_t size)
{
	BFont font;
	font.SetFamilyAndStyle(name, style);
	font.SetSize(size);
	
	vector<CFontStyle*>::iterator i;
	
	for (i = fgFontStyleList.begin(); i != fgFontStyleList.end(); i++)
	{
		if ((*i)->fFont == font)
			return *i;
	}
	
	CFontStyle* result = new CFontStyle(name, style, size);
	fgFontStyleList.push_back(result);
	
	return result;
} /* CFontStyle::Locate */
	
f_unit_t CFontStyle::operator[](const char *s)
{
	int unicode = municode(s);
	
	{
		StBenaphore lock(&fLock);
		
		int i = unicode % fEscapementsCount, j = i - 1;
		if (j < 0) j+= fEscapementsCount;
		
		ASSERT(j >= 0);
		ASSERT(j < fEscapementsCount);
		
		while (true)
		{
			ASSERT(i >= 0);
			ASSERT(i < fEscapementsCount);
	
			if (fEscapements[i].unicode == unicode)
				return fEscapements[i].escapement;
	
			if (fEscapements[i].unicode == 0)
			{
				float w[1];
				char buf[10];
				int cl = mcharlen(s);
				
				strncpy(buf, s, cl);
				buf[cl] = 0;
	
				fFont.GetEscapements(buf, 1, w);
				w[0] *= fFont.Size();
	
				fEscapements[i].unicode = unicode;
				fEscapements[i].escapement = w[0];
				
				fCharCount++;
				
				if (fCharCount > fEscapementsCount / 2)
					ReHash();
				
				return fEscapements[i].escapement;
			}
	
			if (i == j) {
				break;
			}
			
			i = (i + 1) % fEscapementsCount;
		}
		
		ReHash();
	
//		printf("recursion for %d\n", unicode);
	}

	return operator[](s);
} /* CFontStyle::operator[] */
	
CFontStyle::CFontStyle(const char *name, const char *style, f_unit_t size)
	: fLock("font lock")
{
	fEscapementsCount = 0;
	fEscapements = NULL;
	fCharCount = 0;
	fFont.SetFamilyAndStyle(name, style);
	fFont.SetSize(size);
	InitTable();
} /* CFontStyle::CFontStyle */

CFontStyle::~CFontStyle()
{
	if (fEscapements) free(fEscapements);
}

void CFontStyle::InitTable()
{
	int dc = mstrlen(kDefChars);
	f_unit_t *w;

	fEscapementsCount = 2 * dc;
	fEscapements = (EscapementEntry *)calloc(fEscapementsCount, sizeof(EscapementEntry));
	FailNil(fEscapements);
	
	w = (f_unit_t *)malloc(dc * sizeof(f_unit_t));
	FailNil(w);

	fFont.GetEscapements(kDefChars, mstrlen(kDefChars), w);
	
	const char *s = kDefChars;
	
	for (int i = 0; i < dc; i++)
	{
		int cl = mcharlen(s);
		int uc = municode(s);
		int ix = uc % fEscapementsCount;

		while (fEscapements[ix].unicode != 0)
			ix = (ix + 1) % fEscapementsCount;
		
		fEscapements[ix].unicode = uc;
		fEscapements[ix].escapement = w[i] * fFont.Size();
		
		s += cl;
	}
	
	free(w);
} /* CFontStyle::InitTable */

void CFontStyle::ReHash()
{
	int nc = std::max(1, fEscapementsCount * 2);
	EscapementEntry *n = (EscapementEntry *)calloc(nc, sizeof(EscapementEntry));
	FailNil(n);
	
	for (int i = 0; i < fEscapementsCount; i++)
	{
		int ni = fEscapements[i].unicode % nc;
		
		ASSERT(ni >= 0);
		ASSERT(ni < nc);
		
		while (n[ni].unicode != 0)
			ni = (ni + 1) % nc;
		
		n[ni].unicode = fEscapements[i].unicode;
		n[ni].escapement = fEscapements[i].escapement;
	}
	
	if (fEscapements) free(fEscapements);
	fEscapements = n;
	fEscapementsCount = nc;
} /* CFontStyle::ReHash */

void CFontStyle::PrintToStream()
{
	for (int i = 0; i < fEscapementsCount; i++)
		if (fEscapements[i].unicode)
		{
			printf("entry %d: char %d(%c) = %g\n", i, fEscapements[i].unicode,
				fEscapements[i].unicode, fEscapements[i].escapement);
		}
} /* CFontStyle::PrintToStream */

void CFontStyle::Cleanup()
{
	vector<CFontStyle*>::iterator i;
	
	for (i = fgFontStyleList.begin(); i != fgFontStyleList.end(); i++)
		delete *i;
} /* CFontStyle::Cleanup */

f_unit_t CFontStyle::StringWidth(const char *s, int len) {
	f_unit_t r = 0;
	
	while (len > 0) {
		r += CharWidth(s);
		int cl = mcharlen(s);
		s += cl;
		len -= cl;
	}
	
	return r;
} /* CFontStyle::StringWidth */

