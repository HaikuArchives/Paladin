// CLang_diff.cpp
// 
// Copyright (c) 2004, Oliver Tappe <pedevel@hirschkaefer.de>
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name of a copyright holder shall
// not be used in advertising or otherwise to promote the sale, use or other
// dealings in this Software without prior written authorization of the
// copyright holder.

/*
 * This file provides syntax-highlighting and "function"-scanning (here:
 * patch-scanning) for diffs (aka patch-files).
 * It supports context and unified diffs, but the support for unified diffs
 * is better, as the patch-scanner can recognize individual patches within
 * each diffed file, too.
 */

#include <String.h>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

extern "C" {
_EXPORT const char kLanguageName[] = "Diff";
_EXPORT const char kLanguageExtensions[] = "diff;patch";
_EXPORT const char kLanguageCommentStart[] = "";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.diff";
_EXPORT const int16 kInterfaceVersion = 2;
}

// #pragma mark - coloring

// ColorLine
_EXPORT
void
ColorLine(CLanguageProxy& proxy, int& /*state*/)
{
	if (strncmp(proxy.Text(), "diff", 4) == 0
	|| strncmp(proxy.Text(), "@@", 2) == 0) {
		proxy.SetColor(0, kColorComment1);
	} else if (*proxy.Text() == '-' || *proxy.Text() == '<') {
		proxy.SetColor(0, kColorString1);
	} else if (*proxy.Text() == '+' || *proxy.Text() == '>') {
		proxy.SetColor(0, kColorError1);
	} else
		proxy.SetColor(0, kColorText);
}

// #pragma mark - function scanning

// ScanForFunctions
_EXPORT
void
ScanForFunctions(CLanguageProxy& proxy)
{
	const char* text = proxy.Text();
	for(const char* pos = text; pos != NULL; ) {
		if (strncmp(pos, "--- ", 4) == 0 && !isdigit(*(pos+4))) {
			const char* displayPos = pos + 4;
				// skip to filename
			const char* lineEnd = strchr(pos, '\n');
			if (!lineEnd)
				lineEnd = pos + strlen(pos);
			const char* tabPos = strchr(pos, '\t');
			if (tabPos && tabPos < lineEnd)
				// cut line off at tab, in order to drop the date following the filename
				lineEnd = tabPos;
			BString matchLine(pos, lineEnd-pos);
			BString displayLine(displayPos, lineEnd-displayPos);
			proxy.SetNestLevel(0);
			proxy.AddFunction(displayLine.String(), matchLine.String(), pos-text, false);
		} else if (strncmp(pos, "@@", 2) == 0
			|| (strncmp(pos, "--- ", 4) == 0 && isdigit(*(pos+4)))) {
			const char* displayPos = pos + 2;
				// skip over "@@" or "--"
			const char* lineEnd = strchr(pos, '\n');
			if (!lineEnd)
				lineEnd = pos + strlen(pos);
			BString matchLine(pos, lineEnd-pos);
			BString displayLine(displayPos, lineEnd-2-displayPos);
				// -2 in order to skip back over '@@'
			proxy.SetNestLevel(1);
			proxy.AddFunction(displayLine.String(), matchLine.String(), pos-text, false);
		}
		if ((pos = strchr(pos+1, '\n')) != NULL)
			pos++;
	}
}
