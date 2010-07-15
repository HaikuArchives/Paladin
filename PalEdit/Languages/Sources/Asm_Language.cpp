// TODO: find a complete listing of x86 instructions and add them to the
//       keywords file (it currently lacks of many SSE and 3DNow! instructions)

// TODO: there are too many flavours of asm. Try to find a minimum common
//       denominator? (#include's, #define's, //-style comments, etc)

#include <stack>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

extern "C" {
_EXPORT const char kLanguageName[] = "ASM-x86";
_EXPORT const char kLanguageExtensions[] = "as;asm;ASM;nas;s;S";
_EXPORT const char kLanguageCommentStart[] = ";";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.asm";
_EXPORT const int16 kInterfaceVersion = 2;
}

enum {
	START, IDENT, COMMENT, LCOMMENT, STRING, NUMERIC, OPERATOR, SYMBOL, LEAVE
};

#define GETCHAR	(c = (i++ < size) ? text[i - 1] : 0)

bool isOperator(char c)
{
	if (c == '@' || c == '%' || c == '+' || c == '-' || c == '*')
		return true;

	return false;
}

bool isSymbol(char c)
{
	if (c == '(' || c == ')' || c == '[' || c == ']' || c == ',' || c == '$')
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
	int i = 0, s = 0, kws = 0;
	char c;
	bool leave = false;
	bool floating_point = false;
	bool hex_num = false;

	if (state == COMMENT || state == LCOMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);

	if (size <= 0)
		return;

	while (!leave)
	{
		GETCHAR;

		switch (state)
		{
			case START:
				if (isalpha(c) || c == '_')
				{
					kws = proxy.Move(tolower(c), 1);
					state = IDENT;
				}
				else if (c == '/' && text[i] == '*')
				{
					i++;
					state = COMMENT;
				}
				else if (c == ';' || c == '#')
				{
					i++;
					state = LCOMMENT;
				}
				else if (c == '"')
					state = STRING;
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
				else if (c == '\n' || c == 0)
					leave = true;

				if (leave || (state != START && s < i))
				{
					proxy.SetColor(s, kColorText);
					s = i - 1;
				}
			break;

			case COMMENT:
				if ((s == 0 || i > s + 1) && c == '*' && text[i] == '/')
				{
					proxy.SetColor(s - 1, kColorComment1);
					s = i + 1;
					state = START;
				}
				else if (c == 0 || c == '\n')
				{
					proxy.SetColor(s - 1, kColorComment1);
					leave = true;
				}
			break;

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

			case STRING:
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

			case NUMERIC:
			{
				proxy.SetColor(s, kColorNumber1);
				if (isNumeric(text[i - 1]) || (hex_num && isHexNum(text[i - 1])))
					;
				else
					if (text[i - 1] == '.' && floating_point == false)
						floating_point = true;
					else if (text[i - 1] == 'x' && hex_num == false && floating_point == false)
						hex_num = true;
					else
					{
						s = i - 1;
						i--;
						hex_num = false;
						state = START;
					}
			}
			break;

			case OPERATOR:
			{
				proxy.SetColor(s, kColorOperator1);
				if (isOperator(text[i - 1]))
					;
				else
				{
					s = i - 1;
					i--;
					state = START;
				}
			}
			break;

			case SYMBOL:
			{
				proxy.SetColor(s, kColorSeparator1);
				if (isSymbol(text[i - 1]))
					;
				else
				{
					s = i - 1;
					i--;
					state = START;
				}
			}
			break;

			default:
				leave = true;
			break;
		}
	}
}
