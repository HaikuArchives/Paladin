/*	$Id: RState.cpp,v 1.3 2009/06/20 14:24:30 darkwyrm Exp $
	
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

	Created: 12/02/98 15:38:02
*/

#include "rez.h"
#include "RState.h"
#include "REval.h"
#include "SymbolTable.h"
#include <string.h>
#include <typeinfo>
#include <support/Debug.h>
#include <ByteOrder.h>
#include <List.h>

bool inited = false;
intmap RState::sfTypeMap;
intmap gValueMap;

#pragma mark --- RState ---

RState::RState()
{
	fNext = NULL;
} /* RState::RState */

RState::~RState()
{
} /* RState::~RState */

RState* RState::FirstState(int type)
{
	gValueMap.erase(gValueMap.begin(), gValueMap.end());
	return (RState *)sfTypeMap[type];
} /* RState::FirstState */

void RState::FinishType(int type, RState *state)
{
	int sType = htonl(type);
	
	if (sfTypeMap.find(type) != sfTypeMap.end())
		warn("warning: redefinition of type '%4.4s'", &sType);
	
	sfTypeMap[type] = (int)state;
} /* RState::FinishType */

void RState::CopyType(int type1, int type2)
{
	int sType = htonl(type1);

	if (sfTypeMap.find(type1) != sfTypeMap.end())
		warn("warning: redefinition of type '%4.4s'", &sType);
	
	sfTypeMap[type1] = sfTypeMap[type2];
} /* RState::CopyType */

RState* RState::Shift(int v, int token, RElem** head)
{
	if (!fNext) return NULL;

	RSValue *sv = dynamic_cast<RSValue*>(fNext);
	if (sv && sv->fHasDefault)
		return sv->Shift(sv->fValue, sv->fType, head);
	else
		return fNext;
} /* RState::Shift */

void RState::SetNext(RState *next)
{
	ASSERT(next);
	if (!fNext && next != this)
		fNext = next;
	else if (fNext)
		fNext->SetNext(next);
	else
		error("Internal error, next == this || fNext != NULL");
} /* RState::SetNext */

#pragma mark -
#pragma mark --- RSValue and other values ---

RSValue::RSValue(int type)
{
	fType = type;
	fHasDefault = false;
	fValue = 0;
	fIdents = NULL;
} /* RSValue::RSValue */

void RSValue::AddIdentifiers(BList *idents)
{
	fIdents = idents;
} /* RSValue::AddIdentifier */

bool RSValue::ResolveIdentifier(int& v)
{
	if (!fIdents) return false;
	
	for (int i = 0; i < fIdents->CountItems(); i++)
	{
		RSymbol *s = (RSymbol *)fIdents->ItemAt(i);
		if (s->sIdentifier == v)
		{
			v = s->sValue;
			return true;
		}
	}
	return false;
} /* RSValue::ResolveIdentifier */

void RSValue::SetDefaultValue(int v)
{
	fHasDefault = true;
	fValue = v;
} /* RSValue::SetDefaultValue */

#pragma mark -

RSStringValue::RSStringValue(int kind, int size)
	: RSValue(tString)
{
	fKind = kind;
	fSize = size;
} /* RSWStringValue::RSWStringValue */

RState* RSStringValue::Shift(int v, int token, RElem** head)
{
	int t = token;
	
	if (token == tIdent)
	{
		int id = v;

		if (ResolveIdentifier(v))
			t = tString;
		else
			error("Unknown identifier: %s", ST_Ident(id));
	}
	
	if (t == tString || t == tRaw)
	{
		const char *s = (char *)v;
		short l;
		char *p;

		if (fKind == skHex)
			l = *(long *)v;
		else
			l = strlen(s);
		
		int size = l;
		if (fSize > l) size = fSize;

		p = (char *)malloc(size + 2);
		memset(p, 0, size + 2);
		if (!p) error("Insufficient memory");
		
		switch (fKind)
		{
			case skStr:
				memcpy(p, s, l);
				RAddElement(head, p, (fSize ? fSize : l), this);
				break;
			
			case skPStr:
				memcpy(p + 1, s, l);
				p[0] = l;
				RAddElement(head, p, (fSize ? fSize : l + 1), this);
				break;
			
			case skWStr:
				memcpy(p + 2, s, l);
				memcpy(p, &l, 2);
				RAddElement(head, p, (fSize ? fSize : l + 2), this);
				break;
			
			case skCStr:
				memcpy(p, s, l);
				RAddElement(head, p, (fSize ? fSize : l + 1), this);
				break;
			
			case skHex:
				memcpy(p, s + sizeof(long), l);
				RAddElement(head, p, (fSize ? fSize : l), this);
				break;
		}
		
//		free(p);
	}
	else
		error("expected string");
	
	return RState::Shift(v, token, head);
} /* RSWStringValue::Shift */

#pragma mark -
#pragma mark --- RSNrValue ---

RSNrValue::RSNrValue(int size)
	: RSValue(tInt)
{
	fSize = size;
} /* RSNrValue::RSNrValue */

RState* RSNrValue::Shift(int v, int token, RElem** head)
{
	if (token == tIdent)
	{
		int id = v;
		if (ResolveIdentifier(v))
			RAddElement(head, RValue(v), fSize, this);
		else
			error("Unknown identifier: %s", ST_Ident(id));
	}
	else if (token == tInt)
		RAddElement(head, (REval *)v, fSize, this);
	else if (fHasDefault)
	{
		RAddElement(head, (REval *)fValue, fSize, this);
		return fNext->Shift(v, token, head);
	}
	else
		error("internal error 3");
	
	return RSValue::Shift(v, token, head);
} /* RSNrValue::Shift */

#pragma mark -
#pragma mark --- RSArray ---

RSArray::RSArray(RState *data, int ident, int fixedCount)
{
	fNext = new RSArrayNode(data, ident, fixedCount);
} /* RSArray::RSArray */

RState* RSArray::Shift(int v, int token, RElem **)
{
	if (token != tArray)
		error("expected an array");
	
	static_cast<RSArrayNode*>(fNext)->ResetCounter();
	return fNext;
} /* RSArray::Shift */

RSArrayNode::RSArrayNode(RState *data, int ident, int fixedCount)
{
	fHead = data;
	fHead->SetNext(this);
	fIdent = ident;
	fFixedCount = fixedCount;
} /* RSArrayNode::RSArrayNode */

RState* RSArrayNode::Shift(int v, int token, RElem** head)
{
	if (token != tArrayEnd)
	{
		gValueMap[fIdent] = ++fCount;
		
		return fHead->Shift(v, token, head);
	}

	if (fFixedCount && fCount != fFixedCount)
		error("Incorrect nr of array elements");

	return RState::Shift(v, token, head);
} /* RSArrayNode::Shift */

void RSArrayNode::ResetCounter()
{
	fCount = 0;
} /* RSArrayNode::ResetCounter */

#pragma mark -
#pragma mark --- RSSwitch ---

RSSwitch::RSSwitch(BList *cases)
{
	fCases = cases;
} /* RSSwitch::RSSwitch */

void RSSwitch::SetNext(RState *next)
{
	ASSERT(next != this);
	for (int i = 0; i < fCases->CountItems(); i++)
	{
		RCase *rcase = (RCase *)fCases->ItemAt(i);
		rcase->sStates->SetNext(next);
	}
} /* RSSwitch::SetNext */

RState* RSSwitch::Shift(int v, int token, RElem** head)
{
	if (token == tCase)
	{
		for (int i = 0; i < fCases->CountItems(); i++)
		{
			RCase *rcase = (RCase *)fCases->ItemAt(i);
			
			if (rcase->sIdent == v)
			{
				RSValue *sv = dynamic_cast<RSValue*>(rcase->sStates);
				if (sv && sv->fHasDefault)
					return sv->Shift(sv->fValue, sv->fType, head);
				else
					return rcase->sStates;
			}
		}
	}
	else
		error("expected case");
	
	return NULL;
} /* RSSwitch::Shift */
