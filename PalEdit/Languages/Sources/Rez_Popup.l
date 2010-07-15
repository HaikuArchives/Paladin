/*	$Id: Rez_Popup.l,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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
static void AddResource(CLanguageProxy& proxy);
static void AddResType(CLanguageProxy& proxy);
static void AddMarker(CLanguageProxy& proxy);
static void AddInclude(CLanguageProxy& proxy, char openingquote, char closingquote);
static void AddSeparator(CLanguageProxy& proxy);
%}

ws			[ \n\t]*
ident		[a-zA-Z_][a-zA-Z0-9_]*{ws}

%x BLOCK
%x STR
%x CC

%%
												int BLOCKlevel = 0, CC_caller = 0, STR_caller = 0, CC_cnt = 0;
\#{ws}include{ws}\<[^\n>]+\>		AddInclude(proxy, '<', '>');
\#{ws}include{ws}\"[^\n"]+\"		AddInclude(proxy, '"', '"');
\#pragma\ mark\ [^\n]+				AddMarker(proxy);
type{ws}\'[^\']{4}\'						AddResType(proxy);
resource{ws}\'[^\']{4}\'				AddResource(proxy);
\{												BLOCKlevel = 1; BEGIN(BLOCK);
\"												STR_caller = INITIAL; BEGIN(STR);
\'												CC_caller = INITIAL; BEGIN(CC); CC_cnt = 0;
.
<BLOCK>\{								BLOCKlevel++;
<BLOCK>\}								BLOCKlevel--; if (BLOCKlevel == 0) BEGIN(INITIAL);
<BLOCK>\'									CC_caller = BLOCK; BEGIN(CC);
<BLOCK>\"								STR_caller = BLOCK; BEGIN(STR); inString = true;
<BLOCK>.
<STR>\"								BEGIN(STR_caller); inString = false;
<STR>[^\\\n\"]+
<STR>\\.
<STR>.
<CC>\'										BEGIN(CC_caller);
<CC>[^\\\'\n]+							if ((CC_cnt += yyleng) > 4) BEGIN(CC_caller);
<CC>\\.									if (++CC_cnt > 4) BEGIN(CC_caller);
<CC>.										if (++CC_cnt > 4) BEGIN(CC_caller);
<BLOCK,STR,CC><<EOF>>	BEGIN(INITIAL); return 0;
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
	else if (c == '/' && gBuf[gIndx] == '*')
	{
		int space = 3;
		gIndx++;
	
		while (gIndx < gBufSize && !(gBuf[gIndx] == '*' && gBuf[gIndx + 1] == '/'))
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
	else if (c == '/' && gBuf[gIndx] == '/')
	{
		int space = 1;
		gIndx++;
	
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
	else if (c == '\\' && gBuf[gIndx] == '\n')
	{
		gIndx++;
		c = ' ';
	}

	result = (gIndx < gBufSize) ? (tok[0] = c, 1) : YY_NULL;
} /* GetNextChar */

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
		proxy.AddInclude(s, s, false);
	}
	while (false);
} /* AddInclude */

static void AddMarker(CLanguageProxy& proxy)
{
	char *m = strstr(yytext, "mark ");
	if (m)
	{
		m += 5;
		
		proxy.AddSeparator(m);
	}
} /* AddMarker */

static void AddResType(CLanguageProxy& proxy)
{
	char s[] = "type ....";
	strncpy(s + 5, yytext + yyleng - 5, 4);

	proxy.AddFunction(s, yytext, gIndx - yy_n_chars - 1);
} /* AddType */

static void AddResource(CLanguageProxy& proxy)
{
	char s[] = "resource ....";
	strncpy(s + 9, yytext + yyleng - 5, 4);

	proxy.AddFunction(s, yytext, gIndx - yy_n_chars - 1);
} /* AddResource */
