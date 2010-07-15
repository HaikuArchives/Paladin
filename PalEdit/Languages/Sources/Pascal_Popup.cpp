/*	$Id: Pascal_Popup.cpp,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	        
	 Copyright 2003 Kelvin W Sherlock
	
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

	Created: 03/05/2003
*/

#include "CLanguageAddOn.h"

const int kMaxNameSize = 256;

const char* comment(const char* text)
{
	do
	{
		while (isspace(*text))
			text++;
			
		if (*text == '{') 
		{
			while (*text && *text != '}') text++;
			if (*text) text++;
		}
		else if (*text == '(' && text[1] == '*')
		{
			text += 2;	
			while (*text && *text != '*' && text[1] != ')') text++;
			if (*text) text += 2;
		}
		else if (*text == '/' && text[1] == '/')
		{
			while (*text && *text != '\n') text++;
			if (*text) text++;
		}
		else break;
	}
	while (*text);

	return text;
}

const char* parseFunction(const char* text, CLanguageProxy& proxy)
{
	char nameBuf[kMaxNameSize];
	char* name = nameBuf;
	int size = 0;
	int offset = text - proxy.Text();
	
	text = comment(text);
	
	// Valid identifiers can contain numbers, but not in the first position.
	if (!isdigit(*text))
	{
		while (isalnum(*text) || *text == '.' || *text == '_')
		{
			if (size < kMaxNameSize - 1)
			{
				*name++ = *text++;
				size++;
			}
			else text++;
		}
	}

	*name = 0;

	if (size)
	{
 		proxy.AddFunction(nameBuf, nameBuf, offset, false);
	}

	return text;
}

//------------------------------------------------------------------------------

const char* parseUses(const char* text, CLanguageProxy& proxy)
{
	char nameBuf[kMaxNameSize];
	char filenameBuf[kMaxNameSize];
	char* name = nameBuf;
	char* filename = filenameBuf;

	// init buffers
	memset(nameBuf, '\0', sizeof(nameBuf));
	memset(filenameBuf, '\0', sizeof(filenameBuf));

	int size = 0;
	int offset = text - proxy.Text();

	// units are listed from "uses" to the first ";"
	do
	{
		// skip spaces
		while (isspace(*text))
			text++;
			
		// skip comments
		text = comment(text);
		
		while (isalpha(*text) || *text == '_')
		{
			if (size < kMaxNameSize - 1)
			{
				*name++ = *text++;
				// As symbol are not case sensitive in pascal, we consider
				// all unit filenames in lower case on case sensitive file systems
				// like BFS
				*filename++ = tolower(nameBuf[size]);
				size++;
			}
			else text++;
		}

		if (size)
		{
			// Append .pp extension to unit name to get unit filename
			// (only if there is enough space !)
			if (size < kMaxNameSize - 5)
			{
				*filename++ = '.';
				*filename++ = 'p';
				*filename++ = 'p';
				*filename++ = '\0';
			}
			if (proxy.Includes())
			  proxy.AddInclude(nameBuf, filenameBuf, false);

			// Reset buffers
			memset(nameBuf, '\0', sizeof (nameBuf));
			memset(filenameBuf, '\0', sizeof (filenameBuf));

			// seek to begin of buffers
			name = nameBuf;
			filename = filenameBuf;
			size = 0;
		}
	}
	while (*text++ != ';');

	*name = 0;
	*filename = 0;

	return text;
}

//------------------------------------------------------------------------------

void ScanForFunctions(CLanguageProxy& proxy)
{
	const char *text = proxy.Text(), *max = text + proxy.Size();

	if (*max != 0) return;

	while (text < max)
	{
		text = comment(text);
		
		switch (*text++)
		{
			case 'c':
			case 'C':
				if (!strncasecmp(text, "onstructor", 10)) text = parseFunction(text + 10, proxy);
			break;

			case 'd':
			case 'D':
				if (!strncasecmp(text, "estructor", 9)) text = parseFunction(text + 9, proxy);
			break;

			case 'f':
			case 'F':
				if (!strncasecmp(text, "unction", 7))
					text = parseFunction(text + 7, proxy);
			break;

			case 'p':
			case 'P':
				if (!strncasecmp(text, "rocedure", 8))
					text = parseFunction(text + 8, proxy);
			break;

			case 'u':
			case 'U':
				if (!strncasecmp(text, "ses", 3)) text = parseUses(text + 3, proxy);
			break;
		}
	}
}

//------------------------------------------------------------------------------
