/*	$Id: RElem.cpp,v 1.3 2009/06/20 14:24:30 darkwyrm Exp $
	
	Copyright 1996, 1997, 1998
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

	Created: 12/02/98 15:35:47
*/

#include "REval.h"
#include "RElem.h"
#include "rez.h"
#include "RState.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


RElem::RElem(REval *v, int size, int offset, RState *state)
{
	fNext = NULL;
	fType = relEval;
	fValue = v;
	fLabel = NULL;
	fSize = size;
	fOffset = offset;
	fState = state;
	fValue->SetElement(this);
} /* RElem::RElem */

RElem::RElem(const char *s, int size, int offset, RState *state)
{
	fNext = NULL;
	fType = relString;
	fString = (char *)malloc(size);
	if (!fString) error("Insufficient memory");
	memcpy(fString, s, size);
	fLabel = NULL;
	fSize = 8 * size;
	fOffset = offset;
	fState = state;
} /* RElem::RElem */

RElem::RElem(int size, int offset, RState *state)
{
	fNext = NULL;
	fValue = NULL;
	fType = relFill;
	fLabel = NULL;
	fSize = size;
	fOffset = offset;
	fState = state;
} /* RElem::RElem */
	
void RElem::Write()
{
	BMallocIO stream;
	RElem *elem = this;
	
	while (elem)
	{
		elem->Write(this, stream);
		elem = elem->fNext;
	}
	
	gResSize = stream.BufferLength();
	gResData = malloc(gResSize);
	memcpy(gResData, stream.Buffer(), gResSize);
} /* RElem::Write */

void RElem::Write(RElem *head, BPositionIO& stream)
{
	if (verbose >= 2) printf("Writing %d, this: %08x, next: %08x\n", fType, (uint)this, (uint)fNext);
	switch (fType)
	{
		case relEval:
		{
			int t = REvaluate(fValue, head);
			switch (fSize)
			{
				case 8:
				{
					char c = t;
					stream.Write(&c, 1);
					break;
				}
				case 16:
				{
					short s = t;
					stream.Write(&s, 2);
					break;
				}
				case 32:
					stream.Write(&t, 4);
					break;
				default:
					error("unsupported size: %d", fSize);
			}
			break;
		}
		case relString:
			stream.Write(fString, fSize / 8);
			break;
		case relFill:
			// [zooey]: this has been added because gcc throws a warning
			//				about relFill not being handled otherwise.
			//				TODO: shouldn't we actually DO something here?
			break;
	}
} /* RElem::Write */

void RAddElement(RElem **head, REval *v, int size, RState *state)
{
	int offset = 0;
	
	RElem *h = *head;
	
	if (h)
	{
		while (h->fNext)
		{
			offset += h->fSize;
			h = h->fNext;
		}
	}
	
	RElem *n = new RElem(v, size, offset, state);
	if (n->fValue == NULL || n->fValue->fElem != n)
		error("internal error 5");
	
	if (h)
		h->fNext = n;
	else
		*head = n;
} /* RAddElement */

void RAddElement(RElem **head, const char *s, int size, RState *state)
{
	int offset = 0;
	
	RElem *h = *head;
	
	if (h)
	{
		while (h->fNext)
		{
			offset += h->fSize;
			h = h->fNext;
		}
	}
	
	RElem *n = new RElem(s, size, offset, state);
	
	if (h)
		h->fNext = n;
	else
		*head = n;
} /* RAddElement */

int RElem::FindIdentifier(int v)
{
	RSValue *vstate = fState ? dynamic_cast<RSValue*>(fState) : NULL;
	int id = v;
	
	if (!(vstate && vstate->ResolveIdentifier(v)))
		error("Unknown identifier: %s", ST_Ident(id));
	return v;
} /* RElem::FindIdentifier */
