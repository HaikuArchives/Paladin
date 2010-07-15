%{

/*	$Id: rez_parser.y,v 1.3 2009/06/20 14:24:30 darkwyrm Exp $
	
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

	Created: 12/02/98 15:37:42
*/

#include "rez.h"
#include "RTypes.h"
#include "RState.h"
#include "REval.h"

#include <stdio.h>
#include <string.h>

#include <List.h>
#include <ByteOrder.h>

//#define alloca malloc
#define YYDEBUG 1

#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL

extern FILE *yyin;

extern int yylex();

#define yyerror(s)	error(s)

static RState *sState;
static RElem *head;

#define RS(s)	((RState *)(s))
#define RSV(s)	((RSValue *)(s))
#define LST(s)	((BList *)(s))
#define RE(s)	((REval *)(s))

#define CHECKSTATE	if (!sState) error("Internal error (sState == NULL on line %d)", __LINE__)
%}


%token ALIGN ARRAY AS BINARY BITSTRING BOOLEAN BYTE CASE CHANGE CHAR CSTRING
%token DECIMAL DELETE DEREZ FILL HEX INTEGER LITERAL LONGINT OCTAL INCLUDE
%token POINT PSTRING READ RECT RES REZ STRING SWITCH rTYPE WSTRING
%token LABEL IDENT REZVAR STRING NUMBER AND OR EQ NE LT LE GT GE SHR SHL
%token STR_CONST WIDE BIT NIBBLE WORD LONG KEY HEXSTRING UNSIGNED rEOF COUNTOF
%token ARRAYINDEX LBITFIELD LBYTE FORMAT RESSIZE RESOURCE LLONG SHELL LWORD HEX_CONST


%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQ NE
%left LT LE GT GE
%left SHL SHR
%left '+' '-'
%left '*' '/' '%'
%right NEGATE FLIP NOT

%%
s				:	type								{ YYACCEPT; }
				|	resheader '{' datalist '}' ';'		{ if (head) head->Write(); WriteResource($1); YYACCEPT; }
				|	readheader							{ YYACCEPT; }
				|	incl								{ YYACCEPT; }
				|	error								{ return 1; }
				|	rEOF								{ return 1; }
				;

incl			:	INCLUDE strconst ';'					{ Include((char *)$2); }
				;

type			:	rTYPE NUMBER '{' datadecl '}' ';'		{ RState::FinishType($2, RS($4)); }
				|	rTYPE NUMBER AS NUMBER ';'				{ RState::CopyType($2, $4); }
				;

datadecl		:	datatype
				|	LABEL
				|	datadecl ';' datatype					{ $$ = $1; if ($3) RS($1)->SetNext(RS($3)); }
				;

datatype		:	booleantype
				|	chartype
				|	numerictype
				|	stringtype
				|	pointtype
				|	recttype
				|	arraytype
				|	switchtype
				|	filltype
				|	aligntype
				|											{ $$ = 0; }
				;

booleantype		:	BOOLEAN									{ $$ = (int)new RSNrValue(8); }
				|	BOOLEAN '='	e							{ $$ = (int)new RSNrValue(8); RSV($$)->SetDefaultValue($3); }
				|	BOOLEAN symboliclist					{ $$ = (int)new RSNrValue(8); RSV($$)->AddIdentifiers(LST($2)); }
				;

numerictype		:	numericsize
				|	UNSIGNED numericsize					{ $$ = $2; }
				|	radix numericsize						{ $$ = $2; }
				|	UNSIGNED radix numericsize				{ $$ = $3; }
				|	numericsize	'=' e						{ $$ = $1; RSV($$)->SetDefaultValue($3); }
				|	UNSIGNED numericsize '=' e				{ $$ = $2; RSV($$)->SetDefaultValue($4); }
				|	radix numericsize '=' e					{ $$ = $2; RSV($$)->SetDefaultValue($4); }
				|	UNSIGNED radix numericsize '=' e		{ $$ = $3; RSV($$)->SetDefaultValue($5); }
				|	numericsize symboliclist				{ $$ = $1; RSV($$)->AddIdentifiers(LST($2)); }
				|	UNSIGNED numericsize symboliclist		{ $$ = $2; RSV($$)->AddIdentifiers(LST($3)); }
				|	radix numericsize symboliclist			{ $$ = $2; RSV($$)->AddIdentifiers(LST($3)); }
				|	UNSIGNED radix numericsize symboliclist	{ $$ = $3; RSV($$)->AddIdentifiers(LST($4)); }
				;

radix			:	HEX
				|	DECIMAL
				|	OCTAL
				|	BINARY
				|	LITERAL
				;

numericsize		:	BITSTRING								{ $$ = (int)new RSNrValue(32); }
				|	BITSTRING '[' e ']'						{ $$ = (int)new RSNrValue(REvaluate(RE($3), head)); }
				|	BYTE									{ $$ = (int)new RSNrValue(8); }
				|	INTEGER									{ $$ = (int)new RSNrValue(16); }
				|	LONGINT									{ $$ = (int)new RSNrValue(32); }
				;

chartype		:	CHAR									{ $$ = (int)new RSNrValue(8); }
				|	CHAR '=' e								{ $$ = (int)new RSNrValue(8); RSV($$)->SetDefaultValue($3); }
				|	CHAR symboliclist						{ $$ = (int)new RSNrValue(8); RSV($$)->AddIdentifiers(LST($2)); }
				;

stringtype		:	stringspecifier							{ $$ = (int)new RSStringValue($1, 0); }
				|	stringspecifier '[' e ']'				{ $$ = (int)new RSStringValue($1, REvaluate(RE($3), head)); }
				|	stringspecifier '=' strconst			{ $$ = (int)new RSStringValue($1, 0); RSV($$)->SetDefaultValue($3); }
				|	stringspecifier '[' e ']' '=' strconst	{ $$ = (int)new RSStringValue($1, REvaluate(RE($3), head)); RSV($$)->SetDefaultValue($6); }
				|	stringspecifier symboliclist			{ $$ = (int)new RSStringValue($1, 0); RSV($$)->AddIdentifiers(LST($2)); } 
				|	stringspecifier '[' e ']' symboliclist	{ $$ = (int)new RSStringValue($1, REvaluate(RE($3), head)); RSV($$)->AddIdentifiers(LST($5)); } 
				;

stringspecifier	:	STRING									{ $$ = RSStringValue::skStr; }
				|	HEXSTRING								{ $$ = RSStringValue::skHex; }
				|	PSTRING									{ $$ = RSStringValue::skPStr; }
				|	WSTRING									{ $$ = RSStringValue::skWStr; }
				|	CSTRING									{ $$ = RSStringValue::skCStr; }
				;

pointtype		:	POINT									{ $$ = (int)new RSArray(new RSNrValue(16), 0, 2); }
				|	POINT '=' dataarray datalist '}'		{ error("Unimplemented constant declaration"); }
				|	POINT symboliclist						{ error("Unimplemented constant declaration"); }
				;

recttype		:	RECT									{ $$ = (int)new RSArray(new RSNrValue(16), 0, 4); }
				|	RECT '=' dataarray datalist '}'			{ error("Unimplemented constant declaration"); }
				|	RECT symboliclist						{ error("Unimplemented constant declaration"); }
				;

arraytype		:	ARRAY '{' datadecl '}'					{ $$ = (int)new RSArray(RS($3)); }
				|	WIDE ARRAY '{' datadecl '}'				{ $$ = (int)new RSArray(RS($4)); }
				|	ARRAY IDENT '{' datadecl '}'			{ $$ = (int)new RSArray(RS($4), $2); }
				|	WIDE ARRAY IDENT '{' datadecl '}'		{ $$ = (int)new RSArray(RS($5), $3); }
				|	ARRAY '[' e ']' '{' datadecl '}'		{ $$ = (int)new RSArray(RS($6), 0, $3); }
				|	WIDE ARRAY '[' e ']' '{' datadecl '}'	{ $$ = (int)new RSArray(RS($7), 0, $3); }
				;

switchtype		:	SWITCH '{' casestmts '}'				{ $$ = (int)new RSSwitch(LST($3)); }
				;

casestmts		:	casestmts casestmt						{ $$ = $1; LST($1)->AddItem(RS($2)); }
				|	casestmt								{ $$ = (int)new BList; LST($$)->AddItem(RS($1)); }
				;

casestmt		:	CASE IDENT ':' casebody ';'				{ $$ = (int)new RCase($2, RS($4)); }
				;

casebody		:	casebody ';' caseline					{ $$ = $1; RS($1)->SetNext(RS($3)); }
				|	caseline
				;

caseline		:	datatype
				|	LABEL
				|	KEY keytype '=' e						{ $$ = $2; RSV($$)->SetDefaultValue($4); }
				;

keytype			:	BOOLEAN
				|	CHAR
				|	POINT
				|	RECT
				|	numericsize
				|	stringspecifier
				;

filltype		:	FILL fillsize							{ $$ = (int)new RSNrValue($2); RSV($$)->SetDefaultValue((int)RValue(0)); }
				|	FILL fillsize '[' e ']'					{
																int cnt = REvaluate(RE($4), head);
																RSNrValue *s, *t = NULL;

																while (cnt--)
																{
																	s = new RSNrValue($2);
																	s->SetDefaultValue((int)RValue(0));
																	s->SetNext(t);
																	t = s;
																}
																$$ = (int)s;
															}
				;

fillsize		:	BIT										{ $$ = 1; }
				|	NIBBLE									{ $$ = 4; }
				|	BYTE									{ $$ = 8; }
				|	WORD									{ $$ = 16; }
				|	LONG									{ $$ = 32; }
				;

aligntype		:	ALIGN alignsize
				;

alignsize		:	NIBBLE
				|	BYTE
				|	WORD
				|	LONG
				;

symboliclist	:	symbolicvalue							{ $$ = (int)new BList; LST($$)->AddItem(RS($1)); }
				|	symboliclist ',' symbolicvalue			{ $$ = $1; LST($1)->AddItem(RS($3)); }
				;

symbolicvalue	:	IDENT									{ $$ = (int)new RSymbol($1, 0); }
				|	IDENT '=' e								{ $$ = (int)new RSymbol($1, REvaluate(RE($3), head)); }
				|	IDENT '=' dataarray datalist '}'		{ $$ = (int)new RSymbol($1, 0); }
				;

e				:	'(' e ')'								{ $$ = $2; }
				|	e '+' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoPlus); }
				|	e '-' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoMinus); }
				|	e '*' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoMultiply); }
				|	e '/' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoDivide); }
				|	e '%' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoModulus); }
				|	e SHL e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoSHL); }
				|	e SHR e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoSHR); }
				|	e '&' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoBitAnd); }
				|	e '|' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoBitOr); }
				|	e '^' e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoXPwrY); }
				|	'~' e %prec FLIP						{ $$ = (long)RUnaryOp(RE($2), reoFlip); }
				|	'!' e %prec NOT							{ $$ = (long)RUnaryOp(RE($2), reoNot); }
				|	'-' e %prec NEGATE						{ $$ = (long)RUnaryOp(RE($2), reoNegate); }
				|	e AND e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoAnd); }
				|	e OR e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoOr); }
				|	e LT e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoLT); }
				|	e GT e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoGT); }
				|	e LE e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoLE); }
				|	e GE e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoGE); }
				|	e EQ e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoEQ); }
				|	e NE e									{ $$ = (long)RBinaryOp(RE($1), RE($3), reoNE); }
				|	NUMBER									{ $$ = (long)RValue($1); }
				|	IDENT									{ $$ = (long)RIdentifier($1); }
				|	f
				;
		
f				:	COUNTOF '(' IDENT ')'					{ $$ = (long)RFunction(refCountOf, $3); }
				|	LBITFIELD '(' IDENT ',' NUMBER ',' NUMBER ')'
															{ $$ = (long)RFunction(refCopyBits, $3, $5, $7); }
				|	LBYTE '(' IDENT ')'						{ $$ = (long)RFunction(refCopyBits, $3, 0, 8); }
				|	LWORD '(' IDENT ')'						{ $$ = (long)RFunction(refCopyBits, $3, 0, 16); }
				|	LLONG '(' IDENT ')'						{ $$ = (long)RFunction(refCopyBits, $3, 0, 32); }
				;

resheader		:	RES NUMBER '(' NUMBER ',' strconst ')'
															{
																sState = RState::FirstState($2);
																if (!sState)
																{
																	int t = ntohl($2);
																	error("Undefined resource type: %4.4s", &t);
																}
																head = NULL;
																$$ = (int)new ResHeader($2, $4, $6);
															}
				|	RES NUMBER '(' NUMBER ')'
															{
																sState = RState::FirstState($2);
																if (!sState)
																{
																	int t = ntohl($2);
																	error("Undefined resource type: %4.4s", &t);
																}
																head = NULL;
																$$ = (int)new ResHeader((int)$2, (int)$4, (int)0);
															}
				;

readheader		:	READ NUMBER '(' NUMBER ',' strconst ')' strconst ';'
															{ WriteResource((char *)$8, $2, $4, (char *)$6); }
				|	READ NUMBER '(' NUMBER ')' strconst ';'
															{ WriteResource((char *)$6, $2, $4, NULL); }
				;

datalist		:	datalist ',' data
				|	datalist ';' data
				|	data
				;

data			:	e										{ CHECKSTATE; sState = sState->Shift($1, tInt, &head); }
/*				|	IDENT									{ CHECKSTATE; sState = sState->Shift($1, tIdent, &head); }*/
				|	strconst								{ CHECKSTATE; sState = sState->Shift($1, tString, &head); free((char *)$1); }
				|	hexconst								{ CHECKSTATE; sState = sState->Shift($1, tRaw, &head); free((char *)$1); }
				|	switchdata datalist '}'
				|	dataarray datalist '}'					{ CHECKSTATE; sState = sState->Shift(0, tArrayEnd, &head); }
				|
				;

dataarray		:	'{'										{ CHECKSTATE; sState = sState->Shift(0, tArray, &head); }
				;

switchdata		:	IDENT '{'								{ CHECKSTATE; sState = sState->Shift($1, tCase, &head); }
				;

hexconst		:	HEX_CONST
				|	hexconst HEX_CONST						{ char *t, *a, *b;
															  long sa, sb;

															  a = (char *)$1;	sa = *(long *)a;
															  b = (char *)$2;	sb = *(long *)b;

															  t = (char *)malloc(sa + sb + sizeof(long));
															  if (!t) error("insufficient memory");

															  memcpy(t + sizeof(long), a + sizeof(long), sa);
															  memcpy(t + sizeof(long) + sa, b + sizeof(long), sb);
															  *(long*)t = sa + sb;

															  free(a); free(b);
															  $$ = (int)t; }
				;

strconst		:	STR_CONST
				|	strconst STR_CONST						{ char *t, *a, *b;
															  a = (char *)$1;
															  b = (char *)$2;
															  t = (char *)malloc(strlen(a) + strlen(b) + 1);
															  if (!t) error("insufficient memory");
															  strcpy(t, a);
															  strcat(t, b);
															  free(a); free(b);
															  $$ = (int)t; }
				|	SHELL '(' STR_CONST ')'					{ $$ = (int)strdup(getenv((char *)$3)); }
				|	FORMAT '(' fmt ')'						{ $$ = $3; }
				;

fmt				:	STR_CONST ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9, $11); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9, $11, $13); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9, $11, $13, $15); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9, $11, $13, $15, $17); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9, $11, $13, $15, $17, $19); $$ = (int)strdup(b); }
				|	STR_CONST ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg ',' farg						{ char b[1024]; sprintf(b, (char *)$1, $3, $5, $7, $9, $11, $13, $15, $17, $19, $21); $$ = (int)strdup(b); }
				;

farg			:	STR_CONST
				|	NUMBER
				;

%%

