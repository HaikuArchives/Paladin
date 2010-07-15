/*	$Id: KeyBindings.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 02-06-02
*/

#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

//#define kmsg_OpenHeaderSource							'OpnH'
//#define kmsg_AbortShellCommand						'amsg'
#define kmsg_MoveCharacterLeft							'Mchl'
#define kmsg_MoveCharacterRight							'Mchr'
#define kmsg_MoveWordLeft								'Mwdl'
#define kmsg_MoveWordRight								'Mwdr'
#define kmsg_MoveSubwordLeft							'Mswl'
#define kmsg_MoveSubwordRight							'Mswr'
#define kmsg_MoveToBeginningOfLine						'Mbol'
#define kmsg_MoveToEndOfLine							'Meol'
#define kmsg_MoveToPreviousLine							'Mprl'
#define kmsg_MoveToNextLine								'Mnxl'
#define kmsg_MoveToPreviousPage							'Mprp'
#define kmsg_MoveToNextPage								'Mnxp'
#define kmsg_MoveToTopOfPage							'Mbop'
#define kmsg_MoveToBottomOfPage							'Meop'
#define kmsg_MoveToBeginningOfFile						'Mbof'
#define kmsg_MoveToEndOfFile							'Meof'
#define kmsg_DeleteCharacterLeft						'Dchl'
#define kmsg_DeleteCharacterRight						'Dchr'
#define kmsg_DeleteToBeginningOfLine					'Dbol'
#define kmsg_DeleteToEndOfLine							'Deol'
#define kmsg_DeleteToEndOfFile							'Deof'
#define kmsg_ExtendSelectionWithCharacterLeft			'Schl'
#define kmsg_ExtendSelectionWithCharacterRight			'Schr'
#define kmsg_ExtendSelectionWithPreviousWord			'Swdl'
#define kmsg_ExtendSelectionWithNextWord				'Swdr'
#define kmsg_ExtendSelectionWithPreviousSubword			'Sswl'
#define kmsg_ExtendSelectionWithNextSubword				'Sswr'
#define kmsg_ExtendSelectionToCurrentLine				'Slin'
#define kmsg_ExtendSelectionToPreviousLine				'Sprl'
#define kmsg_ExtendSelectionToNextLine					'Snxl'
#define kmsg_ExtendSelectionToBeginningOfLine			'Sbol'
#define kmsg_ExtendSelectionToEndOfLine					'Seol'
#define kmsg_ExtendSelectionToPreviousPage				'Sprp'
#define kmsg_ExtendSelectionToNextPage					'Snxp'
#define kmsg_ExtendSelectionToBeginningOfPage			'Sbop'
#define kmsg_ExtendSelectionToEndOfPage					'Seop'
#define kmsg_ExtendSelectionToBeginningOfFile			'Sbof'
#define kmsg_ExtendSelectionToEndOfFile					'Seof'
#define kmsg_ScrollOneLineUp							'Rlnu'
#define kmsg_ScrollOneLineDown							'Rlnd'
#define kmsg_ScrollPageUp								'Rpgu'
#define kmsg_ScrollPageDown								'Rpgd'
#define kmsg_ScrollToStartOfFile						'Rbof'
#define kmsg_ScrollToEndOfFile							'Reof'

// for emacs users:
#define kmsg_OpenLine									'OpnL'

#define kmsg_Mark										'Mark'
#define kmsg_MarkAll									'MAll'
#define kmsg_MarkWord									'MWrd'
#define kmsg_ExchangeMarkAndPoint						'ExMP'

#define kmsg_CutRegion									'CutR'
#define kmsg_CopyRegion									'CopR'
#define kmsg_ClearRegion								'ClrR'

#define kmsg_CutToEndOfLine								'CutL'
#define kmsg_CutWord									'CutW'
#define kmsg_CutWordBackward							'CtWB'
//#define kmsg_CutSentence								'CutS'

#define kmsg_AppendNextCut								'ApCt'

#define kmsg_Recenter									'Rcnt'

#define kmsg_NrArgument									'Arg '

#define kmsg_SwitchActivePart							'SwAP'
#define kmsg_SplitWindow								'Splt'
#define kmsg_UnsplitWindow								'DelP'


#endif // KEYBINDINGS_H
