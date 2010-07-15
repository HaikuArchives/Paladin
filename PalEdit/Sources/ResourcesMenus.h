/*	$Id: ResourcesMenus.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Rainer Riedl
	
	Distributed under the MIT License

	Created: 2005-05-14
*/

#ifndef RESOURCESMENUS_H
#define RESOURCESMENUS_H


/*** RESOURCE-TYPES ******************/
#define rtyp_Mbar				'MBAR'
#define rtyp_Menu				'MENU'
#define rtyp_Popu				'MENU'


/*** RESOURCE-IDs ********************/

// Menubars
#define rid_Mbar_DocumentWin		0
#define rid_Mbar_GroupWin			1
#define rid_Mbar_ProjectWin			2

// Menus: Document Window
#define rid_Menu_DwFile				0
#define rid_Menu_DwEdit				1
#define rid_Menu_DwText				2
#define rid_Menu_DwSearch			3
#define rid_Menu_DwWindow			4
#define rid_Menu_DwProject			5
#define rid_Menu_DwExtensions		6
#define rid_Menu_DwRecent			10
#define rid_Menu_DwChangeCase		11
// Menus: File Panel (Document Window)
#define rid_Menu_FpMimetypes		20
// Menus: Project/Group Window
#define rid_Menu_PwFile				200
#define rid_Menu_PwEdit				201

// Popupmenus: Document Window
#define rid_Popu_CtxText			30
// Popupmenus: Find-Dialog
#define rid_Popu_FndMethod			81
#define rid_Popu_FndDirname			82
#define rid_Popu_FndFilename		83
#define rid_Popu_FndGrepPattern		84
// Popupmenus: Prefs
#define rid_Popu_PrfFont			90
#define rid_Popu_PrfLang			91
#define rid_Popu_PrfState			92
#define rid_Popu_PrfDefaultLang		93
// Popupmenus: Change Encoding
#define rid_Popu_EncConv			111
// Popupmenus: Info
#define rid_Popu_InfMimetypes		101
#define rid_Popu_InfFont			400
#define rid_Popu_InfEncoding		401
#define rid_Popu_InfLinebreaks		402
#define rid_Popu_InfLanguage		403
#define rid_Popu_InfSrcEncoding		404


#endif // RESOURCESMENUS_H
