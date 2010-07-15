/*	$Id: RState.h,v 1.3 2009/06/20 14:24:30 darkwyrm Exp $
	
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

	
	Created: 12/02/98 15:38:11
*/

#ifndef RSTATE_H
#define RSTATE_H

#include <map>
#include "RElem.h"
#include "SymbolTable.h"

enum {
	tString, tInt, tIdent, tCase, tArray, tArrayEnd, tRaw
};

class BList;
class RState;
struct RElem;

typedef std::map<int, int> intmap;

extern intmap gValueMap;

struct RType {
	int type;
	RState *firstState;
	
	RType (int t, RState *f) : type(t), firstState(f) {};
};

class RState {
public:
			RState();
virtual	~RState();

static		RState* FirstState(int type);
static		void FinishType(int type, RState *states);
static		void CopyType(int type1, int type2);

virtual	RState* Shift(int v, int token, RElem** head);
virtual	void SetNext(RState *next);

			RState *fNext;
static		intmap sfTypeMap;
};

struct RSymbol {
	int sIdentifier;
	int sValue;
	
	RSymbol(int ident, int val) : sIdentifier(ident), sValue(val) {};
};

class RSValue : public RState {
public:
			RSValue(int type);
		
virtual	void SetDefaultValue(int v);
			void AddIdentifiers(BList *idents);
			bool ResolveIdentifier(int& v);
		
			int fType;
			int fValue;
			bool fHasDefault;
			BList *fIdents;
};

class RSNrValue : public RSValue {
public:
			RSNrValue(int size);

virtual	RState* Shift(int v, int token, RElem** head);

			int fSize;
};

class RSStringValue : public RSValue {
public:
		RSStringValue(int kind, int size = 0);

		enum {
			skStr, skPStr, skWStr, skCStr, skHex
		};

virtual RState* Shift(int v, int token, RElem** head);

private:
		int fKind;
		int fSize;
};

class RSArray : public RState {
public:
		RSArray(RState *data, int ident = 0, int fixedCount = 0);

virtual RState* Shift(int v, int token, RElem** head);
};

class RSArrayNode : public RState {
public:
		RSArrayNode(RState *data, int ident = 0, int fixedCount = 0);
		
virtual RState* Shift(int v, int token, RElem** head);
		void ResetCounter();

protected:
		RState *fHead;
		int fIdent;
		int fCount;
		int fFixedCount;
};

struct RCase {
	int sIdent;
	RState *sStates;
	
	RCase(int ident, RState *states) : sIdent(ident), sStates(states) {};
};

class RSSwitch : public RState {
public:
		RSSwitch(BList *cases);

virtual RState* Shift(int v, int token, RElem** head);
virtual void SetNext(RState *next);

protected:
		BList *fCases;
};


#endif
