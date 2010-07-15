/*	$Id: Html_Popup.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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
#include <string>

const long kMaxNameSize = 256;

const char *skip(const char *txt, int ch);
const char *skip_nc(const char *txt, int ch);
const char *Anchor(const char *txt, CLanguageProxy& ao);
const char *Heading(const char *txt, CLanguageProxy& ao);
const char *JavaScript(const char *txt, CLanguageProxy& ao);

const char *skip(const char *txt, int ch)
{
	while (*txt)
	{
		if (*txt == ch)
		{
			txt++;
			break;
		}
		txt++;
	}

	return txt;
} /* skip */

const char *skip_nc(const char *txt, int ch)
{
	while (*txt)
	{
		if (*txt == ch)
		{
			txt++;
			break;
		}
		else if (*txt == '"')
			txt = skip(txt + 1, '"');
		else
			txt++;
	}

	return txt;
} /* skip_nc */

inline const char *skip_white(const char *txt)
{
	while (isspace(*txt))
		txt++;
	return txt;
} /* skip_white */

const char *Anchor(const char *txt, CLanguageProxy& ao)
{
	char name[PATH_MAX];

	txt = skip_white(txt);
	
	do
	{
		if (strncasecmp(txt, "HREF", 4) == 0)
		{
			txt = skip_white(txt + 4);
			
			if (*txt != '=')
				break;
			
			txt = skip_white(txt + 1);
			
			if (*txt != '"')
				break;
			
			const char *file = txt + 1;
			
			txt = skip(txt + 1, '"');
			
			if (*txt == 0)
				break;
			
			int l = std::min((long)PATH_MAX - 1, txt - file - 1);
			strncpy(name, file, l);
			name[l] = 0;
			
			ao.AddInclude(name, name);
		}
		else if (strncasecmp(txt, "NAME", 4) == 0)
		{
			txt = skip_white(txt + 4);
			
			if (*txt != '=')
				break;
			
			txt = skip_white(txt + 1);
			
			if (*txt != '"')
				break;

			int offset = txt - ao.Text();
			const char *anchor = txt + 1;
			
			txt = skip(txt + 1, '"');
			
			if (*txt == 0)
				break;
			
			char *p = name + kMaxNameSize;
			int l = std::min(kMaxNameSize - 1, txt - anchor - 1);
			strncpy(name, anchor, l);
			name[l] = 0;
			
			sprintf(p, "A: %s", name);
			
			ao.AddFunction(p, name, offset);
		}
	}
	while (false);
	
	return skip(txt, '>');
} /* Anchor */

const char *Heading(const char *txt, CLanguageProxy& ao)
{
	const char *hName;
	int level = *txt - '0';
	
	txt = skip(txt + 1, '>');
	
	while (*txt == '<')
		txt = skip(txt + 1, '>');
	
	if (*txt == 0)
		return txt;
	
	hName = txt;
	
	txt = skip(txt + 1, '<');
	
	char name[kMaxNameSize];
	int l = std::min(txt - hName - 1, kMaxNameSize - 1);
	int offset = hName - ao.Text();
	
	strncpy(name, hName, l);
	name[l] = 0;
	
	hName = strtok(name, "\n\t\r");
	
	do
	{
		if (strlen(hName))
		{
			char label[kMaxNameSize + 10];
			sprintf(label, "H%d: %s", level, hName);
			ao.AddFunction(label, hName, offset);
			break;
		}
		hName = strtok(NULL, "\n\r\t");
	}
	while (hName);
	
	return txt;
} /* Heading */

const char *JavaScript(const char *txt, CLanguageProxy& ao)
{
	txt = skip_nc(txt, '>');
	
	while (*txt)
	{
		switch (*txt)
		{
			case '/':
				if (txt[1] == '/')
					txt = skip(txt, '\n');
				break;
			case '"':
			{
				while (*++txt)
				{
					if (*txt == '\\')
						txt++;
					else if (*txt == '"')
						break;
				}
				break;
			}
			case '\'':
			{
				while (*++txt)
				{
					if (*txt == '\\')
						txt++;
					else if (*txt == '\'')
						break;
				}
				break;
			}
			case '{':
				txt = skip(txt + 1, '}');
				break;
			case '(':
				txt = skip(txt + 1, ')');
				break;
			case '<':
				if (strncasecmp(txt, "</script", 8) == 0)
					return txt + 9;
				break;
			default:
				if (strncasecmp(txt, "function", 8) == 0)
				{
					txt += 8;
					while (isspace(*txt)) txt++;
					
					int offset = txt - ao.Text();
					
					std::string name, label;
					
					while (isalnum(*txt))
						name += *txt++;
					
					label = "JS: " + name;
					
					ao.AddFunction(label.c_str(), name.c_str(), offset);
				}
				break;
		}
		txt++;
	}
	
	return txt;
} // JavaScript

void ScanForFunctions(CLanguageProxy& proxy)
{
	const char *text = proxy.Text(), *max = text + proxy.Size();
	if (*max != 0)
		return;
	
	while (text < max)
	{
		text = skip(text, '<');
		text = skip_white(text);
		
		switch (toupper(*text))
		{
			case 0:
				return;
			case 'A':
				if (isspace(*++text))
					text = Anchor(text, proxy);
				else
					text = skip_nc(text, '>');
				break;
			case 'H':
				if (*++text >= '1' && *text <= '6')
				{
					text = Heading(text, proxy);
				}
				else
					text = skip_nc(text, '>');
				break;
			case 'S':
				if (strncasecmp(text, "SCRIPT", 6) == 0)
					text = JavaScript(text, proxy);
				break;
			default:
				text = skip_nc(text + 1, '>');
				break;
		}
	}
} /* ScanForFunctions */
