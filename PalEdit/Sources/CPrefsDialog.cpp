
/*	$Id: CPrefsDialog.cpp,v 1.3 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 10/20/97 20:28:28
*/

#include "pe.h"
#include "CPrefsDialog.h"
#include "PMessages.h"
#include "Utils.h"
#include "PDoc.h"
#include "PApp.h"
#include "HColorUtils.h"
#include "CListBox.h"
#include "CPathsBox.h"
#include "CLanguageInterface.h"
#include "CKeyCapturer.h"
#include "CFindDialog.h"
#include "CStdErrBox.h"
#include "HColorControl.h"
#include "HPreferences.h"
#include "HDefines.h"
#include "HTabSheet.h"
#include "MAlert.h"
#include "CMimeBox.h"
#include "CGrepBox.h"
#include "ResourcesBindings.h"
#include "Prefs.h"
#include "PrefControls.h"

typedef BStringItem CStringItem;

class MyItem : public CStringItem
{
public:
		MyItem(const char *s, int one, int two = 0)
		: CStringItem(s), fOne(one), fTwo(two) {}
		int fOne, fTwo;
};

const unsigned long
	msg_ChangeTarget = 'ChTr',
	msg_DefPageSetup = 'Page',
	msg_StoreSuffix = 'sufx',

	msg_SelectSearchFolder = 'SlsF',
	msg_AddSearchFolder = 'AdsF',
	msg_ChangeSearchFolder = 'CnsF',
	msg_DeleteSearchFolder = 'DlsF',

	msg_SelectedKBCommand = 'KBCm',
	msg_SelectedKBBinding = 'KBBn',
	msg_AddKB = 'AddK',
	msg_DeleteKB = 'DelK',

	msg_SelectToolServer = 'seto',
	msg_LanguageSelected = 'lang';

CPrefsDialog::CPrefsDialog(BRect frame, const char *name, window_type type, int flags,
	BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	fPageSetup = NULL;
	fKBKeys = NULL;
	fCurrentSuffix = NULL;

	BAutolock lock(this);

	font_family ff;
	font_style fs;
	be_plain_font->GetFamilyAndStyle(&ff, &fs);

	strcpy(fFontFamily, gPrefs->GetPrefString(prf_S_FontFamily, ff));
	strcpy(fFontStyle, gPrefs->GetPrefString(prf_S_FontStyle, fs));
	fFontSize = gPrefs->GetPrefDouble(prf_D_FontSize, be_fixed_font->Size());

	BMenuField *mf = dynamic_cast<BMenuField*>(FindView(pnm_Edi_P_Font1Face));
	FailNil(mf);

	fFont = mf->Menu();
	FailNil(fFont);

	for (int i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &ff);
		BMenu *fontItem = new BMenu(ff);
		FailNil(fontItem);
		fFont->AddItem(new BMenuItem(fontItem, new BMessage(msg_FieldChanged)));
		fontItem->SetFont(be_plain_font);

		for (int j = 0; j < count_font_styles(ff); j++)
		{
			get_font_style(ff, j, &fs);

			BMessage *msg = new BMessage(msg_FieldChanged);
			msg->AddString("family", ff);
			msg->AddString("style", fs);
			fontItem->AddItem(new BMenuItem(fs, msg));
		}
	}

	fFont->SetRadioMode(true);

	be_fixed_font->GetFamilyAndStyle(&ff, &fs);

	strcpy(fAltFontFamily, gPrefs->GetPrefString(prf_S_AltFontFamily, ff));
	strcpy(fAltFontStyle, gPrefs->GetPrefString(prf_S_AltFontStyle, fs));
	fAltFontSize = gPrefs->GetPrefDouble(prf_D_AltFontSize, be_fixed_font->Size());

	mf = dynamic_cast<BMenuField*>(FindView(pnm_Edi_P_Font2Face));
	FailNil(mf);

	fAltFont = mf->Menu();
	FailNil(fAltFont);

	for (int i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &ff);
		BMenu *fontItem = new BMenu(ff);
		FailNil(fontItem);
		fAltFont->AddItem(new BMenuItem(fontItem, new BMessage(msg_FieldChanged)));
		fontItem->SetFont(be_plain_font);

		for (int j = 0; j < count_font_styles(ff); j++)
		{
			get_font_style(ff, j, &fs);

			BMessage *msg = new BMessage(msg_FieldChanged);
			msg->AddString("altfamily", ff);
			msg->AddString("altstyle", fs);
			fontItem->AddItem(new BMenuItem(fs, msg));
		}
	}

	fAltFont->SetRadioMode(true);

	fLanguage = static_cast<BMenuField*>(FindView(pnm_Lng_P_Language))->Menu();
	fLanguage->SetRadioMode(true);

	mf = dynamic_cast<BMenuField*>(FindView(pnm_Lng_P_DefaultLanguage));
	FailNil(mf);
	fDefLanguage = mf->Menu();
	FailNil(fDefLanguage);

	CLanguageInterface *intf;
	int cookie = 0;
	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
	{
		fLanguage->AddItem(new BMenuItem(intf->Name(), new BMessage(msg_LanguageSelected)));
		fDefLanguage->AddItem(new BMenuItem(intf->Name(), new BMessage(msg_FieldChanged)));
	}

	fLang = -1; // mark as unitialized, will be initialized when pnm_Lng_P_Language is set
	SetValue(pnm_Lng_P_Language, 1);

	BTextControl *c = static_cast<BTextControl*>(FindView(pnm_Lng_T_Suffixes));
	c->SetModificationMessage(new BMessage(msg_StoreSuffix));

/*	fState = static_cast<BMenuField*>(FindView(pnm_Sta_P_UseStateFormat))->Menu();
	fState->SetRadioMode(true);
	fState->ItemAt(gSavedState)->SetMarked(true);
*/
//	fIncludePaths = dynamic_cast<CPathsBox*>(FindView("incP"));
//	FailNil(fIncludePaths);
//	fIncludePaths->SetPrefName(prf_X_IncludePath);

	fStdErrBox = dynamic_cast<CStdErrBox*>(FindView("StEb"));
	FailNil(fStdErrBox);

//	fMimetypes = dynamic_cast<CMimeBox*>(FindView(pnm_Typ_L_Mimetypes));

	fKBCommands = dynamic_cast<BOutlineListView*>(FindView("kact"));
	fKBKeys = dynamic_cast<BListView*>(FindView("keys"));

	FailNil(fKBCommands);
	FailNil(fKBKeys);

	fCap = dynamic_cast<CKeyCapturer*>(FindView("kebi"));
	FailNil(fCap);

	fGrepBox = dynamic_cast<CGrepBox*>(FindView("grls"));
	FailNil(fGrepBox);

	InitKeybindingPage();

	CancelClicked();
} /* CPrefsDialog::CPrefsDialog */

CPrefsDialog::~CPrefsDialog()
{
//	if (fPageSetup) free(fPageSetup);

	fKeymap.erase(fKeymap.begin(), fKeymap.end());
} /* CPrefsDialog::~CPrefsDialog */

bool CPrefsDialog::QuitRequested()
{
	BButton *ok = static_cast<BButton*>(FindView("ok  "));
	if (ok->IsEnabled())
	{
		MAlert a("The preferences have changed. Save changes before closing?", "Save", "Discard");
		a.SetShortcut(2,B_ESCAPE);
		if (a == 1)
			OkClicked();
		else
			CancelClicked();
	}

	Hide();
	return CDoc::CountDocs() == 0;
} /* CPrefsDialog::QuitRequested */

bool CPrefsDialog::OkClicked()
{
	bool dummy;
	BMenuItem *item = fFont->FindMarked();
	if (item)
	{
		strcpy(fFontFamily, item->Label());
		item = item->Submenu()->FindMarked();
		if (item) strcpy(fFontStyle, item->Label());
	}

	fFontSize = atof(GetText(pnm_Edi_T_Font1Size));

	item = fAltFont->FindMarked();
	if (item)
	{
		strcpy(fAltFontFamily, item->Label());
		item = item->Submenu()->FindMarked();
		if (item) strcpy(fAltFontStyle, item->Label());
	}

	fAltFontSize = atof(GetText(pnm_Edi_T_Font2Size));

	CntrlGetColor(pnm_Col_C_Low,				kColorLow,				prf_C_Low);
	CntrlGetColor(pnm_Col_C_Text,				kColorText,				prf_C_Text);
	CntrlGetColor(pnm_Col_C_Selection,			kColorSelection,		prf_C_Selection);
	CntrlGetColor(pnm_Col_C_Mark,				kColorMark,				prf_C_Mark);
	CntrlGetColor(pnm_Col_C_Keyword1,			kColorKeyword1,			prf_C_Keyword1);
	CntrlGetColor(pnm_Col_C_Keyword2,			kColorKeyword2,			prf_C_Keyword2);
	CntrlGetColor(pnm_Col_C_Comment1,			kColorComment1,			prf_C_Comment1);
	CntrlGetColor(pnm_Col_C_Comment2,			kColorComment2,			prf_C_Comment2);
	CntrlGetColor(pnm_Col_C_String1,			kColorString1,			prf_C_String1);
	CntrlGetColor(pnm_Col_C_String2,			kColorString2,			prf_C_String2);
	CntrlGetColor(pnm_Col_C_Number1,			kColorNumber1,			prf_C_Number1);
	CntrlGetColor(pnm_Col_C_Number2,			kColorNumber2,			prf_C_Number2);
	CntrlGetColor(pnm_Col_C_Operator1,			kColorOperator1,		prf_C_Operator1);
	CntrlGetColor(pnm_Col_C_Operator2,			kColorOperator2,		prf_C_Operator2);
	CntrlGetColor(pnm_Col_C_Separator1,			kColorSeparator1,		prf_C_Separator1);
	CntrlGetColor(pnm_Col_C_Separator2,			kColorSeparator2,		prf_C_Separator2);
	CntrlGetColor(pnm_Col_C_Preprocessor1,		kColorPreprocessor1,	prf_C_Preprocessor1);
	CntrlGetColor(pnm_Col_C_Preprocessor2,		kColorPreprocessor2,	prf_C_Preprocessor2);
	CntrlGetColor(pnm_Col_C_Error1,				kColorError1,			prf_C_Error1);
	CntrlGetColor(pnm_Col_C_Error2,				kColorError2,			prf_C_Error2);
	CntrlGetColor(pnm_Col_C_IdentifierSystem,	kColorIdentifierSystem,	prf_C_IdentifierSystem);
	CntrlGetColor(pnm_Col_C_CharConst,			kColorCharConst,		prf_C_CharConst);
	CntrlGetColor(pnm_Col_C_IdentifierUser,		kColorIdentifierUser,	prf_C_IdentifierUser);
	CntrlGetColor(pnm_Col_C_Tag,				kColorTag,				prf_C_Tag);
	CntrlGetColor(pnm_Col_C_Attribute,			kColorAttribute,		prf_C_Attribute);
	CntrlGetColor(pnm_Col_C_UserSet1,			kColorUserSet1,			prf_C_UserSet1);
	CntrlGetColor(pnm_Col_C_UserSet2,			kColorUserSet2,			prf_C_UserSet2);
	CntrlGetColor(pnm_Col_C_UserSet3,			kColorUserSet3,			prf_C_UserSet3);
	CntrlGetColor(pnm_Col_C_UserSet4,			kColorUserSet4,			prf_C_UserSet4);
	CntrlGetColor(pnm_Inv_C_Invisibles,			kColorInvisibles,		prf_C_Invisibles);

	DefineInvColors(gColor[kColorSelection]);

	gPrefs->SetPrefString(prf_S_FontFamily, fFontFamily);
	gPrefs->SetPrefString(prf_S_FontStyle, fFontStyle);
	gPrefs->SetPrefDouble(prf_D_FontSize, fFontSize);

	gPrefs->SetPrefString(prf_S_AltFontFamily, fAltFontFamily);
	gPrefs->SetPrefString(prf_S_AltFontStyle, fAltFontStyle);
	gPrefs->SetPrefDouble(prf_D_AltFontSize, fAltFontSize);

	gPrefs->SetPrefInt(prf_I_SpacesPerTab, gSpacesPerTab = atoi(GetText(pnm_Edi_T_SpacesPerTab)));

	CntrlGetChkbx(pnm_Edi_X_AutoIndent,				prf_I_AutoIndent,				gAutoIndent);
	CntrlGetChkbx(pnm_Edi_X_SyntaxColoring,			prf_I_SyntaxColoring,			gSyntaxColoring);
	CntrlGetChkbx(pnm_Edi_X_BalanceWhileTyping,		prf_I_BalanceWhileTyping,		gBalance);
	CntrlGetChkbx(pnm_Edi_X_SmartBraces,			prf_I_SmartBraces,				gSmartBrace);
	CntrlGetChkbx(pnm_Edi_X_AlternateHome,			prf_I_AltHome,					dummy);
	CntrlGetChkbx(pnm_Edi_X_ShowTabs,				prf_I_ShowTabs,					dummy);
	CntrlGetChkbx(pnm_Edi_X_SmartWorkspaces,		prf_I_SmartWorkspaces,			dummy);
	CntrlGetChkbx(pnm_Wrp_X_SoftWrapFiles,			prf_I_SoftWrap,					dummy);
	CntrlGetChkbx(pnm_Lng_X_ShowIncludes,			prf_I_ShowIncludes,				gPopupIncludes);
	CntrlGetChkbx(pnm_Lng_X_ShowPrototypes,			prf_I_ShowPrototypes,			gPopupProtos);
	CntrlGetChkbx(pnm_Lng_X_ShowTypes,				prf_I_ShowTypes,				gPopupFuncs);
	CntrlGetChkbx(pnm_Lng_X_SortPopup,				prf_I_SortPopup,				gPopupSorted);
	CntrlGetChkbx(pnm_Wor_X_RedirectStderr,			prf_I_RedirectStdErr,			gRedirectStdErr);
	CntrlGetChkbx(pnm_Wor_X_KeepWorksheetOpen,		prf_I_Worksheet,				gUseWorksheet);
	CntrlGetChkbx(pnm_Fil_X_ZoomOpenPanel,			prf_I_ZoomOpenPanel,			dummy);
	CntrlGetChkbx(pnm_Fil_X_FullPathInTitle,		prf_I_FullPathInTitle, 			dummy);
	CntrlGetChkbx(pnm_Fil_X_MakeBackup,				prf_I_MakeBackup, 				dummy);
	CntrlGetChkbx(pnm_Fil_X_EndWithNewline,			prf_I_EndWithNewline, 			dummy);
	CntrlGetChkbx(pnm_Src_X_InclSearchIgnoreCase,	prf_I_InclSearchIgnoreCase,		dummy);
	CntrlGetChkbx(pnm_Src_X_CenterFoundString,		prf_I_CenterFoundString,		dummy);

	if (IsOn(pnm_Wrp_R_WindowWidth))
		gPrefs->SetPrefInt(prf_I_WrapType, 1);
	else if (IsOn(pnm_Wrp_R_PaperWidth))
		gPrefs->SetPrefInt(prf_I_WrapType, 2);
	else
		gPrefs->SetPrefInt(prf_I_WrapType, 3);

	gPrefs->SetPrefInt(prf_I_WrapCol, atoi(GetText(pnm_Wrp_T_FixedColumn)));

	if (IsOn(pnm_Sup_R_CreateNewDocument))
		gPrefs->SetPrefInt(prf_I_Startup, 1);
	else if (IsOn(pnm_Sup_R_ShowOpenDialog))
		gPrefs->SetPrefInt(prf_I_Startup, 2);
	else
		gPrefs->SetPrefInt(prf_I_Startup, 3);

	int cookie = 0;
	CLanguageInterface *intf;
	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
		intf->SetExtensions(fSuffixes[cookie - 1].c_str());

	item = fDefLanguage->FindMarked();
	gPrefs->SetPrefString(prf_S_DefLang, item ? item->Label() : "None");
	CLanguageInterface::ChooseDefault();


	gPrefs->SetPrefInt(prf_I_RecentSize, gRecentBufferSize = atoi(GetText(pnm_Fil_T_NrOfRecentDocs)));

	fGrepBox->DoOK();
	fStdErrBox->DoOK();

	SetEnabled("ok  ", false);
	SetEnabled("cncl", false);

	PDoc::PostToAll(msg_PrefsChanged, true);
	gFindDialog->PostMessage(msg_PrefsChanged);

	CKeyMapper::Instance().WriteKeymap(fKeymap);

	return false;
} /* CPrefsDialog::OkClicked */

void CPrefsDialog::CntrlSetColor(const char* const viewName, int colorId)
{
	HColorControl *cc;
	if ((cc = dynamic_cast<HColorControl*>(FindView(viewName))))
		cc->SetColor(gColor[colorId]);
} /* CPrefsDialog::CntrlSetColor */

void CPrefsDialog::CntrlGetColor(const char* const viewName, int colorId, const char* const prefName)
{
	HColorControl *cc;
	if ((cc = dynamic_cast<HColorControl*>(FindView(viewName))))
	{
		gColor[colorId] = cc->Color();
		gPrefs->SetPrefColor(prefName, gColor[colorId]);
	}
} /* CPrefsDialog::CntrlGetColor */

void CPrefsDialog::CntrlSetChkbx(const char* const viewName, const char* const prefName, bool presetOn)
{
	SetOn(viewName, gPrefs->GetPrefInt(prefName, presetOn ? 1 : 0));
} /* CPrefsDialog::CntrlSetChkbx */

void CPrefsDialog::CntrlGetChkbx(const char* const viewName, const char* const prefName, bool &setVar)
{
	setVar = IsOn(viewName);
	gPrefs->SetPrefInt(prefName, setVar);
} /* CPrefsDialog::CntrlGetChkbx */

bool CPrefsDialog::CancelClicked()
{
	BMenuItem *item = fFont->FindMarked();
	if (item)
	{
		item = item->Submenu()->FindMarked();
		if (item) item->SetMarked(false);
		fFont->FindMarked()->SetMarked(false);
	}

	item = fFont->FindItem(fFontFamily);
	if (item)
	{
		item->SetMarked(true);
		item = item->Submenu()->FindItem(fFontStyle);
		if (item) item->SetMarked(true);
	}

	item = fAltFont->FindMarked();
	if (item)
	{
		item = item->Submenu()->FindMarked();
		if (item) item->SetMarked(false);
		fAltFont->FindMarked()->SetMarked(false);
	}

	item = fAltFont->FindItem(fAltFontFamily);
	if (item)
	{
		item->SetMarked(true);
		item = item->Submenu()->FindItem(fAltFontStyle);
		if (item) item->SetMarked(true);
	}

	char s[32];
	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_FontSize, 9));
	SetText(pnm_Edi_T_Font1Size, s);

	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_AltFontSize, 10));
	SetText(pnm_Edi_T_Font2Size, s);

	CntrlSetColor(pnm_Col_C_Low,				kColorLow);
	CntrlSetColor(pnm_Col_C_Text,				kColorText);
	CntrlSetColor(pnm_Col_C_Selection,			kColorSelection);
	CntrlSetColor(pnm_Col_C_Mark,				kColorMark);
	CntrlSetColor(pnm_Col_C_Keyword1,			kColorKeyword1);
	CntrlSetColor(pnm_Col_C_Keyword2,			kColorKeyword2);
	CntrlSetColor(pnm_Col_C_Comment1,			kColorComment1);
	CntrlSetColor(pnm_Col_C_Comment2,			kColorComment2);
	CntrlSetColor(pnm_Col_C_String1,			kColorString1);
	CntrlSetColor(pnm_Col_C_String2,			kColorString2);
	CntrlSetColor(pnm_Col_C_Number1,			kColorNumber1);
	CntrlSetColor(pnm_Col_C_Number2,			kColorNumber2);
	CntrlSetColor(pnm_Col_C_Operator1,			kColorOperator1);
	CntrlSetColor(pnm_Col_C_Operator2,			kColorOperator2);
	CntrlSetColor(pnm_Col_C_Separator1,			kColorSeparator1);
	CntrlSetColor(pnm_Col_C_Separator2,			kColorSeparator2);
	CntrlSetColor(pnm_Col_C_Preprocessor1,		kColorPreprocessor1);
	CntrlSetColor(pnm_Col_C_Preprocessor2,		kColorPreprocessor2);
	CntrlSetColor(pnm_Col_C_Error1,				kColorError1);
	CntrlSetColor(pnm_Col_C_Error2,				kColorError2);
	CntrlSetColor(pnm_Col_C_IdentifierSystem,	kColorIdentifierSystem);
	CntrlSetColor(pnm_Col_C_CharConst,			kColorCharConst);
	CntrlSetColor(pnm_Col_C_IdentifierUser,		kColorIdentifierUser);
	CntrlSetColor(pnm_Col_C_Tag,				kColorTag);
	CntrlSetColor(pnm_Col_C_Attribute,			kColorAttribute);
	CntrlSetColor(pnm_Col_C_UserSet1,			kColorUserSet1);
	CntrlSetColor(pnm_Col_C_UserSet2,			kColorUserSet2);
	CntrlSetColor(pnm_Col_C_UserSet3,			kColorUserSet3);
	CntrlSetColor(pnm_Col_C_UserSet4,			kColorUserSet4);
	CntrlSetColor(pnm_Inv_C_Invisibles,			kColorInvisibles);

	CntrlSetChkbx(pnm_Edi_X_AutoIndent,				prf_I_AutoIndent,			true);
	CntrlSetChkbx(pnm_Edi_X_SyntaxColoring,			prf_I_SyntaxColoring,		true);
	CntrlSetChkbx(pnm_Edi_X_BalanceWhileTyping,		prf_I_BalanceWhileTyping,	true);
	CntrlSetChkbx(pnm_Edi_X_BlockCursor,			prf_I_BlockCursor,			false);
	CntrlSetChkbx(pnm_Edi_X_FlashingCursor,			prf_I_FlashCursor,			true);
	CntrlSetChkbx(pnm_Edi_X_SmartBraces,			prf_I_SmartBraces,			true);
	CntrlSetChkbx(pnm_Edi_X_AlternateHome,			prf_I_AltHome,				false);
	CntrlSetChkbx(pnm_Edi_X_ShowTabs,				prf_I_ShowTabs,				true);
	CntrlSetChkbx(pnm_Edi_X_SmartWorkspaces,		prf_I_SmartWorkspaces,		true);
	CntrlSetChkbx(pnm_Dif_X_DiffCaseInsensitive,	prf_I_DiffCaseInsensitive,	false);
	CntrlSetChkbx(pnm_Dif_X_IgnoreWhiteSpace,		prf_I_DiffIgnoreWhiteSpace,	false);
	CntrlSetChkbx(pnm_Lng_X_ShowIncludes,			prf_I_ShowIncludes,			true);
	CntrlSetChkbx(pnm_Lng_X_ShowPrototypes,			prf_I_ShowPrototypes,		false);
	CntrlSetChkbx(pnm_Lng_X_ShowTypes,				prf_I_ShowTypes,			true);
	CntrlSetChkbx(pnm_Lng_X_SortPopup,				prf_I_SortPopup,			false);
	CntrlSetChkbx(pnm_Wor_X_RedirectStderr,			prf_I_RedirectStdErr,		true);
	CntrlSetChkbx(pnm_Wor_X_KeepWorksheetOpen,		prf_I_Worksheet,			true);
	CntrlSetChkbx(pnm_Sta_X_RestorePosition,		prf_I_RestorePosition,		true);
	CntrlSetChkbx(pnm_Sta_X_RestoreFont,			prf_I_RestoreFont,			true);
	CntrlSetChkbx(pnm_Sta_X_RestoreSelection,		prf_I_RestoreSelection,		true);
	CntrlSetChkbx(pnm_Sta_X_RestoreScrollbar,		prf_I_RestoreScrollbar,		true);
	CntrlSetChkbx(pnm_Sta_X_RestoreCwd,				prf_I_RestoreCwd,			true);
	CntrlSetChkbx(pnm_Fil_X_SkipTmpFiles,			prf_I_SkipTmpFiles,			true);
	CntrlSetChkbx(pnm_Fil_X_ZoomOpenPanel,			prf_I_ZoomOpenPanel,		false);
	CntrlSetChkbx(pnm_Fil_X_FullPathInTitle,		prf_I_FullPathInTitle,		true);
	CntrlSetChkbx(pnm_Inc_X_SearchParent,			prf_I_SearchParent,			true);
	CntrlSetChkbx(pnm_Inc_X_BeIncludes,				prf_I_BeIncludes,			true);
	CntrlSetChkbx(pnm_Fil_X_MakeBackup,				prf_I_MakeBackup,			false);
	CntrlSetChkbx(pnm_Fil_X_EndWithNewline,			prf_I_EndWithNewline,		true);
	CntrlSetChkbx(pnm_Fil_X_VerifyOpenFiles,		prf_I_VerifyOpenFiles,		true);
	CntrlSetChkbx(pnm_Src_X_InclSearchIgnoreCase,	prf_I_InclSearchIgnoreCase,	true);
	CntrlSetChkbx(pnm_Src_X_CenterFoundString,		prf_I_CenterFoundString,	false);
	CntrlSetChkbx(pnm_Grp_X_SortGroup,				prf_I_SortGroup,			true);
	CntrlSetChkbx(pnm_Grp_X_RelativeGroupPaths,		prf_I_RelativeGroupPaths,	false);
	CntrlSetChkbx(pnm_Prj_X_SortProjectFiles,		prf_I_SortProjectFiles,		true);
	CntrlSetChkbx(pnm_Prj_X_AutodetectProjects,		prf_I_AutodetectProjects,	true);

	sprintf(s, "%d", gSpacesPerTab);
	SetText(pnm_Edi_T_SpacesPerTab, s);

	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_ContextLines, 3));
	SetText(pnm_Edi_T_ContextLines, s);

	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_ContextChars, 5));
	SetText(pnm_Edi_T_ContextChars, s);

	bool sw;
	SetOn(pnm_Wrp_X_SoftWrapFiles,	sw = gPrefs->GetPrefInt(prf_I_SoftWrap, false));

	int wrap = gPrefs->GetPrefInt(prf_I_WrapType, 3);
	switch (wrap)
	{
		case 1:		SetOn(pnm_Wrp_R_WindowWidth, true); break;
		case 2:		SetOn(pnm_Wrp_R_PaperWidth, true); break;
		default:	SetOn(pnm_Wrp_R_FixedColumn, true); break;
	}

	sprintf(s, "%d", gPrefs->GetPrefInt(prf_I_WrapCol, 80));
	SetText(pnm_Wrp_T_FixedColumn, s);

	SetEnabled(pnm_Wrp_T_FixedColumn, /*sw && */wrap == 3);

	SetText(pnm_Inv_T_TabChar, gTabChar);
	SetText(pnm_Inv_T_SpaceChar, gSpaceChar);
	SetText(pnm_Inv_T_ReturnChar, gReturnChar);
	SetText(pnm_Inv_T_ControlChar, gControlChar);

	switch (gPrefs->GetPrefInt(prf_I_Startup))
	{
		default: SetOn(pnm_Sup_R_CreateNewDocument, true); break;
		case 2: SetOn(pnm_Sup_R_ShowOpenDialog, true); break;
		case 3: SetOn(pnm_Sup_R_DoNothing, true); break;
	}


	fSuffixes.erase(fSuffixes.begin(), fSuffixes.end());

	CLanguageInterface *intf;
	int cookie = 0, i = 0;
	const char *defLang = gPrefs->GetPrefString(prf_S_DefLang, "None");

	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
	{
		fSuffixes.push_back(intf->Extensions());
		if (strcmp(intf->Name(), defLang) == 0)
			SetValue(pnm_Lng_P_DefaultLanguage, i + 3);
		i++;
	}

	SetText(pnm_Lng_T_Suffixes, fSuffixes.size() ? fSuffixes[GetValue(pnm_Lng_P_Language) - 1].c_str() : "");


	sprintf(s, "%d", gRecentBufferSize);
	SetText(pnm_Fil_T_NrOfRecentDocs, s);

//	fIncludePaths->DoCancel();
//	fMimetypes->DoCancel();
	fGrepBox->DoCancel();
	fStdErrBox->DoCancel();

	SetEnabled("ok  ", false);
	SetEnabled("cncl", false);

	CKeyMapper::Instance().ReadKeymap(fKeymap);
	fKBCommands->DeselectAll();

	return false;
} /* CPrefsDialog::CancelClicked */

void CPrefsDialog::UpdateFields()
{
	const char *ff, *fs;
	if (CurrentMessage()->FindString("family", &ff) == B_NO_ERROR &&
		CurrentMessage()->FindString("style", &fs) == B_NO_ERROR)
	{
		BMenuItem *item;

		item = fFont->FindMarked();
		if (item)
		{
			item = item->Submenu()->FindMarked();
			if (item) item->SetMarked(false);
			fFont->FindMarked()->SetMarked(false);
		}

		item = fFont->FindItem(ff);
		if (item)
		{
			item->SetMarked(true);
			item = item->Submenu()->FindItem(fs);
			if (item) item->SetMarked(true);
		}
	}

	if (CurrentMessage()->FindString("altfamily", &ff) == B_NO_ERROR &&
		CurrentMessage()->FindString("altstyle", &fs) == B_NO_ERROR)
	{
		BMenuItem *item;

		item = fAltFont->FindMarked();
		if (item)
		{
			item = item->Submenu()->FindMarked();
			if (item) item->SetMarked(false);
			fAltFont->FindMarked()->SetMarked(false);
		}

		item = fAltFont->FindItem(ff);
		if (item)
		{
			item->SetMarked(true);
			item = item->Submenu()->FindItem(fs);
			if (item) item->SetMarked(true);
		}
	}

/*	bool statePe = (fState->IndexOf(fState->FindMarked()) == 0);
	SetEnabled("restore scrollbar", statePe);
	SetEnabled("restore cwd", statePe);
*/

	SetEnabled(pnm_Wrp_T_FixedColumn, IsOn(pnm_Wrp_R_FixedColumn));

	UpdateKBPage();

} /* CPrefsDialog::UpdateFields */

void CPrefsDialog::GetDefPageSetup()
{
//	BPrintJob prJob("a page setup job");
//
//	if (fPageSetup)
//	{
//		BMessage *s = new BMessage;
//		if (s && s->Unflatten(fPageSetup) == B_NO_ERROR)
//			prJob.SetSettings(s);
//	}
//
//	int result = prJob.ConfigPage();
//
//	if (result == B_NO_ERROR)
//	{
//		BMessage s(prJob.Settings());
//
//		fPageSetupSize = s.FlattenedSize();
//		if (fPageSetup) free(fPageSetup);
//		fPageSetup = (char *)malloc(fPageSetupSize);
//		FailNil(fPageSetup);
//		result = s.Flatten(fPageSetup, fPageSetupSize);
//		FailOSErrMsg(result, "error flattening (%d)");
//
//		UpdateFields();
//	}
} /* CPrefsDialog::GetDefPageSetup */

void CPrefsDialog::MessageReceived(BMessage *msg)
{
	long what = msg->what;

	switch (what)
	{
		case msg_SelectedKBCommand:
			UpdateKBPage();
			break;

		case msg_SelectedKBBinding:
			UpdateKBCapturer();
			break;

		case msg_AddKB:
			AddKeybinding();
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			break;

		case msg_DeleteKB:
			DeleteKeybinding();
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			break;

		case 'addP':
			fGrepBox->DoCancel();
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			break;

		case msg_StoreSuffix:
			if (fSuffixes.size()) {
				fSuffixes[GetValue(pnm_Lng_P_Language) - 1] = GetText(pnm_Lng_T_Suffixes);
			}
			if (fCurrentSuffix != 0) {
				if (strcmp(fCurrentSuffix,fSuffixes[fLang].c_str()) != 0) {
					SetEnabled("ok  ", true);
					SetEnabled("cncl", true);
				}
			}
			break;

		case msg_LanguageSelected:
			fLang = GetValue(pnm_Lng_P_Language) - 1;
			{
				int cookie = 0, i = 0;
				CLanguageInterface *intf = NULL;
				while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL) {
					if (i == fLang) {
						fCurrentSuffix = intf->Extensions();
						break;
					}
					i++;
				}
			}
			if (fSuffixes.size()) {
				SetText(pnm_Lng_T_Suffixes, fSuffixes[fLang].c_str());
			}
			break;

		case msg_FieldChanged:
			SetEnabled("ok  ", true);
			SetEnabled("cncl", true);
			// fall through
		default:
			HDialog::MessageReceived(msg);
	}
} /* CPrefsDialog::MessageReceived */

void CPrefsDialog::CreateField(int kind, BPositionIO& data, BView*& inside)
{
	dRect r;
	char name[256];
	BView *v;

	switch (kind)
	{
		case 'pbox':
			data >> r >> name;
			inside->AddChild(v = new CPathsBox(r.ToBe(), name));
			break;
		case 'gbox':
			data >> r >> name;
			inside->AddChild(v = new CGrepBox(r.ToBe(), name));
			break;
		case 'keyc':
			data >> r >> name;
			inside->AddChild(v = new CKeyCapturer(r.ToBe(), name));
			break;
//		case 'slbx':
//			data >> r >> name;
//			inside->AddChild(v = new CMimeBox(r.ToBe(), name));
//			break;
		case 'ebox':
			data >> r >> name;
			inside->AddChild(v = new CStdErrBox(r.ToBe(), name));
			break;
	}
} /* CPrefsDialog::CreateField */


void CPrefsDialog::RegisterFields()
{
	RegisterFieldCreator('pbox', CreateField);
	RegisterFieldCreator('ebox', CreateField);
	RegisterFieldCreator('slbx', CreateField);
	RegisterFieldCreator('keyc', CreateField);
	RegisterFieldCreator('gbox', CreateField);
} /* CPrefsDialog::RegisterFields */

void CPrefsDialog::InitKeybindingPage()
{
	int resID = rid_Cmnd_Editing;

	while (true)
	{
		long cnt, cmd;
		char s[256];
		const char *name;

		size_t size;
		const void *p = HResources::GetResource(rtyp_Cmnd, resID, size, &name);
		if (p == NULL)
			break;

		BMemoryIO buf(p, size);
		buf >> cnt;

		MyItem *mom = new MyItem(name, 0);
		fKBCommands->AddItem(mom);

		if (resID == rid_Cmnd_Extensions)
		{
			BPopUpMenu menu("hoi");
			PDoc::BuildExtensionsMenu(&menu);
			cmd = 'ex\0\0';

			for (int i = 0; i < menu.CountItems(); i++) {
				uint16 extHash = HashString16(menu.ItemAt(i)->Label());
				fKBCommands->AddUnder(new MyItem(menu.ItemAt(i)->Label(), cmd|extHash), mom);
			}
		}
		else
		{
			while (cnt--)
			{
				buf >> cmd;

				int i = 0;
				do	buf >> s[i];
				while (s[i++]);

				fKBCommands->AddUnder(new MyItem(s, cmd), mom);
			}
		}
		fKBCommands->Collapse(mom);
		resID++;
	}

	fKBCommands->SetSelectionMessage(new BMessage(msg_SelectedKBCommand));
	fKBKeys->SetSelectionMessage(new BMessage(msg_SelectedKBBinding));
} /* CPrefsDialog::InitKeybindingsPage */

void CPrefsDialog::UpdateKBPage()
{
	if (fKBKeys == NULL)
		return;

	MyItem *mi;

	while (fKBKeys->CountItems() > 0)
		delete fKBKeys->RemoveItem((int32)0);

	fCap->SetShortcut(KeyShortcut());

	mi = static_cast<MyItem *>(fKBCommands->ItemAt(fKBCommands->CurrentSelection()));

	if (mi && mi->fOne)
	{
		keymap::iterator ki;

		for (ki = fKeymap.begin(); ki != fKeymap.end(); ki++)
		{
			if ((*ki).second == mi->fOne)
			{
				char s[256];

				CKeyCapturer::DescribeKeys((*ki).first, s);

				fKBKeys->AddItem(new MyItem(s, (*ki).first.prefix, (*ki).first.combo));
			}
		}
	}
} /* CPrefsDialog::UpdateKBPage */

void CPrefsDialog::UpdateKBCapturer()
{
	MyItem *mi = static_cast<MyItem*>(fKBKeys->ItemAt(fKBKeys->CurrentSelection()));

	KeyShortcut ks;

	if (mi)
	{
		ks.prefix = mi->fOne;
		ks.combo = mi->fTwo;
	}

	fCap->SetShortcut(ks);
} /* CPrefsDialog::UpdateKBCapturer */

void CPrefsDialog::AddKeybinding()
{
	MyItem *mi;

	mi = static_cast<MyItem *>(fKBCommands->ItemAt(fKBCommands->CurrentSelection()));

	if (mi && mi->fOne)
	{
		KeyShortcut ks = fCap->Shortcut();

		ks.prefix &= BINDINGMASK;
		ks.combo &= BINDINGMASK;

		keymap::iterator ki = fKeymap.find(ks);
		if (ki != fKeymap.end() && (*ki).second != mi->fOne)
		{
			MInfoAlert a("Do you want to replace the current binding for this shortcut?",
				"Replace", "Cancel");
			if (a.Go() != 1)
				return;
		}

		fKeymap[ks] = mi->fOne;

		UpdateFields();
	}
} /* CPrefsDialog::AddKeybinding */

void CPrefsDialog::DeleteKeybinding()
{
	MyItem *mi;

	mi = static_cast<MyItem *>(fKBKeys->ItemAt(fKBKeys->CurrentSelection()));

	if (mi)
	{
		KeyShortcut ks;

		ks.prefix = mi->fOne & BINDINGMASK;
		ks.combo = mi->fTwo & BINDINGMASK;

		if (fKeymap.find(ks) != fKeymap.end())
		{
			fKeymap.erase(fKeymap.find(ks));
			UpdateFields();
		}
	}
} /* CPrefsDialog::DeleteKeybinding */

void CPrefsDialog::Show()
{
	HDialog::Show();
	if (LockLooper())
	{
		HTabSheet* tabBook = (HTabSheet*)FindView("tabB");
		BRect okFrame = FindView("ok  ")->Frame();
		if (tabBook) {
			BPoint bottomRight = tabBook->AdjustBottomRightOfAllPanes();
			if (bottomRight.x < okFrame.right)
				bottomRight.x = okFrame.right;
			if (bottomRight.y < okFrame.bottom)
				bottomRight.y = okFrame.bottom;
			ResizeTo(bottomRight.x+5, bottomRight.y+5);
		}
		UnlockLooper();
	}
} /* CPrefsDialog::Show */
