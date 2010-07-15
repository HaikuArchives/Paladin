/*	$Id: Bindings.r,v 1.2 2009/02/28 19:22:20 darkwyrm Exp $
	
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
#include "ResourcesBindings.h"
#include "KeyBindings.h"


//		kmsg_OpenHeaderSource, { Shift, '\t' }, "Open Header/Source File",
//		"Abort Shell Command",

resource rtyp_Cmnd (rid_Cmnd_Editing, "Editing commands") {
	{
		kmsg_NrArgument,							"Numerical Argument Prefix",
		kmsg_Recenter,								"Recenter",
		kmsg_OpenLine,								"Open Line",
		kmsg_AppendNextCut,							"Append Next Cut",
		kmsg_ClearRegion,							"Delete Region between Caret and Mark",
		kmsg_CopyRegion,							"Copy Region between Caret and Mark",
		kmsg_CutRegion,								"Cut Region between Caret and Mark",
		kmsg_ExchangeMarkAndPoint,					"Exchange Caret and Mark",
		kmsg_MarkWord,								"Mark Word",
		kmsg_MarkAll,								"Mark All",
		kmsg_Mark,									"Mark",
		kmsg_CutWordBackward,						"Cut Previous Word",
		kmsg_CutWord,								"Cut Next Word",
		kmsg_CutToEndOfLine,						"Cut to End of Line",
		kmsg_DeleteToEndOfFile,						"Delete to End of File",
		kmsg_DeleteToEndOfLine,						"Delete to End of Line",
		kmsg_DeleteToBeginningOfLine,				"Delete to Beginning of Line",
		kmsg_DeleteCharacterRight,					"Delete Character Right",
		kmsg_DeleteCharacterLeft,					"Delete Character Left",
		kmsg_ScrollToEndOfFile,						"Scroll to End of File",
		kmsg_ScrollToStartOfFile,					"Scroll to Start of File",
		kmsg_ScrollPageDown,						"Scroll Page Down",
		kmsg_ScrollPageUp,							"Scroll Page Up",
		kmsg_ScrollOneLineDown,						"Scroll One Line Down",
		kmsg_ScrollOneLineUp,						"Scroll One Line Up",
		kmsg_ExtendSelectionToEndOfFile,			"Extend Selection to End of File",
		kmsg_ExtendSelectionToBeginningOfFile,		"Extend Selection to Beginning of File",
		kmsg_ExtendSelectionToEndOfPage,			"Extend Selection to End of Page",
		kmsg_ExtendSelectionToBeginningOfPage,		"Extend Selection to Beginning of Page",
		kmsg_ExtendSelectionToNextPage,				"Extend Selection to Next Page",
		kmsg_ExtendSelectionToPreviousPage,			"Extend Selection to Previous Page",
		kmsg_ExtendSelectionToCurrentLine,			"Extend Selection to Current Line",
		kmsg_ExtendSelectionToNextLine,				"Extend Selection to Next Line",
		kmsg_ExtendSelectionToPreviousLine,			"Extend Selection to Previous Line",
		kmsg_ExtendSelectionToEndOfLine,			"Extend Selection to End of Line",
		kmsg_ExtendSelectionToBeginningOfLine,		"Extend Selection to Beginning of Line",
		kmsg_ExtendSelectionWithNextSubword,		"Extend Selection with Next SubWord",
		kmsg_ExtendSelectionWithPreviousSubword,	"Extend Selection with Previous SubWord",
		kmsg_ExtendSelectionWithNextWord,			"Extend Selection with Next Word",
		kmsg_ExtendSelectionWithPreviousWord,		"Extend Selection with Previous Word",
		kmsg_ExtendSelectionWithCharacterRight,		"Extend Selection with Character Right",
		kmsg_ExtendSelectionWithCharacterLeft,		"Extend Selection with Character Left",
		kmsg_MoveToEndOfFile,						"Move to End of File",
		kmsg_MoveToBeginningOfFile,					"Move to Beginning of File",
		kmsg_MoveToBottomOfPage,					"Move to Bottom of Page",
		kmsg_MoveToTopOfPage,						"Move to Top of Page",
		kmsg_MoveToNextPage,						"Move to Next Page",
		kmsg_MoveToPreviousPage,					"Move to Previous Page",
		kmsg_MoveToNextLine,						"Move to Next Line",
		kmsg_MoveToPreviousLine,					"Move to Previous Line",
		kmsg_MoveToEndOfLine,						"Move to End of Line",
		kmsg_MoveToBeginningOfLine,					"Move to Beginning of Line",
		kmsg_MoveWordRight,							"Move Word Right",
		kmsg_MoveWordLeft,							"Move Word Left",
		kmsg_MoveSubwordRight,						"Move SubWord Right",
		kmsg_MoveSubwordLeft,						"Move SubWord Left",
		kmsg_MoveCharacterRight,					"Move Character Right",
		kmsg_MoveCharacterLeft,						"Move Character Left"
	}
};

resource rtyp_Cmnd (rid_Cmnd_FileMenu, "File Menu") {
	{
		msg_Quit,									"Quit",
		msg_Info,									"Properties…",
		msg_Revert,									"Revert",
		msg_SaveCopy,								"Save a Copy As…",
		msg_SaveAs,									"Save As…",
		msg_SaveAll,								"Save All",
		msg_Save,									"Save",
		msg_CloseAll,								"Close All",
		msg_Close,									"Close",
		msg_OpenSelected,							"Open Selected File",
		msg_Open,									"Open…",
		msg_NewGroup,								"New Group",
		msg_New,									"New"
	}
};

resource rtyp_Cmnd (rid_Cmnd_EditMenu, "Edit Menu") {
	{
		msg_SelectParagraph,						"Select Paragraph",
		msg_SelectLine,								"Select Line",
		'SALL',										"Select All",
		msg_Clear,									"Clear",
		'PSTE',										"Paste",
		msg_CopyAppend,								"Copy and Append",
		'COPY',										"Copy",
		msg_CutAppend,								"Cut and Append",
		'CCUT',										"Cut",
		msg_Redo,									"Redo",
		msg_Undo,									"Undo"
	}
};

resource rtyp_Cmnd (rid_Cmnd_TextMenu, "Text Menu") {
	{
		msg_Unwrap,									"Remove Linebreaks",
		msg_Wrap,									"Insert Linebreaks",
		msg_Justify,								"Justify",
		msg_Uncomment,								"Uncomment",
		msg_Comment,								"Comment",
		msg_ShiftRight,								"Shift Right",
		msg_ShiftLeft,								"Shift Left",
		msg_ConvertEncoding,						"Change Encoding…",
		msg_ChangeCaseSent,							"Capitalize Sentences",
		msg_ChangeCaseLine,							"Capitalize Lines",
		msg_ChangeCaseCap,							"Capitalize Words",
		msg_ChangeCaseLower,						"Change to Lowercase",
		msg_ChangeCaseUpper,						"Change to Uppercase",
		msg_Twiddle,								"Twiddle",
		msg_Balance,								"Balance"
	}
};

resource rtyp_Cmnd (rid_Cmnd_SearchMenu, "Search Menu") {
	{
		msg_GoToLine,								"Go To Line…",
		msg_FindNextError,							"Jump to Next Error",
		msg_FindInNextFile,							"Find in Next File",
		msg_ReplaceAll,								"Replace All",
		msg_ReplaceAndFindBackward,					"Replace and Find Backwards",
		msg_ReplaceAndFind,							"Replace and Find",
		msg_Replace,								"Replace…",
		msg_EnterReplaceString,						"Enter Replace String",
		msg_EnterSearchString,						"Enter Search String",
		msg_FindSelectionBackward,					"Find Selection Backwards",
		msg_FindSelection,							"Find Selection",
		msg_FindAgainBackward,						"Find Again Backwards",
		msg_FindAgain,								"Find Again",
		msg_FindCmd,								"Find…",
		msg_FindFunction,							"Find Function…",
		msg_PreviousFunction,						"Previous Function",
		msg_NextFunction,							"Next Function"
	}
};

resource rtyp_Cmnd (rid_Cmnd_WindowMenu, "Window Menu") {
	{
		msg_Worksheet,								"Open Worksheet",
		msg_Zoom,									"Zoom",
		msg_Tile,									"Tile",
		msg_Stack,									"Stack",
		msg_ChangeWorkingDir,						"Working Directory…",
		msg_Preferences,							"Preferences…",
	}
};

resource rtyp_Cmnd (rid_Cmnd_ProjectMenu, "Project Menu") {
	{
		msg_IdeAdd,									"Add File",
		msg_IdeRemove,								"Remove File",
		msg_IdeMake,								"Make",
		msg_IdeRun,									"Run",
		msg_IdeRunInTerminal,						"Run in Terminal",
		msg_IdeRunInDebugger,						"Run in Debugger",
		msg_IdeForceRebuild,						"Force Rebuild",
		msg_IdeToggleErrors,						"Toggle Errors…"
	}
};

resource rtyp_Cmnd (rid_Cmnd_Miscellaneous, "Miscellaneous") {
	{
		kmsg_UnsplitWindow,							"Unsplit Window",
		kmsg_SwitchActivePart,						"Switch to Other Part of Window",
		kmsg_SplitWindow,							"Split Window",
		msg_CancelCommand,							"Cancel Executing Command",
		msg_Execute,								"Execute Command",
		msg_SwitchHeaderSource,						"Open Header/Source",
	}
};

resource rtyp_Cmnd (rid_Cmnd_Extensions, "Extensions") {
	{
	}
};

// Ctrl = left Ctrl key on intel keyboard
// Opt = right Ctrl key on intel keyboard
// Cmd = either Alt key on intel keyboard

resource rtyp_Bind (rid_Bind_Editing, "Bindings for Editing") {
	{
		0,				0x61,	0,		0,			kmsg_MoveCharacterLeft,
		0,				0x63,	0,		0,			kmsg_MoveCharacterRight,
		Cmd,			0x61,	0,		0,			kmsg_MoveWordLeft,
		Cmd,			0x63,	0,		0,			kmsg_MoveWordRight,
		Opt,			0x61,	0,		0,			kmsg_MoveSubwordLeft,
		Opt,			0x63,	0,		0,			kmsg_MoveSubwordRight,
		0,				0x20,	0,		0,			kmsg_MoveToBeginningOfLine,
		0,				0x35,	0,		0,			kmsg_MoveToEndOfLine,
		0,				0x57,	0,		0,			kmsg_MoveToPreviousLine,
		0,				0x62,	0,		0,			kmsg_MoveToNextLine,
		0,				0x21,	0,		0,			kmsg_MoveToTopOfPage,
		0,				0x36,	0,		0,			kmsg_MoveToBottomOfPage,
		Cmd,			0x20,	0,		0,			kmsg_MoveToBeginningOfFile,
		Cmd,			0x35,	0,		0,			kmsg_MoveToEndOfFile,
		0,				0x1e,	0,		0,			kmsg_DeleteCharacterLeft,
//		Ctrl,			0x1e,	0,		0,			kmsg_DeleteToBeginningOfLine,
		0,				0x34,	0,		0,			kmsg_DeleteCharacterRight,
//		Ctrl,			0x34,	0,		0,			kmsg_DeleteToEndOfLine,
		Shift,			0x61,	0,		0,			kmsg_ExtendSelectionWithCharacterLeft,
		Shift,			0x63,	0,		0,			kmsg_ExtendSelectionWithCharacterRight,
		Shift|Cmd,		0x61,	0,		0,			kmsg_ExtendSelectionWithPreviousWord,
		Shift|Cmd,		0x63,	0,		0,			kmsg_ExtendSelectionWithNextWord,
		Shift|Opt,		0x61,	0,		0,			kmsg_ExtendSelectionWithPreviousSubword,
		Shift|Opt,		0x63,	0,		0,			kmsg_ExtendSelectionWithNextSubword,
		Shift,			0x57,	0,		0,			kmsg_ExtendSelectionToPreviousLine,
		Shift,			0x62,	0,		0,			kmsg_ExtendSelectionToNextLine,
		Shift,			0x20,	0,		0,			kmsg_ExtendSelectionToBeginningOfLine,
		Shift,			0x35,	0,		0,			kmsg_ExtendSelectionToEndOfLine,
		Shift,			0x21,	0,		0,			kmsg_ExtendSelectionToBeginningOfPage,
		Shift,			0x36,	0,		0,			kmsg_ExtendSelectionToEndOfPage,
		Shift|Cmd,		0x20,	0,		0,			kmsg_ExtendSelectionToBeginningOfFile,
		Shift|Cmd,		0x35,	0,		0,			kmsg_ExtendSelectionToEndOfFile,
		Opt,			0x21,	0,		0,			kmsg_ScrollPageUp,
		Opt,			0x36,	0,		0,			kmsg_ScrollPageDown,
		Opt,			0x20,	0,		0,			kmsg_ScrollToStartOfFile,
		Opt,			0x35,	0,		0,			kmsg_ScrollToEndOfFile,
		Opt,			0x57,	0,		0,			kmsg_ScrollOneLineUp,
		Opt,			0x62,	0,		0,			kmsg_ScrollOneLineDown,
		// keypad-keys:
		0,				0x48,	0,		0,			kmsg_MoveCharacterLeft,
		0,				0x4a,	0,		0,			kmsg_MoveCharacterRight,
		Ctrl,			0x48,	0,		0,			kmsg_MoveWordLeft,
		Ctrl,			0x4a,	0,		0,			kmsg_MoveWordRight,
		Opt,			0x48,	0,		0,			kmsg_MoveSubwordLeft,
		Opt,			0x4a,	0,		0,			kmsg_MoveSubwordRight,
		0,				0x37,	0,		0,			kmsg_MoveToBeginningOfLine,
		0,				0x58,	0,		0,			kmsg_MoveToEndOfLine,
		0,				0x38,	0,		0,			kmsg_MoveToPreviousLine,
		0,				0x59,	0,		0,			kmsg_MoveToNextLine,
		0,				0x39,	0,		0,			kmsg_MoveToTopOfPage,
		0,				0x5a,	0,		0,			kmsg_MoveToBottomOfPage,
		Ctrl,			0x37,	0,		0,			kmsg_MoveToBeginningOfFile,
		Ctrl,			0x58,	0,		0,			kmsg_MoveToEndOfFile,
		Opt,			0x37,	0,		0,			kmsg_MoveToBeginningOfFile,
		Opt,			0x58,	0,		0,			kmsg_MoveToEndOfFile,
		0,				0x65,	0,		0,			kmsg_DeleteCharacterRight,
		Ctrl,			0x65,	0,		0,			kmsg_DeleteToEndOfLine,
		Shift,			0x48,	0,		0,			kmsg_ExtendSelectionWithCharacterLeft,
		Shift,			0x4a,	0,		0,			kmsg_ExtendSelectionWithCharacterRight,
		Shift|Cmd,		0x48,	0,		0,			kmsg_ExtendSelectionWithPreviousWord,
		Shift|Cmd,		0x4a,	0,		0,			kmsg_ExtendSelectionWithNextWord,
		Shift|Opt,		0x48,	0,		0,			kmsg_ExtendSelectionWithPreviousSubword,
		Shift|Opt,		0x4a,	0,		0,			kmsg_ExtendSelectionWithNextSubword,
		Shift,			0x38,	0,		0,			kmsg_ExtendSelectionToPreviousLine,
		Shift,			0x59,	0,		0,			kmsg_ExtendSelectionToNextLine,
		Shift,			0x37,	0,		0,			kmsg_ExtendSelectionToBeginningOfLine,
		Shift,			0x58,	0,		0,			kmsg_ExtendSelectionToEndOfLine,
		Shift,			0x39,	0,		0,			kmsg_ExtendSelectionToBeginningOfPage,
		Shift,			0x5a,	0,		0,			kmsg_ExtendSelectionToEndOfPage,
		Shift|Cmd,		0x37,	0,		0,			kmsg_ExtendSelectionToBeginningOfFile,
		Shift|Cmd,		0x58,	0,		0,			kmsg_ExtendSelectionToEndOfFile,
//		Shift|Opt,		0x37,	0,		0,			kmsg_ExtendSelectionToBeginningOfFile,
//		Shift|Opt,		0x58,	0,		0,			kmsg_ExtendSelectionToEndOfFile,
		Cmd,			0x26,	0,		0,			msg_SwitchHeaderSource
	}
};

resource rtyp_Bind (rid_Bind_FileMenu, "Bindings for File Menu") {
	{
		Cmd,			0x27,	0,		0,			msg_Quit,
//		0,				0,		0, 		0,			msg_Info,
//		0,				0,		0, 		0,			msg_Revert,
//		0,				0,		0, 		0,			msg_SaveCopy,
//		0,				0,		0, 		0,			msg_SaveAs,
		Cmd|Shift,		0x3d,	0,		0,			msg_SaveAll,
		Cmd,			0x3d,	0,		0,			msg_Save,
		Cmd|Shift,		0x28,	0,		0,			msg_CloseAll,
		Cmd,			0x28,	0,		0,			msg_Close,
		Cmd,			0x3e,	0,		0,			msg_OpenSelected,
		Cmd,			0x2f,	0,		0,			msg_Open,
//		0,				0,		0, 		0,			msg_NewGroup,
		Cmd,			0x51,	0,		0,			msg_New
	}
};

resource rtyp_Bind (rid_Bind_EditMenu, "Bindings for Edit Menu") {
	{
		Cmd,			0x2b,	0,		0,			msg_SelectLine,
		Cmd,			0x3c,	0,		0,			'SALL',
		Cmd,			0x4f,	0,		0,			'PSTE',
		Cmd,			0x4e,	0,		0,			'COPY',
		Cmd|Shift,		0x4e,	0,		0,			msg_CopyAppend,
		Cmd,			0x4d,	0,		0,			'CCUT',
		Cmd|Shift,		0x4d,	0,		0,			msg_CutAppend,
		Cmd|Shift,		0x4c,	0,		0,			msg_Redo,
		Cmd,			0x4c,	0,		0,			msg_Undo
	}
};

resource rtyp_Bind (rid_Bind_TextMenu, "Bindings for Text Menu") {
	{
		Cmd,			0x50,	0,		0,			msg_Balance,
		Cmd,			0x11,	0,		0,			msg_Twiddle,
//		0,				0,		0, 		0,			msg_ChangeCaseUpper,
//		0,				0,		0, 		0,			msg_ChangeCaseLower,
//		0,				0,		0, 		0,			msg_ChangeCaseCap,	
//		0,				0,		0, 		0,			msg_ConvertEncoding,
		Cmd,			0x31,	0,		0,			msg_ShiftLeft,
		Cmd,			0x32,	0,		0,			msg_ShiftRight
//		0,				0,		0, 		0,			msg_Comment,
//		0,				0,		0, 		0,			msg_Uncomment,
//		Cmd,			0x33,	0,		0,			msg_Wrap,
//		0,				0,		0, 		0,			msg_Unwrap,
	}
};

resource rtyp_Bind (rid_Bind_SearchMenu, "Bindings for Search Menu") {
	{
		Cmd,			0x53,	0,		0,			msg_GoToLine,	
//		Cmd,			0x2e,	0,		0,			msg_FindNextError,
		Cmd,			0x42,	0,		0,			msg_FindInNextFile,
//		0,				0,		0, 		0,			msg_ReplaceAll,
		Cmd|Shift,		0x44,	0,		0,			msg_ReplaceAndFindBackward,
		Cmd,			0x44,	0,		0,			msg_ReplaceAndFind,
		Cmd,			0x1d,	0,		0,			msg_Replace,
		Cmd|Shift,		0x29,	0,		0,			msg_EnterReplaceString,
		Cmd,			0x29,	0,		0,			msg_EnterSearchString,
		Cmd|Shift,		0x41,	0,		0,			msg_FindSelectionBackward,
		Cmd,			0x41,	0,		0,			msg_FindSelection,
		Cmd|Shift,		0x40,	0,		0,			msg_FindAgainBackward,
		Cmd,			0x40,	0,		0,			msg_FindAgain,
		Cmd,			0x3f,	0,		0,			msg_FindCmd,
		Cmd,			0x2e,	0,	 	0,		 	msg_IncSearch,
		Cmd|Shift,		0x2e,	0,		0,			msg_IncSearchBackward,
		Cmd|Shift,		0x3f,	0,		0,			msg_FindFunction
	}
};

resource rtyp_Bind (rid_Bind_WindowMenu, "Bindings for Window Menu") {
	{
//		0,				0,		0, 		0,			msg_FontTabs,
//		0,				0,		0, 		0,			msg_Info,
//		0,				0,		0, 		0,			msg_Preferences,
//		0,				0,		0, 		0,			msg_ChangeWorkingDir,
//		0,				0,		0, 		0,			msg_Stack,
//		0,				0,		0, 		0,			msg_Tile,
//		0,				0,		0,		0,			msg_Zoom,
		Cmd,			0x1b,	0,		0,			msg_Worksheet
	}
};

resource rtyp_Bind (rid_Bind_ProjectMenu, "Bindings for Project Menu") {
	{
		Cmd,			0x1c,		0,			0,			msg_IdeForceRebuild,
		Ctrl|Cmd,		0x2a,		0,			0,			msg_IdeRunInDebugger,
		Shift|Cmd,		0x2a,		0,			0,			msg_IdeRunInTerminal,
		Cmd,			0x2a,		0,			0,			msg_IdeRun,
		Cmd,			0x52,		0,			0,			msg_IdeMake,
//		0,				0,			0,			0,			msg_IdeRemove,
//		0,				0,			0,			0,			msg_IdeAdd,
		Cmd,			0x2e,		0,			0,			msg_IdeToggleErrors
	}
};

resource rtyp_Bind (rid_Bind_Miscellaneous, "Miscellaneous Bindings") {
	{
		Ctrl,			0x4e,	0,		0,			msg_CancelCommand,
		Cmd,			0x47,	0,		0,			msg_Execute,
		0,				0x5b,	0,		0,			msg_Execute,
		Cmd,			0x26,	0,		0,			msg_SwitchHeaderSource
	}
};

resource rtyp_Bind (rid_Bind_Emacs, "Emacs Bindings") {
	{
		Ctrl,			0x2d,	0,		0,			kmsg_NrArgument,
		Ctrl,			0x44,	0,		0,			kmsg_Recenter,
		Ctrl,			0x2f,	0,		0,			kmsg_OpenLine,
		Ctrl,			0x28,	0,		0x01,		kmsg_AppendNextCut,
//		Cmd,			0x26,	0,		0,			kmsg_ClearRegion,	
		0,				0x28,	0,		0x01,		kmsg_CopyRegion,
		Ctrl,			0x28,	0,		0,			kmsg_CutRegion,
		Ctrl,			0x4d,	Ctrl,	0x4d,		kmsg_ExchangeMarkAndPoint,
		Shift,			0x13,	0,		0x01,		kmsg_MarkWord,
		0,				0x41,	Ctrl,	0x4d,		kmsg_MarkAll,
		Ctrl,			0x5e,	0,		0,			kmsg_Mark,
//		Ctrl,			0x34,	0,		0,			kmsg_CutWordBackward,
		0,				0x3e,	0,		0x01,		kmsg_CutWord,
		Ctrl,			0x43,	0,		0,			kmsg_CutToEndOfLine,
		Ctrl,			0x4c,	0,		0, 			'PSTE',
		0,				0x13,	Ctrl,	0x4d,		kmsg_SplitWindow,
		0,				0x12,	Ctrl,	0x4d,		kmsg_UnsplitWindow,
		0,				0x2f,	Ctrl,	0x4d,		kmsg_SwitchActivePart
	}
};
