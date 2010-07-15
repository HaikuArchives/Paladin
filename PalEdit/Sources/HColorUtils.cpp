/*	$Id: HColorUtils.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/10/97 15:00:45
*/

#include "pe.h"
#include "HColorUtils.h"

const int kDistinctColorTresholdSquare = 10000;

rgb_color DistinctColor(rgb_color highColor, rgb_color lowColor);

rgb_color gColor[kColorEnd], gInvColor[kColorEnd];

//
// This comes from Pavel Cisler:
//

inline int max_value(uchar x)
{
	return 256;
}

rgb_color DistinctColor(rgb_color highColor, rgb_color lowColor)
{
	// Does a simple distance based color comparison, returns an
	// inverse color if colors close enough
	int32 redDelta = (int32)highColor.red - (int32)lowColor.red;
	int32 greenDelta = (int32)highColor.green - (int32)lowColor.green;
	int32 blueDelta = (int32)highColor.blue - (int32)lowColor.blue;

	if (redDelta * redDelta + greenDelta * greenDelta + blueDelta * blueDelta > kDistinctColorTresholdSquare)
		return highColor;

	rgb_color result;
	result.red = 255 - highColor.red; 
	result.green = 255 - highColor.green; 
	result.blue = 255 - highColor.blue;
	
	return result;
} /* DistinctColor */

void DefineInvColors(rgb_color selectionColor)
{
	for (int i = 0; i < kColorEnd; i++)
		gInvColor[i] = DistinctColor(gColor[i], selectionColor);
} /* DefineInvColors */

rgb_color LookupDistinctColor(rgb_color highColor)
{
	for (int i = 0; i < kColorEnd; i++)
		if (*(long*)&gColor[i] == *(long*)&highColor)
			return gInvColor[i];

	return highColor;
} /* LookupDistinctColor */

// and this is mine

unsigned char gSelectedMap[256];
unsigned char gDisabledMap[256];

void InitSelectedMap()
{
	BScreen screen;
	rgb_color c, d;
	int i;
	
	for (i = 0; i < 255; i++)
	{
		d = c = screen.ColorForIndex(i);

		c.red = c.red * 2 / 3;
		c.green = c.green * 2 / 3;
		c.blue = c.blue * 2 / 3;

		gSelectedMap[i] = screen.IndexForColor(c);

		d.red = (c.red + 438) / 3;
		d.green = (c.green + 438) / 3;
		d.blue = (c.blue + 438) / 3;

		gDisabledMap[i] = screen.IndexForColor(d);
	}

	gSelectedMap[255] = 255;
	gDisabledMap[255] = 255;
} /* InitSelectedMap */

//////////////////////////////////////////////////////
//
//   Color conversion routines
//

void rgb2f(rgb_color rgb, float& r, float& g, float& b, float& a)
{
	r = rgb.red / 255.0;
	g = rgb.green / 255.0;
	b = rgb.blue / 255.0;
	a = rgb.alpha / 255.0;
} /* rgb2f */

rgb_color f2rgb(float r, float g, float b, float a)
{
	rgb_color rgb;

	rgb.red = (int)(std::max(0.0, std::min(255.0, r * 255.0)));
	rgb.green = (int)(std::max(0.0, std::min(255.0, g * 255.0)));
	rgb.blue = (int)(std::max(0.0, std::min(255.0, b * 255.0)));
	rgb.alpha = (int)(std::max(0.0, std::min(255.0, a * 255.0)));

	return rgb;
} /* f2rgb */

void rgb2hsv(float r, float g, float b, float& h, float& s, float& v)
{
	float cmin, cmax, delta;
	
	cmax = std::max(r, std::max(g, b));
	cmin = std::min(r, std::min(g, b));
	delta = cmax - cmin;
	
	v = cmax;
	s = cmax ? delta / cmax : 0.0;

	if (s == 0.0)
		h = -1;
	else
	{
		if (r == cmax)
			h = (g - b) / delta;
		else if (g == cmax)
			h = 2 + (b - r) / delta;
		else if (b == cmax)
			h = 4 + (r - g) / delta;
		h /= 6.0;
	}
} /* rgb2hsv */

void hsv2rgb(float h, float s, float v, float& r, float& g, float& b)
{
	float A, B, C, F;
	int i;
	
	if (s == 0.0)
		r = g = b = v;
	else
	{
		if (h >= 1.0 || h < 0.0)
			h = 0.0;
		h *= 6.0;
		i = (int)floor(h);
		F = h - i;
		A = v * (1 - s);
		B = v * (1 - (s * F));
		C = v * (1 - (s * (1 - F)));
		switch (i)
		{
			case 0:	r = v; g = C; b = A; break;
			case 1:	r = B; g = v; b = A; break;
			case 2:	r = A; g = v; b = C; break;
			case 3:	r = A; g = B; b = v; break;
			case 4:	r = C; g = A; b = v; break;
			case 5:	r = v; g = A; b = B; break;
		}
	}
} /* hsv2rgb */

void rgb2ro(rgb_color rgb, roSColor& ro)
{
	rgb2f(rgb, ro.m_Red, ro.m_Green, ro.m_Blue, ro.m_Alpha);
	
	float sat, val;
	rgb2hsv(ro.m_Red, ro.m_Green, ro.m_Blue, ro.m_Hue, sat, val);
} /* rgb2ro */

rgb_color ro2rgb(roSColor& ro)
{
	return f2rgb(ro.m_Red, ro.m_Green, ro.m_Blue);
} /* ro2rgb */

#if !defined(B_BEOS_VERSION_DANO) && !defined(__HAIKU__)
bool operator==(const rgb_color& a, const rgb_color& b)
{
	return (a.red == b.red && a.green == b.green && a.blue == b.blue
		&& a.alpha == b.alpha);
} /* operator== */

bool operator!=(const rgb_color& a, const rgb_color& b)
{
	return !(a == b);
} /* operator!= */
#endif
