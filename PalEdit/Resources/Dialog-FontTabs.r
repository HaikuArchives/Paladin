/*	$Id: Dialog-FontTabs.r,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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


//resource rtyp_Popu (rid_Popu_XFONT, "Font Popup") {
//	"Font",
//	{
//	}
//};
//
//resource rtyp_Popu (rid_Popu_XENCODING, "Encoding Popup") {
//	"Encoding",
//	{
//		Item	{ "UTF-8", 'chng', none, noKey },
//		Item	{ "ISO 8859 1", 'chng', none, noKey },
//		Item	{ "ISO 8859 2", 'chng', none, noKey },
//		Item	{ "ISO 8859 3", 'chng', none, noKey },
//		Item	{ "ISO 8859 4", 'chng', none, noKey },
//		Item	{ "ISO 8859 5", 'chng', none, noKey },
//		Item	{ "ISO 8859 6", 'chng', none, noKey },
//		Item	{ "ISO 8859 7", 'chng', none, noKey },
//		Item	{ "ISO 8859 8", 'chng', none, noKey },
//		Item	{ "ISO 8859 9", 'chng', none, noKey },
//		Item	{ "ISO 8859 10", 'chng', none, noKey },
//		Item	{ "Macintosh Roman", 'chng', none, noKey }
//	}
//};
//
//resource rtyp_Popu (rid_Popu_XLINEBREAKS, "Line Breaks") {
//	"Line Breaks",
//	{
//		Item	{ "Be", 'chng', none, noKey },
//		Item	{ "Mac", 'chng', none, noKey },
//		Item	{ "DOS", 'chng', none, noKey }
//	}
//};

resource 'DLOG' (10, "Window Options") {
	{ 0, 0, 250, 140 }, "Window Options", B_TITLED_WINDOW, NORMAL,
	{
//		PopupMenu	{{ 	10,   8, 160,  28 }, "font",			"Font:",				rid_Popu_XFONT, 30		},
//		Edit		{{ 161,  10, 240,  26 }, "size",			"Size:",				"", "0123456789", 2, 0	},
//
//		PopupMenu	{{  10,  33, 129,  53 }, "encoding",		"Encoding:",			rid_Popu_XENCODING, 50	},
//		PopupMenu	{{ 130,  33, 240,  53 }, "linebreaks",		"Linebreaks:",			rid_Popu_XLINEBREAKS, 60},
//
//		CheckBox	{{  10,  60, 120,  76 }, "show tabs",		"Show Tabstops"									},
//		Edit		{{ 130,  59, 240,  75 }, "tabs",			"Spaces per Tab:",		"", "0123456789", 3, 0	},
//		CheckBox	{{  10,  80, 120,  96 }, "syntaxcol",		"Syntax Colouring"								},
//		CheckBox	{{ 130,  80, 240,  96 }, "show invisibles",	"Show Invisibles"								},

		Button		{{ 180, 110, 240, 130 }, "ok",				"Apply",				'ok  '					},
		Button		{{ 110, 110, 170, 130 }, "cancel",			"Revert",				'cncl'					}
	}
};
