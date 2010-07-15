%{

/*	$Id: rez_scanner.l,v 1.3 2009/06/20 14:24:30 darkwyrm Exp $
	
	Copyright 1996, 1997, 1998
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

	Created: 12/02/98 15:37:52
*/

#include "rez.h"
#include "RTypes.h"
#include "rez_parser.cpp.h"
#include "SymbolTable.h"
#include <support/Debug.h>
#include <map>
#include <ctype.h>
#include <time.h>

#define YY_SKIP_YYWRAP
#define yywrap()	1

static char sBuf[1024], *sbp, *d;

#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_lineno_stack[MAX_INCLUDE_DEPTH];
char *gFileName[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;

#define MAX_MACRO_DEPTH 10
YY_BUFFER_STATE macro_stack[MAX_MACRO_DEPTH];
int macro_stack_ptr = 0;

typedef std::map<int, char*> macromap ;
macromap gMacroTable = *new macromap;

void AddMacro(char *s);
void CopyBuf();

%}


letter		[a-zA-Z]
bindigit	[01]
octdigit	{bindigit}|[2-7]
digit		{octdigit}|[89]
hexdigit	{digit}|[a-fA-F]
alnum		{letter}|{digit}
ident		[a-zA-Z_][a-zA-Z0-9_]*

%x string
%x hexstring
%x comment
%x cc
%x incl
%option yylineno

%%
														time_t now; time(&now); struct tm *now_t = localtime(&now);
align													return ALIGN;
array													return ARRAY;
as														return AS;
binary													return BINARY;
bit														return BIT;
bitstring												return BITSTRING;
boolean													return BOOLEAN;
byte													return BYTE;
case													return CASE;
change													return CHANGE;
char													return CHAR;
cstring													return CSTRING;
decimal													return DECIMAL;
delete													return DELETE;
derez													return DEREZ;
false													yylval = false; return NUMBER;
fill													return FILL;
hex														return HEX;
hexstring												return HEXSTRING;
integer													return INTEGER;
include													return INCLUDE;
key														return KEY;
literal													return LITERAL;
longint													return LONGINT;
nibble													return NIBBLE;
octal													return OCTAL;
point													return POINT;
pstring													return PSTRING;
read													return READ;
rect													return RECT;
resource												return RES;
rez														return REZ;
string													return STRING;
switch													return SWITCH;
true													yylval = true; return NUMBER;
type													return rTYPE;
unsigned												return UNSIGNED;
wide													return WIDE;
word													return WORD;
wstring													return WSTRING;
{ident}													{
															yylval = ST_AddIdent(yytext);
															
															if (gMacroTable.find(yylval) != gMacroTable.end())
															{
																if (macro_stack_ptr >= MAX_INCLUDE_DEPTH)
																	error("Macro's nested too deeply");
	
																macro_stack[macro_stack_ptr++] = YY_CURRENT_BUFFER;
																yy_switch_to_buffer(yy_scan_string(gMacroTable[yylval]));
															}
															else
																return IDENT;
														}
$$arrayindex											return ARRAYINDEX;
$$attributes											warn("attributes are not supported"); yylval = 0; return NUMBER;
$$bitfield												return LBITFIELD;
$$byte													return LBYTE;
$$countof												return COUNTOF;
$$date													strftime(sBuf, 1023, "%A, %B %d, %Y", now_t); yylval = (int)strdup(sBuf); return STR_CONST;
$$day													yylval = now_t->tm_mday; return NUMBER;
$$format												return FORMAT;
$$hour													yylval = now_t->tm_hour; return NUMBER;
$$ID													yylval = gResID; return NUMBER;
$$long													return LLONG;
$$minute												yylval = now_t->tm_min; return NUMBER;
$$minute												yylval = now_t->tm_mon; return NUMBER;
$$name													yylval = (int)gResName; return STR_CONST;
$$packedsize											warn("packedsize is not supported"); yylval = 0; return NUMBER;
$$resource												return RESOURCE;
$$resourcesize											return RESSIZE;
$$second												yylval = now_t->tm_sec; return NUMBER;
$$shell													return SHELL;
$$time													strftime(sBuf, 1023, "%H:%M:%S", now_t); yylval = (int)strdup(sBuf); return STR_CONST;
$$type													yylval = gResType; return NUMBER;
$$version												yylval = (int)"rez for be 1.0a2"; return STR_CONST;
$$weekday												yylval = now_t->tm_wday; return NUMBER;
$$word													return LWORD;
$$year													yylval = now_t->tm_year; return NUMBER;
\"														BEGIN(string); sbp = sBuf;
<string>\"												BEGIN(INITIAL); *sbp = 0; yylval = (int)strdup(sBuf); return STR_CONST;
<string>\n												REJECT;
<string>\\0[bB]{bindigit}+								*sbp++ = strtol(yytext + 3, &d, 2);
<string>\\{octdigit}{3}									*sbp++ = strtol(yytext + 1, &d, 8);
<string>\\0[dD]{digit}{3}								*sbp++ = strtol(yytext + 3, &d, 10);
<string>\\0[xX]{hexdigit}{2}							*sbp++ = strtol(yytext + 3, &d, 16);
<string>\\${hexdigit}{2}								*sbp++ = strtol(yytext + 2, &d, 16);
<string>\\t												*sbp++ = '\t';
<string>\\n												*sbp++ = '\n';
<string>\\r												*sbp++ = '\r';
<string>\\b												*sbp++ = 0x08;
<string>\\?												*sbp++ = 0x7F;
<string>\\f												*sbp++ = 0x0C;
<string>\\v												*sbp++ = 0x0B;
<string>\\.												*sbp++ = yytext[1];
<string>[^\\\"\n]+										strcpy(sbp, yytext); sbp += yyleng;
$\"														BEGIN(hexstring); sbp = sBuf + sizeof(long);
<hexstring>\"											BEGIN(INITIAL); CopyBuf(); return HEX_CONST;
<hexstring>{hexdigit}{2}								*sbp++ = strtol(yytext, &d, 16);
<hexstring>" "
<hexstring>.											error("Illegal character inside hexstring: %c (%d)", *yytext, *yytext);
\'														BEGIN(cc); yylval = 0;
<cc>\'													BEGIN(INITIAL); return NUMBER;
<cc>\\t													yylval = (yylval << 8) | '\t';
<cc>\\n													yylval = (yylval << 8) | '\n';
<cc>\\r													yylval = (yylval << 8) | '\r';
<cc>\\b													yylval = (yylval << 8) | 0x08;
<cc>\\?													yylval = (yylval << 8) | 0x7F;
<cc>\\f													yylval = (yylval << 8) | 0x0C;
<cc>\\v													yylval = (yylval << 8) | 0x0B;
<cc>\\.													yylval = (yylval << 8) | yytext[1];
<cc>.													yylval = (yylval << 8) | yytext[0];
"/*"													BEGIN(comment);
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n
<comment>"*"+"/"										BEGIN(INITIAL);
"//"[^\n]*
0[xX]{hexdigit}{1,8}									yylval = strtol(yytext + 2, &d, 16); return NUMBER;
${hexdigit}{1,8}										yylval = strtol(yytext + 1, &d, 16); return NUMBER;
0{octdigit}{1,12}										yylval = strtol(yytext, &d, 8); return NUMBER;
0[bB]{bindigit}{1,32}									yylval = strtol(yytext + 2, &d, 2); return NUMBER;
{digit}+												yylval = strtol(yytext, &d, 10); return NUMBER;
"||"													return OR;
"&&"													return AND;
"=="													return EQ;
"!="													return NE;
"<<"													return SHL;
">>"													return SHR;
"<"														return LT;
">"														return GT;
"<="													return LE;
">="													return GE;
\#include[ \t]+\"										BEGIN(incl);
<incl>[ \t]*
<incl>[^ \t\n\"]+\"										{
															if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
																error("Includes nested too deeply");

															include_lineno_stack[include_stack_ptr] = yylineno;
															include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
															yytext[yyleng - 1] = 0;
															char **i = gIncludePaths;
															char p[PATH_MAX];
															
															do
															{
																strcpy(p, *i++);
																strcat(p, "/");
																strcat(p, yytext);
	
																yyin = fopen(p, "r");
															}
															while (yyin == NULL && *i);
															
															if (yyin == NULL) error("Could not open file %s for input", yytext);
															gFileName[include_stack_ptr - 1] = strdup(p);
															
															yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
															BEGIN(INITIAL);
														}
<<EOF>>													{
															if (macro_stack_ptr > 0)
															{
																macro_stack_ptr--;
																yy_delete_buffer(YY_CURRENT_BUFFER);
																yy_switch_to_buffer(macro_stack[macro_stack_ptr]);
															}
															else if (--include_stack_ptr < 0)
																return rEOF;
															else
															{
																yy_delete_buffer(YY_CURRENT_BUFFER);
																yy_switch_to_buffer(include_stack[include_stack_ptr]);
																yylineno = include_lineno_stack[include_stack_ptr];
																free(gFileName[include_stack_ptr]);
															}
														}
\#define[ \t]+{ident}/\(								error("Macro's with parameters are not supported yet");
\#define[ \t]+{ident}[ \t]+[^\n]+						AddMacro(yytext);
\#[ \t]+[0-9]+[^\n]*
[ \n\r\t]+
.														return yytext[0];
%%

void AddMacro(char *s)
{
	char *m = s + strlen("#define");
	while (isspace(*m)) m++;
	
	char *c = m;
	while (!isspace(*c)) c++;
	*c++ = 0;
	while (isspace(*c)) c++;
	
	int mn = ST_AddIdent(m);
	
	gMacroTable[mn] = strdup(c);
} /* AddMacro */

void CopyBuf()
{
	size_t size = sbp - sBuf - sizeof(long);
	*(long *)sBuf = size;
	
	yylval = (int)malloc(size + sizeof(long));
	memcpy((void*)yylval, sBuf, size + sizeof(long));
} /* CopyBuf */
