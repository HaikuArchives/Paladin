/*	$Id: CLanguageAddOn.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 12/07/97 22:53:14 by Maarten Hekkelman
*/

#include "pe.h"
#include "PText.h"
#include "PApp.h"
#include "CLanguageAddOn.h"
#include "CLanguageInterface.h"
#include "PMessages.h"
#include "PItalicMenuItem.h"
#include "utf-support.h"
#include "PText.h"
#include "Utils.h"
#include "HError.h"
#include "HColorUtils.h"

CLanguageProxy::CLanguageProxy(CLanguageInterface& intf, const char *text, int size,
	int *starts, rgb_color *colors)
	: fText(text)
	, fSize(size)
	, fInterface(intf)
	, fCIndx(0)
	, fStarts(starts)
	, fColors(colors)
	, fFunctionScanHandler(NULL)
	, fNestLevel(0)
{
} /* CLanguageProxy::CLanguageProxy */

CLanguageProxy::CLanguageProxy(CLanguageInterface& intf, PText& text,
	CFunctionScanHandler* handler)
	: fText(text.Text())
	, fSize(text.Size())
	, fInterface(intf)
	, fCIndx(0)
	, fStarts(NULL)
	, fColors(NULL)
	, fFunctionScanHandler(handler)
	, fNestLevel(0)
{
} /* CLanguageProxy::CLanguageProxy */

int CLanguageProxy::Move(int ch, int state)
{
	return fInterface.AddToCurrentKeyword(ch, state);
} /* CLanguageProxy::Move */

int CLanguageProxy::IsKeyword(int state, int32 inSets)
{
	return fInterface.LookupCurrentKeyword(state, inSets);
} /* CLanguageProxy::IsKeyword */

const char *CLanguageProxy::Text() const
{
	return fText;
} /* CLanguageProxy::Text */

int CLanguageProxy::Size() const
{
	return fSize;
} /* CLanguageProxy::Size */

void CLanguageProxy::SetColor(int start, int color)
{
	if (start < 0)
		start = 0;
	if (color < kColorText || color >= kColorEnd)
		color = kColorText;

	if (fStarts && fCIndx < 99 &&
		(fCIndx == 0 || fColors[fCIndx - 1] != gColor[color]))
	{
		if (fCIndx && start == fStarts[fCIndx - 1])
			fCIndx--;

		fColors[fCIndx] = gColor[color];
		fStarts[fCIndx] = start;
		fCIndx++;
	}
} /* CLanguageProxy::SetColor */

void CLanguageProxy::AddFunction(const char *name, const char *match,
	int offset, bool italic, const char *params)
{
	FailNilMsg(fFunctionScanHandler, "Not a valid call in this context");
	fFunctionScanHandler->AddFunction(name, match, offset, italic, fNestLevel, params);
} /* CLanguageProxy::AddFunction */

void CLanguageProxy::AddInclude(const char *name, const char *open, bool italic)
{
	FailNilMsg(fFunctionScanHandler, "Not a valid call in this context");
	fFunctionScanHandler->AddInclude(name, open, italic);
} /* CLanguageProxy::AddFunction */

void CLanguageProxy::AddSeparator(const char* nm)
{
	FailNilMsg(fFunctionScanHandler, "Not a valid call in this context");
	BString name;
	if (nm) {
		while(isspace(*nm) || *nm=='-')
			nm++;
		const char* nme = nm+strlen(nm);
		while(nme > nm) {
			nme--;
			if (!isspace(*nme) && *nme!='-')
				break;
		}
		name.SetTo(nm, 1+nme-nm);
	}
	fFunctionScanHandler->AddSeparator(name.String());
} /* CLanguageProxy::AddSeparator */

bool CLanguageProxy::Includes() const
{
	return gPopupIncludes;
} /* CLanguageProxy::Includes */

bool CLanguageProxy::Prototypes() const
{
	return gPopupProtos;
} /* CLanguageProxy::Prototypes */

bool CLanguageProxy::Types() const
{
	return gPopupFuncs;
} /* CLanguageProxy::Types */

bool CLanguageProxy::Sorted() const
{
	return gPopupSorted;
} /* CLanguageProxy::Sorted */

void CLanguageProxy::CharInfo(const char *txt, int& unicode, int& len) const
{
	len = mcharlen(txt);
	unicode = municode(txt);
} /* CharInfo */

bool CLanguageProxy::isalpha_uc(int unicode) const
{
	return ::isalpha_uc(unicode);
} /* isalpha_uc */

bool CLanguageProxy::isalnum_uc(int unicode) const
{
	return ::isalnum_uc(unicode);
} /* isalnum_uc */

bool CLanguageProxy::isspace_uc(int unicode) const
{
	return ::isspace_uc(unicode);
} /* isspace_uc */

void CLanguageProxy::IncNestLevel() {
	fNestLevel++;
} /* IncNestLevel */

void CLanguageProxy::DecNestLevel() {
	if (fNestLevel)
		fNestLevel--;
} /* DecNestLevel */

void CLanguageProxy::SetNestLevel(uint32 nestLevel) {
	fNestLevel = nestLevel;
} /* SetNestLevel */
