/*	$Id: CLanguageInterface.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $

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

	Created: 09/19/97 10:49:36
*/

#include "pe.h"
#include "PText.h"
#include "CLanguageInterface.h"
#include "PApp.h"
#include "CLanguageAddOn.h"
#include "CAlloca.h"
#include "Utils.h"
#include "HError.h"
#include "HAppResFile.h"
#include "HPreferences.h"
#include "HColorUtils.h"
#include "ResourcesMisc.h"
#include "Prefs.h"
#include <algorithm>

unsigned char *CLanguageInterface::sfWordBreakTable = NULL;

class ext {
public:
	ext();
	ext(const char *e);

	bool operator<(const ext& e) const;
	bool operator==(const ext& e) const;

	char t[12];
};

static std::map<ext, CLanguageInterface*> sInterfaces;
static CLanguageInterface *sDefault;
vector<CLanguageInterface*> CLanguageInterface::fInterfaces;

ext::ext()
{
	t[0] = 0;
} /* CLanguageInterface::ext::ext */

ext::ext(const char *e)
{
	if (strlen(e) > 11) THROW(("Extension `%s' is too long", e));

	strcpy(t, e);
} /* CLanguageInterface::ext::ext */

bool ext::operator<(const ext& e) const
{
	return strcmp(t, e.t) < 0;
} /* CLanguageInterface::ext::operator< */

bool ext::operator==(const ext& e) const
{
	return strcmp(t, e.t) == 0;
} /* CLanguageInterface::ext::operator== */

#pragma mark -

CLanguageInterface::CLanguageInterface()
	:	fHaveParsedKeywords(false)
{
	if (sfWordBreakTable == NULL)
	{
		sfWordBreakTable = (unsigned char *)HResources::GetResource(rtyp_Wbrt, rid_Wbrt_WordbreakTable);
		if (sfWordBreakTable == NULL) THROW(("Missing Resource!"));
	}

//	fImage = -1;

	fBalance = NULL;
	fScanForFunctions = NULL;
	fColorLine = NULL;
	fFindNextWord = NULL;
	fLanguage = "None";
	fExtensions = "";
	fKeywordFile = NULL;
	fLineCommentStart = fLineCommentEnd = "";
	fInterfaceVersion = 1;
} /* CLanguageInterface::CLanguageInterface */

CLanguageInterface::CLanguageInterface(const char *path, image_id image)
	:	fHaveParsedKeywords(false)
{
	if (sfWordBreakTable == NULL)
	{
		sfWordBreakTable = (unsigned char *)HResources::GetResource(rtyp_Wbrt, rid_Wbrt_WordbreakTable);
		if (sfWordBreakTable == NULL) THROW(("Missing Resource!"));
	}

	fImage = image;
	if (fImage < 0) THROW(("Error loading language extension: %s", strerror(fImage)));

	if (get_image_symbol(fImage, "Balance", B_SYMBOL_TYPE_TEXT, (void**)&fBalance) != B_OK)
		fBalance = NULL;
	if (get_image_symbol(fImage, "ScanForFunctions", B_SYMBOL_TYPE_TEXT, (void**)&fScanForFunctions) != B_OK)
		fScanForFunctions = NULL;
	if (get_image_symbol(fImage, "ColorLine", B_SYMBOL_TYPE_TEXT, (void**)&fColorLine) != B_OK)
		fColorLine = NULL;
	if (get_image_symbol(fImage, "FindNextWord", B_SYMBOL_TYPE_TEXT, (void**)&fFindNextWord) != B_OK)
		fFindNextWord = NULL;
	FailOSErr(get_image_symbol(fImage, "kLanguageName", B_SYMBOL_TYPE_DATA, (void**)&fLanguage));
	FailOSErr(get_image_symbol(fImage, "kLanguageExtensions", B_SYMBOL_TYPE_DATA, (void**)&fExtensions));
	FailOSErr(get_image_symbol(fImage, "kLanguageCommentStart", B_SYMBOL_TYPE_DATA, (void**)&fLineCommentStart));
	FailOSErr(get_image_symbol(fImage, "kLanguageCommentEnd", B_SYMBOL_TYPE_DATA, (void**)&fLineCommentEnd));
	FailOSErr(get_image_symbol(fImage, "kLanguageKeywordFile", B_SYMBOL_TYPE_DATA, (void**)&fKeywordFile));
	int16* versionPtr = NULL;
	get_image_symbol(fImage, "kInterfaceVersion", B_SYMBOL_TYPE_DATA, (void**)&versionPtr);
	fInterfaceVersion = versionPtr ? *versionPtr : 1;
	if (fInterfaceVersion < 2) {
		BString err("Unsupported Language:\n");
		err << path;
		THROW((err.String()));
	}
} /* CLanguageInterface::CLanguageInterface */

CLanguageInterface::~CLanguageInterface()
{
} /* CLanguageInterface::~CLanguageInterface */

template <class T>
void AddInterface(char *s, T* i)
{
	char *e = strtok(s, ";");

	while (e)
	{
		sInterfaces[e] = i;
		e = strtok(NULL, ";");
	}

	free(s);
} /* AddInterface */

void CLanguageInterface::SetupLanguageInterfaces()
{
	sDefault = new CLanguageInterface();
	AddInterface(strdup(""), sDefault);

	char path[PATH_MAX];

	BPath p;
	BEntry e;
	gAppDir.GetEntry(&e);
	e.GetPath(&p);
	strcpy(path, p.Path());

	strcat(path, "/Languages/");

	char plug[PATH_MAX];
	DIR *dir = opendir(path);

	if (!dir)
		return;

	struct dirent *dent;
	struct stat stbuf;

	while ((dent = readdir(dir)) != NULL)
	{
		strcpy(plug, path);
		strcat(plug, dent->d_name);
		status_t err = stat(plug, &stbuf);
		if (!err && S_ISREG(stbuf.st_mode) &&
			strcmp(dent->d_name, ".") && strcmp(dent->d_name, ".."))
		{
			image_id next;
			char *l;

			next = load_add_on(plug);
			if (next > B_ERROR &&
				(err = get_image_symbol(next, "kLanguageName", B_SYMBOL_TYPE_DATA, (void**)&l)) == B_OK)
			{
				if (strlen(l) > 28) THROW(("Language name too long"));
				CLanguageInterface *intf = new CLanguageInterface(plug, next);
				fInterfaces.push_back(intf);

				const char *s = intf->Extensions();
				AddInterface(strdup(s), intf);
			}
		}
	}

	ChooseDefault();
} /* CLanguageInterface::SetupLanguageInterfaces */

CLanguageInterface* CLanguageInterface::FindByExtension(const char *filename)
{
	char *e;

	if (filename)
	{
		try
		{
			if ((e = strrchr(filename, '.')) != NULL && sInterfaces.count(e + 1))
				return sInterfaces[e + 1];

			if (strlen(filename) < 11 && sInterfaces.count(filename))
				return sInterfaces[filename];
		}
		catch (...) {}
	}

	return sDefault;
} /* CLanguageInterface::FindIntf */

static const char *skip(const char *txt)
{
	while (*txt)
	{
		switch (*txt)
		{
			case '\'':
				while (*++txt)
				{
					if (*txt == '\'')
						break;
					if (*txt == '\\' && txt[1])
						txt++;
				}
				break;

			case '"':
				while (*++txt)
				{
					if (*txt == '"')
						break;
					if (*txt == '\\' && txt[1])
						txt++;
				}
				break;

			case '/':
				if (txt[1] == '*')
				{
					txt += 2;
					while (*txt && ! (*txt == '*' && txt[1] == '/'))
						txt++;
				}
				else if (txt[1] == '/')
				{
					txt += 2;
					while (*txt && *txt != '\n')
						txt++;
				}
				break;

			case '{':
			case '[':
			case '(':
			case ')':
			case ']':
			case '}':
				return txt;
		}
		txt++;
	}

	return txt;
} // skip

static bool InternalBalance(CLanguageProxy& proxy, int& start, int& end)
{
	const char *txt = proxy.Text(), *et;
	int size = proxy.Size();

	if (start < 0 || start > end || end > size)
		return false;

	et = txt + end;

	stack<int> bls, sbls, pls;

	while (*txt && txt < et)
	{
		switch (*txt)
		{
			case '{':	bls.push(txt - proxy.Text());	break;
			case '[':	sbls.push(txt - proxy.Text());	break;
			case '(':	pls.push(txt - proxy.Text());	break;
			case '}':	if (!bls.empty()) bls.pop();		break;
			case ']':	if (!sbls.empty()) sbls.pop();		break;
			case ')':	if (!pls.empty()) pls.pop();		break;
		}
		txt = skip(txt + 1);
	}

	char ec = 0, oc = 0;
	stack<int> *s = NULL;

	int db, dsb, dp;

	db = bls.empty() ? -1 : start - bls.top();
	dsb = sbls.empty() ? -1 : start - sbls.top();
	dp = pls.empty() ? -1 : start - pls.top();

	if (db < 0 && dsb < 0 && dp < 0)
		return false;

	if (db >= 0 && (dsb < 0 || db < dsb) && (dp < 0 || db < dp))
	{
		oc = '{';
		ec = '}';
		s = &bls;
	}

	if (dsb >= 0 && (db < 0 || dsb < db) && (dp < 0 || dsb < dp))
	{
		oc= '[';
		ec = ']';
		s = &sbls;
	}

	if (dp >= 0 && (dsb < 0 || dp < dsb) && (db < 0 || dp < db))
	{
		oc = '(';
		ec = ')';
		s = &pls;
	}

	if (ec)
	{
		int l = 1;

		while (*txt)
		{
			if (*txt == ec)
			{
				if (--l == 0)
				{
					start = s->top() + 1;
					end = txt - proxy.Text();
					return true;
				}
				if (!s->empty()) s->pop();
			}
			else if (*txt == oc)
			{
				l++;
				s->push(0);
			}

			txt = skip(txt + 1);
		}
	}

	return false;
} /* InternalBalance */

bool CLanguageInterface::Balance(PText& text, int& start, int& end)
{
	try
	{
		CLanguageProxy proxy(*this, text);

		if (fBalance)
			return fBalance(proxy, start, end);
		else
			return InternalBalance(proxy, start, end);
	}
	catch (...)
	{
		return false;
	}
} /* CLanguageInterface::Balance */

void CLanguageInterface::Balance(PText& text)
{
	try
	{
		int start = std::min(text.Anchor(), text.Caret());
		int end = std::max(text.Anchor(), text.Caret());

		if (! Balance(text, start, end))
			THROW((0));

		if (start == std::min(text.Anchor(), text.Caret()) &&
			end == std::max(text.Anchor(), text.Caret()))
		{
			start--; end++;
			if (! Balance(text, start, end))
				THROW((0));
		}

		text.ChangeSelection(start, end);
	}
	catch (...)
	{
		beep();
	}
} /* CLanguageInterface::Balance */

void CLanguageInterface::ColorLine(const char *text, int size, int& state,
		int *starts, rgb_color *colors)
{
	try
	{
		if (fColorLine)
		{
			CLanguageProxy proxy(*this, text, size, starts, colors);
			fColorLine(proxy, state);
		}
		else if (starts)
		{
			starts[0] = 0;
			colors[0] = gColor[kColorText];
		}
	}
	catch (...)
	{
		beep();
	}
} /* CLanguageInterface::ColorLine */

void CLanguageInterface::ScanForFunctions(PText& text, CFunctionScanHandler& handler)
{
	try
	{
		CLanguageProxy proxy(*this, text, &handler);

		if (fScanForFunctions)
			fScanForFunctions(proxy);
	}
	catch(...)
	{
		beep();
	}
} /* CLanguageInterface::ScanForFunctions */

int CLanguageInterface::FindNextWord(PText& text, int offset, int& mlen)
{
	try
	{
		if (fFindNextWord)
		{
			int line = text.Offset2Line(offset);
			int size;

			if (line >= text.LineCount() - 1)
				size = std::min(text.Size() - offset, 1024);
			else
				size = std::min(text.LineStart(line + 1) - offset, 1024);

			CAlloca txt(size + 1);
			text.TextBuffer().Copy(txt, offset, size);
			txt[size] = 0;

			CLanguageProxy proxy(*this, txt, size);
			int result = fFindNextWord(proxy);

			txt[result + 1] = 0;
			mlen = mstrlen(txt);

			return offset + result;
		}
		else
		{
			int mark = offset, i = offset;
			int unicode, state, len, iLen;

			state = 1;
			mlen = 0;
			iLen = 0;

			while (state > 0 && i < text.Size())
			{
				text.TextBuffer().CharInfo(i, unicode, len);

				int cl = 0;

				if (unicode == '\n')
					cl = 3;
				else if (isspace_uc(unicode))
					cl = 2;
				else if (isalnum_uc(unicode))
					cl = 4;
				else
					switch (unicode)
					{
						case 160:
						case 8199:
						case 8209:
							cl = 1;
							break;
						case '&':
						case '*':
						case '+':
						case '-':
						case '/':
						case '<':
						case '=':
						case '>':
						case '\\':
						case '^':
						case '|':
							cl = 5;
							break;
						default:
							cl = 4;
					}

				unsigned char t = sfWordBreakTable[(state - 1) * 6 + cl];

				state = t & 0x7f;

				if (t & 0x80)
				{
					mark = i + len - 1;
					mlen = iLen + 1;
				}

				iLen++;
				i += len;
			}

			return mark;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
//		beep();
	}

	return offset;
} /* CLanguageInterface::FindNextWord */

CLanguageInterface* CLanguageInterface::NextIntf(int& cookie)
{
	if (cookie >= 0 && cookie < fInterfaces.size())
		return fInterfaces[cookie++];
	else
		return NULL;
} /* CLanguageInterface::NextIntf */

const char *CLanguageInterface::Extensions() const
{
	char extPref[64];

	if (strlen(fLanguage) > 32) THROW(("Language name too long: %s", fLanguage));
	strcpy(extPref, fLanguage);
	strcat(extPref, ".ext");

	return gPrefs->GetPrefString(extPref, fExtensions);
} /* CLanguageInterface::Extensions */

void CLanguageInterface::SetExtensions(const char *ext)
{
	char extPref[32];

	strcpy(extPref, fLanguage);
	strcat(extPref, ".ext");

	gPrefs->SetPrefString(extPref, ext);
} /* CLanguageInterface::SetExtensions */

void CLanguageInterface::ChooseDefault()
{
	const char *d = gPrefs->GetPrefString(prf_S_DefLang, "None");
	vector<CLanguageInterface*>::iterator i;

	for (i = fInterfaces.begin(); i != fInterfaces.end(); i++)
	{
		if (strcmp(d, (*i)->Name()) == 0)
		{
			sDefault = *i;
			return;
		}
	}
} /* CLanguageInterface::ChooseDefault */

int CLanguageInterface::GetIndex(const CLanguageInterface* intf)
{
	vector<CLanguageInterface*>::iterator i = find(fInterfaces.begin(), fInterfaces.end(), intf);
	if (i == fInterfaces.end())
		return -1;
	else
		return i - fInterfaces.begin();
} // CLanguageInterface::GetIndex

CLanguageInterface* CLanguageInterface::FindByName(const char *language)
{
	vector<CLanguageInterface*>::iterator i;

	for (i = fInterfaces.begin(); i != fInterfaces.end(); i++)
	{
		if (strcmp(language, (*i)->Name()) == 0)
			return *i;
	}

	return sDefault;
} // CLanguageInterface::FindByName

int CLanguageInterface::AddToCurrentKeyword(int ch, int state)
{
	if (state > 0 && state <= kKeywordBufSize) {
		fKeywordBuf[state-1] = ch;
		return ++state;
	}
	return 0;
}

int CLanguageInterface::LookupCurrentKeyword(int state, int32 inSets) const
{
	if (state < 2)
		return 0;
	BString word(fKeywordBuf, state-1);
	//printf("LookupCurrentKeyword: '%s' <%i>\n", word.String(), LookupKeyword(word));
	return LookupKeyword(word, inSets);
}

int CLanguageInterface::LookupKeyword(const BString& word, int32 inSets) const
{
	if (!fHaveParsedKeywords) {
		// do lazy loading of keywords-info:
		image_info imageInfo;
		if (get_image_info(fImage, &imageInfo) == B_OK && strlen(fKeywordFile))
			GenerateKeywordMap(imageInfo.name);
		fHaveParsedKeywords = true;
	}
	// Lets search
	KeywordMap::const_iterator iter = fKeywordMap.find(word);
	//cout << "LookupKeyword in Set " << inSets << ": " << word.String() << ":" << endl;
	if (iter != fKeywordMap.end()) {
		// No special sets to search for
		if (inSets == 0)
		{
			//cout << "<S> " << iter->second << ": " << iter->first.String() << endl;
			return iter->second;
		}
		else
		{
			int bit;
			//int ret=0;
			do {
				bit = 1 << (iter->second-1);
				//cout << ">>> AND:[" << (bit & inSets) << ":" << bit << "] " << iter->second << ": " << iter->first.String() << endl;
				if (bit & inSets)
					return bit;
					//ret = bit;
			} while (++iter != fKeywordMap.upper_bound(word));
			//return ret;
		}
	}
	// Nothing found
	return 0;
}

/*
 * Implementation of keyword lookup, a straightforward, map-based lookup:
 * [zooey]:
 *     I know that using a hashmap should be faster, but since we do not know
 *     the amount of words contained in the map beforehand, the memory footprint
 *     of the hashmap would be (much) worse than that of a map.
 *     Benchmarks have indicated that lookup speed is good enough with maps
 *		 anyway, so I have decided to use a map for now.
 */
void CLanguageInterface::GenerateKeywordMap(const char *ext) const
{
	try
	{
		BPath settings;
		bool isNew = false;

		FailOSErr(find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true));

		BString p;
		p << settings.Path() << "/PalEdit/" << fKeywordFile;

		BEntry e;
		FailOSErrMsg(e.SetTo(p.String(), B_FILE_NODE),
						 "Settings directory was not found?");

		BString keywords;
		if (!e.Exists())
		{
			// copy resources into separate file in settings-folder, such that
			// the user can edit that in order to modify the keywords for that
			// specific language:
			isNew = true;

			BFile rf;
			FailOSErr(rf.SetTo(ext, B_READ_ONLY));
			BResources res;
			FailOSErr(res.SetTo(&rf));

			size_t s;
			const char *r = (const char*)res.LoadResource('KeyW', fKeywordFile, &s);

			if (!r) THROW(("Missing resource"));

			BFile txtfile(p.String(), B_CREATE_FILE | B_READ_WRITE);
			CheckedWrite(txtfile, r, s);
			keywords.SetTo(r, s);
		} else {
			BFile txtfile(p.String(), B_READ_ONLY);
			off_t size;
			FailOSErr(txtfile.GetSize(&size));
			char* kw = keywords.LockBuffer(size+1);
			if (kw) {
				CheckedRead(txtfile, kw, size);
				keywords.UnlockBuffer(size);
			}
		}

		const char* kw = keywords.String();
		const char* white = " \n\r\t";
		const char* start = kw + strspn(kw, white);
		const char* end = start + strcspn(start, white);
		BString word;
		char* buf;
		int currType = kKeywordLanguage;
		while (start < end) {
			// ideally, we'd like to use this:
			//			word.SetTo(start, end-start);
			// but the implementation of SetTo() seems to do a strlen() without
			// clamping it to the given length, which (as we give it a pretty
			// long string) results in pathetic performance.
			// So we roll our own SetTo():
			FailNil(buf = word.LockBuffer(end-start+1));
			memcpy(buf, start, end-start);
			buf[end-start] = '\0';
			word.UnlockBuffer(end-start);
			if (!word.Compare("//", 2)) {
				// a comment, so we skip to end of line:
				start += strcspn(start, "\n");
				start += strspn(start, white);
				end = start + strcspn(start, white);
			} else {
				if (word[0] == '-') {
					// it's a keyword-class specifier, we check which one:
					if (!word.ICompare("-Pe-Keywords-Language-"))
						currType = kKeywordLanguage;
					else if (!word.ICompare("-Pe-Keywords-User1-"))
						currType = kKeywordUser1;
					else if (!word.ICompare("-Pe-Keywords-User2-"))
						currType = kKeywordUser2;
					else if (!word.ICompare("-Pe-Keywords-User3-"))
						currType = kKeywordUser3;
					else if (!word.ICompare("-Pe-Keywords-User4-"))
						currType = kKeywordUser4;
					else {
						// be compatible with old style, meaning that an unknown
						// '-' entry bumps the type... Ignore a leading '-' entry
						if (!fKeywordMap.empty())
							currType++;
						// ...and skips to end of line:
						end = start + strcspn(start, "\n");
					}
				} else {
					fKeywordMap.insert(std::pair<BString, int>(word, currType));
				}
				start = end + strspn(end, white);
				end = start + strcspn(start, white);
			}
		}
		// DEBUG-OUTPUT:
		//cout << "Elements in MAP:" << endl;
		//int i = 0;
		//for (multimap<BString, int>::iterator it = fKeywordMap.begin(); it != fKeywordMap.end(); it++) {
		//	cout << ++i << ":  [" << (*it).second << ": " << (*it).first.String() << "]" << endl;
		//}
	}
	catch (HErr& err)
	{
		err.DoError();
	}
} /* GenerateKeywordMap */


// #pragma mark -


CFunctionScanHandler::CFunctionScanHandler()
{
} // CFunctionScanHandler::CFunctionScanHandler

CFunctionScanHandler::~CFunctionScanHandler()
{
} // CFunctionScanHandler::~CFunctionScanHandler()

void CFunctionScanHandler::AddFunction(const char *name, const char *match,
	int offset, bool italic, uint32 nestLevel, const char *params)
{
} // CFunctionScanHandler::AddFunction

void CFunctionScanHandler::AddInclude(const char *name, const char *open,
	bool italic)
{
} // CFunctionScanHandler::AddInclude

void CFunctionScanHandler::AddSeparator(const char* name)
{
}

