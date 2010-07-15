// TODO: double square braquets strings (ie. [[sample]] )
//       they can spread across several lines, and contain nested strings.

#include <stack>
#include <ctype.h>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

extern "C" {
_EXPORT const char kLanguageName[] = "Lua";
_EXPORT const char kLanguageExtensions[] = "lua;Lua;LUA;decor"; // decor is for Dano's decors
_EXPORT const char kLanguageCommentStart[] = "--";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.lua";
_EXPORT const int16 kInterfaceVersion = 2;
}

enum {
	START, 
	IDENT, 
	STRING,  // "strings"
	STRING2, // 'strings'
//	STRING3, // [[strings]]
	LCOMMENT,
	NUMERIC,
	OPERATOR,
	SYMBOL,
};

#define GETCHAR (c = (i++ < size) ? text[i - 1] : 0)

bool isOperator(char c)
{
	if (c == '=' || c == '~' || c == '<' || c == '>' || c == '+' || c == '-' ||
		c == '/' || c == '.' || c == '{' || c == '}')
		return true;
			
	return false;
}

bool isSymbol(char c)
{
	if (c == '(' || c == ')' || c == '[' || c == ']' || c == '&' || 
		c == ',' || c == ';' || c == '$' || c == '#')
		return true;
	
	return false;
}

bool isNumeric(char c)
{
	if (c >= '0' && c <= '9')
		return true;

	return false;
}

bool isHexNum(char c)
{
	if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
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
	bool floating_point = false;
	bool hex_num = false;

//    int sqrBraquetStrCount = 0;

	if (state == LCOMMENT)
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
					kws = proxy.Move(tolower(c), 1);
					state = IDENT;
				}
				else if (c == '-' && text[i] == '-')
				{
					i++;
					state = LCOMMENT;
				}
				else if (c == '"')
					state = STRING;
				else if (c == '\'')
					state = STRING2;
/*
				else if (c == '[' && text[i] == '[')
				{
					sqrBraquetStrCount++;
					state = STRING3;
				}
*/
				else if (isNumeric(c) || (c == '$' && isHexNum(text[i])))
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
				else if (c == '\n' || c == 0)
					leave = true;
					
				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
			break;
			
			// -- format comments
			case LCOMMENT:
				proxy.SetColor(s - 1, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
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
						}
						s = --i;
						state = START;
					}
					else
					{
						proxy.SetColor(s, kColorText);
						s = --i;
						state = START;
					}
				}
				else if (kws)
					kws = proxy.Move((int)(unsigned char) c, kws);
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
/*
			case STRING3:

			break;
*/
			case NUMERIC:
				proxy.SetColor(s, kColorNumber1);
				if (isNumeric(text[i - 1]) || (hex_num && isHexNum(text[i - 1])))
					;
				else
					if (text[i - 1] == '.' && floating_point == false)
						floating_point = true;
					else if (isHexNum(text[i - 1]) && hex_num == false)
						hex_num = true;
					else
					{
						s = i - 1;
						i--;
						state = START;
					}
			break;
		
			case OPERATOR:
				proxy.SetColor(s, kColorOperator1);
				if (isOperator(text[i - 1]))
					;
				else
				{
					s = i - 1;
					i--;
					state = START;
				}
			break;
		
			case SYMBOL:
				proxy.SetColor(s, kColorSeparator1);
				if (isSymbol(text[i - 1]))
					;
				else
				{
					s = i - 1;
					i--;
					state = START;
				}
			break;			
			
			default:
				leave = true;
			break;
		}
	}
}
