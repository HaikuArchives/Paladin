/*	$Id: HtmlCssJsPhp_Language.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $

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

	Created: 12/07/97 22:01:11 by Maarten Hekkelman
*/

#include <cctype>
#include <cstring>
#include <stack>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

using std::stack;

#define DEBUG_PHP 0
const bool CASE_SENSITIVE_PHP  = true;
const bool CASE_SENSITIVE_JS   = true;
const bool CASE_SENSITIVE_TAG  = true;
const bool CASE_SENSITIVE_ATTR = true;


enum Language {
	kLanguageHtml,
	kLanguageJs,
	kLanguagePhp
};

_EXPORT const char kLanguageName[]         = "HTML-CSS-JS-PHP";
_EXPORT const char kLanguageExtensions[]   = "html;htm;php";
_EXPORT const char kLanguageCommentStart[] = "<!--";
_EXPORT const char kLanguageCommentEnd[]   = "-->";
_EXPORT const char kLanguageKeywordFile[]  = "keywords.html-css-js-php";
_EXPORT const int16 kInterfaceVersion      = 2;

enum {
	START = 0,
		TAG_START,
		TAG_KEYWORD_START,
		TAG_KEYWORD_END,
		TAG,
		TAG_STRING_DOUBLE_QUOTES,
		TAG_STRING_SINGLE_QUOTES,
		TAG_ATTRIBUTE,
		SPECIAL,
		COMMENT_DTD,
		COMMENT,
	STYLESHEET,
		CSS_COMMENT,
		CSS_DEFINITION,
	JAVASCRIPT,
		JS_COMMENT_MULTI_LINE,
		JS_COMMENT_SINGLE_LINE,
		JS_IDENTIFIER,
		JS_STRING_SINGLE_QUOTES,
		JS_STRING_DOUBLE_QUOTES,
	PHP_SCRIPT,
		PHP_IDENTIFIER,
		PHP_COMMENT_MULTI_LINE,
		PHP_COMMENT_SINGLE_LINE,
		PHP_STRING_DOUBLE_QUOTES,
		PHP_STRING_SINGLE_QUOTES
};

enum {
	kKwHtmlTag			= 1,	// Set 1
	kKwHtmlAttribute	= 2,	// Set 2
	kKwJsLanguage		= 4,	// Set 3
	kKwJsConstants		= 8,	// Set 4
	kKwPhpLanguage		= 16,	// Set 5
	kKwPhpFunctions		= 32,	// Set 6
	kKwPhpConstants		= 64,	// Set 7
	kKwUserset1			= 128,	// Set 8
	kKwUserset2			= 256,	// Set 9
	kKwUserset3			= 512,	// Set 10
	kKwUserset4			= 1024,	// Set 11
	//
	kKwUsersets			= kKwUserset1|kKwUserset2|kKwUserset3|kKwUserset4,
	kKwJs				= kKwJsLanguage|kKwJsConstants,
	kKwPhp				= kKwPhpLanguage|kKwPhpFunctions|kKwPhpConstants
};

// Color configuration
enum {
	kColHtmlComment					= kColorComment1,
	kColHtmlDtd						= kColorComment2,
	kColHtmlEntity					= kColorCharConst,
	kColHtmlAttribute				= kColorAttribute,
	kColHtmlTagStringDoubleQuotes	= kColorString1,
	kColHtmlTagStringSingleQuotes	= kColorString2,
	//
	kColCssText						= kColorPreprocessor1,
	//
	kColJsStringDoubleQuotes		= kColorString1,
	kColJsStringSingleQuotes		= kColorString2,
	kColJsComment					= kColorComment2,
	kColJsCommentL					= kColorComment1,
	//
	kColPhpLanguage					= kColorKeyword1,
	kColPhpFunction					= kColorKeyword2,
	kColPhpConstants				= kColorCharConst,
	kColPhpStringDoubleQuotes		= kColorString1,
	kColPhpStringSingleQuotes		= kColorString2,
	kColPhpComment					= kColorComment2,
	kColPhpCommentL					= kColorComment1,
};

#pragma mark Debug-Functions


#if DEBUG_PHP

void DEB_PrintState(const int state, const char c, const char *sub="")
{
	const char* name = NULL;
	switch (state)
	{
		case START:						name = "START"; break;
		case TAG_START:					name = "TAG_START"; break;
		case TAG_KEYWORD_START:			name = "TAG_KEYWORD_START"; break;
		case TAG_KEYWORD_END:			name = "TAG_KEYWORD_END"; break;
		case TAG:						name = "TAG"; break;
		case TAG_STRING_DOUBLE_QUOTES:	name = "TAG_STRING_DOUBLE_QUOTES"; break;
		case TAG_STRING_SINGLE_QUOTES:	name = "TAG_STRING_SINGLE_QUOTES"; break;
		case TAG_ATTRIBUTE:				name = "TAG_ATTRIBUTE"; break;
		case SPECIAL:					name = "SPECIAL"; break;
		case COMMENT_DTD:				name = "COMMENT_DTD"; break;
		case COMMENT:					name = "COMMENT"; break;
		case STYLESHEET:				name = "STYLESHEET"; break;
		case CSS_COMMENT:				name = "CSS_COMMENT"; break;
		case CSS_DEFINITION:			name = "CSS_DEFINITION"; break;
		case JAVASCRIPT:				name = "JAVASCRIPT"; break;
		case JS_COMMENT_MULTI_LINE:		name = "JS_COMMENT_MULTI_LINE"; break;
		case JS_COMMENT_SINGLE_LINE:	name = "JS_COMMENT_SINGLE_LINE"; break;
		case JS_IDENTIFIER:				name = "JS_IDENTIFIER"; break;
		case JS_STRING_SINGLE_QUOTES:	name = "JS_STRING_SINGLE_QUOTES"; break;
		case JS_STRING_DOUBLE_QUOTES:	name = "JS_STRING_DOUBLE_QUOTES"; break;
		case PHP_SCRIPT:				name = "PHP_SCRIPT"; break;
		case PHP_IDENTIFIER:			name = "PHP_IDENTIFIER"; break;
		case PHP_COMMENT_MULTI_LINE:	name = "PHP_COMMENT_MULTI_LINE"; break;
		case PHP_COMMENT_SINGLE_LINE:	name = "PHP_COMMENT_SINGLE_LINE"; break;
		case PHP_STRING_DOUBLE_QUOTES:	name = "PHP_STRING_DOUBLE_QUOTES"; break;
		case PHP_STRING_SINGLE_QUOTES:	name = "PHP_STRING_SINGLE_QUOTES"; break;
	}
	if (name)
	{
		printf("### %s(%s)", name, sub);
		printf(isprint(c) ? " [%c]" : " [0x%02X]", c);
		printf("\n");
	}
}

void DEB_PrintSetKw(const int state, int keyword)
{
	const char* name = NULL;
	switch (state)
	{
		case START:						name = "START"; break;
		case TAG_START:					name = "TAG_START"; break;
		case TAG_KEYWORD_START:			name = "TAG_KEYWORD_START"; break;
		case TAG_KEYWORD_END:			name = "TAG_KEYWORD_END"; break;
		case TAG:						name = "TAG"; break;
		case TAG_STRING_DOUBLE_QUOTES:	name = "TAG_STRING_DOUBLE_QUOTES"; break;
		case TAG_STRING_SINGLE_QUOTES:	name = "TAG_STRING_SINGLE_QUOTES"; break;
		case TAG_ATTRIBUTE:				name = "TAG_ATTRIBUTE"; break;
		case SPECIAL:					name = "SPECIAL"; break;
		case COMMENT_DTD:				name = "COMMENT_DTD"; break;
		case COMMENT:					name = "COMMENT"; break;
		case STYLESHEET:				name = "STYLESHEET"; break;
		case CSS_COMMENT:				name = "CSS_COMMENT"; break;
		case CSS_DEFINITION:			name = "CSS_DEFINITION"; break;
		case JAVASCRIPT:				name = "JAVASCRIPT"; break;
		case JS_COMMENT_MULTI_LINE:		name = "JS_COMMENT_MULTI_LINE"; break;
		case JS_COMMENT_SINGLE_LINE:	name = "JS_COMMENT_SINGLE_LINE"; break;
		case JS_IDENTIFIER:				name = "JS_IDENTIFIER"; break;
		case JS_STRING_SINGLE_QUOTES:	name = "JS_STRING_SINGLE_QUOTES"; break;
		case JS_STRING_DOUBLE_QUOTES:	name = "JS_STRING_DOUBLE_QUOTES"; break;
		case PHP_SCRIPT:				name = "PHP_SCRIPT"; break;
		case PHP_IDENTIFIER:			name = "PHP_IDENTIFIER"; break;
		case PHP_COMMENT_MULTI_LINE:	name = "PHP_COMMENT_MULTI_LINE"; break;
		case PHP_COMMENT_SINGLE_LINE:	name = "PHP_COMMENT_SINGLE_LINE"; break;
		case PHP_STRING_DOUBLE_QUOTES:	name = "PHP_STRING_DOUBLE_QUOTES"; break;
		case PHP_STRING_SINGLE_QUOTES:	name = "PHP_STRING_SINGLE_QUOTES"; break;
	}
	const char* kwname = NULL;
	switch (keyword)
	{
		case kKwHtmlTag:		kwname = "HtmlTag"; break;
		case kKwHtmlAttribute:	kwname = "HtmlAttribute"; break;
		case kKwJsLanguage:		kwname = "JsLanguage"; break;
		case kKwJsConstants:	kwname = "JsConstants"; break;
		case kKwPhpLanguage:	kwname = "PhpLanguage"; break;
		case kKwPhpFunctions:	kwname = "PhpFunctions"; break;
		case kKwPhpConstants:	kwname = "PhpConstants"; break;
		case kKwUserset1:		kwname = "Userset1"; break;
		case kKwUserset2:		kwname = "Userset2"; break;
		case kKwUserset3:		kwname = "Userset3"; break;
		case kKwUserset4:		kwname = "Userset4"; break;
	}
	//
	if (name)
	{
		if (kwname)
		{
			printf("    IsKeyword: <%s>\n", kwname);
		}
		else
		{
			printf("    IsKeyword: <%i>\n", keyword);
		}
	}
}

const char* DEB_StateName(int state)
{
	switch (state)
	{
		case START:						return "START";
		case TAG_START:					return "TAG_START";
		case TAG_KEYWORD_START:			return "TAG_KEYWORD_START";
		case TAG_KEYWORD_END:			return "TAG_KEYWORD_END";
		case TAG:						return "TAG";
		case TAG_STRING_DOUBLE_QUOTES:	return "TAG_STRING_DOUBLE_QUOTES";
		case TAG_STRING_SINGLE_QUOTES:	return "TAG_STRING_SINGLE_QUOTES";
		case TAG_ATTRIBUTE:				return "TAG_ATTRIBUTE";
		case SPECIAL:					return "SPECIAL";
		case COMMENT_DTD:				return "COMMENT_DTD";
		case COMMENT:					return "COMMENT";
		case STYLESHEET:				return "STYLESHEET";
		case CSS_COMMENT:				return "CSS_COMMENT";
		case CSS_DEFINITION:			return "CSS_DEFINITION";
		case JAVASCRIPT:				return "JAVASCRIPT";
		case JS_COMMENT_MULTI_LINE:		return "JS_COMMENT_MULTI_LINE";
		case JS_COMMENT_SINGLE_LINE:	return "JS_COMMENT_SINGLE_LINE";
		case JS_IDENTIFIER:				return "JS_IDENTIFIER";
		case JS_STRING_SINGLE_QUOTES:	return "JS_STRING_SINGLE_QUOTES";
		case JS_STRING_DOUBLE_QUOTES:	return "JS_STRING_DOUBLE_QUOTES";
		case PHP_SCRIPT:				return "PHP_SCRIPT";
		case PHP_IDENTIFIER:			return "PHP_IDENTIFIER";
		case PHP_COMMENT_MULTI_LINE:	return "PHP_COMMENT_MULTI_LINE";
		case PHP_COMMENT_SINGLE_LINE:	return "PHP_COMMENT_SINGLE_LINE";
		case PHP_STRING_DOUBLE_QUOTES:	return "PHP_STRING_DOUBLE_QUOTES";
		case PHP_STRING_SINGLE_QUOTES:	return "PHP_STRING_SINGLE_QUOTES";
		default:						return "UNKOWN";
	}
}

#else

	#define DEB_PrintState
	#define DEB_StateName
	#define DEB_PrintSetKw

#endif


#pragma mark Support


//#define GETCHAR  (c = (i++ < size) ? text[i - 1] : 0)
inline char get_char(int &i, int &size, const char *text)
{
	return (i++ < size) ? text[i-1] : 0;
}


#pragma mark ColorLine


_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	char c;					// Current character
	int nxt = 0,			// Position after current character in text (i)
		beg = 0,			// (s)
		kws = 0,			//
		forceState = START,	//
		color,				//
		kwc;				// Keywordcode
	bool leave = false, esc = false, script = false, percent = false, style = false;

	proxy.SetColor(0, kColorText);

	if (size <= 0)
		return;

	script = (state >= JAVASCRIPT);
	percent = false;

#if DEBUG_PHP
	printf("#############################################################################\nColorLine(%s:%s)\n%s\n", DEB_StateName(state), script ? "IsScript" : "NoScript", text);
#endif

	while (!leave)
	{
		c = get_char(nxt, size, text);

		switch (state)
		{
			case START:
				DEB_PrintState(state, c);
				if (c == '<')
					state = TAG_START;
				else if (c == '&')
					state = SPECIAL;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != START) && beg < nxt)
				{
					proxy.SetColor(beg, kColorText);
					beg = nxt-1;
				}
				break;

			case TAG_START:
				DEB_PrintState(state, c);
				if (c == '/')
					state = TAG_KEYWORD_END;
				else if (c == '!')
				{
					state = COMMENT_DTD;
					forceState = START;
				}
				else if (c == '?' || c == '%')
				{
					proxy.SetColor(beg, kColorTag);
					beg = nxt;

					if (strncasecmp(text+beg, "php", 3) == 0)
					{
						proxy.SetColor(beg, kColorTag);
						beg = (nxt += 3);
					}

					state = PHP_SCRIPT;
					percent = (c == '%');
				}
				else if (isalpha(c))
				{
					proxy.SetColor(beg, kColorTag);
					beg = nxt-1;
					kws = proxy.Move(CASE_SENSITIVE_TAG ? c : tolower(c), 1);
					state = TAG_KEYWORD_START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColorTag);
					leave = true;
				}
				else if (!isspace(c))
				{
					nxt--;
					state = TAG;
				}
				break;

			case TAG_KEYWORD_END:
				DEB_PrintState(state, c);
				if (isalpha(c))
				{
					proxy.SetColor(beg, kColorTag);
					beg = nxt-1;
					kws = proxy.Move(CASE_SENSITIVE_TAG ? c : tolower(c), 1);
					state = TAG_KEYWORD_START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColorTag);
					leave = true;
				}
				else if (!isspace(c))
				{
					nxt--;
					state = TAG;
				}
				break;

			case TAG_KEYWORD_START:
				DEB_PrintState(state, c);
				if (!isalnum(c) && c != '-')
				{
					switch (kwc = proxy.IsKeyword(kws, kKwHtmlTag|kKwUsersets))
					{
						case kKwHtmlTag:  color = kColorTag;      break;
						case kKwUserset1: color = kColorUserSet1; break;
						case kKwUserset2: color = kColorUserSet2; break;
						case kKwUserset3: color = kColorUserSet3; break;
						case kKwUserset4: color = kColorUserSet4; break;
						default:          color = kColorText;     break;
					}
					proxy.SetColor(beg, color);
					DEB_PrintSetKw(state, kwc);

					if (text[beg-1] != '/')
					{
						if (strncasecmp(text+beg, "script", 6) == 0)
							script = true;
						else if (strncasecmp(text+beg, "style", 5) == 0)
							style = true;
					}
					state = TAG;
					beg = --nxt;
				}
				else if (kws)
					kws = proxy.Move(CASE_SENSITIVE_TAG ? c : tolower(c), kws);
				break;

			case TAG:
				DEB_PrintState(state, c);
				switch (c)
				{
					case 0:
					case '\n':
						proxy.SetColor(beg, kColorTag);
						leave = true;
						break;
					case '>':
						proxy.SetColor(beg, kColorTag);
						beg = nxt;
						proxy.SetColor(beg, kColorText);
						if (script)
							state = JAVASCRIPT;
						else if (style)
							state = STYLESHEET;
						else
							state = START;
						break;
					case '"':
						proxy.SetColor(beg, kColorTag);
						beg = nxt-1;
						state = TAG_STRING_DOUBLE_QUOTES;
						break;
					case '\'':
						proxy.SetColor(beg, kColorTag);
						beg = nxt-1;
						state = TAG_STRING_SINGLE_QUOTES;
						break;
					case '!':
						if (nxt == beg+2)
						{
							proxy.SetColor(beg, kColorTag);
							state = COMMENT_DTD;
							forceState = START;
						}
						break;
					default:
						if (isalpha(c))
						{
							proxy.SetColor(beg, kColorTag);
							beg = nxt-1;
							kws = proxy.Move(CASE_SENSITIVE_ATTR ? c : tolower(c), 1);
							state = TAG_ATTRIBUTE;
						}
						break;
				}
				break;

			case TAG_STRING_DOUBLE_QUOTES:
				DEB_PrintState(state, c);
				if (c == '"')
				{
					proxy.SetColor(beg, kColHtmlTagStringDoubleQuotes);
					beg = nxt;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(beg, kColHtmlTagStringDoubleQuotes);
					leave = true;
				}
				break;

			case TAG_STRING_SINGLE_QUOTES:
				DEB_PrintState(state, c);
				if (c == '\'')
				{
					proxy.SetColor(beg, kColHtmlTagStringSingleQuotes);
					beg = nxt;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(beg, kColHtmlTagStringSingleQuotes);
					leave = true;
				}
				break;

			case TAG_ATTRIBUTE:
				DEB_PrintState(state, c);
				if (!isalnum(c) && c != '-')
				{
					switch (kwc = proxy.IsKeyword(kws, kKwHtmlAttribute|kKwUsersets))
					{
						case kKwHtmlAttribute: color = kColHtmlAttribute; break;
						case kKwUserset1:      color = kColorUserSet1;    break;
						case kKwUserset2:      color = kColorUserSet2;    break;
						case kKwUserset3:      color = kColorUserSet3;    break;
						case kKwUserset4:      color = kColorUserSet4;    break;
						default:               color = kColorText;        break;
					}
					proxy.SetColor(beg, color);
					DEB_PrintSetKw(state, kwc);

					beg = --nxt;
					state = TAG;
				}
				else if (kws)
					kws = proxy.Move(CASE_SENSITIVE_ATTR ? c : tolower(c), kws);
				break;

			case SPECIAL:
				DEB_PrintState(state, c);
				if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColorText);
					state = START;
					leave = true;
				}
				else if (c == ';')
				{
					proxy.SetColor(beg, kColHtmlEntity);
					beg = nxt;
					state = START;
				}
				else if (isspace(c))
					state = START;
				break;

			case COMMENT_DTD:
				DEB_PrintState(state, c);
				if (c == '-' && text[nxt] == '-' && nxt == beg+3 && text[nxt-2] == '!' && text[nxt-3] == '<')
				{
					proxy.SetColor(beg, kColHtmlDtd);
					beg = nxt-1;
					state = COMMENT;
				}
				else if (c == '>')
				{
					proxy.SetColor(beg, kColHtmlDtd);
					beg = nxt;
					state = forceState;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColHtmlDtd);
					leave = true;
				}
				break;

			case COMMENT:
				DEB_PrintState(state, c);
				if (beg == 0 && c == ':')
				{
					if (strncasecmp(text+nxt, "javascript", 10) == 0)
						forceState = JAVASCRIPT;
					else if (strncasecmp(text+nxt, "php", 3) == 0)
						forceState = PHP_SCRIPT;
				}
				else if (c == '-' && text[nxt] == '-')
				{
					proxy.SetColor(beg, kColHtmlComment);
					beg = ++nxt;
					state = COMMENT_DTD;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColHtmlComment);
					leave = true;
				}
				break;

			case STYLESHEET:
				DEB_PrintState(state, c);

				if (c == '<' && text[nxt] == '/')
				{
					if (strncasecmp(text+nxt+1, "style", 5) == 0)
					{
						nxt++;
						state = TAG_START;
					}
				}
				else if (c == '/' && text[nxt] == '*')
					state = CSS_COMMENT;
//				else if (isalpha(c))
//				{
//					kws = proxy.Move(CASE_SENSITIVE_JS ? c : tolower(c), 1);
//					state = JS_IDENTIFIER;
//				}
				else if (c == '{')
					state = CSS_DEFINITION;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != STYLESHEET) && beg < nxt)
				{
					proxy.SetColor(beg, kColCssText);
					beg = nxt-1;
				}
				break;

			case CSS_COMMENT:
				DEB_PrintState(state, c);
				if ((beg == 0 || nxt > beg+1) && c == '*' && text[nxt] == '/')
				{
					proxy.SetColor(beg, kColJsComment);
					beg = nxt+1;
					state = STYLESHEET;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColJsComment);
					leave = true;
				}
				break;

			case CSS_DEFINITION:
				DEB_PrintState(state, c);
				if (c == '}')
				{
					state = STYLESHEET;
				}
				else if (c == 0 || c == '\n')
					leave = true;
				break;

			case JAVASCRIPT:
				DEB_PrintState(state, c);
				script = false;

				if (c == '<' && strncasecmp(text+nxt, "!--", 3) == 0)
				{
					proxy.SetColor(beg, kColorTag);
					proxy.SetColor(nxt+1, kColorComment1);
					nxt += 3;
					beg = nxt;
				}
				else if (c == '<' && text[nxt] == '/')
				{
					if (strncasecmp(text+nxt+1, "script", 6) == 0)
					{
						nxt++;
						state = TAG_START;
					}
				}
				else if (c == '/' && text[nxt] == '*')
					state = JS_COMMENT_MULTI_LINE;
				else if (c == '/' && text[nxt] == '/')
					state = JS_COMMENT_SINGLE_LINE;
				else if (isalpha(c))
				{
					kws = proxy.Move(CASE_SENSITIVE_JS ? c : tolower(c), 1);
					state = JS_IDENTIFIER;
				}
				else if (c == '\'')
					state = JS_STRING_SINGLE_QUOTES;
				else if (c == '"')
					state = JS_STRING_DOUBLE_QUOTES;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != JAVASCRIPT) && beg < nxt)
				{
					proxy.SetColor(beg, kColorText);
					beg = nxt-1;
				}
				break;

			case JS_COMMENT_MULTI_LINE:
				DEB_PrintState(state, c);
				if ((beg == 0 || nxt > beg+1) && c == '*' && text[nxt] == '/')
				{
					proxy.SetColor(beg, kColJsComment);
					beg = nxt+1;
					state = JAVASCRIPT;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColJsComment);
					leave = true;
				}
				break;

			case JS_COMMENT_SINGLE_LINE:
				DEB_PrintState(state, c);
				if (c == '-' && text[nxt] == '-')
				{
					proxy.SetColor(beg, kColJsCommentL);
					beg = nxt+1;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(beg, kColJsCommentL);
					state = JAVASCRIPT;
					leave = true;
				}
				break;

			case JS_IDENTIFIER:
				DEB_PrintState(state, c);
				if (!isalnum(c) && c != '_')
				{
					if (nxt > beg+1 && (kwc = proxy.IsKeyword(kws, kKwJs|kKwUsersets)) != 0)
					{
						switch (kwc)
						{
							case kKwJsLanguage:  color = kColorKeyword1;  break;
							case kKwJsConstants: color = kColorCharConst; break;
							case kKwUserset1:    color = kColorUserSet1;  break;
							case kKwUserset2:    color = kColorUserSet2;  break;
							case kKwUserset3:    color = kColorUserSet3;  break;
							case kKwUserset4:    color = kColorUserSet4;  break;
							default:             color = kColorText;      break;
						}
						proxy.SetColor(beg, color);
						DEB_PrintSetKw(state, kwc);
					}
					else
					{
						proxy.SetColor(beg, kColorText);
					}

					beg = --nxt;
					state = JAVASCRIPT;
				}
				else if (kws)
					kws = proxy.Move(CASE_SENSITIVE_JS ? c : tolower(c), kws);
				break;

			case JS_STRING_SINGLE_QUOTES:
			case JS_STRING_DOUBLE_QUOTES:
				DEB_PrintState(state, c);
				if (!esc &&
					((state == JS_STRING_SINGLE_QUOTES && c == '\'') ||
					(state == JS_STRING_DOUBLE_QUOTES && c == '"')))
				{
					proxy.SetColor(beg, (state == JS_STRING_SINGLE_QUOTES) ? kColJsStringSingleQuotes : kColJsStringDoubleQuotes);
					beg = nxt;
					state = JAVASCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					if (text[nxt-2] == '\\' && text[nxt-3] != '\\')
					{
						proxy.SetColor(beg, (state == JS_STRING_SINGLE_QUOTES) ? kColJsStringSingleQuotes : kColJsStringDoubleQuotes);
					}
					else
					{
						proxy.SetColor(beg, kColorText);
						state = JAVASCRIPT;
					}

					beg = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_SCRIPT:
				DEB_PrintState(state, c);
				if (((c == '?' && !percent) || (c == '%' && percent)) && text[nxt] == '>')
				{
					proxy.SetColor(beg, kColorTag);
					beg = ++nxt;
					state = START;
				}
				else if (isalpha(c))
				{
					kws = proxy.Move(CASE_SENSITIVE_PHP ? c : tolower(c), 1);
					state = PHP_IDENTIFIER;
				}
				else if (c == '"')
					state = PHP_STRING_DOUBLE_QUOTES;
				else if (c == '\'')
					state = PHP_STRING_SINGLE_QUOTES;
				else if (c == '#' || (c == '/' && text[nxt] == '/'))
					state = PHP_COMMENT_SINGLE_LINE;
				else if (c == '/' && text[nxt] == '*')
					state = PHP_COMMENT_MULTI_LINE;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != START) && beg < nxt)
				{
					proxy.SetColor(beg, kColorText);
					beg = nxt-1;
				}
				break;

			case PHP_IDENTIFIER:
				if (!isalnum(c) && c != '_')
				{
					if (nxt > beg+1 && (kwc = proxy.IsKeyword(kws, kKwPhp|kKwUsersets)))
					{
						DEB_PrintState(state, c, "KWD");
						switch (kwc)
						{
							case kKwPhpLanguage:   color = kColPhpLanguage;  break;
							case kKwPhpFunctions:  color = kColPhpFunction;  break;
							case kKwPhpConstants:  color = kColPhpConstants; break;
							case kKwUserset1:      color = kColorUserSet1;   break;
							case kKwUserset2:      color = kColorUserSet2;   break;
							case kKwUserset3:      color = kColorUserSet3;   break;
							case kKwUserset4:      color = kColorUserSet4;   break;
							default:               color = kColorText;       break;
						}
						proxy.SetColor(beg, color);
						DEB_PrintSetKw(state, kwc);
					}
					else
					{
						DEB_PrintState(state, c, "TXT");
						proxy.SetColor(beg, kColorText);
					}

					beg = --nxt;
					state = PHP_SCRIPT;
				}
				else if (kws)
					DEB_PrintState(state, c, "MOV");
					kws = proxy.Move(CASE_SENSITIVE_PHP ? c : tolower(c), kws);
				break;

			case PHP_STRING_DOUBLE_QUOTES:
				DEB_PrintState(state, c);
				if (!esc && c == '"')
				{
					proxy.SetColor(beg, kColPhpStringDoubleQuotes);
					beg = nxt;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(beg, kColPhpStringDoubleQuotes);
					beg = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_STRING_SINGLE_QUOTES:
				DEB_PrintState(state, c);
				if (!esc && c == '\'')
				{
					proxy.SetColor(beg, kColPhpStringSingleQuotes);
					beg = nxt;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(beg, kColPhpStringSingleQuotes);
					beg = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_COMMENT_MULTI_LINE:
				DEB_PrintState(state, c);
				if ((beg == 0 || nxt > beg+1) && c == '*' && text[nxt] == '/')
				{
					proxy.SetColor(beg, kColPhpComment);
					beg = nxt+1;
					state = PHP_SCRIPT;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColPhpComment);
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[nxt] == '>')
				{
					proxy.SetColor(beg, kColPhpComment);
					beg = nxt-1;
					proxy.SetColor(beg, kColorTag);
					beg = ++nxt;
					state = START;
				}
				break;

			case PHP_COMMENT_SINGLE_LINE:
				DEB_PrintState(state, c);
				if (c == 0 || c == '\n')
				{
					proxy.SetColor(beg, kColPhpCommentL);
					state = PHP_SCRIPT;
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[nxt] == '>')
				{
					proxy.SetColor(beg, kColPhpCommentL);
					beg = nxt-1;
					proxy.SetColor(beg, kColorTag);
					beg = ++nxt;
					state = START;
				}
				break;

			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}

#if DEBUG_PHP
	printf("--> (%s:%s)\n", DEB_StateName(state), script ? "IsScript" : "NoScript");
#endif

//	if (script)
//		state |= 0x8000;
//
//	if (percent)
//		state |= 0x4000;
} /* ColorLine */
// 

#pragma mark FindNextWord


const unsigned char kWordWrapTable[] =
	{
		0x84, 0x85, 0x80, 0x00, 0x83, 0x83,
		0x00, 0x02, 0x80, 0x00, 0x00, 0x00,
		0x00, 0x86, 0x00, 0x00, 0x83, 0x00,
		0x00, 0x86, 0x00, 0x00, 0x00, 0x00,
		0x84, 0x85, 0x80, 0x00, 0x83, 0x83,
		0x84, 0x86, 0x00, 0x00, 0x83, 0x83
	};

int FindNextWord(const CLanguageProxy& proxy)
{
	int mark = 0, i = 0;
	int unicode, state, len;

	state = 1;

	while (state > 0 && i < proxy.Size())
	{
		proxy.CharInfo(proxy.Text() + i, unicode, len);

		int cl = 0;

		if (unicode == '\n')
			cl = 3;
		else if (proxy.isspace_uc(unicode))
			cl = 2;
		else if (proxy.isalnum_uc(unicode))
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
//				case '/':
//				case '<':
				case '=':
//				case '>':
				case '\\':
				case '^':
				case '|':
					cl = 5;
					break;
				default:
					cl = 4;
			}

		unsigned char t = kWordWrapTable[(state - 1) * 6 + cl];

		state = t & 0x7f;

		if (t & 0x80)
			mark = i + len - 1;

		i += len;
	}

	return mark;
} /* FindNextWord */


#pragma mark Balance


Language get_language_for_offset(const CLanguageProxy& proxy, int inOffset, int& outLangStart)
{
	const char *text = proxy.Text();
	int size = proxy.Size(), forceState = START;

	if (inOffset > size)
		inOffset = size;

	char c = 0;
	int i = 0, bo = 0, s = 0;
	bool leave = false, esc = false, script = false, percent = false;
	int state = 0;

	while (i < inOffset)
	{
		c = get_char(i, size, text);

		switch (state)
		{
			case START:
				if (c == '<')
					state = TAG_START;
				else if (c == '&')
					state = SPECIAL;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != START) && s < i)
					s = i - 1;
				break;

			case TAG_START:
				if (c == '/')
					state = TAG_KEYWORD_END;
				else if (c == '!')
				{
					state = COMMENT_DTD;
					forceState = START;
				}
				else if (c == '?' || c == '%')
				{
					s = i;
					outLangStart = s;

					if (strncasecmp(text + s, "php", 3) == 0)
						s = (i += 3);

					state = PHP_SCRIPT;
					percent = (c == '%');
				}
				else if (isalpha(c))
				{
					s = i - 1;
					state = TAG_KEYWORD_START;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				else if (!isspace(c))
				{
					--i;
					state = TAG;
				}
				break;

			case TAG_KEYWORD_END:
				if (isalpha(c))
				{
					s = i - 1;
					state = TAG_KEYWORD_START;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				else if (!isspace(c))
				{
					--i;
					state = TAG;
				}
				break;

			case TAG_KEYWORD_START:
				if (!isalnum(c) && c != '-')
				{
					if (strncasecmp(text + s, "script", 6) == 0 && text[s - 1] != '/')
						script = true;

					state = TAG;
					s = --i;
				}
				break;

			case TAG:
				switch (c)
				{
					case 0:
					case '\n':
						leave = true;
						break;
					case '>':
						s = i;
						if (script)
						{
							outLangStart = s;
							state = JAVASCRIPT;
						}
						else
							state = START;
						break;
					case '"':
						s = i - 1;
						state = TAG_STRING_DOUBLE_QUOTES;
						break;
					case '\'':
						s = i - 1;
						state = TAG_STRING_SINGLE_QUOTES;
						break;
					case '!':
						if (i == s + 2)
						{
							state = COMMENT_DTD;
							forceState = START;
						}
						break;
					default:
						if (isalpha(c))
						{
							s = i - 1;
							state = TAG_ATTRIBUTE;
						}
						break;
				}
				break;

			case TAG_STRING_DOUBLE_QUOTES:
				if (c == '"')
				{
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
					leave = true;
				break;

			case TAG_STRING_SINGLE_QUOTES:
				if (c == '\'')
				{
					s = i;
					state = TAG;
				}
				else if (c == '\n' || c == 0)
					leave = true;
				break;

			case TAG_ATTRIBUTE:
				if (!isalnum(c) && c != '-')
				{
					s = --i;
					state = TAG;
				}
				break;

			case SPECIAL:
				if (c == 0 || c == '\n')
				{
					state = START;
					leave = true;
				}
				else if (c == ';')
				{
					s = i;
					state = START;
				}
				else if (isspace(c))
					state = START;
				break;

			case COMMENT_DTD:
				if (c == '-' && text[i] == '-' && i == s + 3 && text[i - 2] == '!' && text[i - 3] == '<')
				{
					s = i - 1;
					forceState = START;
					state = COMMENT;
				}
				else if (c == '>')
				{
					s = i;
					state = forceState;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				break;

			case COMMENT:
				if (c == ':')
				{
					if (strncasecmp(text, "JavaScript", 10) == 0)
						forceState = JAVASCRIPT;
					else if (strncasecmp(text, "PHP", 3) == 0)
						forceState = PHP_SCRIPT;
				}
				else if (c == '-' && text[i] == '-')
				{
					s = ++i;
					state = COMMENT_DTD;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				break;

			case JAVASCRIPT:
				script = false;

				if (c == '<' && strncasecmp(text + i, "!--", 3) == 0)
				{
					i += 3;
					s = i;
				}
				else if (c == '<' && text[i] == '/')
				{
					if (strncasecmp(text + i + 1, "script", 6) == 0)
					{
						i--;
						state = TAG_START;
						outLangStart = i - 1;
					}
				}
				else if (c == '/' && text[i] == '*')
					state = JS_COMMENT_MULTI_LINE;
				else if (c == '/' && text[i] == '/')
					state = JS_COMMENT_SINGLE_LINE;
				else if (isalpha(c))
					state = JS_IDENTIFIER;
				else if (c == '\'')
					state = JS_STRING_SINGLE_QUOTES;
				else if (c == '"')
					state = JS_STRING_DOUBLE_QUOTES;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != JAVASCRIPT) && s < i)
					s = i - 1;
				break;

			case JS_COMMENT_MULTI_LINE:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					s = i + 1;
					state = JAVASCRIPT;
				}
				else if (c == 0 || c == '\n')
					leave = true;
				break;

			case JS_COMMENT_SINGLE_LINE:
				if (c == '-' && text[i] == '-')
				{
					s = i + 1;
					state = TAG;
					outLangStart = s;
				}
				else if (c == '\n' || c == 0)
				{
					state = JAVASCRIPT;
					leave = true;
				}
				break;

			case JS_IDENTIFIER:
				if (!isalnum(c) && c != '_')
				{
					s = --i;
					state = JAVASCRIPT;
				}
				break;

			case JS_STRING_SINGLE_QUOTES:
			case JS_STRING_DOUBLE_QUOTES:
				if (!esc &&
					((state == JS_STRING_SINGLE_QUOTES && c == '\'') ||
					(state == JS_STRING_DOUBLE_QUOTES && c == '"')))
				{
					s = i;
					state = JAVASCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					if (!(text[i - 2] == '\\' && text[i - 3] != '\\'))
						state = JAVASCRIPT;

					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_SCRIPT:
				if (((c == '?' && !percent) || (c == '%' && percent)) &&
					text[i] == '>')
				{
					s = ++i;
					state = START;
					outLangStart = s;
				}
				else if (isalpha(c))
				{
					state = PHP_IDENTIFIER;
				}
				else if (c == '"')
					state = PHP_STRING_DOUBLE_QUOTES;
				else if (c == '\'')
					state = PHP_STRING_SINGLE_QUOTES;
				else if (c == '#' || (c == '/' && text[i] == '/'))
					state = PHP_COMMENT_SINGLE_LINE;
				else if (c == '/' && text[i] == '*')
					state = PHP_COMMENT_MULTI_LINE;
				else if (c == 0 || c == '\n')
					leave = true;

				if ((leave || state != START) && s < i)
					s = i - 1;
				break;

			case PHP_IDENTIFIER:
				if (!isalnum(c) && c != '_')
				{
					s = --i;
					state = PHP_SCRIPT;
				}
				break;

			case PHP_STRING_DOUBLE_QUOTES:
				if (!esc && c == '"')
				{
					s = i;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_STRING_SINGLE_QUOTES:
				if (!esc && c == '\'')
				{
					s = i;
					state = PHP_SCRIPT;
				}
				else if (c == '\n' || c == 0)
				{
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;

			case PHP_COMMENT_MULTI_LINE:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					s = i + 1;
					state = PHP_SCRIPT;
				}
				else if (c == 0 || c == '\n')
				{
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[i] == '>')
				{
					s = i - 1;
					s = ++i;
					state = START;
					outLangStart = s;
				}
				break;

			case PHP_COMMENT_SINGLE_LINE:
				if (c == 0 || c == '\n')
				{
					state = PHP_SCRIPT;
					leave = true;
				}
				else if (((percent && c == '%') || (!percent && c == '?')) &&
					text[i] == '>')
				{
					s = i - 1;
					s = ++i;
					state = START;
					outLangStart = s;
				}
				break;

			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}

		if (leave)
		{
			while (c != '\n' && c != 0 && i < inOffset)
				c = get_char(i, size, text);

			s = i;
			leave = false;
		}
	}

	if (state >= JAVASCRIPT && state <= JS_STRING_DOUBLE_QUOTES)
		return kLanguageJs;
	else if (state >= PHP_SCRIPT && state <= PHP_STRING_SINGLE_QUOTES)
		return kLanguagePhp;
	else
		return kLanguageHtml;
} /* get_language_for_offset */

const char *skip_in(const char *txt)
{
	if (*txt == '!' && txt[1] == '-' && txt[2] == '-')
	{
		txt += 3;

		while (*txt && ! (txt[0] == '-' && txt[1] != '-'))
			txt++;
	}

	while (*txt && *txt != '>')
		txt++;

	return txt;
} // skip_in

const char *skip_out(const char *txt)
{
	while (*txt && *txt != '<')
		txt++;

	return txt;
} // skip_out

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
					while (*txt && !(*txt == '*' && txt[1] == '/'))
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

_EXPORT bool Balance(CLanguageProxy& proxy, int& start, int& end)
{
	int langStart;
	Language lang = get_language_for_offset(proxy, start, langStart);
	const char *txt = proxy.Text();
	int size = proxy.Size();

	if (lang == kLanguageHtml)
	{
		bool in = false;

		if (start < 0 || start > end || end > size)
			return false;

		const char *st = txt + start;

		while (txt < st)
		{
			const char *t = in ? skip_in(txt + 1) : skip_out(txt + 1);
			in = !in;

			if (*t && st < t)
			{
				start = txt - proxy.Text() + 1;
				end = t - proxy.Text();
				return true;
			}

			txt = t;
		}
	}
	else
	{
		if (start < 0 || start > end || end > size)
			return false;

		const char *et(txt + end);
		const char *st(txt + langStart);

		stack<int> bls, sbls, pls;

		while (*txt && txt < et)
		{
			switch (*txt)
			{
				case '{':	bls.push(txt - st);				break;
				case '[':	sbls.push(txt - st);			break;
				case '(':	pls.push(txt - st);				break;
				case '}':	if (!bls.empty()) bls.pop();	break;
				case ']':	if (!sbls.empty()) sbls.pop();	break;
				case ')':	if (!pls.empty()) pls.pop();	break;
			}
			txt = skip(txt + 1);
		}

		char ec = 0, oc = 0;
		stack<int> *s = 0;

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
						start = s->top() + 1 + langStart;
						end = txt - st + langStart;
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
	}

	return false;
} /* Balance */
