/*	$Id: Python_Language.cpp,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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

_EXPORT const char kLanguageName[] = "Python";
_EXPORT const char kLanguageExtensions[] = "py";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.py";
_EXPORT const int16 kInterfaceVersion = 2;

enum {
	START, IDENT, OTHER, COMMENT, LCOMMENT, STRING1, STRING2, STRING3,
	CHAR_CONST, LEAVE, PRAGMA1, PRAGMA2, PRAGMA3, NUMERIC, OPERATOR, SYMBOL
};

#define GETCHAR			(c = (i++ < size) ? text[i - 1] : 0)

bool isOperator(char c)
{
	if (c == '+' || c=='-' || c=='*' || c=='/' || c=='%' || c=='=' || c=='>' || c=='<' || c=='&' || c=='|' || c=='!' || c=='.' || c==':' )
		return true;
			
	return false;
}

bool isSymbol(char c)
{
	if (c=='{' || c=='}' || c=='(' || c==')' || c=='[' || c==']' || c==',' ||  c==';')
		return true;
	
	return false;
}

bool isNumeric(char c)
{
	if (c>='0' && c<='9')
		return true;

	return false;
}

_EXPORT void ColorLine(CLanguageProxy& proxy, int& state)
{
	const char *text = proxy.Text();
	int size = proxy.Size();
	int i = 0, s = 0, kws = 0, esc = 0;
	char c;
	bool leave = false;
	
	// floating point flag, true when the NUMERIC: label finds a . inside a number, and checks to make sure that a number with two '.' is invalid. (and not highlighted as numeric)
	bool floating_point = false;
	// same flag, only for hex numbers. allows proper highlighting only for 1 x per number. (0x21 is ok. 0x023x31 is not. will look wierd.)
	bool hex_num = false;

	
	switch (state)
	{
		case COMMENT:  proxy.SetColor(0, kColorComment1); break;
		case LCOMMENT: proxy.SetColor(0, kColorComment1); break;
		case STRING3:  proxy.SetColor(0, kColorString1);  break;
		default:       proxy.SetColor(0, kColorText);     break;
	}
	
	if (size <= 0)
		return;
	
	while (!leave)
	{
		GETCHAR;
		
		switch (state) {
			case START:
				if (c == '#')
					state = LCOMMENT;
				else if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(c, 1);
					state = IDENT;
				}
				else if (c == '"')
				{
					if (text[i] == '"' && text[i + 1] == '"')
						state = COMMENT;
					else
						state = STRING1;
				}
				else if (c == '\'')
				{
					if (text[i] == '\'' && text[i + 1] == '\'')
						state = STRING3;
					else
						state = STRING2;
				}
				// m7m: pe-on-steroids highlighting plugin starts
				else if (isNumeric(c))
				{
					state = NUMERIC;
				}
				else if (isOperator(c))
				{
					state = OPERATOR;
				}
				else if (isSymbol(c))
				{
					state = SYMBOL;
				}
				// steroids end.
				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
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
//							default:ASSERT(false);
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
			
			case LCOMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
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
			
			case STRING3:
				if (c == '\'' && text[i] == '\'' && text[i + 1] == '\'' && !esc)
				{
					proxy.SetColor(s, kColorString1);
					s = i + 2;
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
			
			case COMMENT:
				if (c == '"' && text[i] == '"' && text[i + 1] == '"')
				{
					proxy.SetColor(s, kColorComment1);
					s = i + 2;
					state = START;
				}
				else if (c == '\n' || c == 0)
				{
					proxy.SetColor(s, kColorComment1);
					leave = true;
				}
				break;
				
			case NUMERIC:
			{
				proxy.SetColor(s, kColorNumber1);
				if (isNumeric(text[i-1]))
					;
				else
					if (text[i-1]=='.' && floating_point==false && hex_num==false)
						floating_point = true;
					else if (text[i-1]=='x' && hex_num==false && floating_point==false)
						hex_num = true;
					else
					{
						s=i-1;
						i--;
						state = START;
					}
			}
			break;
			
			case OPERATOR:
			{
				proxy.SetColor(s, kColorOperator1);
				if (isOperator(text[i-1]))
					;
				else
				{
					s=i-1;
					i--;
					state = START;
				}
			}
			break;
			
			case SYMBOL:
			{
				proxy.SetColor(s, kColorSeparator1);
				if (isSymbol(text[i-1]))
					;
				else
				{
					s=i-1;
					i--;
					state = START;
				}
			}
			break;			
			
			default:	// error condition, gracefully leave the loop
				leave = true;
				break;
		}
	}
} /* ColorLine */
