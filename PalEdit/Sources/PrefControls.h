/*	$Id: PrefControls.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

	Copyright 2005 Rainer Riedl

	Distributed under the MIT License

	Created: 2006-11-24
*/

#ifndef PREFS_CONTROLS_H
#define PREFS_CONTROLS_H


// Color Panel
#define pnm_Col_C_Low					"lowc"
#define pnm_Col_C_Text					"txtc"
#define pnm_Col_C_Selection				"selc"
#define pnm_Col_C_Mark					"mrkc"
#define pnm_Col_C_Keyword1				"keyc"
#define pnm_Col_C_Keyword2				"ke2c"
#define pnm_Col_C_Comment1				"cmtc"
#define pnm_Col_C_Comment2				"cm2c"
#define pnm_Col_C_String1				"strc"
#define pnm_Col_C_String2				"tgsc"
#define pnm_Col_C_Number1				"numc"
#define pnm_Col_C_Number2				"nu2c"
#define pnm_Col_C_Operator1				"opec"
#define pnm_Col_C_Operator2				"op2c"
#define pnm_Col_C_Separator1			"sepc"
#define pnm_Col_C_Separator2			"se2c"
#define pnm_Col_C_Preprocessor1			"proc"
#define pnm_Col_C_Preprocessor2			"pr2c"
#define pnm_Col_C_Error1				"errc"
#define pnm_Col_C_Error2				"er2c"
#define pnm_Col_C_IdentifierSystem		"sidc"
#define pnm_Col_C_CharConst				"chcc"
#define pnm_Col_C_IdentifierUser		"uidc"
#define pnm_Col_C_Tag					"tagc"
#define pnm_Col_C_Attribute				"attr"
#define pnm_Col_C_UserSet1				"us1c"
#define pnm_Col_C_UserSet2				"us2c"
#define pnm_Col_C_UserSet3				"us3c"
#define pnm_Col_C_UserSet4				"us4c"
// Differences
#define pnm_Dif_X_DiffCaseInsensitive	"dica"
#define pnm_Dif_X_IgnoreWhiteSpace		"diwh"
// Editor
#define pnm_Edi_X_ShowTabs				"stab"
#define pnm_Edi_X_SyntaxColoring		"syco"
#define pnm_Edi_X_AutoIndent			"auin"
#define pnm_Edi_X_BalanceWhileTyping	"baty"
#define pnm_Edi_X_SmartBraces			"smbr"
#define pnm_Edi_X_BlockCursor			"blcu"
#define pnm_Edi_X_FlashingCursor		"flcu"
#define pnm_Edi_X_SmartWorkspaces		"smwo"
#define pnm_Edi_X_AlternateHome			"alho"
#define pnm_Edi_T_SpacesPerTab			"tabs"	/* TODO: Check other occurences */
#define pnm_Edi_T_ContextLines			"ctxl"
#define pnm_Edi_T_ContextChars			"ctxc"
#define pnm_Edi_T_Font1Size				"fons"
#define pnm_Edi_T_Font2Size				"alfs"
#define pnm_Edi_P_Font1Face				"font"	/* TODO: Check other occurences */
#define pnm_Edi_P_Font2Face				"alfo"
// File Types
#define pnm_Typ_L_Mimetypes				"mimetypes"
// Files
#define pnm_Fil_X_MakeBackup			"maba"
#define pnm_Fil_X_EndWithNewline		"fonl"
#define pnm_Fil_X_VerifyOpenFiles		"vofm"
#define pnm_Fil_T_NrOfRecentDocs		"resi"
#define pnm_Fil_X_SkipTmpFiles			"sktm"
#define pnm_Fil_X_ZoomOpenPanel			"zoop"
#define pnm_Fil_X_FullPathInTitle		"fupa"
// Grep Patterns
//		GrepListBox		"grls"
// Groups
#define pnm_Grp_X_SortGroup				"sogr"
#define pnm_Grp_X_RelativeGroupPaths	"rlgp"
// Include Paths
#define pnm_Inc_X_SearchParent			"sepa"
#define pnm_Inc_X_BeIncludes			"incl"
//				PathBox			"incP" },
// Invisibles
#define pnm_Inv_T_ControlChar			"scca"
#define pnm_Inv_C_Invisibles			"invc"
#define pnm_Inv_T_TabChar				"stca"
#define pnm_Inv_T_SpaceChar				"ssca"
#define pnm_Inv_T_ReturnChar			"srca"
// Keybindings
//				OutlineList		"kact"
//				List			"keys"
//				KeyCapture		"kebi"
//#define pnm_Key_B_Add		"adke",		'AddK'
//#define pnm_Key_B_Delete	"deke",		'DelK'
//			TabSheetEnd			{
// Languages
#define pnm_Lng_X_ShowIncludes			"shin"
#define pnm_Lng_X_ShowPrototypes		"shpr"
#define pnm_Lng_X_ShowTypes				"shty"
#define pnm_Lng_X_SortPopup				"sopo"
#define pnm_Lng_P_Language				"lang"
#define pnm_Lng_T_Suffixes				"sufx"
#define pnm_Lng_P_DefaultLanguage		"dela"
// Projects
#define pnm_Prj_X_SortProjectFiles		"prso"
#define pnm_Prj_X_AutodetectProjects	"prad"
// Searching
//				PathBox			"srcP"
#define pnm_Src_X_InclSearchIgnoreCase	"isic"
#define pnm_Src_X_CenterFoundString		"cesf"
// Startup
#define pnm_Sup_R_CreateNewDocument		"swne"
#define pnm_Sup_R_ShowOpenDialog		"swop"
#define pnm_Sup_R_DoNothing				"swno"
// State
#define pnm_Sta_X_RestorePosition		"repo"
#define pnm_Sta_X_RestoreFont			"refo"
#define pnm_Sta_X_RestoreSelection		"rese"
#define pnm_Sta_X_RestoreScrollbar		"resc"
#define pnm_Sta_X_RestoreCwd			"recw"
#define pnm_Sta_P_UseStateFormat		"sast"
// Worksheet
#define pnm_Wor_X_KeepWorksheetOpen		"wosh"
#define pnm_Wor_X_RedirectStderr		"rdse"
//				StdErrBox		"StEb"
// Wrapping
#define pnm_Wrp_X_SoftWrapFiles			"sowr"
#define pnm_Wrp_R_WindowWidth			"wrwi"
#define pnm_Wrp_R_PaperWidth			"wrpa"
#define pnm_Wrp_R_FixedColumn			"wrfi"
#define pnm_Wrp_T_FixedColumn			"wrft"
// Buttons
//#define pnm_But_B_	"ok  ",		"Apply",			'ok  '
//#define pnm_But_B_	"cncl",		"Revert",			'cncl'


#endif // PREFS_CONTROLS_H
