/*	$Id: Perl_Language.cpp,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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

_EXPORT const char kLanguageName[] = "Perl";
_EXPORT const char kLanguageExtensions[] = "pl";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.pl";
_EXPORT const int16 kInterfaceVersion = 2;

enum {
	START, IDENT, OTHER, COMMENT, LCOMMENT, STRING1, STRING2,
	LEAVE, REGEX1, REGEX2, SCOPE, VAR, SUB1, SUB2
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws = 0, esc = 0;
	char c, mc = 0;
	bool leave = false;
	
	if (state == COMMENT || state == LCOMMENT)
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
				if (c == '#' && text[i - 2] != '$')
					state = LCOMMENT;
				else if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(c, 1);
					state = IDENT;
				}
				else if (c == '"')
					state = STRING1;
				else if (c == '\'')
					state = STRING2;
				else if (c == '&')
					state = SCOPE;
				else if (c == '$')
				{
					if (isalpha(text[i]) || text[i] == '#')
					{
						i++;
						state = VAR;
					}
					else
					{
						i++;
						proxy.SetColor(s, kColorText);
						s = i;
					}
				}
				else if (c == '@' || c == '*')
					state = VAR;
				else if (c == '/')
				{
					mc = '/';
					state = REGEX1;
				}
				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;
			
			case LCOMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
				break;
			
			case IDENT:
				if (!isalnum(c) && c != '_')
				{
					int kwc;

					if (i >= s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
//							default:ASSERT(false);
						}
					}
					else
					{
						proxy.SetColor(s, kColorText);
					}
					
					if (i == s + 2)
					{
						switch (*(text + s))
						{
							case 'm': state = REGEX1; break;
							case 's': state = REGEX2; break;
							case 'q': state = REGEX1; break;
							case 'y': state = REGEX2; break;
							default:  state = START; break;
						}
					}
					else if (i == s + 3)
					{
						if (strncmp(text + s, "qq", 2) == 0)
							state = REGEX1;
						else if (strncmp(text + s, "qx", 2) == 0)
							state = REGEX1;
						else if (strncmp(text + s, "qw", 2) == 0)
							state = REGEX1;
						else if (strncmp(text + s, "tr", 2) == 0)
							state = REGEX2;
						else
							state = START;
					}
					else if (i == s + 4 && strncmp(text + s, "sub", 3) == 0)// && ci < 2)
						state = SUB1;
					else
						state = START;
					
					if (state == START || state == SUB1)
						s = --i;
					else
					{
						switch (c)
						{
							case '(':	mc = ')'; break;
							case '{':	mc = '}'; break;
							case '[':	mc = ']'; break;
							case '<':	mc = '>'; break;
							case ' ':	state = START; s = --i; break;
							default:	mc = c; break;
						}
						
						if (state != START)
							s = i - 1;
					}
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;
			
			case SUB1:
				if (isalpha(c))
					state = SUB2;
				else if (!isspace(c))
				{
					state = START;
					s = --i;
				}
				break;
			
			case SUB2:
				if (!isalnum(c) && c != '\'' && c != '_')
				{
					proxy.SetColor(s, kColorText);
					state = START;
					s = --i;
				}
				break;
			
			case STRING1:
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
			
			case STRING2:
				if (c == '\'' && !esc)
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
			
			case REGEX1:
				if (c == 0)	// don't like this
				{
					proxy.SetColor(s, kColorText);
					s = i;
					state = START;
				}
				else if (c == mc && !esc)
				{
					proxy.SetColor(s, kColorCharConst);
					s = i;
					state = START;
				}
				else
					esc = !esc && (c == '\\');
				break;
			
			case REGEX2:
				if (c == 0)	// don't like this
				{
					proxy.SetColor(s, kColorText);
					s = i;
					state = START;
				}
				else if (c == mc && !esc)
				{
					if (mc == ')' || mc == '}' || mc == ']' || mc == '>')
					{
						switch (text[i])
						{
							case '(':	mc = ')'; break;
							case '{':	mc = '}'; break;
							case '[':	mc = ']'; break;
							case '<':	mc = '>'; break;
							case ' ':	state = START; s = --i; break;
							default:	mc = text[i]; break;
						}
						i++;
					}

					if (state != START)
						state = REGEX1;
				}
				else
					esc = !esc && (c == '\\');
				break;
			
			case SCOPE:
				if (c == '\'' || (!isalnum(c) && c != '_'))
				{
					proxy.SetColor(s, kColorText);
					state = START;
				}
				break;
			
			case VAR:
				if (!isalnum(c) && c != '_')
				{
					proxy.SetColor(s, kColorText);
					state = START;
				}
				break;
			
			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* CperlIntf::ColorLine */
