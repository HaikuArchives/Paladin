/*	$Id: Sql_Language.cpp,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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
	Modified from CLang_cpp.cpp for SQL by Kelvin W Sherlock
*/

#include <stack>
#include <ctype.h>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

extern "C" {
_EXPORT const char kLanguageName[] = "SQL";
_EXPORT const char kLanguageExtensions[] = "sql";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.sql";
_EXPORT const int16 kInterfaceVersion = 2;
}

enum {
	START,
	COMMENT,
	STRING1,	// '...'
	STRING2,	// "...."
	IDENT
};

inline bool isident(char c)
{
	return (isalnum(c) || c == '_' || c == '.');	
}

inline bool isidentf(char c)
{
	return (isalpha(c));	
}

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws = 0;
	char c;
	bool leave = false;
	
	if (state == COMMENT)
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

				if (isidentf(c) )
				{
					kws = proxy.Move(tolower(c), 1);
					state = IDENT;
				}
				else if (c == '#')
				{
					state=COMMENT;	
				}
				// --.*\n is a comment, too
				else if (c == '-' && text[i] == '-')
				{
					i++;
					state = COMMENT;
				}
				else if (c == '\'')
					state = STRING1;
				else if (c == '"')
					state = STRING2;

				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
				break;
			
			// #.*$
			case COMMENT:
				 if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;	
					state = START;
				}
				break;

			case IDENT:
				if (!isident(c))
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
					kws = proxy.Move((int)(unsigned char)tolower(c), kws);
				break;
			
			
			// '....'
			case STRING1:
				if (c == '\'')
				{
					proxy.SetColor(s, kColorString1);
					s = i;
					state = START;			
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorText);
					state = START;
					s = size;
					leave = true;	
				}
				break;

			// '....'
			case STRING2:
				if (c == '"')
				{
					proxy.SetColor(s, kColorString1);
					s = i;
					state = START;			
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorText);
					state = START;
					s = size;
					leave = true;	
				}
				break;			
			
			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* ColorLine */
