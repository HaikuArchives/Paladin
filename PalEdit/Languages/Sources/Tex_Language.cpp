/*	$Id: Tex_Language.cpp,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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

_EXPORT const char kLanguageName[] = "TeX";
_EXPORT const char kLanguageExtensions[] = "tex";
_EXPORT const char kLanguageCommentStart[] = "%";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.tex";
_EXPORT const int16 kInterfaceVersion = 2;

enum {
	START, COMMAND1, COMMAND2, CONSTCHAR, 
		LCOMMENT, MATH, WORD,
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws = 0, esc = 0;
	char c;
	bool leave = false;

	if (state == LCOMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);
	
	if (size <= 0)
		return;
	
	while (!leave)
	{
		GETCHAR;
		if (c == '`')
		{
			GETCHAR;
			c = ' ';
		}
		
		switch (state)
		{
			case START:
				if (c == '\\')
					state = COMMAND1;
				else if (c == '%')
					state = LCOMMENT;
				else if (c == '$')
					state = MATH;
				else if (c == '\n' || c == 0)
					leave = true;
				else if (isalnum(c))
					state = WORD;
                /* This is a bit strange to have some of the characters covered above, but who cares?*/
				else if (c == '#' || c == '&'
						|| c == '~' || c == '_' || c == '^'
						|| c == '{' || c == '}'
						|| c == '[' || c == ']' 
						)
					state = CONSTCHAR;
					
				if ((leave || state != START) && s < i)
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;

			case CONSTCHAR:			
				proxy.SetColor(s, kColorCharConst);
				s = --i;
				state = START;
				break;

			case WORD:
				if (!isalnum(c))
				{
					proxy.SetColor(s, kColorText);
					s = --i;
					state = START;
				}
				break;
			
			case LCOMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
				break;
			
			case COMMAND1:
                                     if (c=='('){
                                              state = MATH;
                                     } else if ((isalnum(c)) || (c=='@'))   /* a generic command has been found. */
			    /* Note that commands with "@" in their name only appear in .cls or .sty files */
                                     {
					kws = proxy.Move(c, 1);
					state = COMMAND2;
				}
				else   /* we are escaping a special text character such as \# or \$*/
				{
					proxy.SetColor(s, kColorText);
					s = i;
					state = START;
				}
				break;
			
			case COMMAND2:    /* Inside the name of a generic command */
				if (!(isalnum(c)) && !(c=='@')) /* found end of command name */
				{      /* now check the command name against a keyword list */
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0)
					{
						switch (kwc)
						{    /* use a specific keyword category color */
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
//							default:	ASSERT(false);
						}
					}
					else      /* use a generic keyword color */
					{
						proxy.SetColor(s, kColorKeyword1);
					}
					
					s = --i;
					state = START;
				}
				else if (kws) /* still in command name */
					kws = proxy.Move((int)(unsigned char)c, kws);
				break;
			
			
			case MATH:
                                     /* both "$" and "\)" will end math mode */
				if ( (c == '$' && !esc) || (c == ')' && esc) )
				{
					proxy.SetColor(s, kColorString1);
					s = i;
					state = START;
				} 
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorString1);
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

_EXPORT int FindNextWord(const CLanguageProxy& proxy)
{
	int mark = 0, i = 0;
	int unicode, state, len;
	const char *txt = proxy.Text();
	
	state = 1;
	
	while (state && i < proxy.Size())
	{
		proxy.CharInfo(txt + i, unicode, len);
		
		if (proxy.isspace_uc(unicode))
		{	
			state = 0;
		}
		else
		{
			mark = i + len - 1;
		}
		
		i += len;
	}
	
	return mark;
} /* FindNextWord */
