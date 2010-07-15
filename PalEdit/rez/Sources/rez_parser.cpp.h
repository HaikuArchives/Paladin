/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

