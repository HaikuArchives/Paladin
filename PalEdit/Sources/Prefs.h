/*	$Id: Prefs.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

	Copyright 2005 Rainer Riedl

	Distributed under the MIT License

	Created: 2005-06-04
*/

#ifndef PREFS_H
#define PREFS_H


const char* const prf_X_GrepPatFind				= "regxpatfind";
const char* const prf_X_GrepPatName				= "regxpatname";
const char* const prf_X_GrepPatRepl				= "regxpatrepl";
const char* const prf_X_IncludePath				= "includepath";
const char* const prf_X_Mimetype				= "mimetype";
const char* const prf_X_SearchPath				= "searchpath";
const char* const prf_X_StdErrPattern			= "stderrpattern";

const char* const prf_C_Attribute				= "attribute color";
const char* const prf_C_CharConst				= "char constant color";
const char* const prf_C_Comment1				= "comment color";
const char* const prf_C_Comment2				= "alt comment color";
const char* const prf_C_Error1					= "error color";
const char* const prf_C_Error2					= "alt error color";
const char* const prf_C_IdentifierSystem		= "system identifier color";
const char* const prf_C_IdentifierUser			= "user identifier color";
const char* const prf_C_Invisibles				= "invisibles color";
const char* const prf_C_Keyword1				= "keyword color";
const char* const prf_C_Keyword2				= "alt keyword color";
const char* const prf_C_Low						= "low color";
const char* const prf_C_Mark					= "mark color";
const char* const prf_C_Number1					= "number color";
const char* const prf_C_Number2					= "alt number color";
const char* const prf_C_Operator1				= "operator color";
const char* const prf_C_Operator2				= "alt operator color";
const char* const prf_C_Preprocessor1			= "preprocessor color";
const char* const prf_C_Preprocessor2			= "altprocessor color";
const char* const prf_C_Selection				= "selection color";
const char* const prf_C_Separator1				= "separator color";
const char* const prf_C_Separator2				= "alt separator color";
const char* const prf_C_String1					= "string color";
const char* const prf_C_String2					= "tagstring color";
const char* const prf_C_Tag						= "tag color";
const char* const prf_C_Text					= "text color";
const char* const prf_C_UserSet1				= "user1";
const char* const prf_C_UserSet2				= "user2";
const char* const prf_C_UserSet3				= "user3";
const char* const prf_C_UserSet4				= "user4";

// Todo: There are integer prefs with the same name, error?!
const char* const prf_D_AltFontSize				= "alt font size";
const char* const prf_D_FontSize				= "font size";

const char* const prf_I_AltFontSize				= "alt font size";
const char* const prf_I_AltHome					= "althome";
const char* const prf_I_AutoIndent				= "auto indent";
const char* const prf_I_AutodetectProjects		= "autodetect projects";
const char* const prf_I_BalanceWhileTyping		= "balance";
const char* const prf_I_BeIncludes				= "beincludes";
const char* const prf_I_BlockCursor				= "block cursor";
const char* const prf_I_CenterFoundString		= "centerfound";
const char* const prf_I_ContextChars			= "contextchars";
const char* const prf_I_ContextLines			= "contextlines";
const char* const prf_I_DiffCaseInsensitive		= "diffcase";
const char* const prf_I_DiffIgnoreWhiteSpace	= "diffwhite";
const char* const prf_I_EndWithNewline			= "nl at eof";
const char* const prf_I_FlashCursor				= "flash cursor";
const char* const prf_I_FontSize				= "font size";
const char* const prf_I_FullPathInTitle			= "fullpath";
const char* const prf_I_InclSearchIgnoreCase	= "isearch_igncase";
const char* const prf_I_MakeBackup				= "backup";
const char* const prf_I_RecentSize				= "recent size";
const char* const prf_I_RedirectStdErr			= "redirect stderr";
const char* const prf_I_RelativeGroupPaths		= "relative group paths";
const char* const prf_I_RestoreCwd				= "restore cwd";
const char* const prf_I_RestoreFont				= "restore font";
const char* const prf_I_RestorePosition			= "restore position";
const char* const prf_I_RestoreScrollbar		= "restore scrollbar";
const char* const prf_I_RestoreSelection		= "restore selection";
const char* const prf_I_SavedState				= "saved state";
const char* const prf_I_ScrollwheelLines		= "scrollwheel lines";
						// Currently not in GUI
const char* const prf_I_SearchBackwards			= "Search Backwards";
const char* const prf_I_SearchBatch				= "Search Batch";
const char* const prf_I_SearchEntireWord		= "Search Entire Word";
const char* const prf_I_SearchIgnoreCase		= "Search Ignore Case";
const char* const prf_I_SearchMultikind			= "Search Multikind";
const char* const prf_I_SearchParent			= "parent";
const char* const prf_I_SearchRecursive			= "Search Recursive";
const char* const prf_I_SearchTextFilesOnly		= "Search Text Files Only";
const char* const prf_I_SearchWhichDir			= "Search Whichdir";
const char* const prf_I_SearchWithGrep			= "Search with Grep";
const char* const prf_I_SearchWrap				= "Search Wrap";
const char* const prf_I_ShowIncludes			= "includes";
const char* const prf_I_ShowInvisibles			= "show invisibles";
const char* const prf_I_ShowPrototypes			= "protos";
const char* const prf_I_ShowTabs				= "show tabs";
const char* const prf_I_SkipHtmlExt				= "skiphtmlext";
const char* const prf_I_SkipTmpFiles			= "skiptmp";
const char* const prf_I_SmartBraces				= "smart braces";
const char* const prf_I_SmartWorkspaces			= "window to workspace";
const char* const prf_I_SoftWrap				= "softwrap";
const char* const prf_I_SortGroup				= "sortgroup";
const char* const prf_I_SortPopup				= "sortpopup";
const char* const prf_I_SortProjectFiles		= "sortproject";
const char* const prf_I_SpacesPerTab			= "spaces per tab";
const char* const prf_I_Startup					= "startup";
const char* const prf_I_StdErrInitEd			= "stderrinited";
const char* const prf_I_SyntaxColoring			= "syntax coloring";
const char* const prf_I_ShowTypes				= "types";
const char* const prf_I_VerifyOpenFiles			= "verify";
const char* const prf_I_Worksheet				= "worksheet";
const char* const prf_I_WrapCol					= "wrapcol";
const char* const prf_I_WrapType				= "wraptype";
const char* const prf_I_ZoomOpenPanel			= "zoomopen";

const char* const prf_R_DefaultDocumentRect		= "default document rect";

const char* const prf_S_AltFontFamily			= "alt font family";
const char* const prf_S_AltFontStyle			= "alt font style";
const char* const prf_S_ControlChar				= "control char";
const char* const prf_S_DefLang					= "def lang";
const char* const prf_S_FontFamily				= "font family";
const char* const prf_S_FontStyle				= "font style";
const char* const prf_S_LastFindAndOpen			= "last find&open";
const char* const prf_S_ReturnChar				= "return char";
const char* const prf_S_SpaceChar				= "space char";
const char* const prf_S_TabChar					= "tab char";


#endif // PREFS_H
