/*	$Id: Utils.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 12/04/97 14:08:52 by Maarten Hekkelman
*/

#ifndef UTILS_H
#define UTILS_H

extern uchar gSelectedMap[256];

inline unsigned int Hash(unsigned int h, unsigned int c)
{
	return c + ((h << 7) | (h >> 25));
} /* Hash */

const char kDirectorySeparator = '/';

double Round(double);
void ReadCString(BPositionIO& buf, int max, char *s);
void InitSelectedMap();
void MakeOutline(BRegion& rgn);
bool RegionIsValid(BRegion& rgn);
void OpenInTracker(const entry_ref& ref);
void SendToIDE(const BMessage& msg, BMessage *reply = NULL);
void RelativePath(const entry_ref& a, const entry_ref& b, char* path);
void RelativePath(const BPath& pa, const BPath& pb, char* path);
void TruncPathString(char *path, int textTraitsID, int width);
uint16 HashString16(const char *string);

#endif // UTILS_H
