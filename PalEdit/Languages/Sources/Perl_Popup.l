/*	$Id: Perl_Popup.l,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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
static void AddFunction(CLanguageProxy& proxy);
static void AddInclude(CLanguageProxy& proxy, char openingquote, char closingquote);
%}

ws			[ \n\t]*
ident		[a-zA-Z_][a-zA-Z0-9_]*{ws}

%x block
%x string

%%
												int blocklevel = 0, string_caller = 0;
"#"[^\n]*\n
"require"{ws}\"[^"]+\";				AddInclude(proxy, '"', '"');
"sub"[ \n\t]+[^{]+\{						AddFunction(proxy); BEGIN(block); blocklevel = 1;
\{												BEGIN(block); blocklevel = 1;
<block>\{									blocklevel++;
<block>\}									blocklevel--; if (blocklevel == 0) BEGIN(INITIAL);
<block>\"									string_caller = block; BEGIN(string); inString = true;
<block>.
<string>\"									BEGIN(string_caller); inString = false;
<string>[^\\\n\"]+
<string>\\.
<string>.
<block,string><<EOF>>				BEGIN(INITIAL); return 0;
%%

/*
<block>\'										cc_caller = block; BEGIN(cc);
<cc>\'											BEGIN(cc_caller);
<cc>[^\\\'\n]+									if ((cc_cnt += yyleng) > 4) BEGIN(cc_caller);
<cc>\\.											if (++cc_cnt > 4) BEGIN(cc_caller);
<cc>.											if (++cc_cnt > 4) BEGIN(cc_caller);

*/

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
	else if (c == '#')
	{
		int space = 0;
		
		do
		{
			c = gBuf[gIndx++];
			space++;
		}
		while (gIndx < gBufSize && c != '\n');

		result = max = std::min(max, space);

		while (max--)
			*tok++ = ' ';

		return;
	}

	result = (gIndx < gBufSize) ? (tok[0] = c, 1) : YY_NULL;
} /* GetNextChar */

static void AddFunction(CLanguageProxy& proxy)
{
	char *s = yytext + yyleng - 1;
	int offset = gIndx - yy_n_chars - 1;

	do
	{
		while (isspace(*--s))
			*s = 0;

		s = yytext + 3;
		while (isspace(*s))
			s++;
		
		proxy.AddFunction(s, s, offset);
	}
	while (false);
} /* AddFunction */

static void AddInclude(CLanguageProxy& proxy, char openingquote, char closingquote)
{
	char *s, *i = yytext;
	
	do
	{
		s = strrchr(i, closingquote);
		if (!s) break;
		*s = 0;
		s = strrchr(i, openingquote);
		if (!s) break;
		
		s++;
		proxy.AddInclude(s, s);
	}
	while (false);
} /* AddInclude */
