/*	$Id: Tex_Popup.l,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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

	Created: 10/16/97 13:56:45
*/

%{
#include "CLanguageAddOn.h"
#define MAXTITLE_LEN 80

static const char *gBuf = NULL;
static int gIndx;
static int gBufSize;

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
static void Add(int level, CLanguageProxy& proxy, int hasOptional);
%}

withoptional	[ \t\n]*\[[^\]]*\][ \t\n]*\{[^}]+\}.*\n
withoutoptional [ \t\n]*\{[^}]+\}.*\n

%%
\%.*\-\*\-\{[^}]+\}\-\*\-.*\n							Add(0,proxy,0);
\%[^\n]+\n										// comment line
"\\part"\*?{withoptional}							Add(0, proxy,1);
"\\part"\*?{withoutoptional}							Add(0, proxy,0);
"\\chapter"\*?{withoptional}						Add(1, proxy,1);
"\\chapter"\*?{withoutoptional}						Add(1, proxy,0);
"\\section"\*?{withoptional}							Add(2, proxy,1);
"\\section"\*?{withoutoptional}						Add(2, proxy,0);
"\\subsection"\*?{withoptional}						Add(3, proxy,1);
"\\subsection"\*?{withoutoptional}					Add(3, proxy,0);
"\\subsubsection"\*?{withoptional}					Add(4, proxy,1);
"\\subsubsection"\*?{withoutoptional}				Add(4, proxy,0);
"\\paragraph"\*?{withoptional}						Add(5, proxy,1);
"\\paragraph"\*?{withoutoptional}					Add(5, proxy,0);
"\\subparagraph"\*?{withoptional}					Add(6, proxy,1);
"\\subparagraph"\*?{withoutoptional}				Add(6, proxy,0);
.
%%

_EXPORT void ScanForFunctions(CLanguageProxy& proxy)
{
	static sem_id fps = create_sem(1, "tex func popup semaphore");
	
	if (acquire_sem(fps) == B_NO_ERROR)
	{
		gBuf = proxy.Text();
		gBufSize = proxy.Size();
		gIndx = 0;

		yyrestart(NULL);
		yylex(proxy);
		
		release_sem(fps);
	}
} /* ScanForFunctions */

void GetNextChar(char *tok, int& result, int)
{
	int c = gBuf[gIndx++];
	
	result = (gIndx < gBufSize) ? (tok[0] = c, 1) : YY_NULL;
} /* GetNextChar */

static void Add(int level, CLanguageProxy& proxy, int hasOptional)
{
	char *s, *f, *select;
	int i, nesting, argLength = 0;
	char c = 0;
	char braceOpen,braceClose;

/*	This is a bit of a kludge.  More text from the flex regexp is acquired
	and the brace nesting problem is handled here.
	The simplest solution to this problem would be to construct the proper
	regular expression using the flex part.  But this cannot properly deal with
	nested brackets.  If someone knows of an elegant way to search for 
	something like the contents of the outermost braces using a regexp in:
	\section{\protect{Nb$_{3}$Sn} in Industry} {not part of the title} 
 	please let me know.
*/

/* \%[^\n]+\n													// comment line
*/

/* When present, the optional argument will be used instead of the manditory one.*/
/* "\\section"\*?{realtitle}								Add(2, proxy,0); */
	
	if (hasOptional)
	{
		braceOpen = '[';
		braceClose = ']';
	} 
	else
	{
		braceOpen = '{';
		braceClose = '}';
	}

	s = strchr(yytext, braceOpen); /* Must be present since flex called us.*/
	s = s + 1;  /* move past the opening brace */
	nesting=1; /* we are one level deep because of the first brace */
	for (i=0; (c != '\0') && (nesting>0); i++)
	{
		c = *(s+i);
		if (c== braceOpen )
		{
			nesting++;
		}
		else if (c == braceClose)
		{
			nesting--;
		}
		argLength=i;  /* This is a bit unnecessary but it makes for code legibility.*/
	}


/* if there are nested braces in the section heading and the final closing brace does not appear */
/* on the same line as the first closing brace, then "nesting" will not be zero. */
/* The current behavior is to put "..." after the last reasonable thing it found. */ 
/* Note that this regexp method will not allow you to put two section commands on the same line.*/

/* The convoluted methods here are needed to gracefully accommodate legitimate LaTeX cases */
/* such as */
/*
 \section{\protect{N\lowercase{b}$_3$S\lowercase{n}}
in Industry}
*/

	if (nesting==0) /* Indicates found argument properly */
	{		
		f = (char *)calloc(1, argLength + 1 + level * 2); /* the plus 1 is for the '\0' */
		select = (char *)calloc(1, argLength + 1);
		if (select && f){ /* When cannot calloc, just fail silently/benignly and do nothing */
				while (level--)
					strcat(f, "  "); /* 2 spaces for each level deep */
				strncat(f, s,argLength);
				strncat(select,s,argLength);		
				proxy.AddFunction(f, select, gIndx - yy_n_chars - 1);
				
				free(f);
				free(select);
		}
	} 
	else 
	{
		f = (char *)calloc(1, argLength + 1 + level * 2 + 2 ); /* add 2 for - '\n' + "..." */
		select = (char *)calloc(1, argLength + 1);
		if (select && f){ /* When cannot calloc, just fail silently/benignly and do nothing */
				while (level--)
					strcat(f, "  "); /* 2 spaces for each level deep */
				strncat(select,s,argLength);		
				argLength--;		/* strip off the "\n" */
				strncat(f, s,argLength);
				strcat(f,"...");
				proxy.AddFunction(f, select, gIndx - yy_n_chars - 1);
				
				free(f);
				free(select);
		}
	}

} /* Add */
