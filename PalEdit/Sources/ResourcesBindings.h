/*	$Id: ResourcesBindings.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Rainer Riedl
	
	Distributed under the MIT License

	Created: 2005-05-14
*/

#ifndef RESOURCESBINDINGS_H
#define RESOURCESBINDINGS_H


/*** RESOURCE-TYPES ******************/
#define rtyp_Bind				'Bind'
#define rtyp_Cmnd				'Cmnd'


/*** RESOURCE-IDs ********************/

// Commands (Don't change numbers until correspondent code is changed
#define rid_Cmnd_Editing			0
#define rid_Cmnd_FileMenu			1
#define rid_Cmnd_EditMenu			2
#define rid_Cmnd_TextMenu			3
#define rid_Cmnd_SearchMenu			4
#define rid_Cmnd_WindowMenu			5
#define rid_Cmnd_ProjectMenu		6
#define rid_Cmnd_Miscellaneous		8
#define rid_Cmnd_Extensions			9

// Bindings (Don't change numbers until correspondent code is changed
#define rid_Bind_Editing			0
#define rid_Bind_FileMenu			1
#define rid_Bind_EditMenu			2
#define rid_Bind_TextMenu			3
#define rid_Bind_SearchMenu			4
#define rid_Bind_WindowMenu			5
#define rid_Bind_ProjectMenu		6
#define rid_Bind_Miscellaneous		8
#define rid_Bind_Emacs				9


#endif // RESOURCESBINDINGS_H
