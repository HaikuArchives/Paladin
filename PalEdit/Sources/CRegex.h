/*	$Id: CRegex.h,v 1.4 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe - published under the MIT license.
*/

#ifndef _CRegex_h_
#define _CRegex_h_

#include <String.h>

#include <pcre.h>

using std::vector;

extern const status_t krx_NoMatch;
extern const status_t krx_NotBOL;
extern const status_t krx_NotEOL;

class CRegex
{
public:
		CRegex();
		CRegex(const char* pattern, bool ignoreCase, bool fullWord=false,
			   bool backward=false);
		~CRegex();
		
		status_t SetTo(const char* pattern, bool ignoreCase, 
					   bool fullWord=false, bool backward=false);
		status_t Match(const char* subject, int32 len, int32 offset, 
					   int options=0);
		char* ReplaceString(const char* subject, int32 len, const char* repl);

		status_t InitCheck() const;
		const BString& ErrorStr() const;

		int MatchStart(int index=0) const;
		int MatchLen(int index=0) const;
		const BString& MatchStr(const char* subject, int index=0) const;
private:
		status_t _Init(const char* pattern, bool ignoreCase, bool fullWord,
					   bool backward);
		void _Cleanup();

		status_t fInitCheck;

		pcre* fRegex;
		BString fErrorStr;
		bool fBackward;

		struct MatchInfo
		{
			int start;
			int len;
			mutable BString str;
		};
		vector<MatchInfo> fMatchInfos;

		// hide copy constructor
		CRegex(const CRegex&);
};

inline status_t CRegex::InitCheck() const
{
	return fInitCheck;
}

inline const BString& CRegex::ErrorStr() const
{
	return fErrorStr;
}

#endif
