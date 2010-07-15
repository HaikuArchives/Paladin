/*	$Id: REval.cpp,v 1.2 2009/06/06 18:29:12 darkwyrm Exp $
	
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

	Created: 12/02/98 15:37:01
*/

#include "REval.h"
#include "RState.h"
#include <stdarg.h>
#include "rez.h"
#include <math.h>

long REval::Evaluate(RElem *head)
{
	switch (fType)
	{
		case retValue:
			return fValue;
		
		case retIdentifier:
			if (!fElem) error("internal error 4");
			return fElem->FindIdentifier(fValue);
		
		case retOperator:
		{
			switch (fOperator)
			{
				case reoPlus:
					return REvaluate(fLeft, head) + REvaluate(fRight, head);

				case reoMinus:
					return REvaluate(fLeft, head) - REvaluate(fRight, head);

				case reoMultiply:
					return REvaluate(fLeft, head) - REvaluate(fRight, head);

				case reoDivide:
					return REvaluate(fLeft, head) / REvaluate(fRight, head);

				case reoModulus:
					return REvaluate(fLeft, head) % REvaluate(fRight, head);

				case reoNegate:
					return - REvaluate(fLeft, head);

				case reoSHL:
					return REvaluate(fLeft, head) << REvaluate(fRight, head);

				case reoSHR:
					return REvaluate(fLeft, head) >> REvaluate(fRight, head);

				case reoBitAnd:
					return REvaluate(fLeft, head) & REvaluate(fRight, head);

				case reoBitOr:
					return REvaluate(fLeft, head) | REvaluate(fRight, head);

				case reoBitFlip:
					return ~REvaluate(fLeft, head);

				case reoNot:
					return !REvaluate(fLeft, head);

				case reoAnd:
					return REvaluate(fLeft, head) && REvaluate(fRight, head);

				case reoOr:
					return REvaluate(fLeft, head) || REvaluate(fRight, head);

				case reoLT:
					return REvaluate(fLeft, head) < REvaluate(fRight, head);

				case reoGT:
					return REvaluate(fLeft, head) > REvaluate(fRight, head);

				case reoLE:
					return REvaluate(fLeft, head) <= REvaluate(fRight, head);

				case reoGE:
					return REvaluate(fLeft, head) >= REvaluate(fRight, head);

				case reoEQ:
					return REvaluate(fLeft, head) == REvaluate(fRight, head);

				case reoNE:
					return REvaluate(fLeft, head) != REvaluate(fRight, head);

				case reoXPwrY:
					return (long)pow(REvaluate(fLeft, head), REvaluate(fRight, head));

				default:
					error("internal error 1");
			}
			break;
		}
		
		case retFunction:
		{
			switch (fFunc)
			{
				case refCountOf:
					return gValueMap[fParams[0]];
				
				case refCopyBits:
					return 0;
			}
			break;
		}
		
		default:
			error("internal error 2");
	}

	return 0;	// dummy value
} /* REval::Evaluate */

void REval::SetElement(RElem *elem)
{
	if (fLeft)
		fLeft->SetElement(elem);
	if (fRight)
		fRight->SetElement(elem);
	fElem = elem;
} /* REval::SetElement */

long REvaluate(REval *e, RElem *head)
{
	return e->Evaluate(head);
} /* REvaluate */

REval* RBinaryOp(REval *a, REval *b, REvalOp op)
{
	REval *r = new REval;
	
	r->fLeft = a;
	r->fRight = b;
	r->fType = retOperator;
	r->fOnStack = false;
	r->fOperator = op;
	r->fElem = NULL;
	
	return r;
} /* RBinaryOp */

REval* RUnaryOp(REval *a, REvalOp op)
{
	REval *r = new REval;
	
	r->fLeft = a;
	r->fRight = NULL;
	r->fType = retOperator;
	r->fOnStack = false;
	r->fOperator = op;
	r->fElem = NULL;
	
	return r;
} /* RUnaryOp */

REval* RValue(long v)
{
	REval *r = new REval;
	
	r->fLeft = r->fRight = NULL;
	r->fType = retValue;
	r->fOnStack = false;
	r->fValue = v;
	r->fElem = NULL;
	
	return r;
} /* RValue */

REval* RIdentifier(long v)
{
	REval *r = new REval;
	
	r->fLeft = r->fRight = NULL;
	r->fType = retIdentifier;
	r->fOnStack = false;
	r->fValue = v;
	r->fElem = NULL;
	
	return r;
} /* RIdentifier */

REval* RFunction(RFuncs f, ...)
{
	REval *r = new REval;
	
	r->fLeft = r->fRight = NULL;
	r->fType = retFunction;
	r->fOnStack = false;
	r->fFunc = f;
	r->fElem = NULL;
	
	va_list vl;
	va_start(vl, f);
	
	switch (f)
	{
		case refCountOf:
			r->fParams[0] = va_arg(vl, long);
			break;
			
		case refCopyBits:
			r->fParams[0] = va_arg(vl, long);
			r->fParams[1] = va_arg(vl, long);
			r->fParams[2] = va_arg(vl, long);
			break;
			
	}
	
	va_end(vl);
	
	return r;
} /* RFunction */
