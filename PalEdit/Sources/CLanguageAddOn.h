/*	$Id: CLanguageAddOn.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 12/07/97 21:46:44 by Maarten Hekkelman
*/

#ifndef CLANGUAGEADDON_H
#define CLANGUAGEADDON_H

// these constants define the different keyword-types that are returned
// by IsKeyword() (see below):
enum {
	kKeywordLanguage = 1,
	kKeywordUser1,
	kKeywordUser2,
	kKeywordUser3,
	kKeywordUser4
};

class CFunctionScanHandler;
class CLanguageInterface;
class PText;

/*
	The next class is the proxy between Pe and the syntax colouring plug-in.
	It provides every information needed for colouring the text. Note however that
	the same proxy is used for different tasks (I know, I'm lazy). Therefore it is
	possible to SetColor in a Balance call. That's not wise though and may result in
	crashes. Pe should check this, but as I said, I'm lazy.
*/

class CLanguageProxy {
public:
		CLanguageProxy(CLanguageInterface& intf, const char *text, int size, int *starts = NULL, rgb_color *colors = NULL);
		CLanguageProxy(CLanguageInterface& intf, PText& text, CFunctionScanHandler* handler = NULL);

/*
	Use the next two methods to recognize keywords in the text.
	Move is used to walk through a keyword map and reports back the state it is
	currently in. Always start with a state 1 (one).
	If the result of a Move is 0 (zero), than you haven't fed it a keyword.
	If the state is not zero, you can test if it's a valid keyword by calling IsKeyword.
	IsKeyword returns the set the keyword is in, or zero if it's not a keyword.
	A value of 1 (one) corresponds to a standard keyword, a value of 2 means a
	keyword from user set 1 and so on.

	You can always call these functions but I think they are only useful when doing
	syntax colouring.
*/
virtual	int Move(int ch, int state);
virtual	int IsKeyword(int state, int32 inSets=0);

/*
	The next pair of calls return the text to colour or parse and it's size.
	When doing syntax colouring Pe guarantees to return only one line of
	text with Text, the other two situations get the full text.
*/

virtual	const char *Text() const;
virtual	int Size() const;

/*
	Call SetColor only while doing syntax colouring, nasty things will happen otherwise.
	You pass the offset from the beginning of the text and the colour the text should
	change to. The value of color should be one of the enums of the beginning of this
	header file.
	You don't have to gather info yourself, calling SetColor with a color value the same
	as the current color is a noop.
*/
virtual	void SetColor(int start, int color);

/*
	The next three calls are here for adding functions and includes to the function popup.
	the name parameter is used to define the Label of the menu items.
	The match parameter is the string that has to be selected in the text.
	Offset should be the nr of characters match can be found from the beginning of the
	text. This value may be incorrect but should be 'near by', preferrably before.
	The open parameter is of course the name of the include file.
*/
virtual	void AddFunction(const char *name, const char *match, int offset,
	bool italic = false, const char *params = "");
virtual	void AddInclude(const char *name, const char *open, bool italic = false);
virtual	void AddSeparator(const char* name = NULL);

/*
	These report the status of the corresponding checkboxes
*/
virtual	bool Includes() const;
virtual	bool Prototypes() const;
virtual	bool Types() const;
virtual	bool Sorted() const;

/*
	These methods are used for wrapping, they classify the characters in the text
*/
virtual	void CharInfo(const char *txt, int& unicode, int& len) const;
virtual	bool isalpha_uc(int unicode) const;
virtual	bool isalnum_uc(int unicode) const;
virtual	bool isspace_uc(int unicode) const;

virtual	void IncNestLevel();
virtual	void DecNestLevel();
virtual	void SetNestLevel(uint32 nestLevel);

private:
		const char *fText;
		int fSize;
		CLanguageInterface& fInterface;
		int fCIndx;
		int *fStarts;
		rgb_color *fColors;
		CFunctionScanHandler* fFunctionScanHandler;
		uint32 fNestLevel;
};

/*
	These five strings should be exported by the plugin.
	- LanguageName is the name of the language
	- LanguageExtensions is the same string as you see in the suffixes box
		in the preferences
	- LanguageCommentStart is the string used for commenting text,
		it's pasted before each line
	- LanguageCommentEnd is likewise pasted behind every line to be
		commented
	- LanguageKeywordFile is the name of the keyword file in settings and
		the name of the resource containing a default keyword file.
		This resource should have type 'KeyW'
*/

extern "C" {

#if !__INTEL__
#pragma export on
#endif

extern const char kLanguageName[];
extern const char kLanguageExtensions[];
extern const char kLanguageCommentStart[];
extern const char kLanguageCommentEnd[];
extern const char kLanguageKeywordFile[];
extern const int16 kInterfaceVersion;

/*
	These are the prototypes for the three functions you should implement.
	All three are optional, but leaving them all out is may not be a good idea.
*/

/*
	Balance is called to select text between matching brackets e.g.
	start and end are the current selection start and end. Change them to
	select some other part of text.
*/
_EXPORT bool Balance(CLanguageProxy& proxy, int& start, int& end);

/*
	ScanForFunctions, the name says it all. Scan the text and report which
	function could be found where.
*/
_EXPORT void ScanForFunctions(CLanguageProxy& proxy);

/*
	ColorLine is called to report the colour changes in the current line.
	The state parameter tells which state the last line ended in. Useful
	for doing multiline style e.g.
	At most 99 colour changes can be recorded, more are ignored.
*/
_EXPORT void ColorLine(CLanguageProxy& proxy, int& state);

/*
	FindNextWord is called to help find the next line break for softwrapping.
	The function is passed just a proxy that has a pointer to a piece of text
	and its size. FindNextWord should return the offset at which the next break
	may occur.

	This function is new for version 2.0
*/

_EXPORT int FindNextWord(const CLanguageProxy& proxy);

}

#if !__INTEL__
#pragma export reset
#endif

#endif // CLANGUAGEADDON_H
