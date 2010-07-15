/*	$Id: Oberon_Language.cpp,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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

#include "CLanguageAddOn.h"
#include "HColorUtils.h"

_EXPORT const char kLanguageName[] = "Oberon-2";
_EXPORT const char kLanguageExtensions[] = "ob2;Mod";
_EXPORT const char kLanguageCommentStart[] = "(*";
_EXPORT const char kLanguageCommentEnd[] = "*)";
_EXPORT const char kLanguageKeywordFile[] = "keywords.ob2";
_EXPORT const int16 kInterfaceVersion = 2;

_EXPORT bool Balance(CLanguageProxy& proxy, int& start, int& end)
{
	const char *txt = proxy.Text();
	int i = 1;
	int a, c;
	
	a = start;
	c = end;
	
	int lbl = 0, lpl = 0, lal = 0, rbl = 0, rpl = 0, ral = 0;
	int A = a - 1, C = c;
	
	if ((txt[A] == '(' && txt[C] == ')') ||
		(txt[A] == '[' && txt[C] == ']') ||
		(txt[A] == '{' && txt[C] == '}'))
	{
		A--;
		C++;
	}
	
	int aa = A, cc = C;

	while (A >= 0 && lbl >= 0 && lpl >= 0 && lal >= 0)
	{
		switch (txt[A--])
		{
			case '[':	lbl--; break;
			case '(':	lpl--; break;
			case '{':	lal--; break;
			case ']':	lbl++; break;
			case ')':	lpl++; break;
			case '}':	lal++; break;
		}
	}

	while (C < proxy.Size() - 1 && rbl >= 0 && rpl >= 0 && ral >= 0)
	{
		switch (txt[C++])
		{
			case '[':	rbl++; break;
			case '(':	rpl++; break;
			case '{':	ral++; break;
			case ']':	rbl--; break;
			case ')':	rpl--; break;
			case '}':	ral--; break;
		}
	}

	if (C >= proxy.Size() - 1 && A < 0)
		return false;

	if (C - c < a - A)
	{
		char c = txt[C - 1], m = 0;

		switch (c)
		{
			case ')': m = '('; break;
			case '}': m = '{'; break;
			case ']': m = '['; break;
		}

		int level = 1;
		A = aa;

		while (level && A >= 0)
		{
			if (txt[A] == c)
				level++;
			else if (txt[A] == m)
				level--;
			A--;
		}

		if (level == 0)
		{
			start = A + 2;
			end = C - 1;
			return true;
		}
		else
			return false;
	}
	else
	{
		char c = txt[A + 1], m = 0;

		switch (c)
		{
			case '(': m = ')'; break;
			case '{': m = '}'; break;
			case '[': m = ']'; break;
		}

		i = 0;
		int level = 1;
		C = cc;

		while (level && C < proxy.Size() - 1)
		{
			if (txt[C] == c)
				level++;
			else if (txt[C] == m)
				level--;
			C++;
		}

		if (level == 0)
		{
			start = A + 2;
			end = C - 1;
			return true;
		}
		else
			return false;
	}
} /* Balance */

enum {
	START, IDENT, OTHER, COMMENT, XCOMMENT, STRING,
	CHAR_CONST, LEAVE
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws = 0, esc = 0;
	char c;
	bool leave = false;
	
	if (state == COMMENT || state == XCOMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);
	
	if (size <= 0)
		return;
	
	while (!leave)
	{
		GETCHAR;
		
		switch (state) {
			case START:
				if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(c, 1);
					state = IDENT;
				}
				else if (c == '(' && text[i] == '*')
					state = COMMENT;
				else if (c == '<' && text[i] == '*')
					state = XCOMMENT;
				else if (c == '"')
					state = STRING;
				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;
			
			case COMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == ')')
				{
					proxy.SetColor(s, kColorComment1);
					s = i + 1;
					state = START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
				break;

			case XCOMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '>')
				{
					proxy.SetColor(s, kColorComment1);
					s = i + 1;
					state = START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
				break;

			case IDENT:
				if (!isalnum(c) && c != '_')
				{
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
//							default:	ASSERT(false);
						}
					}
					else
					{
						proxy.SetColor(s, kColorText);
					}
					
					s = --i;
					state = START;
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;
			
			case STRING:
				if (c == '"' && !esc)
				{
					proxy.SetColor(s, kColorString1);
					s = i;
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					if (text[i - 2] == '\\' && text[i - 3] != '\\')
					{
						proxy.SetColor(s, kColorString1);
					}
					else
					{
						proxy.SetColor(s, kColorText);
						state = START;
					}
					
					s = size;
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
				break;
			
			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* ColorLine */

