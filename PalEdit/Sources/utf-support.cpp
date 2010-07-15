/*	$Id: utf-support.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 02/16/98 22:02:55
*/

#include "pe.h"

#include <String.h>

#include "HAppResFile.h"
#include "HError.h"
#include "ResourcesUtf.h"
#include "utf-support.h"

unsigned char *alphaTable, *numTable, *alnumTable;

unsigned short *mappings[11];

using namespace HResources;

#include <stdio.h>
void InitUTFTables()
{
	numTable = (unsigned char *)GetResource(rtyp_Utbl, rid_Utbl_TableNumbers);
	FailNilRes(numTable);
	
	alphaTable = (unsigned char *)GetResource(rtyp_Utbl, rid_Utbl_TableLetters);
	FailNilRes(numTable);
	
	alnumTable = (unsigned char *)malloc(8192);
	FailNil(alnumTable);
	
	int *a, *b, *c;
	a = (int *)alphaTable;
	b = (int *)numTable;
	c = (int *)alnumTable;
	
	for (int i = 0; i < 8192 / sizeof(int); i++)
		*c++ = *a++ | *b++;
	
	for (int i = rid_Umap_Map01; i <= rid_Umap_Map11; i++)
	{
		mappings[i-1] = (i == rid_Umap_Map10) ? NULL : (unsigned short *)GetResource(rtyp_Umap, i);
	}
} /* InitUTFTables */

int mstrlen(const char *s)
{
	int result = 0, i = 0;
	
	while (*s)
	{
		if ((*s & 0x80) == 0)
			i = 0;
		else if ((*s & 0xC0) == 0x80)
			i = 0;	// this is a following char ????
		else if ((*s & 0xE0) == 0xC0)
			i = 1;
		else if ((*s & 0xF0) == 0xE0)
			i = 2;
		else if ((*s & 0xF8) == 0xF0)
			i = 3;
		else if ((*s & 0xFC) == 0xF8)
			i = 4;
		else if ((*s & 0xFE) == 0xFC)
			i = 5;
		
		result++;
		s++;
		
		while (i-- && (*s & 0xC0) == 0x80)
			s++;
	}
	
	return result;
} /* mstrlen */

void mstrcpy(char *dst, const char *src, int count)
{
	const char *max = src + strlen(src);
	
	ASSERT(src);
	
	while (*src && count-- && src < max)
	{
		int i = 0;
		
		if ((*src & 0x80) == 0)
			i = 0;
		else if ((*src & 0xE0) == 0xC0)
			i = 1;
		else if ((*src & 0xF0) == 0xE0)
			i = 2;
		else if ((*src & 0xF8) == 0xF0)
			i = 3;
		else if ((*src & 0xFC) == 0xF8)
			i = 4;
		else if ((*src & 0xFE) == 0xFC)
			i = 5;
		
		*dst++ = *src++;
		while (i-- && (*src & 0xC0) == 0x80)
			*dst++ = *src++;
	}

	*dst = 0;
} /* mstrcpy */

int mcharlen(const char *src)
{
	int result, i;
	
	if ((*src & 0x80) == 0)
		return 1;
	else if ((*src & 0xE0) == 0xC0)
		i = 1;
	else if ((*src & 0xF0) == 0xE0)
		i = 2;
	else if ((*src & 0xF8) == 0xF0)
		i = 3;
	else if ((*src & 0xFC) == 0xF8)
		i = 4;
	else if ((*src & 0xFE) == 0xFC)
		i = 5;
	else
		return 1;	// clearly this is an error
	
	result = 1;
	src++;

	while (i-- && (*src++ & 0xC0) == 0x80)
		result++;
	
	return result;
} /* mcharlen */

char *moffset(char *s, int count)
{
	char *r = s;
	
	while (count--)
	{
		int l = mcharlen(r);
		if (!l) return r;
		r += l;
	}
	
	return r;
} /* moffset */

int municode(const char *s) {
	const uchar *us = (uchar *)s;
	int unicode = 0;

	switch (mcharlen(s))
	{
		case 1:
			unicode = *us;
			break;
		case 2:
			unicode = ((us[0] << 6) & 0x07C0) | (us[1] & 0x003F);
			break;
		case 3:
			unicode = ((us[0] << 12) & 0x0F000) | ((us[1] << 6) & 0x0FC0) | (us[2] & 0x003F);
			break;
		case 4:
			unicode = ((us[0] << 18) & 0x01C0000) | ((us[1] << 12) & 0x03F000) |
				((us[2] << 6) & 0x0FC0) | (us[3] & 0x003F);
			break;
		case 5:
			unicode = ((us[0] << 24) & 0x03000000) | ((us[1] << 18) & 0x0FC0000) |
				((us[2] << 12) & 0x03F000) | ((us[3] << 6) & 0x0FC0) | (us[4] & 0x003F);
			break;
		default:
			// help!
			PRINT(("char (%s) is %d bytes long!\n", s, mcharlen(s)));
			break;
	}

	return unicode;
} /* municode */

int mprevcharlen(const char *s)
{
	if (*--s > 0 || (*s & 0x00C0) != 0x0080)
		return 1;

	int result = 2;
	while ((*--s & 0x00C0) == 0x0080 && result < 5)
		result++;

	ASSERT(mcharlen(s) == result);
	return result;
} /* mprevcharlen */

bool isalpha_uc(int unicode)
{
	int byte, bit;
	
	byte = unicode >> 3;
	if (byte >= 8192)
		// index exceeds our table size, we assume false!
		return false;
	bit = unicode & 0x07;
	
	return (alphaTable[byte] & (1 << bit)) != 0;
} /* isalpha_uc */

bool isnum_uc(int unicode)
{
	int byte, bit;
	
	byte = unicode >> 3;
	if (byte >= 8192)
		// index exceeds our table size, we assume false!
		return false;
	bit = unicode & 0x07;
	
	return (numTable[byte] & (1 << bit)) != 0;
} /* isnum_uc */

bool isalnum_uc(int unicode)
{
	int byte, bit;
	
	byte = unicode >> 3;
	if (byte >= 8192)
		// index exceeds our table size, we assume false!
		return false;
	bit = unicode & 0x07;
	
	return (alnumTable[byte] & (1 << bit)) != 0;
} /* isalnum_uc */

bool isspace_uc(int unicode)
{
	if (unicode < 128)
		return isspace(unicode);
	else
		return (unicode >= 8192 && unicode <= 8203) || unicode == 12288;
} /* isspace_uc */

int maptounicode(int charset, char ch)
{
	if (charset < 1 || charset > 11 || mappings[charset - 1] == NULL)
		THROW(("unsupported characterset: %d", charset - 1));
	
	return mappings[charset - 1][(int)(unsigned char)ch];
} /* maptounicode */

enum {
	ccNoClass,
	ccRoman,
	ccGreek,
	ccCyrillic,
	ccArmenian,
	ccHebrew,
	ccArabic,
	ccDevanagari,
	ccBengali,
	ccGurmukhi,
	ccGujarati,
	ccOriya,
	ccTamil,
	ccTelugu,
	ccKannada,
	ccMalayalam,
	ccThai,
	ccLao,
	ccTibetan,
	ccGeorgian,
	ccHangul,
	ccHiragana,
	ccKatakana,
	ccKanji,
	ccBopomofo
};

int mclass(int unicode)
{
	int r;
	int msb = unicode >> 8;
	int lsb = unicode & 0x00ff;

	if ((msb >= 0x00004e00 && msb <= 0x00009fa5) || (msb >= 0x0000f900 && msb <= 0x0000fa2d))
		r = ccKanji;
	else if (msb >= 0x0000ac00 && msb <= 0x0000d7a3)
		r = ccHangul;
	else switch (msb)
	{
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x1e:
		case 0x20:
		case 0x21:	r = ccRoman;																		break;
		case 0x1f:
		case 0x03:	r = ccGreek;																			break;
		case 0x04:	r = ccCyrillic;																		break;
		case 0x05:	r = (lsb < 0x00d0) ? ccArmenian : ccHebrew;								break;
		case 0x06:
		case 0xfc:
		case 0xfd:
		case 0xfe:	r = ccArabic;																		break;
		case 0x09:	r = (lsb < 0x0080) ? ccDevanagari : ccBengali;								break;
		case 0x0a:	r = (lsb < 0x0080) ? ccGurmukhi : ccGujarati;								break;
		case 0x0b:	r = (lsb < 0x0080) ? ccOriya : ccTamil;										break;
		case 0x0c:	r = (lsb < 0x0080) ? ccTelugu : ccKannada;									break;
		case 0x0d:	r = ccMalayalam;																	break;
		case 0x0e:	r = (lsb < 0x0080) ? ccThai : ccLao;											break;
		case 0x0f:	r = ccTibetan;																		break;
		case 0x10:	r = ccGeorgian;																		break;
		case 0x11:	r = ccHangul;																		break;
		case 0x30:	r = (lsb >= 0x041 && lsb <= 0x091) ? ccHiragana : ccKatakana;		break;
		case 0x31:	r = (lsb < 0x31) ? ccBopomofo : ccHangul;									break;
			break;
		case 0xfb:
			if (lsb <= 0x06)
				r = ccRoman;
			else if (lsb <= 0x17)
				r = ccArmenian;
			else if (lsb <= 0x4f)
				r = ccHebrew;
			else
				r = ccArabic;
			break;
		case 0xff:
			if (lsb <= 0x5a)
				r = ccRoman;
			else if (lsb <= 0x09f)
				r = ccKatakana;
			else
				r = ccHangul;
			break;
		default:	r = ccNoClass;
	}

	return r;
} /* mclass */

static const char* gEncodings[] = {
	"UTF-8",
	"ISO 8859-1",
	"ISO 8859-2",
	"ISO 8859-3",
	"ISO 8859-4",
	"ISO 8859-5",
	"ISO 8859-6",
	"ISO 8859-7",
	"ISO 8859-8",
	"ISO 8859-9",
	"ISO 8859-10",
	"Macintosh Roman",
	"Shift-JIS",
	"EUC Packed Japanese",
	"JIS",
	"Windows 1252",
	"Unicode",
	"KOI8-R",
	"Windows 1251",
	"DOS 866",
	"DOS 437",
	"EUC Korean",
	"ISO 8859-13",
	"ISO 8859-14",
	"ISO 8859-15"
};

const int CEncodingRoster::sfMaxSupportedEncoding 
	= sizeof(gEncodings) / sizeof(const char*) - 1;

bool CEncodingRoster::IsSupportedEncoding(int encoding)
{
	return !(encoding < 0 || encoding > sfMaxSupportedEncoding);
}	

const char* CEncodingRoster::EncodingNameByIdx(int encoding)
{
	if (IsSupportedEncoding(encoding))
		return gEncodings[encoding];
	else
		return "<unknown encoding>";
}

CTextEncodingConverter::CTextEncodingConverter()
	: fStatus(B_NO_INIT)
	, fEncoding(0)
	, fConvertedText(NULL)
	, fConvertedSize(0)
	, fErrorPos(-1)
{
}

CTextEncodingConverter::CTextEncodingConverter(int encoding)
	: fStatus(B_NO_INIT)
	, fEncoding(0)
	, fConvertedText(NULL)
	, fConvertedSize(0)
	, fErrorPos(-1)
{
	_Init(encoding);
}

CTextEncodingConverter::~CTextEncodingConverter()
{
	_Cleanup();
}

status_t CTextEncodingConverter::SetTo(int encoding)
{
	_Cleanup();
	_Init(encoding);
	return fStatus;
}

status_t CTextEncodingConverter::ConvertToUtf8(BString& docText)
{
	if (!CEncodingRoster::IsSupportedEncoding(fEncoding))
		return B_UNSUPPORTED;

	fErrorPos = -1;
	status_t res = B_OK;
	if (fEncoding != B_UNICODE_UTF8)
	{
		int charset = fEncoding - 1;
			// map from BFont-encoding to charset conversions
		res = _DoConversion(docText.String(), docText.Length(), 
							fConvertedText, fConvertedSize, charset, true);
		if (res == B_OK)
		{	// check if any substitutes are found:
			const char* substPos = strchr(fConvertedText, B_SUBSTITUTE);
			if (substPos)
			{
				char* nativeText;
				int nativeSize;
				res = _DoConversion(fConvertedText, fConvertedSize, nativeText, 
									nativeSize, charset, false);
				if (res == B_OK)
				{
					int sz = std::min((int)docText.Length(), nativeSize);
					int i;
					const char* t = docText.String();
					char* ut = nativeText;
					for(i=0; i<sz; ++i)
						if (*t++ != *ut++)
							break;
					if (i < sz)
						fErrorPos = i;
					free(nativeText);
				}
			}
			// export converted string
			docText.SetTo(fConvertedText, fConvertedSize);
		}
		if (res != B_OK)
			_Cleanup();
	}
	else
		_CheckUtf8(docText);
	return res;
}

status_t CTextEncodingConverter::ConvertFromUtf8(BString& docText)
{
	if (!CEncodingRoster::IsSupportedEncoding(fEncoding))
		return B_UNSUPPORTED;

	fErrorPos = -1;
	status_t res = B_OK;
	if (fEncoding != B_UNICODE_UTF8)
	{
		int charset = fEncoding - 1;
			// map from BFont-encoding to charset conversions
		res = _DoConversion(docText.String(), docText.Length(), 
							fConvertedText, fConvertedSize, charset, false);
		if (res == B_OK)
		{	// check if any substitutes are found:
			const char* substPos = strchr(fConvertedText, B_SUBSTITUTE);
			if (substPos)
			{
				char* utf8Text;
				int utf8Size;
				res = _DoConversion(fConvertedText, fConvertedSize, utf8Text, 
									utf8Size, charset, true);
				if (res == B_OK)
				{
					int sz = std::min((int)docText.Length(), utf8Size);
					int i;
					const char* t = docText.String();
					char* ut = utf8Text;
					for(i=0; i<sz; ++i)
						if (*t++ != *ut++)
							break;
					if (i < sz)
						fErrorPos = i;
					free(utf8Text);
				}
			}
			// export converted string
			docText.SetTo(fConvertedText, fConvertedSize);
		}
		if (res != B_OK)
			_Cleanup();
	}
	else
		_CheckUtf8(docText);
	return res;
}

int CTextEncodingConverter::Encoding() const
{
	return fEncoding;
}

bool CTextEncodingConverter::HadToSubstitute() const
{
	return fErrorPos != -1;
}

int CTextEncodingConverter::ErrorPos() const
{
	return fErrorPos;
}

status_t CTextEncodingConverter::InitCheck() const
{
	return fStatus;
}

void CTextEncodingConverter::_Init(int encoding)
{
	fEncoding = encoding;
	if (!CEncodingRoster::IsSupportedEncoding(fEncoding))
		fStatus = B_UNSUPPORTED;
	else
		fStatus = B_OK;
}

status_t CTextEncodingConverter::_DoConversion(const char* text, int size,
											   char*& outText, int& outSize,
											   int charset, bool toUtf8)
{
	const float incFactor = 1.5;
	int32 srcSize = size;
	int32 srcLen = 0;
	int32 srcOffset = 0;
	int32 destSize = std::max(size, 128);
	int32 destLen;
	int32 destOffset = 0;
	outText = NULL;
	int32 state = 0;
	status_t res = B_OK;
	while (srcOffset < srcSize)
	{
		destSize = (int32)(destSize * incFactor);
		outText = (char *)realloc(outText, destSize);
		FailNil(outText);
		srcLen = srcSize - srcOffset;
		destLen = destSize - destOffset;
		if (toUtf8)
			res = convert_to_utf8(charset, text+srcOffset, &srcLen, 
								  outText+destOffset, &destLen, &state);
		else
			res = convert_from_utf8(charset, text+srcOffset, &srcLen, 
									outText+destOffset, &destLen, &state);
		if (res != B_OK)
			break;
		srcOffset += srcLen;
		destOffset += destLen;
	}
	if (res == B_OK)
		outSize = destOffset;
	else
	{
		free(outText);
		outSize = 0;
	}
	return res;
}

void CTextEncodingConverter::_CheckUtf8(const BString& docText)
{
	// check validity of utf-8 string
	bool ok = true;
	int j, i, l;
	unsigned char c;
	for( i=0; i < docText.Length(); )
	{
		j = 1;
		if ((docText[i] & 0xc0) == 0xc0)
		{
			// determine charlength manually...
			while((docText[i+j] & 0xc0) == 0x80)
				++j;
			c = docText[i];
			// ...and crosscheck with bit-encoded length:
			if ((c & 0xE0) == 0xC0)
				l = 2;
			else if ((c & 0xF0) == 0xE0)
				l = 3;
			else if ((c & 0xF8) == 0xF0)
				l = 4;
			else if ((c & 0xFC) == 0xF8)
				l = 5;
			else if ((c & 0xFE) == 0xFC)
				l = 6;
			else
				l = 0;	// clearly this is an error
			if (j != l)
				ok = false;
		}
		else if ((docText[i] & 0xc0) == 0x80)
		{
			// utf8-sub-char at start
			ok = false;
		}
		if (!ok)
			break;
		i += j;
	}
	if (!ok)
		fErrorPos = i;
}

void CTextEncodingConverter::_Cleanup()
{
	free(fConvertedText);
	fConvertedText = NULL;
	fConvertedSize = 0;
	fErrorPos = -1;
}

