/*	$Id: Dialog-Preferences.r,v 1.2 2009/02/28 19:22:20 darkwyrm Exp $

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
#include "PrefControls.h"


resource rtyp_Popu (rid_Popu_PrfFont, "Font menu") {
	"Font",
	{
	}
};

resource rtyp_Popu (rid_Popu_PrfLang, "Language menu") {
	"Language",
	{
	}
};

/*
resource rtyp_Popu (rid_Popu_PrfState, "Saved State") {
	"Language",
	{
		Item		{ "Pe",		'chng',	none,	noKey	},
		Item		{ "BeIDE",	'chng',	none,	noKey	}
	}
};
*/

resource rtyp_Popu (rid_Popu_PrfDefaultLang, "Default Language menu") {
	"Language",
	{
		Item		{ "None",	'chng',	none,	noKey	},
		Separator	{									}
	}
};


resource 'DLOG' (132, "Preferences") {
	{0, 0, 415, 266}, "Preferences", B_TITLED_WINDOW, NORMAL,
	{
		TabbedBook				{{   0,   0, 415, 266 }, "tabB" 															},
			TabSheet			{ "Editor", "How the editor should behave"													},
				CheckBox		{{   0,   0, 140,  16 }, pnm_Edi_X_ShowTabs,			"Show Tabstops"						},
				Edit			{{ 150,   0, 290,  16 }, pnm_Edi_T_SpacesPerTab,		"Spaces per Tab:",		"", "0123456789", 3, 90		},

				CheckBox		{{   0,  20, 140,  36 }, pnm_Edi_X_SyntaxColoring,		"Syntax Coloring"					},
				CheckBox		{{ 150,  20, 290,  36 }, pnm_Edi_X_AutoIndent,			"Auto Indent"						},

				CheckBox		{{   0,  40, 140,  56 }, pnm_Edi_X_BalanceWhileTyping,	"Balance While Typing"				},
				CheckBox		{{ 150,  40, 290,  56 }, pnm_Edi_X_SmartBraces,			"Smart Braces"						},

				CheckBox		{{   0,  60, 140,  76 }, pnm_Edi_X_SmartWorkspaces,		"Smart Workspaces"					},
				CheckBox		{{ 150,  60, 290,  76 }, pnm_Edi_X_AlternateHome,		"Alternate Home"					},

				PopupMenu		{{   0, 81, 190, 97 }, pnm_Edi_P_Font1Face,			"Font:",				rid_Popu_PrfFont, 50		},
				Edit			{{ 200, 81, 290, 97 }, pnm_Edi_T_Font1Size,			"Size:",				"", "0123456789", 2, 40		},

				PopupMenu		{{   0, 101, 190, 117 }, pnm_Edi_P_Font2Face,			"Alt. Font:",			rid_Popu_PrfFont, 50		},
				Edit			{{ 200, 101, 290, 117 }, pnm_Edi_T_Font2Size,			"Size:",				"", "0123456789", 2, 40		},

			TabSheetEnd			{																							},

			TabSheet			{ "Files", "Options for files on disk"														},
				CheckBox		{{ 0,  0, 290,  16 }, pnm_Fil_X_MakeBackup,			"Backup files when saving"				},
			
				Edit			{{   0, 24, 180, 40 }, pnm_Fil_T_NrOfRecentDocs,		"Number of Recent Documents", "", "0123456789", 2, 140	},
			TabSheetEnd			{																							},

			TabSheet			{ "Keybindings", "The bindings between key shortcuts and actions"							},

				Caption			{{   0,   0,  40,  16 }, "actcap",		"Action:"											},
				OutlineList		{{  42,   2, 288, 131 }, "kact"																},

				Caption			{{   0, 139,  40, 155 }, "keycap",		"Keys:"												},
				List			{{  42, 141, 168, 179 }, "keys"																},

				KeyCapture		{{ 180, 139, 290, 155 }, "kebi"																},

				Button			{{ 180, 159, 230, 179 }, "adke",		"Add",							'AddK'				},
				Button			{{ 240, 159, 290, 179 }, "deke",		"Delete",						'DelK'				},
			TabSheetEnd			{																							},

			TabSheet			{ "Languages", "Options for Syntax Coloring and the Function Popup"						},
				CheckBox		{{   0,   0, 140,  16 }, pnm_Lng_X_ShowIncludes,		"Show Includes"						},
				CheckBox		{{   0,  20, 140,  36 }, pnm_Lng_X_ShowPrototypes,		"Show Function Prototypes"			},
				CheckBox		{{ 150,   0, 290,  16 }, pnm_Lng_X_ShowTypes,			"Show Types"						},
				CheckBox		{{ 150,  20, 290,  36 }, pnm_Lng_X_SortPopup,			"Sort Alphabetically"				},

				Line			{{   0,  40, 290,  41 }},

				Caption			{{   0,  50, 200,  66 }, "lc",							"Suffix Mapping:"					},
				PopupMenu		{{   0,  70, 120,  86 }, pnm_Lng_P_Language,			"Language:",			rid_Popu_PrfLang, 55},
				Edit			{{ 130,  70, 290,  86 }, pnm_Lng_T_Suffixes,			"Suffixes:",			"", "", 0, 45},

				Line			{{   0,  93, 290,  94 }},

				PopupMenu		{{   0, 101, 200, 117 }, pnm_Lng_P_DefaultLanguage,		"Default Language:",	rid_Popu_PrfDefaultLang, 90	},
			TabSheetEnd			{																							},

			TabSheet			{ "Search Patterns", "Standard Search Patterns for the Find window"							},

				GrepListBox		{{   0,   0, 290, 170 }, "grls"																},

			TabSheetEnd			{																							},

			TabSheet			{ "Startup", "What to do when PalEdit is launched"												},
				RadioButton		{{   0,   0, 145,  16 }, pnm_Sup_R_CreateNewDocument,	"Create New Document"				},
				RadioButton		{{   0,  20, 145,  36 }, pnm_Sup_R_ShowOpenDialog,		"Show Open Dialog"					},
				RadioButton		{{   0,  40, 290,  56 }, pnm_Sup_R_DoNothing,			"Do Nothing"						},
				Caption			{{  0,  56, 290,  72 }, "s_w_c",		" (only useful when used with a worksheet)"	},

			TabSheetEnd			{																							},

			TabSheet			{ "Syntax Colors", "The colors used for syntax highlighting"								},
				ColorControl	{{   0,   0, 145,  16 }, pnm_Col_C_Low,					"Background"						},
				ColorControl	{{ 155,   0, 290,  16 }, pnm_Col_C_Text,				"Normal Text"						},

				ColorControl	{{   0,  16, 145,  32 }, pnm_Col_C_Selection,			"Selection"							},
				ColorControl	{{ 155,  16, 290,  32 }, pnm_Col_C_Mark,				"Mark"								},

				Line			{{   0,  36, 290,  37 }																		},

				ColorControl	{{   0,  40, 110,  56 }, pnm_Col_C_Keyword1,			"Keywords"							},
				ColorControl	{{ 110,  40, 145,  56 }, pnm_Col_C_Keyword2,			""									},
				ColorControl	{{ 155,  40, 255,  56 }, pnm_Col_C_Comment1,			"Comments"							},
				ColorControl	{{ 255,  40, 290,  56 }, pnm_Col_C_Comment2,			""									},

				ColorControl	{{   0,  56, 110,  72 }, pnm_Col_C_String1,				"Strings"							},
				ColorControl	{{ 110,  56, 145,  72 }, pnm_Col_C_String2,				""									},
				ColorControl	{{ 155,  56, 255,  72 }, pnm_Col_C_Number1,				"Numbers"							},
				ColorControl	{{ 255,  56, 290,  72 }, pnm_Col_C_Number2,				""									},

				ColorControl	{{   0,  72, 110,  88 }, pnm_Col_C_Operator1,			"Operators"							},
				ColorControl	{{ 110,  72, 145,  88 }, pnm_Col_C_Operator2,			""									},
				ColorControl	{{ 155,  72, 255,  88 }, pnm_Col_C_Separator1,			"Separators"						},
				ColorControl	{{ 255,  72, 290,  88 }, pnm_Col_C_Separator2,			""									},

				ColorControl	{{   0,  88, 110, 104 }, pnm_Col_C_Preprocessor1,		"PreProcessor"						},
				ColorControl	{{ 110,  88, 145, 104 }, pnm_Col_C_Preprocessor2,		""									},
				ColorControl	{{ 155,  88, 255, 104 }, pnm_Col_C_Error1,				"Errors"							},
				ColorControl	{{ 255,  88, 290, 104 }, pnm_Col_C_Error2,				""									},

				Line			{{   0, 108, 290, 109 }},

				ColorControl	{{   0, 112, 145, 128 }, pnm_Col_C_IdentifierSystem,	"System Identifiers"				},
				ColorControl	{{ 155, 112, 290, 128 }, pnm_Col_C_CharConst,			"Char Constants"					},
				ColorControl	{{   0, 128, 145, 144 }, pnm_Col_C_IdentifierUser,		"User Identifiers"					},
				ColorControl	{{ 155, 128, 255, 144 }, pnm_Col_C_Tag,					"Tags/Attribut."					},
				ColorControl	{{ 255, 128, 290, 144 }, pnm_Col_C_Attribute,			""									},

				Line			{{   0, 148, 290, 149 }																		},

				ColorControl	{{   0, 152, 145, 168 }, pnm_Col_C_UserSet1,			"User defined 1"					},
				ColorControl	{{ 155, 152, 290, 168 }, pnm_Col_C_UserSet2,			"User defined 2"					},
				ColorControl	{{   0, 168, 145, 184 }, pnm_Col_C_UserSet3,			"User defined 3"					},
				ColorControl	{{ 155, 168, 290, 184 }, pnm_Col_C_UserSet4,			"User defined 4"					},

			TabSheetEnd			{																							},

			TabSheet			{ "Worksheet", "Options having effect on worksheets"										},
				CheckBox		{{   0,   0, 284,  16 }, pnm_Wor_X_KeepWorksheetOpen,	"Always keep a worksheet open"		},
				CheckBox		{{   0,  20, 284,  36 }, pnm_Wor_X_RedirectStderr,		"Redirect stderr to Error Window"	},
				StdErrBox		{{   0,  40, 284, 166 }, "StEb"																},
			TabSheetEnd			{																							},

			TabSheet			{ "Wrapping", "Options for softwrapping" },
				CheckBox		{{   0,   0, 100,  16 }, pnm_Wrp_X_SoftWrapFiles,		"Use word wrap"						},

				Caption			{{   0,  40, 238,  56 }, "w",			"Wrap to:"				},

				RadioButton		{{   0,  60, 100,  76 }, pnm_Wrp_R_WindowWidth,			"Window Width"						},
				RadioButton		{{   0,  80, 100,  96 }, pnm_Wrp_R_PaperWidth,			"Paper Width"						},
				RadioButton		{{   0, 100,  98, 116 }, pnm_Wrp_R_FixedColumn,			"Fixed Column:"						},

				Edit			{{ 100, 100, 140, 116 }, pnm_Wrp_T_FixedColumn,			"",		"", "0123456789", 3, 0		},

			TabSheetEnd			{																							},

			Button				{{ 335, 235, 390, 255 }, "ok  ",		"Apply",			'ok  '							},
			Button				{{ 265, 235, 320, 255 }, "cncl",		"Revert",			'cncl'							},
		TabbedBookEnd			{}
	}
};
