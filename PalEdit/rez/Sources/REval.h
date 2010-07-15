/*	$Id: REval.h,v 1.2 2009/06/06 18:29:12 darkwyrm Exp $
	
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

	Created: 12/02/98 15:36:34
*/

#ifndef REVAL_H
#define REVAL_H

struct RElem;

enum REvalType {
	retValue,
	retOperator,
	retFunction,
	retIdentifier,
	retOffset
};

enum REvalOp {
	reoPlus, reoMinus, reoMultiply, reoDivide, reoModulus, reoNegate, 
	reoSHL, reoSHR, reoBitAnd, reoBitOr, reoBitFlip, reoNot, reoAnd, reoOr, 
	reoLT, reoGT, reoLE, reoGE, reoEQ, reoNE, reoXPwrY, reoFlip
};

enum RFuncs {
	refCountOf,
	refCopyBits
};

struct REval {
	REval *fLeft, *fRight;
	REvalType fType;
	bool fOnStack;
	union {
		long fValue;
		REvalOp fOperator;
		RFuncs fFunc;
	};
	RElem *fElem;
	long fParams[10];
	
	long Evaluate(RElem *head);
	void SetElement(RElem *elem);
};

long REvaluate(REval *e, RElem *head);
REval* RBinaryOp(REval *a, REval *b, REvalOp op);
REval* RUnaryOp(REval *a, REvalOp op);
REval* RValue(long v);
REval* RFunction(RFuncs f, ...);
REval* RIdentifier(long v);

#endif
