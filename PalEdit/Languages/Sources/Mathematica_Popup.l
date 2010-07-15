/*	$Id: Mathematica_Popup.l,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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
*/

%{
#include "CLanguageAddOn.h"

static const char *gBuf = NULL;
static int gIndx;
static int gBufSize;
static bool inString;

#undef YY_DECL
#define YY_DECL	static int yylex(CLanguageProxy& proxy)
#undef YY_INPUT
#define YY_INPUT(tok,result,max)	GetNextChar(tok, result, max)
#define YY_SKIP_YYWRAP
#undef yywrap
#define yywrap()	1
#undef ECHO
#define ECHO

static void GetNextChar(char *tok, int& result, int max);
static void AddFunction(CLanguageProxy& proxy, int level);
static void AddPrototype(CLanguageProxy& proxy);
static void AddInclude(CLanguageProxy& proxy);
%}

ws			[ \n\t]*
letter		[a-zA-Z_]
digit		[0-9]
ident		{letter}({letter}|{digit})*{ws}
params		\([^)]*\){ws}
proc		PROCEDURE{ws}(\(("VAR"{ws})?{ident}\:{ws}{ident}\){ws})?

%x block
%x string
%x import

%%
																int string_caller = 0, nesting = 0;
{proc}\*?{ident}(\*{ws})?{params}?				AddFunction(proxy, nesting); nesting++;
{proc}^{ident}(\*{ws})?{params}?					AddPrototype(proxy);
"BEGIN"														BEGIN(block);
"IMPORT"													BEGIN(import);
\"																string_caller = INITIAL; BEGIN(string); inString = true;
.
<block>"END"{ws}{ident}								BEGIN(INITIAL); nesting = std::max(--nesting, 0);
<block>\"													string_caller = block; BEGIN(string);
<block>{ident}
<block>.
<string>\"													BEGIN(string_caller); inString = false;
<string>.
<import>{ident}/":="
<import>[a-zA-Z][a-zA-Z0-9]*						AddInclude(proxy);
<import>;													BEGIN(INITIAL);
<import>.
<block,string,import><<EOF>>						BEGIN(INITIAL); return 0;
%%

_EXPORT void ScanForFunctions(CLanguageProxy& proxy)
{
	static sem_id fps = create_sem(1, "func popup semaphore");
	
	if (acquire_sem(fps) == B_NO_ERROR)
	{
		gBuf = proxy.Text();
		gBufSize = proxy.Size();
		gIndx = 0;
		inString = false;

		yyrestart(NULL);
		yylex(proxy);
		
		release_sem(fps);
	}
} /* ScanForFunctions */

void GetNextChar(char *tok, int& result, int max)
{
	int c = gBuf[gIndx++];
	
	if (inString)
		;
	else if (c == '(' && gBuf[gIndx] == '*')
	{
		int space = 3;
		gIndx++;
	
		while (gIndx < gBufSize && !(gBuf[gIndx] == '*' && gBuf[gIndx + 1] == ')'))
		{
			gIndx++;
			space++;
		}
		
		result = max = std::min(max, space);
		gIndx += 2;

		while (max--)
			*tok++ = ' ';
		
		return;
	}

	result = (gIndx < gBufSize) ? (tok[0] = c, 1) : YY_NULL;
} /* GetNextChar */

static void AddFunction(CLanguageProxy& proxy, int nesting)
{
	char *s, *i;
	
	i = yytext + strlen("PROCEDURE");

	while (isspace(*i)) i++;

	if (*i == '(')
	{
		while (*++i != ')')
			;
		while (isspace(*++i))
			;
	}
	
	s = i;
	while (isalnum(*s))
		s++;
	*s = 0;

	if (strlen(i))
	{
		s = i;
		while (nesting-- > 0 && s > yytext)
		{
			*--s = ' ';
			*--s = ' ';
		}
		
		proxy.AddFunction(s, i, gIndx - yy_n_chars - 1);
	}
} /* AddFunction */

static void AddPrototype(CLanguageProxy& proxy)
{
	char *s, *i;
	
	i = yytext;
	
	i += strlen("PROCEDURE");
	while (isspace(*i)) i++;

	do
	{
		s = strchr(i, '(');
		if (!s) break;
		*s = 0;
		
		while (isspace(*--s) || *s == '*')
			*s = 0;
	
		proxy.AddFunction(i, i, gIndx - yy_n_chars - 1, true);
	}
	while (false);
} /* AddPrototype */

static void AddInclude(CLanguageProxy& proxy)
{
	if (strlen(yytext) < 60)
	{
		char s[64];
		strcpy(s, yytext);
		strcat(s, ".Mod");
		
		proxy.AddInclude(yytext, s);
	}
} /* AddInclude */
