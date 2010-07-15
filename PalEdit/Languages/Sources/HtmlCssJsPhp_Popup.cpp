/*	$Id: HtmlCssJsPhp_Popup.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

/* TODO:
	- AddInclude() for include, include_once, require and require_once
	- PHP class support
*/
#include <map>
#include <string>
#include "CLanguageAddOn.h"

const long kMaxNameSize = 256;

const char* const SORT_PREFIX_PHP	= "PHP:  ";
const char* const SORT_PREFIX_JS	= "JS:  ";
const char* const SORT_PREFIX_NAME	= "A:  ";
const char* const SORT_PREFIX_HREF	= "HREF:  ";
const char* const SORT_PREFIX_HEAD1	= "H";
const char* const SORT_PREFIX_HEAD2	= ":  ";

struct PopupMenu
{
	BString label;
	BString text;
	const char * position;
	bool italic;
	
//	PopupMenu (const char *lab, const char *txt, const char * pos)
//		: label(lab), text(txt), position(pos) {}
	
	PopupMenu (BString &lab, BString &txt, const char * pos, bool ita=false)
		: label(lab), text(txt), position(pos), italic(ita) {}
};
typedef std::vector<PopupMenu> PopupList;

void add_to_popup(const char* name, PopupList &pul, CLanguageProxy& proxy);
bool isident(char ch);
const char *skip(const char *txt, int ch);
const char *skip_nc(const char *txt, int ch);
void skip_string(const char *&txt);
const char *skip_block(const char *txt, char open, char close);
const char *skip_whitespace(const char *txt);
void skip_attributes(const char *&ptr);
const char *anchor(const char *txt, CLanguageProxy& proxy, PopupList &pul, bool sorted);
const char *heading(const char *txt, PopupList &pul, bool sorted);
bool get_attribute(const char *&ptr, const char *&namBeg, int& namLen, const char *&valBeg, int& valLen);
void php_function(const char *&txt, PopupList &pul, BString &className, bool sorted);
void php_class(const char *&txt, PopupList &pul, BString &className, bool sorted);
const char *JavaScript(const char *txt, PopupList &lstJsFunctions, PopupList &lstJsClasses, bool sorted);
const char *PhpScript(const char *txt, PopupList &lstPhpFunctions, PopupList &lstPhpClasses, bool sorted);


#pragma mark Support Functions


void add_to_popup(const char* name, PopupList &pul, CLanguageProxy& proxy)
{
	if (pul.size() > 0)
	{
		proxy.AddSeparator(name);
		for (uint i=0; i<pul.size(); i++)
		{
			proxy.AddFunction(pul[i].label.String(), pul[i].text.String(),
				pul[i].position - proxy.Text(), pul[i].italic);
		}
	}
}


bool isident(char ch)
{
	return isalnum(ch) || ch == '_';
} /* isident */


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


void skip_string(const char *&txt)
{
	char quote = *txt;

	while (*++txt && *txt != quote)
		if (*txt == '\\')
			txt++;
} /* skip_string */


const char *skip_block(const char *txt, char open, char close)
{
	while (*txt)
	{
		if (*txt == open)
			txt = skip_block(txt + 1, open, close);

		else if (*txt == close)
			return txt + 1;

		else if (*txt == '"' || *txt == '\'')
		{
			skip_string(txt);
			++txt;
		}
		else
			++txt;
	}
	return txt;
} /* block */


inline const char *skip_whitespace(const char *txt)
{
	while (isspace(*txt))
		txt++;
	return txt;
} /* skip_white */


void skip_attributes(const char *&ptr)
{
	const char *nam = 0, *val = 0;
	int namLen = 0, valLen = 0;

	// Skip tagname
	if (*(++ptr) != 0 && *ptr == '/')
		ptr++;
	while (*ptr != 0 && isalnum(*ptr))
		ptr++;
	// Skip attributes
	while (get_attribute(ptr, nam, namLen, val, valLen)) ;
	// skip ">"
	ptr++;
}


bool get_attribute(const char *&ptr, const char *&namBeg, int& namLen, const char *&valBeg, int& valLen)
{
	const char *namEnd=0, *valEnd=0;

	namBeg = valBeg = 0;
	ptr = skip_whitespace(ptr);
	if (*ptr != 0 && *ptr != '>')
	{
		// Search for attribue name
		namBeg = namEnd = ptr;
		// Gracefully also accept non alphanumeric characters
		while (*namEnd != 0 && *namEnd != '=' && !isspace(*namEnd))
			namEnd++;
		ptr = skip_whitespace(namEnd);
		// Search for attribue value
		if (*ptr == '=') {
			ptr = skip_whitespace(ptr+1);
			if (*ptr == '"' || *ptr == '\'')
			{
				// Value is enclosed
				const char *encl = ptr;
				valBeg = valEnd = (ptr + 1);
				while (*valEnd != 0 && *valEnd != *encl)
					valEnd++;
				ptr = valEnd+1;
			}
			else
			{
				// Value is not enclosed
				valBeg = valEnd = ptr;
				while (isalnum(*valEnd) || *valEnd == '_')
					valEnd++;
				ptr = valEnd;
			}
		}
		//
		namLen = namEnd - namBeg;
		valLen = valEnd - valBeg;
		return true;
	} else {
		return false;
	}
} /* get_attribute */


void php_function(const char *&txt, PopupList &pul, BString &className, bool sorted)
{
	BString label, function, params;
	txt = skip_whitespace(txt+8);

	const char *beg = txt;
	while (isident(*++txt)) ;
	function.SetTo(beg, txt-beg);

	txt = skip_whitespace(txt);
	if (*txt == '(') {
		const char* ptr = txt;
		txt = skip_block(txt+1, '(', ')');
		params.SetTo(ptr+1, txt-ptr-2);
		params.Prepend("  (");
		params.Append(")");
	}
	if (sorted)
	{
		label << SORT_PREFIX_PHP;
		if (className != "")
		{
			label << className << "::";
		}
	}
	else
	{
		if (className != "")
		{
			label << "• ";
		}
	}
	label << function << params;
	pul.insert(pul.end(), PopupMenu(label, function, beg));
} /* php_function */


void php_class(const char *&txt, PopupList &pul, BString &className, bool sorted)
{
	BString label;

	txt = skip_whitespace(txt+5);
	const char *beg = txt;
	while (isident(*++txt)) ;
	className.SetTo(beg, txt-beg);

	while (*++txt && *txt != '{') ;

//	txt = skip_whitespace(txt);
//	if (*txt == '(') {
//		const char* beg = txt;
//		txt = skip_block(txt+1, '(', ')');
//		params.SetTo(beg+1, txt-beg-2);
//		params.Prepend("  (");
//		params.Append(")");
//	}
	if (!sorted)
		pul.insert(pul.end(), PopupMenu(className, className, beg, true));
} /* php_class */


const char *anchor(const char *txt, CLanguageProxy& proxy, PopupList &pul, bool sorted)
{
	const char *nam, *val;
	int namLen, valLen, len;

	while (get_attribute(txt, nam, namLen, val, valLen))
	{
		if (valLen > 0)
		{
			if (strncasecmp(nam, "HREF", 4) == 0)
			{
				if (*val != '#')
				{
					BString label(SORT_PREFIX_HREF), include(val, valLen);
					label << include;
					proxy.AddInclude(label.String(), include.String());
				}
			}
			else if (strncasecmp(nam, "NAME", 4) == 0)
			{
				BString function(val, valLen);
				BString label(sorted ? SORT_PREFIX_NAME : "");
				label << function;
				pul.insert(pul.end(), PopupMenu(label, function, txt));
			}
		}

	};
	return txt;
} /* Anchor */


const char *heading(const char *txt, PopupList &pul, bool sorted)
{
	char endTag[6];
	bool wasSpace = false;
	const char *ptr = txt;
	BString label, heading;
	int level = (*txt - '0');

	if (sorted)
	{
		label << SORT_PREFIX_HEAD1 << level << SORT_PREFIX_HEAD2;
	}
	else
	{
		for (int i=1; i<level; i++)
			label << "       ";
		label << "(" << level << ")  ";
	}
	
	skip_attributes(ptr);
	sprintf(endTag, "</h%c>", *txt);
	txt = ptr;
	while (*ptr != 0) {
		if (*ptr == '<') {
			if (strncasecmp(ptr, endTag, 5) == 0)
			{
				break;
			}
			skip_attributes(ptr);
		} else {
			if (isspace(*ptr))
			{
				if (!wasSpace)
					label << *ptr;
				wasSpace = true;
			}
			else
			{
				label << *ptr;
				wasSpace = false;
			}
			ptr++;
		}
	}
	if (label.Length() > 100)
	{
		label.Truncate(100);
		label << "…";
	}
	heading.SetTo(txt, ptr-txt);

	pul.insert(pul.end(), PopupMenu(label, heading, txt));

	return ptr;
} /* Heading */


#pragma mark Main Functions


const char *JavaScript(const char *txt, PopupList &lstJsFunctions, PopupList &lstJsClasses, bool sorted)
{
	txt = skip_nc(txt, '>');
	
	while (*txt)
	{
		switch (*txt)
		{
			case '/':
				if (*(txt + 1) == '/')
					txt = skip(txt, '\n') - 1;
				break;

			case '"':
			case '\'':
				skip_string(txt);
				break;

			case '(':
				txt = skip_block(txt + 1, '(', ')');
				break;

			case '{':
				txt = skip_block(txt + 1, '{', '}');
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
					
					const char *beg = txt;
					while ((isalnum(*txt) || *txt == '.' || *txt == '_'))
					{
						txt++;
					}

					BString label, function(beg, txt-beg);
					if (sorted)
						label << SORT_PREFIX_JS;
					label << function;

					lstJsFunctions.insert(lstJsFunctions.end(), PopupMenu(label, function, beg));
				}
				break;
		}
		txt++;
	}
	
	return txt;
} /* JavaScript */


const char *PhpScript(const char *txt, PopupList &lstPhpFunctions, PopupList &lstPhpClasses, bool sorted)
{
	std::map <int,int,int> headings;
	BString class_name;
	while (*txt)
	{
//printf("%c", *txt);
		switch (*txt)
		{
			case '/':
				// Comment: "//..."
				if (*(txt + 1) == '/')
					txt = skip(txt + 2, '\n') - 1;
				// Comment: "/*..."
				else if (*(txt + 1) == '*')
				{
					txt++;
					while ( *++txt &&
							!(*txt == '?' && *(txt + 1) == '>') &&
							!(*txt == '*' && *(txt + 1) == '/'))
						;
					txt++;
				}
				break;

			case '"':
			case '\'':
				skip_string(txt);
				break;

			case '}':
				//printf("\nCLASSEND FOUND\n");
				class_name = "";
				break;

			case '{':
				txt = skip_block(txt + 1, '{', '}');
				break;

			case '(':
				txt = skip_block(txt + 1, '(', ')');
				break;

			case '?':
				if (*++txt == '>')
					return txt + 1;
				break;

			default:
//printf(" <%c%c%c%c> ", *(txt+0), *(txt+1), *(txt+2), *(txt+3));
				if (isident(*txt)) {
					const char *ptr = txt-1;
					while (*++ptr && isident(*ptr)) ;
					int len = ptr - txt;

					if (len == 8 && strncasecmp(txt, "function", 8) == 0)
					{
						php_function(txt, class_name == "" ? lstPhpFunctions : lstPhpClasses, class_name, sorted);
					}
					else if (len == 5 && strncasecmp(txt, "class", 5) == 0)
					{
						php_class(txt, lstPhpClasses, class_name, sorted);
					}
					else
					{
						//printf("[[");for (const char* p=txt; p<ptr; p++)printf("%c", *p);printf(":%li]]", ptr - txt);
						txt = ptr;
					}
				}
				break;
		}
		txt++;
	}
//printf("\n");
	return txt;
} /* PhpScript */


void ScanForFunctions(CLanguageProxy& proxy)
{
	bool sorted = proxy.Sorted();
	PopupList lstHeadings, lstAnchors, lstPhpFunctions, lstPhpClasses, lstJsFunctions, lstJsClasses;

	const char *text = proxy.Text(), *max = text + proxy.Size();
	if (*max != 0)
		return;

	if (strncasecmp(text, "<!--:javascript", 15) == 0)
		text = JavaScript(text, lstJsFunctions, lstJsClasses, sorted);
	else if (strncasecmp(text, "<!--:php", 8) == 0)
		text = PhpScript(text + 11, lstPhpFunctions, lstPhpClasses, sorted);

	while (*text && text < max)
	{
		text = skip(text, '<');
		text = skip_whitespace(text);
		
		switch (toupper(*text))
		{
			case '?':
				text = PhpScript(text, lstPhpFunctions, lstPhpClasses, sorted);
				break;

			case 'A':
				if (isspace(*++text))
					text = anchor(text, proxy, lstAnchors, sorted);
				else
					text = skip_nc(text, '>');
				break;

			case 'L':
				if (strncasecmp(text, "LINK", 4) == 0)
					text = anchor(text + 4, proxy, lstAnchors, sorted);
				else
					text = skip_nc(text, '>');
				break;

			case 'H':
				if (*++text >= '1' && *text <= '6')
				{
					text = heading(text, lstHeadings, sorted);
				}
				else
					text = skip_nc(text, '>');
				break;

			case 'S':
				if (strncasecmp(text, "SCRIPT", 6) == 0)
					text = JavaScript(text, lstJsFunctions, lstJsClasses, sorted);
				break;

			default:
				text = skip_nc(text + 1, '>');
				break;
		}
	}

	add_to_popup("PHP-Classes",   lstPhpClasses,   proxy);
	add_to_popup("PHP-Functions", lstPhpFunctions, proxy);
	add_to_popup("JS-Classes",    lstJsClasses,    proxy);
	add_to_popup("JS-Functions",  lstJsFunctions,  proxy);
	add_to_popup("HTML-Anchors",  lstAnchors,      proxy);
	add_to_popup("HTML-Headings", lstHeadings,     proxy);
} /* ScanForFunctions */
