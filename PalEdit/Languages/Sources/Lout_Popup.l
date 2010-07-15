/*	$Id: Lout_Popup.l,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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


  Notes:
    Put together in 2006, by Oscar Lesta, from "CperlFuncPopup.l" and some bits
    of "CtexFuncPopup.l". Don't ask me how this work, it just does it! :-)

  Caveats:
    - This assumes that lout's data is installed under:
      "/boot/home/config/etc/lout/"
      *but*
      if the enviromental variable "$LOUTLIB" is found, it's value is used
      instead (make sure it is correct!).
    - Assumes that *your* files end with .lt (lout's files) and .ld (lout's
      databases).
    - Assumes that chapters, sections, subsections, etc. are in the form:
      @Section
        @Title { A Title }
      ie. @Title must *immediately* follow @Chapter, @Section, etc.

    - Untitled chapters/sections/overheads/etc handling needs improvements.
*/


%{
#include "CLanguageAddOn.h"

static const int kMaxNameLength = 70;

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

static void GetNextChar(char* tok, int& result, int max);
static void AddInclude(CLanguageProxy& proxy, const char ext[]);
static void AddSysDataBase(CLanguageProxy& proxy);
static void AddSysInclude(CLanguageProxy& proxy);
static void AddSection(CLanguageProxy& proxy, int level);
%}

ws				[ \n\t]*

%x string

%%
											int string_caller = 0;
\"											string_caller = INITIAL; BEGIN(string); inString = true;
<string>\"									BEGIN(string_caller); inString = false;
"#"[^\n]*\n									// comment line
\@DataBase{ws}\@Reference{ws}\{[^\n]+\}		AddInclude(proxy, ".ld");
\@SysDataBase{ws}\@Reference{ws}\{[^\n]+\}	AddSysDataBase(proxy);
\@Include{ws}\{[^\n]+\}						AddInclude(proxy, ".lt");
\@SysInclude{ws}\{[^\n]+\}					AddSysInclude(proxy);
\@Overhead{ws}\@Title{ws}\{[^\n]+\}			AddSection(proxy, 0);	// for Slides
\@Chapter{ws}\@PartTitle{ws}\{[^\n]+\}		AddSection(proxy, 0);
\@Chapter{ws}\@PartNumber{ws}\{[^\n]+\}		AddSection(proxy, 0);
\@Chapter{ws}\@Title{ws}\{[^\n]+\}			AddSection(proxy, 1);
\@Section{ws}\@Title{ws}\{[^\n]+\}			AddSection(proxy, 2);
\@SubSection{ws}\@Title{ws}\{[^\n]+\}		AddSection(proxy, 3);
\@SubSubSection{ws}\@Title{ws}\{[^\n]+\}	AddSection(proxy, 4);
<<EOF>>										BEGIN(INITIAL); return 0;
%%


_EXPORT void ScanForFunctions(CLanguageProxy& proxy)
{
	static sem_id fps = create_sem(1, "func popup semaphore");

	if (acquire_sem(fps) == B_NO_ERROR) {
		gBuf = proxy.Text();
		gBufSize = proxy.Size();
		gIndx = 0;
		inString = false;

		yyrestart(NULL);
		yylex(proxy);

		release_sem(fps);
	}
}


void GetNextChar(char* tok, int& result, int max)
{
	int c = gBuf[gIndx++];

	if (inString)
		;
	else if (c == '#') {
		int space = 0;

		do {
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
}


char* trimstring(char str[])
{
	char *start, *end;

	start = str;
	while (isspace(*start))					// skip leading whites...
		start++;

	if (*start == '\0')						// are we at string's end yet?
		return start;

	end = start + strlen(start) - 1;
	while ((end > start) && isspace(*end))	// skip trailing whites...
		end--;

	*++end = '\0';

	return start;
}


static void AddInclude(CLanguageProxy& proxy, const char ext[])
{
	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);
	char* tmp = (char*) calloc(1, strlen(s) + strlen(ext) + 1);
	if (!tmp)
		return;

	strcat(tmp, s);
	if (!strstr(tmp, ext))
		strcat(tmp, ext);
	proxy.AddInclude(s, tmp);
	free(tmp);
}


static void AddSysDataBase(CLanguageProxy& proxy)
{
	static const char* kLoutDataDir = "/boot/home/config/etc/lout/data/";
	static const size_t kLoutDataDirLen = strlen(kLoutDataDir);
	static const char* kLoutLib = getenv("LOUTLIB");

	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);
	char* tmp;

	if (kLoutLib != NULL) {
		tmp = (char*) calloc(1, strlen(kLoutLib) + 6 + strlen(s) + 4);
		if (!tmp)
			return;
		strcat(tmp, kLoutLib);
		strcat(tmp, "/data/");
	}
	else {
		tmp = (char*) calloc(1, kLoutDataDirLen + strlen(s) + 4);
		if (!tmp)
			return;
		strcat(tmp, kLoutDataDir);
	}

	strcat(tmp, s);
	strcat(tmp, ".ld");
	proxy.AddInclude(s, tmp);
	free(tmp);
}


static void AddSysInclude(CLanguageProxy& proxy)
{
	static const char* kLoutIncludeDir = "/boot/home/config/etc/lout/include/";
	static const size_t kLoutIncludeDirLen = strlen(kLoutIncludeDir);
	static const char* kLoutLib = getenv("LOUTLIB");

	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);
	char* tmp;

	if (kLoutLib != NULL) {
		tmp = (char*) calloc(1, strlen(kLoutLib) + 9 + strlen(s) + 4);
		if (!tmp)
			return;
		strcat(tmp, kLoutLib);
		strcat(tmp, "/include/");
	}
	else {
		tmp = (char*) calloc(1, kLoutIncludeDirLen + strlen(s) + 4);
		if (!tmp)
			return;
		strcat(tmp, kLoutIncludeDir);
	}

	strcat(tmp, s);
	proxy.AddInclude(s, tmp);
	free(tmp);
}


static void AddSection(CLanguageProxy& proxy, int level)
{
	static const char* kUnnamed = "<unnamed>";
	static const int kUnnamedLen = strlen(kUnnamed);

	char *s, *i = yytext;

	s = strrchr(i, '}');
	if (!s) return;
	*s = 0;
	s = strrchr(i, '{');
	if (!s) return;

	s++;

	s = trimstring(s);
	char* tmp;

	if (*s && s[0] != ' ') {
		tmp = (char*) calloc(1, strlen(s) + level * 2 + 1);	// level * 2 spaces
		if (!tmp)
			return;
		while (level--)
			strcat(tmp, "  ");				// 2 spaces for each level deep
		strncat(tmp, s, kMaxNameLength);	// trucate if title is too long.
	}
	else {
		tmp = (char*) calloc(1, kUnnamedLen + level * 2 + 1);	// level * 2 spaces
		if (!tmp)
			return;
		while (level--)
			strcat(tmp, "  ");		// 2 spaces for each level deep
		strcat(tmp, kUnnamed);
	}
	proxy.AddFunction(tmp, s, 0);
	free(tmp);
}
