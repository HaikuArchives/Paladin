/*	$Id: CStdErrParser.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 29 October, 1998 15:29:28
*/

#include "pe.h"
#include "CStdErrParser.h"
#include "CMessageItem.h"
#include "CFindDialog.h"
#include "Utils.h"
#include "HError.h"

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

CStdErrParser::CStdErrParser(BListView *errList, const char *cwd)
{
	fErrList = errList;
	fCWD = strdup(cwd);
} // CStdErrParser::CStdErrParser

CStdErrParser::~CStdErrParser()
{
	free(fCWD);

	vector<CErrPatInfo>::iterator ri;
	for (ri = fPatterns.begin(); ri != fPatterns.end(); ++ri)
		delete ri->fInfo;
} // CStdErrParser::~CStdErrParser

void CStdErrParser::Add(const char *txt, int len)
{
	fErrBuffer.append(txt, txt + len);

	while (MatchOne(false))
		;
} // CStdErrParser::Add

void CStdErrParser::Flush()
{
	while (true)
	{
		while (fErrBuffer[0] == '\n')
			fErrBuffer.erase(0, 1);
		
		if (fErrBuffer.length() == 0)
			break;
		
		if (!MatchOne(true))
			break;
	}
} // CStdErrParser::Flush

void CStdErrParser::AddPattern(const char *pat, int msg, int file, int line, int warning)
{
	CErrPatInfo info;

	info.fInfo = new CRegex(pat, false);
	status_t res = info.fInfo->InitCheck();
	if (res != B_OK)
	{
		THROW((info.fInfo->ErrorStr().String()));
	}
	
	info.fMsg = msg;
	info.fFile = file;
	info.fLine = line;
	info.fWarning = warning;
	
	fPatterns.push_back(info);
} // CStdErrParser::AddPattern

bool CStdErrParser::MatchOne(bool flush)
{
	vector<CErrPatInfo>::iterator ri;
	CMessageItem *item = NULL;
	entry_ref ref;
	int m = fErrBuffer.length();
	for (ri = fPatterns.begin(); item == NULL && ri != fPatterns.end(); ++ri)
	{
		CRegex& rx = *ri->fInfo;
		const char *text = fErrBuffer.c_str();
		int size = fErrBuffer.length();
	
		int r = rx.Match(text, size, 0);
		if (r)
		{
			if (r == krx_NoMatch)
				continue;
			else
				THROW((rx.ErrorStr().String()));
		}
	
		BString file;
		if (text[rx.MatchStart(ri->fFile)] != kDirectorySeparator && fCWD)
			file << fCWD << kDirectorySeparator;
		file << rx.MatchStr(text, ri->fFile);

		if (get_ref_for_path(file.String(), &ref) == B_OK 
			&& BEntry(&ref).Exists())
		{
			int line = strtoul(rx.MatchStr(text, ri->fLine).String(), NULL, 10);
			bool warning = false;
			
			if (ri->fWarning)
			{
				BString wstr = rx.MatchStr(text, ri->fWarning);
				warning = (wstr.ICompare("warning", 7) == 0);
			}
			
			item = new CMessageItem(rx.MatchStr(text, ri->fMsg).String(),
									rx.MatchLen(ri->fMsg),
									warning 
										? CMessageItem::msgWarning 
										: CMessageItem::msgError,
									&ref, line);
			fErrBuffer.erase(rx.MatchStart(), rx.MatchLen());
			m = rx.MatchStart();
		}
	}
	
	int found = false;

	if (item || flush)
	{
		BAutolock lock(fErrList->Window());

		if (lock.IsLocked())
		{
			font_height fi;
			be_plain_font->GetHeight(&fi);
			float h = fi.ascent + fi.descent + 2;
	
			while (m > 0)
			{
				int l = (int)fErrBuffer.find('\n');
				
				CMessageItem *i = new CMessageItem(fErrBuffer.c_str(), std::min(m, l), CMessageItem::msgInfo);
				fErrList->AddItem(i);
				i->SetHeight(h);
				
				fErrBuffer.erase(0, std::min(l + 1, m));
				m -= l + 1;
				
				found = true;
			}
			
			if (item)
			{
				fErrList->AddItem(item);
				item->SetHeight(h);
				
				found = true;
			}

			if (found)
			{
				fErrList->Select(fErrList->CountItems() - 1);
				fErrList->ScrollToSelection();
				
				if (fErrList->Window()->IsHidden())
					fErrList->Window()->Show();
			}
		}
	}
	
	return found;
} // CStdErrParser::MatchOne
