/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ALIGN = 258,
     ARRAY = 259,
     AS = 260,
     BINARY = 261,
     BITSTRING = 262,
     BOOLEAN = 263,
     BYTE = 264,
     CASE = 265,
     CHANGE = 266,
     CHAR = 267,
     CSTRING = 268,
     DECIMAL = 269,
     DELETE = 270,
     DEREZ = 271,
     FILL = 272,
     HEX = 273,
     INTEGER = 274,
     LITERAL = 275,
     LONGINT = 276,
     OCTAL = 277,
     INCLUDE = 278,
     POINT = 279,
     PSTRING = 280,
     READ = 281,
     RECT = 282,
     RES = 283,
     REZ = 284,
     STRING = 285,
     SWITCH = 286,
     rTYPE = 287,
     WSTRING = 288,
     LABEL = 289,
     IDENT = 290,
     REZVAR = 291,
     NUMBER = 292,
     AND = 293,
     OR = 294,
     EQ = 295,
     NE = 296,
     LT = 297,
     LE = 298,
     GT = 299,
     GE = 300,
     SHR = 301,
     SHL = 302,
     STR_CONST = 303,
     WIDE = 304,
     BIT = 305,
     NIBBLE = 306,
     WORD = 307,
     LONG = 308,
     KEY = 309,
     HEXSTRING = 310,
     UNSIGNED = 311,
     rEOF = 312,
     COUNTOF = 313,
     ARRAYINDEX = 314,
     LBITFIELD = 315,
     LBYTE = 316,
     FORMAT = 317,
     RESSIZE = 318,
     RESOURCE = 319,
     LLONG = 320,
     SHELL = 321,
     LWORD = 322,
     HEX_CONST = 323,
     NOT = 324,
     FLIP = 325,
     NEGATE = 326
   };
#endif
/* Tokens.  */
#define ALIGN 258
#define ARRAY 259
#define AS 260
#define BINARY 261
#define BITSTRING 262
#define BOOLEAN 263
#define BYTE 264
#define CASE 265
#define CHANGE 266
#define CHAR 267
#define CSTRING 268
#define DECIMAL 269
#define DELETE 270
#define DEREZ 271
#define FILL 272
#define HEX 273
#define INTEGER 274
#define LITERAL 275
#define LONGINT 276
#define OCTAL 277
#define INCLUDE 278
#define POINT 279
#define PSTRING 280
#define READ 281
#define RECT 282
#define RES 283
#define REZ 284
#define STRING 285
#define SWITCH 286
#define rTYPE 287
#define WSTRING 288
#define LABEL 289
#define IDENT 290
#define REZVAR 291
#define NUMBER 292
#define AND 293
#define OR 294
#define EQ 295
#define NE 296
#define LT 297
#define LE 298
#define GT 299
#define GE 300
#define SHR 301
#define SHL 302
#define STR_CONST 303
#define WIDE 304
#define BIT 305
#define NIBBLE 306
#define WORD 307
#define LONG 308
#define KEY 309
#define HEXSTRING 310
#define UNSIGNED 311
#define rEOF 312
#define COUNTOF 313
#define ARRAYINDEX 314
#define LBITFIELD 315
#define LBYTE 316
#define FORMAT 317
#define RESSIZE 318
#define RESOURCE 319
#define LLONG 320
#define SHELL 321
#define LWORD 322
#define HEX_CONST 323
#define NOT 324
#define FLIP 325
#define NEGATE 326




/* Copy the first part of user declarations.  */
#line 1 "rez/Sources/rez_parser.y"


/*	$Id: rez_parser.cpp,v 1.1 2008/12/20 23:35:55 darkwyrm Exp $
	
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

#include <cstdio>
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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 317 "rez/Sources/rez_parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  19
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   781

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  92
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  170
/* YYNRULES -- Number of states.  */
#define YYNSTATES  328

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    91,     2,     2,     2,    76,    71,     2,
      88,    89,    74,    72,    87,    73,     2,    75,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    86,    82,
       2,    83,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    84,     2,    85,    70,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    80,    69,    81,    90,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    77,    78,    79
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,    11,    13,    15,    17,    19,    23,
      30,    36,    38,    40,    44,    46,    48,    50,    52,    54,
      56,    58,    60,    62,    64,    65,    67,    71,    74,    76,
      79,    82,    86,    90,    95,   100,   106,   109,   113,   117,
     122,   124,   126,   128,   130,   132,   134,   139,   141,   143,
     145,   147,   151,   154,   156,   161,   165,   172,   175,   181,
     183,   185,   187,   189,   191,   193,   199,   202,   204,   210,
     213,   218,   224,   230,   237,   245,   254,   259,   262,   264,
     270,   274,   276,   278,   280,   285,   287,   289,   291,   293,
     295,   297,   300,   306,   308,   310,   312,   314,   316,   319,
     321,   323,   325,   327,   329,   333,   335,   339,   345,   349,
     353,   357,   361,   365,   369,   373,   377,   381,   385,   389,
     392,   395,   398,   402,   406,   410,   414,   418,   422,   426,
     430,   432,   434,   436,   441,   450,   455,   460,   465,   473,
     479,   489,   497,   501,   505,   507,   509,   511,   513,   517,
     521,   522,   524,   527,   529,   532,   534,   537,   542,   547,
     551,   557,   565,   575,   587,   601,   617,   635,   655,   677,
     679
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      93,     0,    -1,    95,    -1,   122,    80,   124,    81,    82,
      -1,   123,    -1,    94,    -1,     1,    -1,    57,    -1,    23,
     129,    82,    -1,    32,    37,    80,    96,    81,    82,    -1,
      32,    37,     5,    37,    82,    -1,    97,    -1,    34,    -1,
      96,    82,    97,    -1,    98,    -1,   102,    -1,    99,    -1,
     103,    -1,   105,    -1,   106,    -1,   107,    -1,   108,    -1,
     114,    -1,   116,    -1,    -1,     8,    -1,     8,    83,   120,
      -1,     8,   118,    -1,   101,    -1,    56,   101,    -1,   100,
     101,    -1,    56,   100,   101,    -1,   101,    83,   120,    -1,
      56,   101,    83,   120,    -1,   100,   101,    83,   120,    -1,
      56,   100,   101,    83,   120,    -1,   101,   118,    -1,    56,
     101,   118,    -1,   100,   101,   118,    -1,    56,   100,   101,
     118,    -1,    18,    -1,    14,    -1,    22,    -1,     6,    -1,
      20,    -1,     7,    -1,     7,    84,   120,    85,    -1,     9,
      -1,    19,    -1,    21,    -1,    12,    -1,    12,    83,   120,
      -1,    12,   118,    -1,   104,    -1,   104,    84,   120,    85,
      -1,   104,    83,   129,    -1,   104,    84,   120,    85,    83,
     129,    -1,   104,   118,    -1,   104,    84,   120,    85,   118,
      -1,    30,    -1,    55,    -1,    25,    -1,    33,    -1,    13,
      -1,    24,    -1,    24,    83,   126,   124,    81,    -1,    24,
     118,    -1,    27,    -1,    27,    83,   126,   124,    81,    -1,
      27,   118,    -1,     4,    80,    96,    81,    -1,    49,     4,
      80,    96,    81,    -1,     4,    35,    80,    96,    81,    -1,
      49,     4,    35,    80,    96,    81,    -1,     4,    84,   120,
      85,    80,    96,    81,    -1,    49,     4,    84,   120,    85,
      80,    96,    81,    -1,    31,    80,   109,    81,    -1,   109,
     110,    -1,   110,    -1,    10,    35,    86,   111,    82,    -1,
     111,    82,   112,    -1,   112,    -1,    97,    -1,    34,    -1,
      54,   113,    83,   120,    -1,     8,    -1,    12,    -1,    24,
      -1,    27,    -1,   101,    -1,   104,    -1,    17,   115,    -1,
      17,   115,    84,   120,    85,    -1,    50,    -1,    51,    -1,
       9,    -1,    52,    -1,    53,    -1,     3,   117,    -1,    51,
      -1,     9,    -1,    52,    -1,    53,    -1,   119,    -1,   118,
      87,   119,    -1,    35,    -1,    35,    83,   120,    -1,    35,
      83,   126,   124,    81,    -1,    88,   120,    89,    -1,   120,
      72,   120,    -1,   120,    73,   120,    -1,   120,    74,   120,
      -1,   120,    75,   120,    -1,   120,    76,   120,    -1,   120,
      47,   120,    -1,   120,    46,   120,    -1,   120,    71,   120,
      -1,   120,    69,   120,    -1,   120,    70,   120,    -1,    90,
     120,    -1,    91,   120,    -1,    73,   120,    -1,   120,    38,
     120,    -1,   120,    39,   120,    -1,   120,    42,   120,    -1,
     120,    44,   120,    -1,   120,    43,   120,    -1,   120,    45,
     120,    -1,   120,    40,   120,    -1,   120,    41,   120,    -1,
      37,    -1,    35,    -1,   121,    -1,    58,    88,    35,    89,
      -1,    60,    88,    35,    87,    37,    87,    37,    89,    -1,
      61,    88,    35,    89,    -1,    67,    88,    35,    89,    -1,
      65,    88,    35,    89,    -1,    28,    37,    88,    37,    87,
     129,    89,    -1,    28,    37,    88,    37,    89,    -1,    26,
      37,    88,    37,    87,   129,    89,   129,    82,    -1,    26,
      37,    88,    37,    89,   129,    82,    -1,   124,    87,   125,
      -1,   124,    82,   125,    -1,   125,    -1,   120,    -1,   129,
      -1,   128,    -1,   127,   124,    81,    -1,   126,   124,    81,
      -1,    -1,    80,    -1,    35,    80,    -1,    68,    -1,   128,
      68,    -1,    48,    -1,   129,    48,    -1,    66,    88,    48,
      89,    -1,    62,    88,   130,    89,    -1,    48,    87,   131,
      -1,    48,    87,   131,    87,   131,    -1,    48,    87,   131,
      87,   131,    87,   131,    -1,    48,    87,   131,    87,   131,
      87,   131,    87,   131,    -1,    48,    87,   131,    87,   131,
      87,   131,    87,   131,    87,   131,    -1,    48,    87,   131,
      87,   131,    87,   131,    87,   131,    87,   131,    87,   131,
      -1,    48,    87,   131,    87,   131,    87,   131,    87,   131,
      87,   131,    87,   131,    87,   131,    -1,    48,    87,   131,
      87,   131,    87,   131,    87,   131,    87,   131,    87,   131,
      87,   131,    87,   131,    -1,    48,    87,   131,    87,   131,
      87,   131,    87,   131,    87,   131,    87,   131,    87,   131,
      87,   131,    87,   131,    -1,    48,    87,   131,    87,   131,
      87,   131,    87,   131,    87,   131,    87,   131,    87,   131,
      87,   131,    87,   131,    87,   131,    -1,    48,    -1,    37,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    92,    92,    93,    94,    95,    96,    97,   100,   103,
     104,   107,   108,   109,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   125,   126,   127,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     144,   145,   146,   147,   148,   151,   152,   153,   154,   155,
     158,   159,   160,   163,   164,   165,   166,   167,   168,   171,
     172,   173,   174,   175,   178,   179,   180,   183,   184,   185,
     188,   189,   190,   191,   192,   193,   196,   199,   200,   203,
     206,   207,   210,   211,   212,   215,   216,   217,   218,   219,
     220,   223,   224,   239,   240,   241,   242,   243,   246,   249,
     250,   251,   252,   255,   256,   259,   260,   261,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   291,   292,   294,   295,   296,   299,   310,
     323,   325,   329,   330,   331,   334,   336,   337,   338,   339,
     340,   343,   346,   349,   350,   367,   368,   377,   378,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   393,
     394
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ALIGN", "ARRAY", "AS", "BINARY",
  "BITSTRING", "BOOLEAN", "BYTE", "CASE", "CHANGE", "CHAR", "CSTRING",
  "DECIMAL", "DELETE", "DEREZ", "FILL", "HEX", "INTEGER", "LITERAL",
  "LONGINT", "OCTAL", "INCLUDE", "POINT", "PSTRING", "READ", "RECT", "RES",
  "REZ", "STRING", "SWITCH", "rTYPE", "WSTRING", "LABEL", "IDENT",
  "REZVAR", "NUMBER", "AND", "OR", "EQ", "NE", "LT", "LE", "GT", "GE",
  "SHR", "SHL", "STR_CONST", "WIDE", "BIT", "NIBBLE", "WORD", "LONG",
  "KEY", "HEXSTRING", "UNSIGNED", "rEOF", "COUNTOF", "ARRAYINDEX",
  "LBITFIELD", "LBYTE", "FORMAT", "RESSIZE", "RESOURCE", "LLONG", "SHELL",
  "LWORD", "HEX_CONST", "'|'", "'^'", "'&'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "NOT", "FLIP", "NEGATE", "'{'", "'}'", "';'", "'='", "'['", "']'",
  "':'", "','", "'('", "')'", "'~'", "'!'", "$accept", "s", "incl", "type",
  "datadecl", "datatype", "booleantype", "numerictype", "radix",
  "numericsize", "chartype", "stringtype", "stringspecifier", "pointtype",
  "recttype", "arraytype", "switchtype", "casestmts", "casestmt",
  "casebody", "caseline", "keytype", "filltype", "fillsize", "aligntype",
  "alignsize", "symboliclist", "symbolicvalue", "e", "f", "resheader",
  "readheader", "datalist", "data", "dataarray", "switchdata", "hexconst",
  "strconst", "fmt", "farg", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   124,
      94,    38,    43,    45,    42,    47,    37,   324,   325,   326,
     123,   125,    59,    61,    91,    93,    58,    44,    40,    41,
     126,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    92,    93,    93,    93,    93,    93,    93,    94,    95,
      95,    96,    96,    96,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    98,    98,    98,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
     100,   100,   100,   100,   100,   101,   101,   101,   101,   101,
     102,   102,   102,   103,   103,   103,   103,   103,   103,   104,
     104,   104,   104,   104,   105,   105,   105,   106,   106,   106,
     107,   107,   107,   107,   107,   107,   108,   109,   109,   110,
     111,   111,   112,   112,   112,   113,   113,   113,   113,   113,
     113,   114,   114,   115,   115,   115,   115,   115,   116,   117,
     117,   117,   117,   118,   118,   119,   119,   119,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   121,   121,   121,   121,   121,   122,   122,
     123,   123,   124,   124,   124,   125,   125,   125,   125,   125,
     125,   126,   127,   128,   128,   129,   129,   129,   129,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   131,
     131
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     5,     1,     1,     1,     1,     3,     6,
       5,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1,     3,     2,     1,     2,
       2,     3,     3,     4,     4,     5,     2,     3,     3,     4,
       1,     1,     1,     1,     1,     1,     4,     1,     1,     1,
       1,     3,     2,     1,     4,     3,     6,     2,     5,     1,
       1,     1,     1,     1,     1,     5,     2,     1,     5,     2,
       4,     5,     5,     6,     7,     8,     4,     2,     1,     5,
       3,     1,     1,     1,     4,     1,     1,     1,     1,     1,
       1,     2,     5,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     3,     1,     3,     5,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     4,     8,     4,     4,     4,     7,     5,
       9,     7,     3,     3,     1,     1,     1,     1,     3,     3,
       0,     1,     2,     1,     2,     1,     2,     4,     4,     3,
       5,     7,     9,    11,    13,    15,    17,    19,    21,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     6,     0,     0,     0,     0,     7,     0,     5,     2,
       0,     4,   155,     0,     0,     0,     0,     0,     0,     1,
     150,     0,     0,   156,     8,     0,     0,     0,    24,   131,
     130,     0,     0,     0,     0,     0,   153,     0,   151,     0,
       0,     0,   145,   132,     0,   144,   150,   150,   147,   146,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    45,
      25,    47,    50,    63,    41,     0,    40,    48,    44,    49,
      42,    64,    61,    67,    59,     0,    62,    12,     0,    60,
       0,     0,    11,    14,    16,     0,    28,    15,    17,    53,
      18,    19,    20,    21,    22,    23,   152,     0,     0,     0,
       0,     0,   131,   121,     0,   119,   120,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   150,   150,     0,     0,
     154,     0,   158,   157,     0,     0,     0,   139,    10,   100,
      99,   101,   102,    98,     0,    24,     0,     0,   105,     0,
      27,   103,     0,    52,    95,    93,    94,    96,    97,    91,
       0,    66,     0,    69,     0,     0,     0,    29,     0,    24,
      30,     0,    36,     0,     0,    57,     0,     0,     0,     0,
       0,   108,   122,   123,   128,   129,   124,   126,   125,   127,
     115,   114,   117,   118,   116,   109,   110,   111,   112,   113,
       3,   143,   142,   149,   148,   170,   169,   159,     0,     0,
       0,    24,     0,     0,     0,     0,    26,     0,    51,     0,
     150,   150,     0,     0,    78,     0,    24,     0,    31,     0,
      37,     9,    13,     0,    38,    32,    55,     0,   133,     0,
     135,   137,   136,     0,     0,   141,   138,     0,    70,     0,
      46,   106,   150,   104,     0,     0,     0,     0,    76,    77,
      24,     0,     0,     0,    39,    33,    34,    54,     0,   160,
       0,    72,    24,     0,    92,    65,    68,    24,     0,    71,
       0,    35,     0,    58,     0,     0,   140,     0,   107,    83,
       0,    82,     0,    81,    73,    24,    56,     0,   161,    74,
      85,    86,    87,    88,    89,    90,     0,    79,     0,   134,
       0,     0,    80,    75,   162,    84,     0,   163,     0,   164,
       0,   165,     0,   166,     0,   167,     0,   168
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,   223,   224,   292,
     293,   306,    94,   159,    95,   143,   150,   151,    42,    43,
      10,    11,    44,    45,    46,    47,    48,    49,    51,   207
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -160
static const yytype_int16 yypact[] =
{
      96,  -160,    61,    -6,    -3,     3,  -160,    46,  -160,  -160,
      28,  -160,  -160,    37,    42,   -19,    81,    85,    11,  -160,
     -23,    64,   101,  -160,  -160,   141,   143,   151,   375,   107,
    -160,   104,   111,   118,   133,   138,  -160,   232,  -160,   232,
     232,   232,   611,  -160,    39,  -160,   -23,   -23,   171,   183,
     131,   159,   166,    74,    78,   182,   106,   -33,  -160,   184,
     -14,  -160,    13,  -160,  -160,    90,  -160,  -160,  -160,  -160,
    -160,    17,  -160,    18,  -160,   186,  -160,  -160,   274,  -160,
     407,    55,  -160,  -160,  -160,   196,    19,  -160,  -160,   -22,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,   248,   249,   250,
     252,   254,  -160,  -160,   425,  -160,  -160,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   209,   -23,   -23,    63,    65,
    -160,    -7,  -160,  -160,    61,    61,    61,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,   215,   375,   232,   232,   213,   232,
     217,  -160,   232,   217,  -160,  -160,  -160,  -160,  -160,   218,
     220,   217,   220,   217,   293,    14,   196,    20,   224,   429,
      21,   232,   217,    61,   232,   217,   219,   223,   225,   226,
     227,  -160,   631,   650,   535,   535,   285,   285,   285,   285,
     187,   187,   668,   687,   705,   -48,   -48,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,  -160,   230,   -29,   -16,
     -25,   375,    95,   464,   481,   221,   611,   272,   611,   232,
     -23,   -23,   278,    12,  -160,   238,   375,   232,    23,   232,
     217,  -160,  -160,   232,   217,   611,   183,   529,  -160,   282,
    -160,  -160,  -160,    -7,    61,  -160,  -160,   127,  -160,   241,
    -160,   611,   -23,  -160,   546,    73,   113,   240,  -160,  -160,
     375,   130,   594,   232,   217,   611,   611,    24,   257,   260,
      47,  -160,   375,   170,  -160,  -160,  -160,   321,   163,  -160,
     256,   611,    61,   217,   300,    -7,  -160,   172,  -160,  -160,
     177,  -160,   267,  -160,  -160,   375,   183,   261,   266,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,   273,   216,   192,  -160,
      -7,   232,  -160,  -160,   275,   611,    -7,   276,    -7,   277,
      -7,   279,    -7,   280,    -7,   281,    -7,  -160
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -160,  -160,  -160,  -160,  -121,  -159,  -160,  -160,   289,   -74,
    -160,  -160,    75,  -160,  -160,  -160,  -160,  -160,   148,  -160,
      66,  -160,  -160,  -160,  -160,  -160,   -53,   155,   -36,  -160,
    -160,  -160,   -39,   150,  -145,  -160,  -160,    -2,  -160,  -154
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -25
static const yytype_int16 yytable[] =
{
      15,   103,   144,   104,   105,   106,   167,   128,   129,   153,
     232,   170,    29,   148,    30,   220,    27,   221,   161,    23,
     163,   148,   222,    23,   212,    12,   122,   123,   124,    23,
     205,    16,    23,   172,    17,    31,   175,    32,    33,    13,
      18,   206,    34,    14,    35,    36,    19,   145,   148,   225,
      37,   146,   148,   148,   148,   148,   148,    38,   148,   148,
     244,   173,   174,    24,   246,    39,   245,    40,    41,   149,
     252,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   269,
     247,    28,   228,   258,   226,    23,   152,     1,   227,   154,
     160,   162,   171,   229,   233,   261,   263,   282,    20,    12,
     213,   214,    50,   216,   230,   139,   218,   234,   291,     2,
     125,   126,     3,    13,     4,    21,   127,    14,     5,   286,
      22,   298,   208,   209,   210,   235,   168,   169,   237,   278,
     155,   156,   157,   158,   203,   126,   204,   126,   291,    52,
     127,   287,   127,     6,   275,   126,   314,   140,   141,   142,
     127,   134,   317,   135,   319,   136,   321,   137,   323,    25,
     325,   236,   327,    26,   308,   264,   248,   169,    53,   251,
      54,   255,   256,   254,    59,   300,    61,    96,    55,   301,
      63,   262,    97,   265,   276,   126,    67,   266,    69,    98,
     127,   302,    72,    59,   303,    61,    99,    74,   271,   169,
      76,   279,   169,   273,   283,    67,   304,    69,   131,    56,
      57,   100,    58,    59,    60,    61,   101,   281,    62,    63,
      64,    23,    79,    65,    66,    67,    68,    69,    70,   130,
      71,    72,   270,    73,   294,   169,    74,    75,   132,    76,
     289,   288,   126,   299,   169,   133,   102,   127,    30,   120,
     121,   122,   123,   124,   138,    78,   164,   102,   147,    30,
     290,    79,    80,   313,   169,   315,   201,   202,   165,    31,
     296,    32,    33,   176,   177,   178,    34,   179,    35,   180,
      31,   200,    32,    33,    37,   211,   215,    34,   -24,    35,
      38,    38,   219,   222,   217,    37,   231,   148,   238,    39,
     239,    40,    41,   257,   240,   241,   242,   243,   260,   268,
      39,   272,    40,    41,    56,    57,   277,    58,    59,    60,
      61,   115,   116,    62,    63,    64,   295,   297,    65,    66,
      67,    68,    69,    70,   284,    71,    72,   285,    73,   307,
     309,    74,    75,   310,    76,   289,   311,   120,   121,   122,
     123,   124,   316,   318,   320,   305,   322,   324,   326,   166,
      78,   259,   253,   312,     0,   290,    79,    80,    56,    57,
       0,    58,    59,    60,    61,     0,     0,    62,    63,    64,
       0,     0,    65,    66,    67,    68,    69,    70,     0,    71,
      72,     0,    73,     0,     0,    74,    75,     0,    76,    77,
       0,     0,     0,    58,    59,     0,    61,     0,     0,     0,
       0,    64,     0,     0,    78,    66,    67,    68,    69,    70,
      79,    80,    56,    57,     0,    58,    59,    60,    61,     0,
       0,    62,    63,    64,     0,     0,    65,    66,    67,    68,
      69,    70,     0,    71,    72,     0,    73,     0,     0,    74,
      75,     0,    76,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,     0,     0,     0,     0,     0,    78,     0,
       0,     0,     0,     0,    79,    80,     0,     0,     0,     0,
       0,     0,     0,     0,   117,   118,   119,   120,   121,   122,
     123,   124,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,     0,     0,   181,     0,     0,     0,     0,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,     0,
       0,     0,     0,   117,   118,   119,   120,   121,   122,   123,
     124,     0,     0,     0,     0,     0,     0,     0,     0,   249,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
       0,     0,     0,     0,     0,     0,   250,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   111,   112,   113,
     114,   115,   116,     0,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,     0,     0,     0,     0,   117,   118,
     119,   120,   121,   122,   123,   124,     0,   120,   121,   122,
     123,   124,     0,     0,   267,   117,   118,   119,   120,   121,
     122,   123,   124,     0,     0,     0,     0,     0,     0,     0,
       0,   274,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,     0,     0,     0,     0,     0,     0,     0,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,     0,
       0,     0,     0,   117,   118,   119,   120,   121,   122,   123,
     124,   109,   110,   111,   112,   113,   114,   115,   116,   280,
     117,   118,   119,   120,   121,   122,   123,   124,   107,     0,
     109,   110,   111,   112,   113,   114,   115,   116,     0,     0,
     117,   118,   119,   120,   121,   122,   123,   124,   109,   110,
     111,   112,   113,   114,   115,   116,     0,     0,     0,   117,
     118,   119,   120,   121,   122,   123,   124,   109,   110,   111,
     112,   113,   114,   115,   116,     0,     0,     0,   118,   119,
     120,   121,   122,   123,   124,   109,   110,   111,   112,   113,
     114,   115,   116,     0,     0,     0,     0,     0,   119,   120,
     121,   122,   123,   124,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   120,   121,   122,
     123,   124
};

static const yytype_int16 yycheck[] =
{
       2,    37,    35,    39,    40,    41,    80,    46,    47,    62,
     169,    85,    35,    35,    37,   160,     5,   162,    71,    48,
      73,    35,    10,    48,   145,    48,    74,    75,    76,    48,
      37,    37,    48,    86,    37,    58,    89,    60,    61,    62,
      37,    48,    65,    66,    67,    68,     0,    80,    35,    35,
      73,    84,    35,    35,    35,    35,    35,    80,    35,    35,
      89,    83,    84,    82,    89,    88,    82,    90,    91,    83,
     215,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   243,
     211,    80,   166,    81,    80,    48,    83,     1,    84,     9,
      83,    83,    83,    83,    83,   226,    83,    83,    80,    48,
     146,   147,    48,   149,   167,     9,   152,   170,   277,    23,
      81,    82,    26,    62,    28,    88,    87,    66,    32,    82,
      88,   285,   134,   135,   136,   171,    81,    82,   174,   260,
      50,    51,    52,    53,    81,    82,    81,    82,   307,    48,
      87,   272,    87,    57,    81,    82,   310,    51,    52,    53,
      87,    87,   316,    89,   318,    87,   320,    89,   322,    88,
     324,   173,   326,    88,   295,   228,    81,    82,    37,   215,
      37,   220,   221,   219,     7,     8,     9,    80,    37,    12,
      13,   227,    88,   229,    81,    82,    19,   233,    21,    88,
      87,    24,    25,     7,    27,     9,    88,    30,    81,    82,
      33,    81,    82,   252,   267,    19,   290,    21,    87,     3,
       4,    88,     6,     7,     8,     9,    88,   263,    12,    13,
      14,    48,    55,    17,    18,    19,    20,    21,    22,    68,
      24,    25,   244,    27,    81,    82,    30,    31,    89,    33,
      34,    81,    82,    81,    82,    89,    35,    87,    37,    72,
      73,    74,    75,    76,    82,    49,    80,    35,    84,    37,
      54,    55,    56,    81,    82,   311,   126,   127,     4,    58,
     282,    60,    61,    35,    35,    35,    65,    35,    67,    35,
      58,    82,    60,    61,    73,    80,    83,    65,    82,    67,
      80,    80,    84,    10,    87,    73,    82,    35,    89,    88,
      87,    90,    91,    35,    89,    89,    89,    87,    80,    37,
      88,    80,    90,    91,     3,     4,    86,     6,     7,     8,
       9,    46,    47,    12,    13,    14,    80,    37,    17,    18,
      19,    20,    21,    22,    87,    24,    25,    87,    27,    82,
      89,    30,    31,    87,    33,    34,    83,    72,    73,    74,
      75,    76,    87,    87,    87,   290,    87,    87,    87,    80,
      49,   223,   217,   307,    -1,    54,    55,    56,     3,     4,
      -1,     6,     7,     8,     9,    -1,    -1,    12,    13,    14,
      -1,    -1,    17,    18,    19,    20,    21,    22,    -1,    24,
      25,    -1,    27,    -1,    -1,    30,    31,    -1,    33,    34,
      -1,    -1,    -1,     6,     7,    -1,     9,    -1,    -1,    -1,
      -1,    14,    -1,    -1,    49,    18,    19,    20,    21,    22,
      55,    56,     3,     4,    -1,     6,     7,     8,     9,    -1,
      -1,    12,    13,    14,    -1,    -1,    17,    18,    19,    20,
      21,    22,    -1,    24,    25,    -1,    27,    -1,    -1,    30,
      31,    -1,    33,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    49,    -1,
      -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    -1,    89,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    42,    43,    44,
      45,    46,    47,    -1,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    72,    73,    74,
      75,    76,    -1,    -1,    85,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    40,    41,    42,    43,    44,    45,    46,    47,    85,
      69,    70,    71,    72,    73,    74,    75,    76,    38,    -1,
      40,    41,    42,    43,    44,    45,    46,    47,    -1,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    40,    41,
      42,    43,    44,    45,    46,    47,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    40,    41,    42,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    70,    71,
      72,    73,    74,    75,    76,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    71,    72,
      73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,
      75,    76
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    23,    26,    28,    32,    57,    93,    94,    95,
     122,   123,    48,    62,    66,   129,    37,    37,    37,     0,
      80,    88,    88,    48,    82,    88,    88,     5,    80,    35,
      37,    58,    60,    61,    65,    67,    68,    73,    80,    88,
      90,    91,   120,   121,   124,   125,   126,   127,   128,   129,
      48,   130,    48,    37,    37,    37,     3,     4,     6,     7,
       8,     9,    12,    13,    14,    17,    18,    19,    20,    21,
      22,    24,    25,    27,    30,    31,    33,    34,    49,    55,
      56,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   114,   116,    80,    88,    88,    88,
      88,    88,    35,   120,   120,   120,   120,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    69,    70,    71,
      72,    73,    74,    75,    76,    81,    82,    87,   124,   124,
      68,    87,    89,    89,    87,    89,    87,    89,    82,     9,
      51,    52,    53,   117,    35,    80,    84,    84,    35,    83,
     118,   119,    83,   118,     9,    50,    51,    52,    53,   115,
      83,   118,    83,   118,    80,     4,   100,   101,    81,    82,
     101,    83,   118,    83,    84,   118,    35,    35,    35,    35,
      35,    89,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
      82,   125,   125,    81,    81,    37,    48,   131,   129,   129,
     129,    80,    96,   120,   120,    83,   120,    87,   120,    84,
     126,   126,    10,   109,   110,    35,    80,    84,   101,    83,
     118,    82,    97,    83,   118,   120,   129,   120,    89,    87,
      89,    89,    89,    87,    89,    82,    89,    96,    81,    85,
      85,   120,   126,   119,   120,   124,   124,    35,    81,   110,
      80,    96,   120,    83,   118,   120,   120,    85,    37,   131,
     129,    81,    80,   124,    85,    81,    81,    86,    96,    81,
      85,   120,    83,   118,    87,    87,    82,    96,    81,    34,
      54,    97,   111,   112,    81,    80,   129,    37,   131,    81,
       8,    12,    24,    27,   101,   104,   113,    82,    96,    89,
      87,    83,   112,    81,   131,   120,    87,   131,    87,   131,
      87,   131,    87,   131,    87,   131,    87,   131
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 92 "rez/Sources/rez_parser.y"
    { YYACCEPT; ;}
    break;

  case 3:
#line 93 "rez/Sources/rez_parser.y"
    { if (head) head->Write(); WriteResource((yyvsp[(1) - (5)])); YYACCEPT; ;}
    break;

  case 4:
#line 94 "rez/Sources/rez_parser.y"
    { YYACCEPT; ;}
    break;

  case 5:
#line 95 "rez/Sources/rez_parser.y"
    { YYACCEPT; ;}
    break;

  case 6:
#line 96 "rez/Sources/rez_parser.y"
    { return 1; ;}
    break;

  case 7:
#line 97 "rez/Sources/rez_parser.y"
    { return 1; ;}
    break;

  case 8:
#line 100 "rez/Sources/rez_parser.y"
    { Include((char *)(yyvsp[(2) - (3)])); ;}
    break;

  case 9:
#line 103 "rez/Sources/rez_parser.y"
    { RState::FinishType((yyvsp[(2) - (6)]), RS((yyvsp[(4) - (6)]))); ;}
    break;

  case 10:
#line 104 "rez/Sources/rez_parser.y"
    { RState::CopyType((yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); ;}
    break;

  case 13:
#line 109 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); if ((yyvsp[(3) - (3)])) RS((yyvsp[(1) - (3)]))->SetNext(RS((yyvsp[(3) - (3)]))); ;}
    break;

  case 24:
#line 122 "rez/Sources/rez_parser.y"
    { (yyval) = 0; ;}
    break;

  case 25:
#line 125 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); ;}
    break;

  case 26:
#line 126 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); RSV((yyval))->SetDefaultValue((yyvsp[(3) - (3)])); ;}
    break;

  case 27:
#line 127 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); RSV((yyval))->AddIdentifiers(LST((yyvsp[(2) - (2)]))); ;}
    break;

  case 29:
#line 131 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 30:
#line 132 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 31:
#line 133 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(3) - (3)]); ;}
    break;

  case 32:
#line 134 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); RSV((yyval))->SetDefaultValue((yyvsp[(3) - (3)])); ;}
    break;

  case 33:
#line 135 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (4)]); RSV((yyval))->SetDefaultValue((yyvsp[(4) - (4)])); ;}
    break;

  case 34:
#line 136 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (4)]); RSV((yyval))->SetDefaultValue((yyvsp[(4) - (4)])); ;}
    break;

  case 35:
#line 137 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(3) - (5)]); RSV((yyval))->SetDefaultValue((yyvsp[(5) - (5)])); ;}
    break;

  case 36:
#line 138 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(1) - (2)]); RSV((yyval))->AddIdentifiers(LST((yyvsp[(2) - (2)]))); ;}
    break;

  case 37:
#line 139 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (3)]); RSV((yyval))->AddIdentifiers(LST((yyvsp[(3) - (3)]))); ;}
    break;

  case 38:
#line 140 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (3)]); RSV((yyval))->AddIdentifiers(LST((yyvsp[(3) - (3)]))); ;}
    break;

  case 39:
#line 141 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(3) - (4)]); RSV((yyval))->AddIdentifiers(LST((yyvsp[(4) - (4)]))); ;}
    break;

  case 45:
#line 151 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(32); ;}
    break;

  case 46:
#line 152 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(REvaluate(RE((yyvsp[(3) - (4)])), head)); ;}
    break;

  case 47:
#line 153 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); ;}
    break;

  case 48:
#line 154 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(16); ;}
    break;

  case 49:
#line 155 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(32); ;}
    break;

  case 50:
#line 158 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); ;}
    break;

  case 51:
#line 159 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); RSV((yyval))->SetDefaultValue((yyvsp[(3) - (3)])); ;}
    break;

  case 52:
#line 160 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue(8); RSV((yyval))->AddIdentifiers(LST((yyvsp[(2) - (2)]))); ;}
    break;

  case 53:
#line 163 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSStringValue((yyvsp[(1) - (1)]), 0); ;}
    break;

  case 54:
#line 164 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSStringValue((yyvsp[(1) - (4)]), REvaluate(RE((yyvsp[(3) - (4)])), head)); ;}
    break;

  case 55:
#line 165 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSStringValue((yyvsp[(1) - (3)]), 0); RSV((yyval))->SetDefaultValue((yyvsp[(3) - (3)])); ;}
    break;

  case 56:
#line 166 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSStringValue((yyvsp[(1) - (6)]), REvaluate(RE((yyvsp[(3) - (6)])), head)); RSV((yyval))->SetDefaultValue((yyvsp[(6) - (6)])); ;}
    break;

  case 57:
#line 167 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSStringValue((yyvsp[(1) - (2)]), 0); RSV((yyval))->AddIdentifiers(LST((yyvsp[(2) - (2)]))); ;}
    break;

  case 58:
#line 168 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSStringValue((yyvsp[(1) - (5)]), REvaluate(RE((yyvsp[(3) - (5)])), head)); RSV((yyval))->AddIdentifiers(LST((yyvsp[(5) - (5)]))); ;}
    break;

  case 59:
#line 171 "rez/Sources/rez_parser.y"
    { (yyval) = RSStringValue::skStr; ;}
    break;

  case 60:
#line 172 "rez/Sources/rez_parser.y"
    { (yyval) = RSStringValue::skHex; ;}
    break;

  case 61:
#line 173 "rez/Sources/rez_parser.y"
    { (yyval) = RSStringValue::skPStr; ;}
    break;

  case 62:
#line 174 "rez/Sources/rez_parser.y"
    { (yyval) = RSStringValue::skWStr; ;}
    break;

  case 63:
#line 175 "rez/Sources/rez_parser.y"
    { (yyval) = RSStringValue::skCStr; ;}
    break;

  case 64:
#line 178 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(new RSNrValue(16), 0, 2); ;}
    break;

  case 65:
#line 179 "rez/Sources/rez_parser.y"
    { error("Unimplemented constant declaration"); ;}
    break;

  case 66:
#line 180 "rez/Sources/rez_parser.y"
    { error("Unimplemented constant declaration"); ;}
    break;

  case 67:
#line 183 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(new RSNrValue(16), 0, 4); ;}
    break;

  case 68:
#line 184 "rez/Sources/rez_parser.y"
    { error("Unimplemented constant declaration"); ;}
    break;

  case 69:
#line 185 "rez/Sources/rez_parser.y"
    { error("Unimplemented constant declaration"); ;}
    break;

  case 70:
#line 188 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(RS((yyvsp[(3) - (4)]))); ;}
    break;

  case 71:
#line 189 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(RS((yyvsp[(4) - (5)]))); ;}
    break;

  case 72:
#line 190 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(RS((yyvsp[(4) - (5)])), (yyvsp[(2) - (5)])); ;}
    break;

  case 73:
#line 191 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(RS((yyvsp[(5) - (6)])), (yyvsp[(3) - (6)])); ;}
    break;

  case 74:
#line 192 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(RS((yyvsp[(6) - (7)])), 0, (yyvsp[(3) - (7)])); ;}
    break;

  case 75:
#line 193 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSArray(RS((yyvsp[(7) - (8)])), 0, (yyvsp[(3) - (8)])); ;}
    break;

  case 76:
#line 196 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSSwitch(LST((yyvsp[(3) - (4)]))); ;}
    break;

  case 77:
#line 199 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(1) - (2)]); LST((yyvsp[(1) - (2)]))->AddItem(RS((yyvsp[(2) - (2)]))); ;}
    break;

  case 78:
#line 200 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new BList; LST((yyval))->AddItem(RS((yyvsp[(1) - (1)]))); ;}
    break;

  case 79:
#line 203 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RCase((yyvsp[(2) - (5)]), RS((yyvsp[(4) - (5)]))); ;}
    break;

  case 80:
#line 206 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); RS((yyvsp[(1) - (3)]))->SetNext(RS((yyvsp[(3) - (3)]))); ;}
    break;

  case 84:
#line 212 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (4)]); RSV((yyval))->SetDefaultValue((yyvsp[(4) - (4)])); ;}
    break;

  case 91:
#line 223 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSNrValue((yyvsp[(2) - (2)])); RSV((yyval))->SetDefaultValue((int)RValue(0)); ;}
    break;

  case 92:
#line 224 "rez/Sources/rez_parser.y"
    {
																int cnt = REvaluate(RE((yyvsp[(4) - (5)])), head);
																RSNrValue *s, *t = NULL;

																while (cnt--)
																{
																	s = new RSNrValue((yyvsp[(2) - (5)]));
																	s->SetDefaultValue((int)RValue(0));
																	s->SetNext(t);
																	t = s;
																}
																(yyval) = (int)s;
															;}
    break;

  case 93:
#line 239 "rez/Sources/rez_parser.y"
    { (yyval) = 1; ;}
    break;

  case 94:
#line 240 "rez/Sources/rez_parser.y"
    { (yyval) = 4; ;}
    break;

  case 95:
#line 241 "rez/Sources/rez_parser.y"
    { (yyval) = 8; ;}
    break;

  case 96:
#line 242 "rez/Sources/rez_parser.y"
    { (yyval) = 16; ;}
    break;

  case 97:
#line 243 "rez/Sources/rez_parser.y"
    { (yyval) = 32; ;}
    break;

  case 103:
#line 255 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new BList; LST((yyval))->AddItem(RS((yyvsp[(1) - (1)]))); ;}
    break;

  case 104:
#line 256 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(1) - (3)]); LST((yyvsp[(1) - (3)]))->AddItem(RS((yyvsp[(3) - (3)]))); ;}
    break;

  case 105:
#line 259 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSymbol((yyvsp[(1) - (1)]), 0); ;}
    break;

  case 106:
#line 260 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSymbol((yyvsp[(1) - (3)]), REvaluate(RE((yyvsp[(3) - (3)])), head)); ;}
    break;

  case 107:
#line 261 "rez/Sources/rez_parser.y"
    { (yyval) = (int)new RSymbol((yyvsp[(1) - (5)]), 0); ;}
    break;

  case 108:
#line 264 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 109:
#line 265 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoPlus); ;}
    break;

  case 110:
#line 266 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoMinus); ;}
    break;

  case 111:
#line 267 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoMultiply); ;}
    break;

  case 112:
#line 268 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoDivide); ;}
    break;

  case 113:
#line 269 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoModulus); ;}
    break;

  case 114:
#line 270 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoSHL); ;}
    break;

  case 115:
#line 271 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoSHR); ;}
    break;

  case 116:
#line 272 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoBitAnd); ;}
    break;

  case 117:
#line 273 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoBitOr); ;}
    break;

  case 118:
#line 274 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoXPwrY); ;}
    break;

  case 119:
#line 275 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RUnaryOp(RE((yyvsp[(2) - (2)])), reoFlip); ;}
    break;

  case 120:
#line 276 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RUnaryOp(RE((yyvsp[(2) - (2)])), reoNot); ;}
    break;

  case 121:
#line 277 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RUnaryOp(RE((yyvsp[(2) - (2)])), reoNegate); ;}
    break;

  case 122:
#line 278 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoAnd); ;}
    break;

  case 123:
#line 279 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoOr); ;}
    break;

  case 124:
#line 280 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoLT); ;}
    break;

  case 125:
#line 281 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoGT); ;}
    break;

  case 126:
#line 282 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoLE); ;}
    break;

  case 127:
#line 283 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoGE); ;}
    break;

  case 128:
#line 284 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoEQ); ;}
    break;

  case 129:
#line 285 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RBinaryOp(RE((yyvsp[(1) - (3)])), RE((yyvsp[(3) - (3)])), reoNE); ;}
    break;

  case 130:
#line 286 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RValue((yyvsp[(1) - (1)])); ;}
    break;

  case 131:
#line 287 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RIdentifier((yyvsp[(1) - (1)])); ;}
    break;

  case 133:
#line 291 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RFunction(refCountOf, (yyvsp[(3) - (4)])); ;}
    break;

  case 134:
#line 293 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RFunction(refCopyBits, (yyvsp[(3) - (8)]), (yyvsp[(5) - (8)]), (yyvsp[(7) - (8)])); ;}
    break;

  case 135:
#line 294 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RFunction(refCopyBits, (yyvsp[(3) - (4)]), 0, 8); ;}
    break;

  case 136:
#line 295 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RFunction(refCopyBits, (yyvsp[(3) - (4)]), 0, 16); ;}
    break;

  case 137:
#line 296 "rez/Sources/rez_parser.y"
    { (yyval) = (long)RFunction(refCopyBits, (yyvsp[(3) - (4)]), 0, 32); ;}
    break;

  case 138:
#line 300 "rez/Sources/rez_parser.y"
    {
																sState = RState::FirstState((yyvsp[(2) - (7)]));
																if (!sState)
																{
																	int t = ntohl((yyvsp[(2) - (7)]));
																	error("Undefined resource type: %4.4s", &t);
																}
																head = NULL;
																(yyval) = (int)new ResHeader((yyvsp[(2) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(6) - (7)]));
															;}
    break;

  case 139:
#line 311 "rez/Sources/rez_parser.y"
    {
																sState = RState::FirstState((yyvsp[(2) - (5)]));
																if (!sState)
																{
																	int t = ntohl((yyvsp[(2) - (5)]));
																	error("Undefined resource type: %4.4s", &t);
																}
																head = NULL;
																(yyval) = (int)new ResHeader((int)(yyvsp[(2) - (5)]), (int)(yyvsp[(4) - (5)]), (int)0);
															;}
    break;

  case 140:
#line 324 "rez/Sources/rez_parser.y"
    { WriteResource((char *)(yyvsp[(8) - (9)]), (yyvsp[(2) - (9)]), (yyvsp[(4) - (9)]), (char *)(yyvsp[(6) - (9)])); ;}
    break;

  case 141:
#line 326 "rez/Sources/rez_parser.y"
    { WriteResource((char *)(yyvsp[(6) - (7)]), (yyvsp[(2) - (7)]), (yyvsp[(4) - (7)]), NULL); ;}
    break;

  case 145:
#line 334 "rez/Sources/rez_parser.y"
    { CHECKSTATE; sState = sState->Shift((yyvsp[(1) - (1)]), tInt, &head); ;}
    break;

  case 146:
#line 336 "rez/Sources/rez_parser.y"
    { CHECKSTATE; sState = sState->Shift((yyvsp[(1) - (1)]), tString, &head); free((char *)(yyvsp[(1) - (1)])); ;}
    break;

  case 147:
#line 337 "rez/Sources/rez_parser.y"
    { CHECKSTATE; sState = sState->Shift((yyvsp[(1) - (1)]), tRaw, &head); free((char *)(yyvsp[(1) - (1)])); ;}
    break;

  case 149:
#line 339 "rez/Sources/rez_parser.y"
    { CHECKSTATE; sState = sState->Shift(0, tArrayEnd, &head); ;}
    break;

  case 151:
#line 343 "rez/Sources/rez_parser.y"
    { CHECKSTATE; sState = sState->Shift(0, tArray, &head); ;}
    break;

  case 152:
#line 346 "rez/Sources/rez_parser.y"
    { CHECKSTATE; sState = sState->Shift((yyvsp[(1) - (2)]), tCase, &head); ;}
    break;

  case 154:
#line 350 "rez/Sources/rez_parser.y"
    { char *t, *a, *b;
															  long sa, sb;

															  a = (char *)(yyvsp[(1) - (2)]);	sa = *(long *)a;
															  b = (char *)(yyvsp[(2) - (2)]);	sb = *(long *)b;

															  t = (char *)malloc(sa + sb + sizeof(long));
															  if (!t) error("insufficient memory");

															  memcpy(t + sizeof(long), a + sizeof(long), sa);
															  memcpy(t + sizeof(long) + sa, b + sizeof(long), sb);
															  *(long*)t = sa + sb;

															  free(a); free(b);
															  (yyval) = (int)t; ;}
    break;

  case 156:
#line 368 "rez/Sources/rez_parser.y"
    { char *t, *a, *b;
															  a = (char *)(yyvsp[(1) - (2)]);
															  b = (char *)(yyvsp[(2) - (2)]);
															  t = (char *)malloc(strlen(a) + strlen(b) + 1);
															  if (!t) error("insufficient memory");
															  strcpy(t, a);
															  strcat(t, b);
															  free(a); free(b);
															  (yyval) = (int)t; ;}
    break;

  case 157:
#line 377 "rez/Sources/rez_parser.y"
    { (yyval) = (int)strdup(getenv((char *)(yyvsp[(3) - (4)]))); ;}
    break;

  case 158:
#line 378 "rez/Sources/rez_parser.y"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 159:
#line 381 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); (yyval) = (int)strdup(b); ;}
    break;

  case 160:
#line 382 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); (yyval) = (int)strdup(b); ;}
    break;

  case 161:
#line 383 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (7)]), (yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)])); (yyval) = (int)strdup(b); ;}
    break;

  case 162:
#line 384 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (9)]), (yyvsp[(3) - (9)]), (yyvsp[(5) - (9)]), (yyvsp[(7) - (9)]), (yyvsp[(9) - (9)])); (yyval) = (int)strdup(b); ;}
    break;

  case 163:
#line 385 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (11)]), (yyvsp[(3) - (11)]), (yyvsp[(5) - (11)]), (yyvsp[(7) - (11)]), (yyvsp[(9) - (11)]), (yyvsp[(11) - (11)])); (yyval) = (int)strdup(b); ;}
    break;

  case 164:
#line 386 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (13)]), (yyvsp[(3) - (13)]), (yyvsp[(5) - (13)]), (yyvsp[(7) - (13)]), (yyvsp[(9) - (13)]), (yyvsp[(11) - (13)]), (yyvsp[(13) - (13)])); (yyval) = (int)strdup(b); ;}
    break;

  case 165:
#line 387 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (15)]), (yyvsp[(3) - (15)]), (yyvsp[(5) - (15)]), (yyvsp[(7) - (15)]), (yyvsp[(9) - (15)]), (yyvsp[(11) - (15)]), (yyvsp[(13) - (15)]), (yyvsp[(15) - (15)])); (yyval) = (int)strdup(b); ;}
    break;

  case 166:
#line 388 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (17)]), (yyvsp[(3) - (17)]), (yyvsp[(5) - (17)]), (yyvsp[(7) - (17)]), (yyvsp[(9) - (17)]), (yyvsp[(11) - (17)]), (yyvsp[(13) - (17)]), (yyvsp[(15) - (17)]), (yyvsp[(17) - (17)])); (yyval) = (int)strdup(b); ;}
    break;

  case 167:
#line 389 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (19)]), (yyvsp[(3) - (19)]), (yyvsp[(5) - (19)]), (yyvsp[(7) - (19)]), (yyvsp[(9) - (19)]), (yyvsp[(11) - (19)]), (yyvsp[(13) - (19)]), (yyvsp[(15) - (19)]), (yyvsp[(17) - (19)]), (yyvsp[(19) - (19)])); (yyval) = (int)strdup(b); ;}
    break;

  case 168:
#line 390 "rez/Sources/rez_parser.y"
    { char b[1024]; sprintf(b, (char *)(yyvsp[(1) - (21)]), (yyvsp[(3) - (21)]), (yyvsp[(5) - (21)]), (yyvsp[(7) - (21)]), (yyvsp[(9) - (21)]), (yyvsp[(11) - (21)]), (yyvsp[(13) - (21)]), (yyvsp[(15) - (21)]), (yyvsp[(17) - (21)]), (yyvsp[(19) - (21)]), (yyvsp[(21) - (21)])); (yyval) = (int)strdup(b); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2612 "rez/Sources/rez_parser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 397 "rez/Sources/rez_parser.y"



