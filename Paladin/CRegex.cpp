/*	$Id: CRegex.cpp,v 1.2 2010/07/08 22:06:40 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe - published under the MIT license.
*/

#include "CRegex.h"

const status_t krx_NoMatch = PCRE_ERROR_NOMATCH;
const status_t krx_NotBOL = PCRE_NOTBOL;
const status_t krx_NotEOL = PCRE_NOTEOL;

CRegex::CRegex()
	: fInitCheck(B_NO_INIT)
	, fRegex(NULL)
	, fErrorStr(NULL)
{
}

CRegex::CRegex(const char* pattern, bool ignoreCase, bool fullWord, 
			   bool backward)
	: fInitCheck(B_NO_INIT)
	, fRegex(NULL)
	, fErrorStr(NULL)
{
	_Init(pattern, ignoreCase, fullWord, backward);
}

CRegex::~CRegex()
{
	_Cleanup();
}

status_t CRegex::SetTo(const char* pattern, bool ignoreCase, bool fullWord,
					   bool backward)
{
	_Cleanup();
	return _Init(pattern, ignoreCase, fullWord, backward);
}

status_t CRegex::Match(const char* subject, int32 len, int32 offset, 
					   int options)
{
	if (!fRegex)
		return B_NO_INIT;
	if (!subject)
		return B_BAD_VALUE;
	int res;
	int infoCount;
	res = pcre_fullinfo(fRegex, NULL, PCRE_INFO_CAPTURECOUNT, &infoCount);
	if (res != 0)
		return res;
	infoCount++;
	int vs = infoCount*3;
	int *mvect = new int [vs];
	res = pcre_exec(fRegex, NULL, subject, len, offset, options, mvect, vs);
	fMatchInfos.clear();
	if (res >= 0)
	{
		for( int i=0; i<res; ++i)
		{
			MatchInfo mi;
			mi.start = mvect[i*2];
			mi.len = mvect[1+i*2]-mvect[i*2];
			if (i==1 && fBackward)
			{	// [zooey]: we inserted a greedy anchor-to-front in order to
				// match the last occurence of the pattern. Now we need to
				// skip this anchor match and adjust the total match as needed.
				// If anyone knows a better way, please tell!
				fMatchInfos[0].start += mi.len;
				fMatchInfos[0].len -= mi.len;
				continue;
			}
			fMatchInfos.push_back(mi);
		}
		res = 0;
	}
	delete [] mvect;
	return res;
}

int CRegex::MatchStart(unsigned int index) const
{
	if (fMatchInfos.size() <= index)
		return 0;
	return fMatchInfos[index].start;
}

int CRegex::MatchLen(unsigned int index) const
{
	if (fMatchInfos.size() <= index)
		return 0;
	return fMatchInfos[index].len;
}

static BString DefaultString;

const BString& CRegex::MatchStr(const char* subject, unsigned int index) const
{
	if (!subject || fMatchInfos.size() <= index)
		return DefaultString;
	const MatchInfo& mi = fMatchInfos[index];
	if (mi.len && !mi.str.Length())
		mi.str.SetTo(subject+mi.start, mi.len);
	return mi.str;
}

int
CRegex::MatchCount(void) const
{
	return fMatchInfos.size();
}

char* CRegex::ReplaceString(const char* subject, int32 len, const char* repl)
{
	if (!subject || fInitCheck != B_OK || fMatchInfos.empty())
		return NULL;
	
	BString replStr;

	char c;
	int rl = strlen(repl);
	for(int i=0; i<rl; ++i)
	{
		c = repl[i];
		if (c == '\\' || c == '$')
		{
			c = repl[++i];
			if (c >= '1' && c <= '9')
				replStr << MatchStr(subject, c-'0');
			else if (c == '\\') 
			{	// de-escape newline, carriage-return, tab and backslash:
				if (c == 'n')
					replStr << '\n';
				else if (c == 'r')
					replStr << '\r';
				else if (c == 't')
					replStr << '\t';
				else if (c == '\\')
					replStr << '\\';
			}
		}
		else
			replStr << c;
	}
	char* resBuf = (char*)malloc(replStr.Length()+1);
	if (resBuf)
	{
		replStr.CopyInto(resBuf, 0, replStr.Length());
		resBuf[replStr.Length()] = '\0';
	}
	return resBuf;
}

status_t CRegex::_Init(const char* patt, bool ignoreCase, bool fullWord,
					   bool backward)
{
	if (!patt || !strlen(patt))
	{
		fErrorStr = "Pattern is empty!";
		return B_BAD_VALUE;
	}
	uint32 options = PCRE_UTF8 | PCRE_MULTILINE;
	if (ignoreCase)
		options |= PCRE_CASELESS;
	BString pattern;
	if (fullWord)
		pattern << "\\b" << patt << "\\b";
	else
		pattern = patt;
	if (backward)
	{
		// anchor greedily (across multiple lines) to start of subject:
		pattern = BString("(\\A(?s).*)") << pattern;
		fBackward = true;
	}
	const char* errStr;
	int errPos;
	fRegex = pcre_compile(pattern.String(), options, &errStr, &errPos, NULL);
	if (!fRegex) 
	{
		fErrorStr << errStr << " at:\n\t" << pattern.String()+errPos;
		fInitCheck = B_ERROR;
	}
	else
		fInitCheck = B_OK;
	return fInitCheck;
}

void CRegex::_Cleanup()
{
	if (fRegex)
	{
		pcre_free(fRegex);
		fRegex = NULL;
	}
	fErrorStr.Truncate(0);
	fBackward = false;
	fInitCheck = B_NO_INIT;
	fMatchInfos.clear();
}

