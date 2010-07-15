/*	$Id: HColorUtils.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/10/97 15:00:41
*/

#ifndef CCOLORUTILS_H
#define CCOLORUTILS_H

#include "HLibHekkel.h"

struct IMPEXP_LIBHEKKEL roSColor
{ 
    float  m_Red;
    float  m_Green;
    float  m_Blue;
    float  m_Alpha;
    float  m_Hue;
};

IMPEXP_LIBHEKKEL enum {
	kColorLow,
	kColorSelection,
	kColorInvisibles,
	kColorMark,
	// Constants below are used for syntax coloring
	kColorText,				// simple Text
	kColorKeyword1,			// like c's    : if
	kColorComment1,			// like c's    : /* comment */
	kColorString1,			// like c's    : "this is a string"
	kColorCharConst,		// like c's    : 'c'
	kColorUserSet1,
	kColorUserSet2,
	kColorUserSet3,
	kColorUserSet4,
	kColorTag,				// like (from <html>) : <image>
	kColorAttribute,		// like html's: <... class="">
	kColorKeyword2,			// like c++'s  : class
	kColorComment2,			// like c++'s  : // comment
	kColorString2,			// like html's : "this is a string"
	kColorIdentifierSystem,	// like java's : java.util.String
	kColorIdentifierUser,	// like (from c's int foo;) : foo
	kColorNumber1,			// like c's : 4.5
	kColorNumber2,			// like c's : 4
	kColorPreprocessor1,	// like c's : #define
	kColorPreprocessor2,	// like doxygen's : @param
	kColorError1,			// like c's : 5.4.3
	kColorError2,			// like c's : for int (
	kColorOperator1,		// like c's : +
	kColorOperator2,		// like c's : &
	kColorSeparator1,		// like c's : {
	kColorSeparator2,		// like c's : ;
	//
	kColorEnd				// just a marker, same as kColorText if you care
};

extern IMPEXP_LIBHEKKEL rgb_color gColor[kColorEnd], gInvColor[kColorEnd];

extern IMPEXP_LIBHEKKEL unsigned char gSelectedMap[256];
extern IMPEXP_LIBHEKKEL unsigned char gDisabledMap[256];
IMPEXP_LIBHEKKEL void InitSelectedMap();

IMPEXP_LIBHEKKEL void rgb2ro(rgb_color rgb, roSColor& ro);
IMPEXP_LIBHEKKEL rgb_color ro2rgb(roSColor& ro);
IMPEXP_LIBHEKKEL 
void rgb2hsv(float r, float g, float b, float& h, float& s, float& v);
IMPEXP_LIBHEKKEL 
void hsv2rgb(float h, float s, float v, float& r, float& g, float& b);
IMPEXP_LIBHEKKEL 
void rgb2f(rgb_color rgb, float& r, float& g, float& b, float& a);
IMPEXP_LIBHEKKEL rgb_color f2rgb(float r, float g, float b, float a = 0.0);

IMPEXP_LIBHEKKEL 
rgb_color DistinctColor(rgb_color highColor, rgb_color lowColor);
IMPEXP_LIBHEKKEL void DefineInvColors(rgb_color selectionColor);
IMPEXP_LIBHEKKEL rgb_color LookupDistinctColor(rgb_color highColor);

#if !defined(B_BEOS_VERSION_DANO) && !defined(__HAIKU__)
IMPEXP_LIBHEKKEL bool operator==(const rgb_color& a, const rgb_color& b);
IMPEXP_LIBHEKKEL bool operator!=(const rgb_color& a, const rgb_color& b);
#endif

#endif // CCOLORUTILS_H
