/*	$Id: Antlr_Popup.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 04/19/98 19:30:25
*/

#include "CLanguageAddOn.h"

const int kMaxNameSize = 256;

inline bool isidentf(char c)	
{
	return (isalpha(c) || ((c) == '_'));
}

inline bool isident(char c)	
{
	return (isalnum(c) || ((c) == '_'));
}

bool is_template(const char *text);
const char *comment(const char *text, bool strippp = true);
const char *parens(const char *text, char open);
const char *skip(const char *text, char ch);
const char *skip_ne(const char *text, char ch);
const char *preprocessor(const char *text, CLanguageProxy& proxy);
const char *ident(const char *text, CLanguageProxy& proxy, char *&scope);
const char *i_extern(const char *text);
void parse(const char *text);

const char *skip(const char *text, char ch)
{
	while (*text)
	{
		if (*text == ch)
		{
			text++;
			break;
		}
		if (*text == '\\' && text[1] != 0)
			text++;
		text++;
	}

	return text;
} /* skip */

const char *skip_ne(const char *text, char ch)
{
	while (*text)
	{
		if (*text == ch)
		{
			text++;
			break;
		}
		text++;
	}

	return text;
} /* skip_ne */

const char *comment(const char *text, bool strippp)
{
	do
	{
		while (isspace (*text))
			text++;
		
		if (text[0] == '/')
		{
			if (text[1] == '*')
			{
				text += 2;

				do
				{
					text = skip_ne(text, '*');
					if (*text == '/')
					{
						text++;
						break;
					}
				}
				while (*text != '\0');
			}
			else if (text[1] == '/')
			{
				text = skip(text, '\n');
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
} /* comment */

const char *parens(const char *text, char open)
{
	int c;
	char close = '\0';
	
	switch (open)
	{
		case '(':	close = ')'; break;
		case '{':	close = '}'; break;
		case '[':	close = ']'; break;
//		default:	ASSERT(false); return text;
	}

	while (true)
	{
		text = comment(text);

		c = *text++;
		
		if (c == '\'') 
		{
			text = skip(text, '\'');
			continue;
		}
		
		if (c == '"') 
		{
			text = skip(text, '"');
			continue;
		}
		
		if (c == '#') 
		{
			text = skip(text, '\n');
			continue;
		}
		
		if (c == open)
		{
			text = parens(text, open);
			continue;
		}
		
		if (c == close)
			return text;
		
		if (c == '\0')
			return text - 1;
	}
} /* parens */

inline void name_append(const char*& text, char*& name, int& size)
{
	if (size < kMaxNameSize - 1)
	{
		*name++ = *text++;
		size++;
	}
	else
		text++;
} /* name_append */

const char *preprocessor(const char *text, CLanguageProxy& proxy)
{
	char nameBuf[kMaxNameSize], *name = nameBuf;
	int size = 0, offset = text - proxy.Text();
	
	while (isspace(*text))
		text++;
	
	if (strncmp(text, "include", 7) == 0)
	{
		text += 7;
		
		while (isspace(*text))
			text++;
		
		if (*text == '"')
		{
			text++;
			
			while (*text && *text != '"' && *text != '\n')
				name_append(text, name, size);
			*name = 0;
			
			if (proxy.Includes())
				proxy.AddInclude(nameBuf, nameBuf, false);
		}
		else if (*text == '<')
		{
			text++;
			
			while (*text && *text != '>' && *text != '\n')
				name_append(text, name, size);
			*name = 0;
			
			if (proxy.Includes())
				proxy.AddInclude(nameBuf, nameBuf, false);
		}
	}
	else if (strncmp(text, "pragma", 6) == 0)
	{
		text += 6;
		
		while (isspace(*text))
			text++;
			
		if (strncmp(text, "mark", 4) == 0)
		{
			text += 4;

			while (isspace(*text))
				text++;
			
			while (*text && *text != '\n')
				name_append(text, name, size);

			*name = 0;
			
			if (nameBuf[0] == '-')
				proxy.AddSeparator(nameBuf+1);
			else
				proxy.AddFunction(nameBuf, nameBuf, offset);
		}
	}
	
	return skip(text, '\n');
} /* preprocessor */

const char *i_extern(const char *text)
{
	if (*text == '"')
	{
		text = skip(text + 1, '"');
		text = comment(text);
		
		if (*text == '{')
			return text + 1;
	}
	
	return text;
} /* i_extern */

const char *ident(const char *text, CLanguageProxy& proxy, char *&scope)
{
	const char *start = text, *id = start;
	char nameBuf[kMaxNameSize], *name = nameBuf;
	int size = 0, offset = start - proxy.Text();
	bool destructor = false;
	
	while (isident(*text))
		name_append(text, name, size);
	
	*name = 0;

	text = comment(text);
	
	if (strcmp(nameBuf, "extern") == 0)
		return i_extern(text);
	else if (strcmp(nameBuf, "header") == 0) {
		// first add "header"
		while (!isspace(*text)) {
			name_append(text, name, size);
		}
		// then a space
		*name++ = ' ';
		// skip past the double quote
		text = skip(text, '"');
		// save the identifier location
		start = text;
		// put in the identifier [up to next quote]
		while (*text != '"')
			name_append(text, name, size);
		// terminate
		*name = 0;
		// add it
		char match[256];
		long l = std::min((long)255, text - start);
		
		strncpy(match, start, l);
		match[l] = 0;
		
		if (proxy.Types())
			proxy.AddFunction(nameBuf, match, offset, false);

		// walk past the double quote
		text++;
		// eat the code
		text = parens(text + 1, '{');
		return text;
	}
	else if (strcmp(nameBuf, "options") == 0) {
		// first add the scope label
		for (char * c = scope ; (size < kMaxNameSize) ; size++) {
			if (*c == '\0') break;
			*name++ = *c++;
		}
		// then a space
		*name++ = ' ';
		// save the "options" location
		start = text;		
		// then add "options"
		while (!isspace(*text)) {
			name_append(text, name, size);
		}
		// terminate
		*name = 0;
		// add it
		char match[256];
		long l = std::min((long)255, text - start);
		
		strncpy(match, start, l);
		match[l] = 0;
		
		if (proxy.Types())
			proxy.AddFunction(nameBuf, match, offset, false);

		// walk past the double quote
		text++;
		// eat the code
		text = parens(text + 1, '{');
		return text;
	}
	else if (strcmp(nameBuf, "class") == 0 ||
			 strcmp(nameBuf, "struct") == 0 ||
			 strcmp(nameBuf, "union") == 0)
	{
		*name++ = ' ';
		
		if (isidentf(*text))
		{
			while (isident(*text))
				name_append(text, name, size);
			*name = 0;
			
			text = comment(text);
			
			if (*text == ':')
			{
				text = comment(text + 1);
				
				while (isident(*text))
					text++;
				
				text = comment(text);
				
				while (isident(*text))
					text++;
				
				text = comment(text);
			}
			
			if (*text == '{' && proxy.Types())
			{
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
		
		while (isidentf(*text))
		{
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
	
	if (is_template(text))
	{
		name_append(text, name, size);
		
		text = comment(text);
		
		while (isident(*text))
		{
			while (isident(*text))
				name_append(text, name, size);
		
			text = comment(text);
			
			if (*text == ',')
			{
				name_append(text, name, size);
				text = comment(text);
			}
		}

		if (*text == '>')
		{
			name_append(text, name, size);
			
			text = comment(text);
		}
	}
	
	while (*text == ':' && text[1] == ':')
	{
		name_append(text, name, size);
		name_append(text, name, size);
		
		text = comment(text);
		
		id = name;
		
		if (*text == '~')
		{
			name_append(text, name, size);
			text = comment(text);
			destructor = true;
		}
		
		if (isidentf(*text))
			while (isident(*text))
				name_append(text, name, size);
		
		text = comment(text);
		
		if (is_template(text))
		{
			name_append(text, name, size);
			text = comment(text);
			
			while (isident(*text))
			{
				while (isident(*text))
					name_append(text, name, size);
				
				text = comment(text);
				
				if (*text == ',')
				{
					name_append(text, name, size);
					text = comment(text);
				}
			}
			
			if (*text == '>')
			{
				name_append(text, name, size);
				text = comment(text);
			}
		}
	}
	
	if (!destructor && strcmp(id, "operator") == 0)
	{
		if (*text == '(')
			name_append(text, name, size);
		
		text = comment(text);
		
		while (*text != '(' && size < kMaxNameSize)
		{
			if (isidentf(*text))
			{
				while (isident(*text))
					name_append(text, name, size);
			}
			else if (! isspace(*text))
			{
				while (! isspace(*text) && ! isidentf(*text) && *text != '(' &&
							! (*text == '/' && (text[1] == '*' || text[1] == '/')))
					name_append(text, name, size);
			}

			text = comment(text);
		}
	}
	
	*name = 0;
	
	if (*text == '(')
	{
		char match[256];
		long l = std::min((long)255, text - start);
		
		strncpy(match, start, l);
		match[l] = 0;
		
		text = parens(text + 1, '(');
		text = comment(text);
		
		if (*text == ':')
		{
			while (*text != '{' && *text != ';')
				text++;
			if (*text == '{')
			{
				text = parens(text + 1, '{');
				proxy.AddFunction(nameBuf, match, offset, false);
			}
			else
			{
				text++;
				if (proxy.Prototypes())
					proxy.AddFunction(nameBuf, match, offset, true);
			}
			if (*text == '\n')
				text++;
			return text;
		}
		
		if (*text == ';')
		{
			if (proxy.Prototypes())
				proxy.AddFunction(nameBuf, match, offset, true);
			return text + 1;
		}
		
		if (isidentf(*text) || *text == '{')
		{
			proxy.AddFunction(nameBuf, match, offset, false);
			text = skip_ne(text, '{');
			text = parens(text, '{');
			if (*text == '\n')
				text ++;
			return text;
		}
	}
	
	return text;
} /* ident */

void ScanForFunctions(CLanguageProxy& proxy)
{
	const char *text = proxy.Text(), *max = text + proxy.Size();
	char * scope = "file";
	if (*max != 0)
		return;
	
	while (text < max)
	{
		text = comment(text, false);
		
		switch (*text)
		{
			case 0:
				return;
			case '\'':
				text = skip(text + 1, '\'');
				break;
			case '"':
				text = skip(text + 1, '"');
				break;
			case '(':
			case '{':
			case '[':
				text = parens(text + 1, *text);
				break;
			case '#':
				text = preprocessor(text + 1, proxy);
				break;
			default:
				if (isidentf(*text))
					text = ident(text, proxy, scope);
				else
					text++;
				break;
		}
	}
} /* ScanForFunctions */

bool is_template(const char *text)
{
	if(*text == '<')
	{
		const char *p = comment(text + 1);
		return isidentf(*p);
	}

	return false;
} /* is_template */
