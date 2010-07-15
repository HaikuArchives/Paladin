/*	$Id: CStdErrParser.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 29 October, 1998 15:26:40
*/

#ifndef CSTDERRPARSER_H
#define CSTDERRPARSER_H

#include <string>
#include <vector>

#include "CRegex.h"

using std::string;
using std::vector;

class CStdErrParser
{
  public:
	CStdErrParser(BListView *errList, const char *cwd);
	virtual ~CStdErrParser();
	
	void Add(const char *txt, int len);
	void Flush();
	
	void AddPattern(const char *pat, int msg, int file, int line, int warning);

  private:
	
	bool MatchOne(bool flush);
	
	char *fCWD;
	BListView *fErrList;
	string fErrBuffer;
	
	struct CErrPatInfo
	{
		CRegex* fInfo;
		int fMsg;
		int fFile;
		int fLine;
		int fWarning;
	};
	
	vector<CErrPatInfo> fPatterns;
};

#endif // CSTDERRPARSER_H
