//
// A Pe Language add-on for the Lout Document Formatting System
// ftp://ftp.it.usyd.edu.au/jeff/lout/
// http://lout.sourceforge.net/
//
// (Lout is to TeX/LaTeX like BeOS/Haiku is to Windows/Linux)
//
// Written in 2006 by Oscar Lesta.
//

/*
  Lout's Syntax 101:

  - There are only line comments, they start with # and end with the next \n

  - There is a series of predefined symbols and operators.
    (those listed in Keywords.lt, under the "-Pe-Keywords-Language-" category.
    Here we'll assign kColorKeyword1 to those.

  - Beyond that, anything that matches the RegEx: @[_A-Za-z0-9]+
    (ie.: @MyNewSymbol) is a valid user (or package) defined symbol.

    We'll assign kColorKeyword2 to those, unless the keyword is found in
    one of the "-Pe-Keywords-UserX-" categories.

    Almost all of the symbols defined in the packages provided with lout 3.31
    are already listed there.

  - There is a set of concatenation symbols (here we blindly assign
    kColorOperator1 to those).

  - After that, it comes the part we don't even attempt to support because it
    would require way too much work to properly colouring them.
    Among other things:

    - context sensitive keywords not starting with an @ (parameters).
      "def", "line", "left" and "right" should be coloured or not?.
    - measurement units: [+,-]?[0-9].?[0-9]?[c,i,p,m,f,s,v,w,b,r,d,y,z]?[e,h,o,x,k,t]?
    - named @Char's: see Lout's docs for a full list.
    - named colors: see Lout's docs for a full list.

    - ToDo: refine the string handling, particulary:
      - `singly qouted' (example: `I can't parse this right')
      - ``double qouted'' (example: ``Neither I can parse this one'')
*/

#include "CLanguageAddOn.h"
#include "HColorUtils.h"

_EXPORT const char kLanguageName[] = "Lout";
_EXPORT const char kLanguageExtensions[] = "lt;ld";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.lt";
_EXPORT const int16 kInterfaceVersion = 2;


static inline bool
isOperator(char c)
{
	switch (c) {
		case '/': 
			return true;	// Can be: /, // or ^/
		case '|': 
			return true;	// Can be: |, || or ^|
		case '^': 
			return true;
		case '&': 
			return true;	// Can be: &, && or &&&
		case '{': 
			return true;
		case '}': 
			return true;
		default: 
			return false;
	}
}


enum {
	START,
	STRING,
	COMMAND,
	COMMENT,
	OPERATOR,
};


_EXPORT void
ColorLine(CLanguageProxy& proxy, int& state)
{
	int size = proxy.Size();

	if (size <= 0)
		return;

	if (state == COMMENT)
		proxy.SetColor(0, kColorComment1);
	else
		proxy.SetColor(0, kColorText);

	const char* text = proxy.Text();

	int i = 0, s = 0, kws = 0, esc = 0;
	char c;

	bool leave = false;

	while (!leave)
	{
		c = (i++ < size) ? text[i - 1] : 0;

		switch (state)
		{
			case START:
				s = i - 1;
				proxy.SetColor(s, kColorText);

				if (c == '@')	{
					state = COMMAND;
					kws = proxy.Move(c, 1);
				}
				else if (c == '#')
					state = COMMENT;
				else if (isOperator(c))
					state = OPERATOR;
				else if (c == '"')
					state = STRING;
				else if (c == '\n' || c == 0)
					leave = true;
			break;

			case COMMENT:
				proxy.SetColor(s, kColorComment1);
				leave = true;
				if (text[size - 1] == '\n')
					state = START;
			break;

			case COMMAND:
				if (!isalnum(c) && c != '_') {
					int kwc;

					if (i > s + 1 && (kwc = proxy.IsKeyword(kws)) != 0) {
						switch (kwc) {
							case 1:	proxy.SetColor(s, kColorKeyword1); break;
							case 2:	proxy.SetColor(s, kColorUserSet1); break;
							case 3:	proxy.SetColor(s, kColorUserSet2); break;
							case 4:	proxy.SetColor(s, kColorUserSet3); break;
							case 5:	proxy.SetColor(s, kColorUserSet4); break;
						}
					}
					else
						// use kColorIdentifierUser instead?
						proxy.SetColor(s, kColorKeyword2);

					i--;
					state = START;
				}
				else if (kws)
					kws = proxy.Move(c, kws);
			break;

			case OPERATOR:
				proxy.SetColor(s, kColorOperator1);
				if (!isOperator(text[i - 1])) {
					i--;
					state = START;
				}
			break;

			case STRING:
				if (c == '"' && !esc) {
					proxy.SetColor(s, kColorString1);
					state = START;
				} else if (c == '\n' || c == 0) {
					if (text[i - 2] == '\\' && text[i - 3] != '\\') {
						proxy.SetColor(s, kColorString1);
					} else {
						proxy.SetColor(s, kColorText);
						state = START;
					}
					leave = true;
				}
				else
					esc = !esc && (c == '\\');
			break;

			default:
				leave = true;
			break;
		}
	}
}
