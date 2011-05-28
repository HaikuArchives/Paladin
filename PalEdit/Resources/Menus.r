/*	$Id: Menus.r,v 1.2 2009/02/28 19:22:20 darkwyrm Exp $
	
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
#include "PMessages.h"


/* Edit Window Menu */

resource rtyp_Menu (rid_Menu_DwFile, "File Menu") {
	"File",
	{
		Item		{ "New",					msg_New,						none,	'N'		},
/*		Item		{ "New Group",				msg_NewGroup,					none,	noKey	}, */
		Item		{ "Open…",					msg_Open,						none,	'O'		},
		Item		{ "Open Selection",			msg_OpenSelected,				none,	'D'		},
		Submenu		{ rid_Menu_DwRecent															},	
		Item		{ "Close",					msg_Close,						none,	'W'		},
		Separator	{																			},
		Item		{ "Save",					msg_Save,						none,	'S'		},
		Item		{ "Save As…",				msg_SaveAs,						none,	noKey	},
		Item		{ "Save a Copy As…",		msg_SaveCopy,					none,	noKey	},
		Item		{ "Save All",				msg_SaveAll,					none,	noKey	},
		Item		{ "Revert…",				msg_Revert,						none,	noKey	},
		Separator	{																			},
		Item		{ "Properties…",			msg_Info,						none,	noKey	},
		Separator	{																			},
		Item		{ "Page Setup…",			msg_PageSetup,					none,	noKey	},
		Item		{ "Print…",					msg_Print,						none,	'P'		},
		Separator	{																			},
		Item		{ "About PalEdit…",			msg_About,						none,	noKey	},
		Separator	{																			},
		Item		{ "Quit",					msg_Quit,						none,	'Q'		}
	}
};

resource rtyp_Menu (rid_Menu_DwEdit, "Edit Menu") {
	"Edit",
	{
		Item		{ "Undo",					msg_Undo,						none,	'Z'		},
		Item		{ "Redo",					msg_Redo,						shift,	'Z'		},
		Separator	{																			},
		Item		{ "Cut",					'CCUT',							none,	'X'		},
		Item		{ "Copy",					'COPY',							none,	'C'		},
		Item 		{ "Paste",					'PSTE',							none,	'V'		},
		Item		{ "Clear",					msg_Clear,						none,	noKey	},
		Separator	{																			},
		Item		{ "Select All",				'SALL',							none,	'A'		},
		Item		{ "Select Line",			msg_SelectLine,					none,	'L'		},
		Item		{ "Select Paragraph",		msg_SelectParagraph,			none,	noKey	}
	}
};

resource rtyp_Menu (rid_Menu_DwText, "Text Menu") {
	"Text",
	{
		Item		{ "Balance",				msg_Balance,					none,	'B'		},
		Item		{ "Twiddle",				msg_Twiddle,					none,	'\''	},
		Submenu		{ rid_Menu_DwChangeCase														},
		Item		{ "Change Encoding…",		msg_ConvertEncoding,			none,	noKey	},
		Separator	{																			},
		Item		{ "Shift Left",				msg_ShiftLeft,					none,	'['		},
		Item		{ "Shift Right",			msg_ShiftRight,					none,	']'		},
		Separator	{																			},
		Item		{ "Comment",				msg_Comment,					none,	noKey	},
		Item		{ "Uncomment",				msg_Uncomment,					none,	noKey	},
		Separator	{																			},
		Item		{ "Justify",				msg_Justify,					none,	noKey	},
		Item		{ "Insert Linebreaks",		msg_Wrap,						none,	noKey	},
		Item		{ "Remove Linebreaks",		msg_Unwrap,						none,	noKey	}
	}
};

resource rtyp_Menu (rid_Menu_DwSearch, "Search Menu") {
	"Search",
	{
		Item		{ "Find…",					msg_FindCmd,					none,	'F'		},
		Item		{ "Find Again",				msg_FindAgain,					none,	'G'		},
		Item		{ "Find Selection",			msg_FindSelection,				none,	'H'		},
		Item		{ "Enter Search String",	msg_EnterSearchString,			none,	'E'		},
		Separator	{																			},
		Item		{ "Replace",				msg_Replace,					none,	'='		},
		Item		{ "Replace & Find",			msg_ReplaceAndFind,				none,	'T'		},
		Item		{ "Replace All",			msg_ReplaceAll,					none,	noKey	},
		Separator	{																			},
		Item		{ "Find in Next File",		msg_FindInNextFile,				none,	'J'		},
		Separator	{																			},
		Item		{ "Jump to Next Error",		msg_FindNextError,				none,	noKey	},
		Item		{ "Go To Line…",			msg_GoToLine,					none,	','		},
		Item		{ "Find Function…",			msg_FindFunction,				none,	noKey	},
		Item		{ "Previous Function",		msg_PreviousFunction,			none,	noKey	},
		Item		{ "Next Function",			msg_NextFunction,				none,	noKey   },
 		Separator	{																			},
		Item		{ "Find Differences…",		msg_FindDifferences,			none,	noKey	}
	}
};

resource rtyp_Menu (rid_Menu_DwWindow, "Window Menu") {
	"Window",
	{
		Item		{ "Preferences…",			msg_Preferences,				none,	noKey	},
		Item		{ "Working Directory…",		msg_ChangeWorkingDir,			none,	noKey	},
		Separator	{																			},
		Item		{ "Stack",					msg_Stack,						none,	noKey	},
		Item		{ "Tile",					msg_Tile,						none,	noKey	},
		Item		{ "Zoom",					msg_Zoom,						none,	noKey	},
		Separator	{																			},
		Item		{ "Worksheet",				msg_Worksheet,					none,	'0'		},
		Separator	{																			}
	}
};

resource rtyp_Menu (rid_Menu_DwExtensions, "Extensions") {
	"Extensions",
	{
	}
};

resource rtyp_Menu (rid_Menu_DwProject, "Project") {
	"Project",
	{
		Item		{ "Add File",				msg_IdeAdd,						none,	noKey	},
		Item		{ "Remove File",			msg_IdeRemove,					none,	noKey	},
		Separator	{																			},
		Item		{ "Find in Project Files…",	msg_IdeFindInProjectFiles,		shift,	'F'		},
		Separator	{																			},
		Item		{ "Make",					msg_IdeMake,					none,	'M'		},
		Separator	{																			},
		Item		{ "Run",					msg_IdeRun,						none,	'R'		},
		Item		{ "Run Logged…",			msg_IdeRunInTerminal,			shift,	'R'		},
		Item		{ "Run in Debugger",		msg_IdeRunInDebugger,			control,'R'		},
		Separator	{																			},
		Item		{ "Force Rebuild",			msg_IdeForceRebuild,			none,	'-'		},
		Separator	{																			},
		Item		{ "Toggle Errors…",			msg_IdeToggleErrors,			none,	'I'		},
	}
};

resource rtyp_Menu (rid_Menu_DwRecent, "Recent Menu") {
	"Open Recent",
	{
	}
};

resource rtyp_Menu (rid_Menu_DwChangeCase, "Change Case Menu") {
	"Change Case",
	{
		Item		{ "Upper Case",				msg_ChangeCaseUpper,			none,	noKey	},
		Item		{ "Lower Case",				msg_ChangeCaseLower,			none,	noKey	},
		Item		{ "Capitalize Words",		msg_ChangeCaseCap,				none,	noKey	},
		Item		{ "Capitalize Lines",		msg_ChangeCaseLine,				none,	noKey	},
		Item		{ "Capitalize Sentences",	msg_ChangeCaseSent,				none,	noKey	}
	}
};

resource rtyp_Mbar (rid_Mbar_DocumentWin, "Menu Bar") {
	{
		rid_Menu_DwFile,
		rid_Menu_DwEdit,
		rid_Menu_DwText,
		rid_Menu_DwSearch,
		rid_Menu_DwExtensions,
		rid_Menu_DwWindow,
		rid_Menu_DwProject
	}
};

resource rtyp_Menu (rid_Menu_FpMimetypes, "Mime types") {
	"Mime Type",
	{
	}
};

/* Project Window Menu */

resource rtyp_Menu (rid_Menu_PwFile)
{
	"File",
	{
		Item		{ "Close",						msg_Close,					none,	'W'		},
		Separator	{																			},
		Item		{ "Save",						msg_Save,					none,	'S'		},
		Item		{ "Save As…",					msg_SaveAs,					none,	noKey	},
		Item		{ "Revert…",					msg_Revert,					none,	noKey	},
		Separator	{																			},
		Item		{ "Quit",						msg_Quit,					none,	'Q'		}
	}
};

resource rtyp_Menu (rid_Menu_PwEdit)
{
	"Edit",
	{
		Item		{ "Edit as Text",				msg_EditAsText,				none,	noKey	}
	}
};

resource rtyp_Mbar (rid_Mbar_ProjectWin)
{
	{ rid_Menu_PwFile, rid_Menu_PwEdit }
};

resource rtyp_Mbar (rid_Mbar_GroupWin)
{
	{ rid_Menu_PwFile }
};

/* Context Menus */

resource rtyp_Popu (rid_Popu_CtxText, "Main view") {
	"pop-up",
	{
		Item		{ "Save",						msg_Save,					none,	noKey	},
		Separator	{																			},
		Item		{ "Cut",						'CCUT',						none,	noKey	},
		Item		{ "Copy",						'COPY',						none,	noKey	},
		Item 		{ "Paste",						'PSTE',						none,	noKey	},
		Item		{ "Clear",						msg_Clear,					none,	noKey	},
		Item		{ "Select All",					'SALL',						none,	noKey	},
		Separator	{																			},
		Item		{ "Comment",					msg_Comment,				none,	noKey	},
		Item		{ "Uncomment",					msg_Uncomment,				none,	noKey	},
		Separator	{																			},
		Item		{ "Syntax Colouring",			msg_ToggleSyntaxColoring,	none,	noKey	},
		Item		{ "Show Invisibles",			msg_ToggleShowInvisibles,	none,	noKey	},
		Item		{ "Soft Wrap",					msg_SoftWrap,				none,	noKey	},
		Item		{ "Alternate Font",				msg_ToggleFont,				none,	noKey	}
	}
};
