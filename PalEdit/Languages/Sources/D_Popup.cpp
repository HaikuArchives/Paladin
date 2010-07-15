/*	based on CCppPopup.cpp, which is

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


	This is basically a copycat version of CCppPopup.cpp, with several things
	removed/simplified, as D's syntax is somehow simpler than C++.
	Hacked up in 2006 by:
	- Oscar Lesta.
*/

// ToDos:
// - more clean up of C++'s peculiarities is needed.
// - imports (included files) are shown, but you can't open them.
// - "Show Types" (specially for classes) and "Prototypes" need a lot of work.

#include "CLanguageAddOn.h"

const int kMaxNameSize = 256;

const char* comment(const char text[], bool strippp = true, CLanguageProxy* proxy = NULL);
const char* parens(const char text[], char open);
const char* skip(const char text[], char ch);
const char* skip_ne(const char text[], char ch);
const char* preprocessor(const char text[], CLanguageProxy& proxy);
const char* params(const char text[], char* &name, int& size);
const char* ident(const char text[], CLanguageProxy& proxy);
const char* i_extern(const char text[]);
void pragma(const char* &text, int offset, CLanguageProxy& proxy);


////////////////////////////////////////////////////////////////////////////////

inline bool isidentf(char c)
{
	// Could this be the first character of an identifier?
	return (isalpha(c) || ((c) == '_'));
}


inline bool isident(char c)
{
	// Could this be a subsequent character of an identifier?
	return (isalnum(c) || ((c) == '_'));
}


const char*
skip(const char text[], char ch)
{
	while (*text) {
		if (*text == ch) {
			text++;
			break;
		}
		if (*text == '\\' && text[1] != 0)
			text++;
		text++;
	}

	return text;
}


const char*
skip_ne(const char text[], char ch)
{
	while (*text) {
		if (*text == ch) {
			text++;
			break;
		}
		text++;
	}

	return text;
}


const char*
comment(const char text[], bool strippp, CLanguageProxy* proxy)
{
	do {
		while (isspace(*text))
			text++;

		if (text[0] == '/') {
			if (text[1] == '+') {
				text += 2;

				do {
					text = skip_ne(text, '+');
					if (*text == '/') {
						text++;
						break;
					}
				} while (*text != '\0');
			}
			else if (text[1] == '*') {
				text += 2;

				do {
					text = skip_ne(text, '*');
					if (*text == '/') {
						text++;
						break;
					}
				} while (*text != '\0');
			}
			else if (text[1] == '/') {
				text += 2;
				const char *commentEnd = skip(text, '\n');

				// if a proxy is given, we look for "#pragma mark"
				if (proxy) {
					int offset = text - proxy->Text();

					while (isspace(*text) && text < commentEnd)
						text++;

					if (*text == '#') {
						text++;

						while (isspace(*text) && text < commentEnd)
							text++;

						if (strncmp(text, "pragma", 6) == 0) {
							text += 6;
							pragma(text, offset, *proxy);
						}
					}
				}

				text = commentEnd;
			}
			else
				break;
		}
		else if (*text == '#' && text[1] != '#' && strippp)
			text = skip(text, '\n');
		else
			break;
	}
	while (*text);

	return text;
}


const char*
parens(const char text[], char open)
{
	int c;
	char close = 0;
	
	switch (open) {
		case '(':	close = ')'; break;
		case '{':	close = '}'; break;
		case '[':	close = ']'; break;
	}

	while (true) {
		text = comment(text);

		c = *text++;
		
		if (c == '\'') {
			text = skip(text, '\'');
			continue;
		}
		
		if (c == '"') {
			text = skip(text, '"');
			continue;
		}
		
		if (c == '#') {
			text = skip(text, '\n');
			continue;
		}
		
		if (c == open) {
			text = parens(text, open);
			continue;
		}
		
		if (c == close)
			return text;
		
		if (c == '\0')
			return text - 1;
	}
}


inline void
name_append(const char*& text, char*& name, int& size)
{
	if (size < kMaxNameSize - 1) {
		*name++ = *text++;
		size++;
	}
	else
		text++;
}


inline void
name_append(const char*& text, int textSize, char*& name, int& nameSize)
{
	while (textSize-- > 0) {
		name_append(text, name, nameSize);
	}
}


const char*
class_struct_union(const char *start, const char *&text,
	const char *nameBuf, char *name, int &size, int &offset,
	CLanguageProxy &proxy)
{
	*name++ = ' ';
	size++;

	char tempBuf[kMaxNameSize];
	int tempPos = 0;

	// always only get the last possible identifier - that will
	// filter out things like "class IMPEXP_BE BClass"
	while (isidentf(text[0])) {
		char *temp = tempBuf;
		tempPos = 0;

		while (isident(*text))
			name_append(text, temp, tempPos);

		text = comment(text);
	}

	if (tempPos != 0) {
		const char *temp = tempBuf;
		name_append(temp, tempPos, name, size);

		// may be a class member ( ("." <identifier>)* )
		while (*text == '.' && isidentf(text[1])) {
			name_append(text, name, size);
			name_append(text, name, size);

			text = comment(text);

			if (isidentf(*text))
				while (isident(*text))
					name_append(text, name, size);

			text = comment(text);
		}

		*name = 0;

		// [zooey]: ugly *HACK* to avoid something like
		//	     struct stat *get_stat() {
		// to be seen as struct-decl when in fact it's a function...
		const char *delimiterPos = strpbrk( text, ";{(");
		if (delimiterPos && *delimiterPos == '(') 
		{	// If '(' is found before ';' or '{', we handle this as
			// a function, not a type-declaration:
			return text;
		}

		while (*text != '{' && *text != ';') {
			// skip inheritance decls (like 'public x, private y'):
			if (!*text)
				return text;
			text = comment(text + 1);
		}

		if (*text == '{' && proxy.Types()) {
			char match[256];
			long l = std::min((long)255, text - start);
			
			strncpy(match, start, l);
			match[l] = 0;
	
			proxy.AddFunction(nameBuf, match, offset, false);
		}
	}

	if (*text == '{')
		text = parens(text + 1, '{');

	text = comment(text);

	while (isidentf(*text)) {
		char match[kMaxNameSize];

		name = strchr(nameBuf, ' ') + 1;

		strncpy(match, nameBuf, name - nameBuf - 1);
		match[name - nameBuf - 1] = 0;

		while (isident(*text))
			name_append(text, name, size);

		*name = 0;

		if (proxy.Types())
			proxy.AddFunction(nameBuf, match, offset, false);

		text = comment(text);
		while (*text == ',' || *text == '*')
			text = comment(text + 1);
	}

	return text;
}


// This kind of pragmas are not D's ones, they're just an extension for Pe
// to aid in code-browsing using the function's popup menu.
void
pragma(const char*& text, int offset, CLanguageProxy& proxy)
{
	while (isspace(*text))
		text++;

	if (strncmp(text, "mark", 4) == 0) {
		text += 4;

		while (isspace(*text))
			text++;
		
		char nameBuf[kMaxNameSize], *name = nameBuf;
		int size = 0;

		while (*text && *text != '\n')
			name_append(text, name, size);

		*name = 0;

		proxy.AddSeparator(nameBuf);
	}
}


const char*
preprocessor(const char text[], CLanguageProxy& proxy)
{
	char nameBuf[kMaxNameSize], *name = nameBuf;
	int size = 0, offset = text - proxy.Text();
	
	while (isspace(*text))
		text++;

	if (strncmp(text, "pragma", 6) == 0) {
		text += 6;
		pragma(text, offset, proxy);
	} else if (strncmp(text, "line", 4) == 0) {
		;
	}

	return skip(text, '\n');
}


const char*
i_extern(const char text[])
{
	if (*text == '(') {
		text = skip(text + 1, ')');
		text = comment(text);

		if (*text == '{')
			return text + 1;
	}

	return text;
}


/** Puts all function parameters in the input buffer (\a text) to the
 *	provided buffer \a name.
 */
const char*
params(const char text[], char*& name, int& size)
{
	int c;

	while (true)
	{
		text = comment(text);

		c = *text;

		if (c == '\'') {
			text = skip(text + 1, '\'');
			continue;
		}

		if (c == '"') {
			text = skip(text + 1, '"');
			continue;
		}

		if (c == '#') {
			text = skip(text + 1, '\n');
			continue;
		}

		if (c == '(') {
			text = parens(text + 1, '(');
			continue;
		}

		if (c == ')')
			return text + 1;

		if (c == '\0')
			return text;

		if (isidentf(c))
		{
			// fetch a single parameter
			int numParens = 0;
			while(*text != ')' && *text != ',' && *text != '=')
			{
				if (*text == '(') {
					const char* end = parens(text + 1, '(');
					name_append(text, end - text, name, size);
					numParens++;
				} 
				else
					name_append(text, name, size);
			}

			if (numParens != 0 && numParens != 2) {
				// this ain't a valid parameter
				// (returning this value is likely to let ident() fail...)
				// ToDo: we should be able to cleanly break out of a parameter scan!
				return text - 1;
			}

			// cut off trailing spaces from the name
			while(size > 0 && isspace(name[-1])) {
				name--;
				size--;
			}

			// eat a default value
			if (*text == '=') {
				while(*text != ')' && *text != ',') {
					if (*text == '(')
						text = parens(text+1, '(');
					else
						text++;
				}
				text = comment(text);
			}

			// If there is a parameter following, add ", " to the name
			if (*text == ',') {
				name_append(text, name, size);
				if (size < kMaxNameSize - 1) {
					*name++ = ' ';
					size++;
				}
			}
			continue;
		}
		text++;
	}
}


const char*
ident(const char text[], CLanguageProxy& proxy)
{
	const char *start = text, *id = start;
	char nameBuf[kMaxNameSize], *name = nameBuf;
	char paramBuf[kMaxNameSize*10], *param = paramBuf;
	int size = 0, paramSize = 0, offset = start - proxy.Text();

	while (isident(*text))
		name_append(text, name, size);

	*name = 0;

	text = comment(text);

	if (strncmp(nameBuf, "extern", 6) == 0) {
		return i_extern(text);
	}
	else if (strncmp(nameBuf, "import", 6) == 0)
	{
		*name++ = ' ';

		while (isspace(*text))
			text++;

		// repeat until end of import line.
		while (*text && *text != ';' && *text != '\n') {
			while (*text && *text != ',' && *text != ';' && *text != '\n')
				name_append(text, name, size);

			*name = 0;

			if (proxy.Includes())
				proxy.AddInclude(nameBuf, nameBuf, false);
		}

		return text;

	}
	else if (strncmp(nameBuf, "version", 7) == 0) {
		// version (BlaBla) { }
		*name++ = ' ';
		text = skip(text, '(');

		if (isidentf(*text)) {
			while (isident(*text))
				name_append(text, name, size);
			*name = 0;

			while (*text != '{' && *text != ';') {
				// skip comments:
				text = comment(text + 1);
			}

			if (*text == '{') {
				char match[256];
				long l = std::min((long) 255, text - start);

				strncpy(match, start, l);
				match[l] = 0;

				proxy.AddSeparator(nameBuf);
				proxy.IncNestLevel();
			}
		}
		return text + 1;
	}
	else if (strcmp(nameBuf, "class") == 0 ||
			strcmp(nameBuf, "struct") == 0 ||
			strcmp(nameBuf, "union") == 0)
	{
		return class_struct_union(start, text, nameBuf, name, size, offset, proxy);
	}

	while (*text == '.' && isidentf(text[1])) {
		name_append(text, name, size);
		name_append(text, name, size);

		text = comment(text);

		id = name;

		if (*text == '~') {
			name_append(text, name, size);
			text = comment(text);
		}

		if (isidentf(*text))
			while (isident(*text))
				name_append(text, name, size);

		text = comment(text);
	}

	*name = 0;

	if (*text == '(') {
		char match[256];
		long l = std::min((long) 255, text - start);

		strncpy(match, start, l);
		match[l] = 0;

		text = params(text + 1, param, paramSize);
		*param = 0;
		text = comment(text);

		if (*text == ':') {
			// This is obviously a constructor with member initialization
			while (*text != '{' && *text != ';')
				text++;
			if (*text == '{') {
				text = parens(text + 1, '{');
				proxy.AddFunction(nameBuf, match, offset, false, paramBuf);
			} else {
				text++;
				if (proxy.Prototypes())
					proxy.AddFunction(nameBuf, match, offset, true, paramBuf);
			}
			if (*text == '\n')
				text++;
			return text;
		}

		if (*text == ';') {
			if (proxy.Prototypes())
				proxy.AddFunction(nameBuf, match, offset, true, paramBuf);
			return text + 1;
		}

		if (isidentf(*text) || *text == '{') {
			proxy.AddFunction(nameBuf, match, offset, false, paramBuf);
			text = skip_ne(text, '{');
			text = parens(text, '{');
			if (*text == '\n')
				text ++;
			return text;
		}
	}

	if (*text == '=') {
		// eat away assignments that look like function declarations,
		// as for instance
		// 	const int kMyConstant = sizeof(int);
		// which clearly isn't a function declaration...
		text = comment(text+1);
		if (isidentf(*text))
			while (isident(*text))
				text++;
	}

	return text;
}


void
ScanForFunctions(CLanguageProxy& proxy)
{
	const char* text = proxy.Text();
	const char* max = text + proxy.Size();

	if (*max != 0)
		return;

	while (text < max) {
		text = comment(text, false, &proxy);

		switch (*text) {
			case 0:	return;

			case '\'':	text = skip(text + 1, '\'');			break;
			case '"':	text = skip(text + 1, '"');				break;
			case '(':
			case '{':
			case '[':	text = parens(text + 1, *text);			break;
			case '#':	text = preprocessor(text + 1, proxy);	break;

			case '}':
				text++;
				proxy.DecNestLevel();

			default:
				if (isidentf(*text))
					text = ident(text, proxy);
				else
					text++;
			break;
		}
	}
}
