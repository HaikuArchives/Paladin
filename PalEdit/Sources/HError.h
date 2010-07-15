/*	$Id: HError.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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
	
	Bevat macros die handig zijn bij error checking
*/

#ifndef HERROR_H
#define HERROR_H

#include "HLibHekkel.h"

#include <cerrno>
#include <syslog.h>

#if DEBUG
#	define ASSERT_OR_THROW(x)		ASSERT(x); if (!(x)) throw HErr("Assertion failed at %s:%d: %s", __FILE__, __LINE__, #x)
#else
#	define ASSERT_OR_THROW(x)		if (!(x)) throw HErr("Assertion failed at %s:%d: %s", __FILE__, __LINE__, #x)
#endif

IMPEXP_LIBHEKKEL const char kDefaultMessage[] = "An OS error occurred: %s";

class IMPEXP_LIBHEKKEL HErr
{
  protected:
	HErr();
  public:
	HErr(int err, ...) throw();
	HErr(const char *msg, ...) throw();
	
	virtual void	DoError() throw();
	
	operator int()	{ return fErr; };
	operator char*() { return fMessage; };
	
  protected:
	long fErr;
	char fMessage[256];
};

//#define	THROW(args)				do { _sPrintf("exception thrown at File: \"%s\"; Line: %d\n", __FILE__, __LINE__); throw HErr args; } while (false)
//#define	THROW(args)				do { syslog(LOG_DEBUG, "exception thrown at File: \"%s\"; Line: %d", __FILE__, __LINE__); throw HErr args; } while (false)
#define	THROW(args)				do { HErr __err_obj args; syslog(LOG_DEBUG, "Throwing: \"%s\" at %s:%d", (char *)__err_obj, __FILE__, __LINE__); throw __err_obj; } while (false)

#define FailNil(p)								do {	if ((p) == NULL) 										THROW(("Insufficient memory"));	} while (false)
#define FailNilMsg(p,msg)						do {	if ((p) == NULL)										THROW((msg));	} while (false)
#define FailNilRes(p)							do {	if ((p) == NULL)										THROW(("Missing Resource"));	} while (false)
#define FailNilResMsg(p,msg)				do {	if ((p) == NULL)										THROW((msg));	} while (false)
#define FailOSErr(err)							do {	status_t __err = (err); if (__err != B_OK)		THROW((kDefaultMessage, strerror(__err))); } while (false)
#define FailOSErrMsg(err,msg)				do {	status_t __err = (err); if (__err != B_OK)		THROW((msg, strerror(__err))); } while (false)
#define FailOSErr2(err)						do {	status_t __err = (err); if (__err < B_OK)		THROW((kDefaultMessage, strerror(__err))); } while (false)
#define FailOSErr2Msg(err,msg)				do {	status_t __err = (err); if (__err < B_OK)		THROW((msg, strerror(__err))); } while (false)
#define FailIOErr(err)							do {	status_t __err = (err); if (__err < B_OK)		THROW((kDefaultMessage, strerror(__err))); } while (false)
#define FailIOErrMsg(err,msg)				do {	status_t __err = (err); if (__err < B_OK)		THROW((msg, strerror(__err))); } while (false)
#define FailPErr(err)							do {	status_t __err = (err); if (__err < 0)				THROW((kDefaultMessage, strerror(errno))); } while (false)
#define CheckedWrite(stream,data,size)	do {	if (stream.Write(data, size) < size)					THROW(("A write error occurred: %s", strerror(errno)));	} while (false)
#define CheckedRead(stream,data,size)	do {	if (stream.Read(data, size) < size)					THROW(("A read error occurred: %s", strerror(errno)));	} while (false)
#define CheckedRead2(stream,data)		do {	if (stream.Read(&data, sizeof(data)) < sizeof(data))		THROW(("A read error occurred: %s", strerror(errno)));	} while (false)
#define FailMessageTimedOutOSErr(err)	do {	status_t __err = (err); if (__err != B_OK)	{ if (__err == B_WOULD_BLOCK || __err == B_TIMED_OUT) syslog(LOG_DEBUG, "File %s; Line %d; # SendMessage dropped\n", __FILE__, __LINE__); else THROW((kDefaultMessage, strerror(__err))); } } while (false)

#endif // HERROR_H
