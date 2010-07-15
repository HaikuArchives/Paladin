/*	$Id: Dialog-Info.r,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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


resource rtyp_Popu (rid_Popu_InfMimetypes, "Mimetype") {
	"Mimetype:",
	{
	}
};

resource rtyp_Popu (rid_Popu_InfFont, "Font Popup") {
	"Font",
	{
	}
};

resource rtyp_Popu (rid_Popu_InfEncoding, "Encoding Popup") {
	"Encoding",
	{
		Item	{ "UTF-8",				'chng', none, noKey },
		Item	{ "ISO 8859-1",			'chng', none, noKey },
		Item	{ "ISO 8859-2",			'chng', none, noKey },
		Item	{ "ISO 8859-3",			'chng', none, noKey },
		Item	{ "ISO 8859-4",			'chng', none, noKey },
		Item	{ "ISO 8859-5",			'chng', none, noKey },
		Item	{ "ISO 8859-6",			'chng', none, noKey },
		Item	{ "ISO 8859-7",			'chng', none, noKey },
		Item	{ "ISO 8859-8",			'chng', none, noKey },
		Item	{ "ISO 8859-9",			'chng', none, noKey },
		Item	{ "ISO 8859-10",		'chng', none, noKey },
		Item	{ "Macintosh Roman",	'chng', none, noKey },
		Item	{ "Shift-JIS",			'chng', none, noKey },
		Item	{ "EUC Packed Japanese",'chng', none, noKey },
		Item	{ "JIS",				'chng', none, noKey },
		Item	{ "Windows 1252",		'chng', none, noKey },
		Item	{ "Unicode",			'chng', none, noKey },
		Item	{ "KOI8-R",				'chng', none, noKey },
		Item	{ "Windows 1251",		'chng', none, noKey },
		Item	{ "DOS 866",			'chng', none, noKey },
		Item	{ "DOS 437",			'chng', none, noKey },
		Item	{ "EUC Korean",			'chng', none, noKey },
		Item	{ "ISO 8859-13",		'chng', none, noKey },
		Item	{ "ISO 8859-14",		'chng', none, noKey },
		Item	{ "ISO 8859-15",		'chng', none, noKey }
	}
};

resource rtyp_Popu (rid_Popu_InfSrcEncoding, "Source Encoding Popup") {
	"Source Encoding",
	{
		Item	{ "UTF-8",				'chng', none, noKey },
		Item	{ "ISO 8859-1",			'chng', none, noKey },
		Item	{ "ISO 8859-2",			'chng', none, noKey },
		Item	{ "ISO 8859-3",			'chng', none, noKey },
		Item	{ "ISO 8859-4",			'chng', none, noKey },
		Item	{ "ISO 8859-5",			'chng', none, noKey },
		Item	{ "ISO 8859-6",			'chng', none, noKey },
		Item	{ "ISO 8859-7",			'chng', none, noKey },
		Item	{ "ISO 8859-8",			'chng', none, noKey },
		Item	{ "ISO 8859-9",			'chng', none, noKey },
		Item	{ "ISO 8859-10",		'chng', none, noKey },
		Item	{ "Macintosh Roman",	'chng', none, noKey },
		Item	{ "Shift-JIS",			'chng', none, noKey },
		Item	{ "EUC Packed Japanese",'chng', none, noKey },
		Item	{ "JIS",				'chng', none, noKey },
		Item	{ "Windows 1252",		'chng', none, noKey },
		Item	{ "Unicode",			'chng', none, noKey },
		Item	{ "KOI8-R",				'chng', none, noKey },
		Item	{ "Windows 1251",		'chng', none, noKey },
		Item	{ "DOS 866",			'chng', none, noKey },
		Item	{ "DOS 437",			'chng', none, noKey },
		Item	{ "EUC Korean",			'chng', none, noKey },
		Item	{ "ISO 8859-13",		'chng', none, noKey },
		Item	{ "ISO 8859-14",		'chng', none, noKey },
		Item	{ "ISO 8859-15",		'chng', none, noKey }
	}
};

resource rtyp_Popu (rid_Popu_InfLinebreaks, "Line Breaks") {
	"Line Breaks",
	{
		Item	{ "Be/Unix",			'chng', none, noKey },
		Item	{ "Mac",				'chng', none, noKey },
		Item	{ "DOS",				'chng', none, noKey }
	}
};

resource rtyp_Popu (rid_Popu_InfLanguage, "Language menu") {
	"Language",
	{
		Item		{ "None", 'chng', none, noKey	},
		Separator	{								}
	}
};

resource 'DLOG' (6, "Info") {
	{0, 0, 400, 250}, "File Options", B_TITLED_WINDOW, NORMAL,
	{
		TabbedBook				{{   0,   0, 400, 250 },	"tabbook"																},

			TabSheet			{ "Editor",					"Editor Options"														},
				CheckBox		{{   0,   0, 140,  16 },	"show tabs",		"Show Tabstops"										},
				Edit			{{ 150,   0, 275,  16 },	"tabs",				"Spaces per Tab:",	"", "0123456789", 3, 120		},
				CheckBox		{{   0,  20, 140,  36 },	"syntaxcol",		"Syntax Colouring"									},
				CheckBox		{{ 150,  20, 275,  36 },	"show invisibles",	"Show Invisibles"									},
				
				Line			{{   0,  43, 275,  44 }																				},
				
				PopupMenu		{{  -1,  51, 140,  71 },	"font",				"Font:",			rid_Popu_InfFont, 30			},
				Edit			{{ 150,  51, 275,  71 },	"size",				"Size:",			"", "0123456789", 2, 90			},
				
				Line			{{   0,  78, 275,  79 }																				},
			
				PopupMenu		{{  -1,  84, 275, 104 },	"source encoding",	"Input Encoding:",	rid_Popu_InfSrcEncoding, 110		},
				PopupMenu		{{  -1, 104, 275, 124 },	"encoding",			"Output Encoding:",	rid_Popu_InfEncoding, 110		},
				PopupMenu		{{  -1, 126, 275, 146 },	"linebreaks",		"Linebreaks:",		rid_Popu_InfLinebreaks, 110		},
				
				Line			{{   0, 151, 275, 152 }},
				
				PopupMenu		{{  -1, 157, 275, 177 },	"language",			"Language:",		rid_Popu_InfLanguage, 80		},
			
			TabSheetEnd			{																									},
			
			TabSheet			{ "Statistics",				"Statistical and State Information"										},
				Caption			{{   0,   0,  75,  16 },	"cname",			"Name:"												},
				Caption			{{  80,   0, 240,  16 },	"name",				"Not Saved"											},
				Caption			{{   0,  20,  75,  36 },	"ctime",			"Last saved:"										},
				Caption			{{  80,  20, 240,  36 },	"time",				""													},
				PopupMenu		{{  -1,  38, 240,  58 },	"mime",				"Mimetype:",		rid_Popu_InfMimetypes, 77		},
				Caption			{{   0,  60,  75,  76 },	"csize",			"Size:"												},
				Caption			{{  80,  60, 240,  76 },	"docsize",			""													},
				Caption			{{   0,  80,  75,  96 },	"clines",			"Lines:"											},
				Caption			{{  80,  80, 240,  96 },	"lines",			""													},
			TabSheetEnd			{},
			
			TabSheet			{ "Wrapping",				"Options for Soft Wrapping"												},
				CheckBox		{{   0,   0, 100,  16 },	"softwrap",			"Soft Wrap Files"									},
				
				Caption			{{   0,  40, 238,  56 },	"w",				"And when wrapping use these settings:"				},
				
				RadioButton		{{   0,  60, 100,  76 },	"wrapwindow",		"Window Width"										},
				RadioButton		{{   0,  80, 100,  96 },	"wrappaper",		"Paper Width"										},
				RadioButton		{{   0, 100,  98, 116 },	"wrapfixed",		"Fixed Column:"										},
				
				Edit			{{ 100, 100, 140, 116 },	"wrapfixedto",		"",					"", "0123456789", 3, 0			},

			TabSheetEnd			{																									},
			
			Button				{{ 330, 220, 390, 240 },	"ok",				"Apply",			'ok  '							},
			Button				{{ 260, 220, 320, 240 },	"cancel",			"Revert",			'cncl'							},
		TabbedBookEnd			{																									}
	}
};
