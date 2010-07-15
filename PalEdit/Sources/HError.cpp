/*	$Id: HError.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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
#include <stdarg.h>
#include "HError.h"
#include "MAlert.h"

HErr::HErr()
{
} /* HErr::HErr */

HErr::HErr(int err, ...) throw()
{
	fErr = err;
	fMessage[0] = 0;
	
	if (err == 0)
		strcpy(fMessage, "No Error");
	else if (err < 0)
		strcpy(fMessage, strerror(err));
	else
		sprintf(fMessage, "An unknown error occured (nr: %d)", err);

//#if DEBUG
//	if (err)
//	{
//		SERIAL_PRINT(("%s\n", fMessage));
//	}
//#endif
} /* HErr::HErr */

HErr::HErr(const char *errString, ...) throw()
{
	va_list vl;
	va_start(vl, errString);
	vsprintf(fMessage, errString, vl);
	va_end(vl);
	fErr = -1;
	
//#if DEBUG
//	SERIAL_PRINT(("%s\n", fMessage));
//#endif
} // HErr::HErr

void HErr::DoError () throw ()
{
	if (fErr)
	{
		beep();
		MStopAlert a(fMessage);
		a.Go();
	}
} /* HErr::DoError */
