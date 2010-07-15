/*	$Id: Cursor.r,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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
*/

#include "Types.r"

//resource 'CURS' (0, "Splitter cursor") {
//	16, 1,
//	{ 7, 7 },
//	{
//		0x01, 0x00,
//		0x03, 0x80,
//		0x07, 0xc0,
//		0x0f, 0xe0,
//		0x03, 0x80,
//		0x03, 0x80,
//		0xff, 0xfe,
//		0x00, 0x00,
//		0xff, 0xfe,
//		0x03, 0x80,
//		0x03, 0x80,
//		0x0f, 0xe0,
//		0x07, 0xc0,
//		0x03, 0x80,
//		0x01, 0x00,
//		0x00, 0x00
//	},
//	{
//		0x03, 0x80,
//		0x07, 0xc0,
//		0x0f, 0xe0,
//		0x1f, 0xf0,
//		0x07, 0xc0,
//		0x07, 0xc0,
//		0xff, 0xfe,
//		0xff, 0xfe,
//		0xff, 0xfe,
//		0x07, 0xc0,
//		0x07, 0xc0,
//		0x1f, 0xf0,
//		0x0f, 0xe0,
//		0x07, 0xc0,
//		0x03, 0x80,
//		0x00, 0x00
//	}
//};

resource 'CURS' (0, "Splitter cursor") {
	16, 1,
	{ 7, 7 },
	{
		0x00, 0x00,		0x02, 0x00,		0x07, 0x00,		0x0d, 0x80,
		0x18, 0xc0,		0x30, 0x60,		0x7f, 0xf0,		0x00, 0x00,
		0x7f, 0xf0,		0x30, 0x60,		0x18, 0xc0,		0x0d, 0x80,
		0x07, 0x00,		0x02, 0x00,		0x00, 0x00,		0x00, 0x00
	},
	{
		0x02, 0x00,		0x07, 0x00,		0x0f, 0x80,		0x1f, 0xc0,
		0x3f, 0xe0,		0x7f, 0xf0,		0xff, 0xf8,		0xff, 0xf8,
		0xff, 0xf8,		0x7f, 0xf0,		0x3f, 0xe0,		0x1f, 0xc0,
		0x0f, 0x80,		0x07, 0x00,		0x02, 0x00,		0x00, 0x00,
	}
};


//0000 0000 0000 0000 //0000 0010 0000 0000 //0000 0111 0000 0000 //0000 1101 1000 0000 
//0001 1000 1100 0000 //0011 0000 0110 0000 //0111 1111 1111 0000 //0000 0000 0000 0000 
//0111 1111 1111 0000 //0011 0000 0110 0000 //0001 1000 1100 0000 //0000 1101 1000 0000 
//0000 0111 0000 0000 //0000 0010 0000 0000 //0000 0000 0000 0000 //0000 0000 0000 0000 
