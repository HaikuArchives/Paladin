/*	$Id: PMessages.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#ifndef PMESSAGES_H
#define PMESSAGES_H

#include "CMessages.h"

#define msg_New							'New '
#define msg_Open						'Open'
#define msg_BtnOpen						'BOpn'
#define msg_CommandLineOpen				'Cmdl'
#define msg_OpenRecent					'OpnR'
#define msg_Save						'Save'
#define msg_SaveAs						'SvAs'
#define msg_Revert						'Rvrt'
#define msg_PageSetup					'PagS'
#define msg_Print						'Prin'
#define msg_Close						'Clse'
#define msg_Quit						'Quit'
#define msg_Undo						'Undo'
#define msg_Redo						'Redo'
#define msg_Clear						'Cler'
#define msg_CutAppend					'CutA'
#define msg_CopyAppend					'CpyA'
#define msg_OpenSelected				'Opnd'
#define msg_SelectLine					'Line'
#define msg_SelectParagraph				'Para'
#define msg_SelectLines					'Lins'
//#define msg_FontTabs					'Font'
#define msg_ChangeFontAndTabs			'FTch'
#define msg_ToggleFont					'TgFn'

#define msg_ChangeCaseUpper				'CasU'
#define msg_ChangeCaseLower				'CasL'
#define msg_ChangeCaseCap				'CasW'
#define msg_ChangeCaseLine				'CsLn'
#define msg_ChangeCaseSent				'CsSn'

#define msg_Balance						'Baln'
#define msg_Twiddle						'Twdl'
#define msg_ShiftLeft					'SftL'
#define msg_ShiftRight					'SftR'

#define msg_FindCmd						'FnCm'
#define msg_Find						'Find'
#define msg_FindAgain					'FndA'
#define msg_FindSelection				'FndS'
#define msg_FindSelectionBackward		'FnSB'

#define msg_DlgFind						'DFnd'
#define msg_DlgReplace					'DRpl'
#define msg_DlgReplaceAndFind			'DR&F'
#define msg_DlgReplaceAll				'DRAl'

#define msg_EnterSearchString			'ESSt'
#define msg_Replace						'Rplc'
#define msg_ReplaceAndFind				'RpFn'
#define msg_ReplaceAndFindBackward		'FpFB'
#define msg_ReplaceAll					'RpAl'
#define msg_Zoom						'Zoom'
#define msg_Stack						'Stac'
#define msg_Tile						'Tile'
#define msg_SelectWindow				'SlWn'
	
#define msg_OpenInclude					'OpnI'
#define msg_JumpToProcedure				'JPrc'
#define msg_JumpToMarker				'JMrk'
	
#define msg_FindAgainBackward			'FdAB'
#define msg_EnterReplaceString			'EnRS'
#define msg_FindNextError				'FErr'
#define msg_FindPreviousError			'FErB'
#define msg_FindInNextFile				'FNxt'
	
#define msg_OutputWillFollow			'OutP'
#define msg_Execute						'Exec'
#define msg_ExecFinished				'Fnsh'
#define msg_TypeString					'TypS'
#define msg_Preferences					'Pref'
	
#define msg_Select						'Slct'
#define msg_SaveAll						'SavA'
#define msg_CloseAll					'ClsA'
	
#define msg_Comment						'Cmnt'
#define msg_Uncomment					'UCmt'

#define msg_Wrap						'Wrap'
#define msg_Unwrap						'UWrp'
#define msg_Justify						'Jstf'

#define msg_PrefsChanged				'PCgn'
	
#define msg_SaveCopy					'SvCp'
#define msg_DoSaveCopy					'DSCp'
	
#define msg_SwitchHeaderSource			'SwHS'
	
#define msg_PerformExtension			'PExt'
	
#define msg_ChangeWorkingDir			'CDir'
#define msg_ChangedWorkingDir			'Ccwd'
	
#define msg_About						'Abou'

#define msg_SmoothUpdate				'UpSm'
	
#define msg_Worksheet					'WrkS'
	
#define msg_ToggleSyntaxColoring		'Sntx'
	
#define msg_AddDialog					'ADlg'
#define msg_RemoveDialog				'RDlg'
	
#define msg_GoToLine					'GoTo'
#define msg_DoGoToLine					'DGTo'
#define msg_FindFunction				'FndF'
#define msg_PreviousFunction			'FprF'
#define msg_NextFunction				'FnxF'
	
#define msg_Info						'Info'
	
#define msg_ConvertEncoding				'CEnc'
#define msg_DoConvertEncoding			'DCEn'
	
#define msg_ToggleShowInvisibles		'TInv'
	
#define msg_FindDifferences				'FDif'
	
#define msg_IdeSetTarget				'IdST'
#define msg_IdeAdd						'IdAd'
#define msg_IdeRemove					'IdRm'
#define msg_IdeMake						'IdMk'
#define msg_IdeForceRebuild				'IdFb'
#define msg_IdeRun						'IdRn'
#define msg_IdeRunInTerminal			'IdRt'
#define msg_IdeRunInDebugger			'IdRd'
#define msg_IdeRunWithArgs				'IdRa'
#define msg_IdeBringToFront				'IdFr'
#define msg_IdeProjectToGroup			'IdGr'
#define msg_IdeToggleErrors				'IdTe'
#define msg_IdeFindInProjectFiles		'IdFn'

#define msg_CancelCommand				'Cncl'

#define msg_IncSearch					'InSr'
#define msg_IncSearchBackward			'InSB'
#define msg_IncSearchBtn				'InBt'

#define msg_SoftWrap					'SfWr'

#define msg_FuncPopup					'PopF'
#define msg_HeaderPopup					'PopH'
#define msg_ReadOnly					'RdOn'

#define msg_MergeTo1					'Mrg1'
#define msg_MergeTo2					'Mrg2'
#define msg_RefreshDiffs				'RfrD'
#define msg_DiffFile1					'Dff1'
#define msg_DiffFile2					'Dff2'

#define msg_SelectError					'SelE'


// And now some other global constants like specifiers:

#define spec_RefSpecifier				B_SPECIFIERS_END + 1

#define msg_RecentSelected				'Rec\0'

#define msg_StdErr						'diag'
#define msg_StdOut						'outp'

#define msg_ShowPath					'sPth'

#define msg_Nothing						0
#define msg_EditAsText					'EdTx'
#define msg_NewGroup					'NewG'
#define msg_EditAsProject				'EdPr'


#endif // PMESSAGES_H
