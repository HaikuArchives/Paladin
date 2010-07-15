#include <SupportDefs.h>
#include <string.h>
#include <ctype.h>
#include "CLanguageAddOn.h"

const int kMaxNameSize = 256;

const char *comment(const char *text)
{
	do
	{
		while (isspace (*text))
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
		else break;
	}
	while (*text);

	return text;
}

const char *parseFunction(const char *text, CLanguageProxy& proxy)
{
	char nameBuf[kMaxNameSize], *name = nameBuf;
	int size = 0;
	int offset = text - proxy.Text();
	
	text = comment(text);
	
	while (isalnum(*text) || *text == ':' || *text == '_')
	{
			if (size < kMaxNameSize - 1)
			{
				*name++ = *text++;
				size++;
			}
			else text++;
	}
	*name = 0;
	if (size)
		proxy.AddFunction(nameBuf, nameBuf, offset, false);

	return text;
}

void ScanForFunctions(CLanguageProxy& proxy)
{
	const char *text = proxy.Text(), *max = text + proxy.Size();
	if (*max != 0)
		return;
	
	while (text < max)
	{
		text = comment(text);
		
		switch (*text++)
		{
		case 'f':
		case 'F':
			if (!strncasecmp(text, "unction", 7)) text = parseFunction(text + 7, proxy);
		break;
		}
	}
}
