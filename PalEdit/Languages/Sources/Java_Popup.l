/*	$Id: Java_Popup.l,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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
static char *gClassName = NULL;

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
static void AddPrototype(CLanguageProxy& proxy);
static void AddInclude(CLanguageProxy& proxy, char openingquote, char closingquote);
static void EnterNewClass();
%}

ws			[ \n\t]*
ident		[a-zA-Z_][a-zA-Z0-9_]*{ws}
params		\([a-zA-Z0-9 \t\n&*,\[\]_<>\.]*\){ws}
oper		operator{ws}[-+*/=!><|&%^()\[\]~]+{ws}
throws		throws{ws}{ident}({ws},{ws}{ident})*{ws}
implements	implements{ws}({ident}{ws}\,{ws})*{ident}

%x block
%x class
%x string
%x cc
%x pragma

%%
												int blocklevel = 0, cc_caller = 0, string_caller = 0, cc_cnt = 0;
class{ws}{ident}+{implements}?\{				EnterNewClass(); BEGIN(class);
<class>{ident}{params}/{throws}?\{				AddFunction(proxy);
<class>{ident}{params}/{throws}?;				AddPrototype(proxy);
<class>\{										blocklevel = 1; BEGIN(block);
<class>\}										BEGIN(INITIAL);
<class>\"										string_caller = class; BEGIN(string);
<class>\'										cc_caller = INITIAL; BEGIN(cc); cc_cnt = 0;
.
<block>\{										blocklevel++;
<block>\}										blocklevel--; if (blocklevel == 0) BEGIN(class);
<block>\"										string_caller = block; BEGIN(string); inString = true;
<block>\'										cc_caller = block; BEGIN(cc);
<block>[^'"{}\n]+
<block>.
<string>\"										BEGIN(string_caller); inString = false;
<string>[^\\\n\"]+
<string>\\.
<string>.
<cc>\'											BEGIN(cc_caller);
<cc>[^\\\'\n]+									if ((cc_cnt += yyleng) > 4) BEGIN(cc_caller);
<cc>\\.											if (++cc_cnt > 4) BEGIN(cc_caller);
<cc>.											if (++cc_cnt > 4) BEGIN(cc_caller);
<block,string,cc,class><<EOF>>					BEGIN(INITIAL); return 0;
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

static void AddFunction(CLanguageProxy& proxy)
{
	char *s, *i;
	
	i = yytext;
	int il = strlen(i);
	s = i + il;
	
	do
	{
		s = strchr(i, '(');
		if (!s) break;
		*s = 0;
		
		while (isspace(*--s))
			*s = 0;

		char *f = (char *)malloc(strlen(gClassName) + strlen(i) + 1);
		if (!f) break;
		strcpy(f, gClassName);
		strcat(f, i);
	
		proxy.AddFunction(f, i, gIndx - yy_n_chars - 1);
		
		free(f);
	}
	while (false);
} /* AddFunction */

static void AddPrototype(CLanguageProxy& proxy)
{
	char *s, *i;
	
	i = yytext;
	int il = strlen(i);
	s = i + il;
	
	do
	{
		s = strchr(i, '(');
		if (!s) break;
		*s = 0;
		
		while (isspace(*--s))
			*s = 0;
	
		char *f = (char *)malloc(strlen(gClassName) + strlen(i) + 1);
		if (!f) break;
		strcpy(f, gClassName);
		strcat(f, i);
	
		proxy.AddFunction(f, i, gIndx - yy_n_chars - 1, true);
		
		free(f);
	}
	while (false);
} /* AddPrototype */

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

static void EnterNewClass()
{
	if (gClassName) free(gClassName);
	
	char *s = yytext + strlen("class ");
	
	while (isspace(*s)) s++;
	
	char *c = s;
	
	while (isalnum(*s) || *s == '_') s++;
	*s++ = '.';
	*s = 0;
	
	gClassName = (char *)malloc(strlen(c) + 1);
	if (gClassName)
		strcpy(gClassName, c);
} /* EnterNewClass */

/*
^Subject:\ [^\n]*																				AddSubject(func);
*/
