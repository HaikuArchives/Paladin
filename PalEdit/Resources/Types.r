/*	$Id: Types.r,v 1.1 2008/12/20 23:35:49 darkwyrm Exp $
	
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

#include "ResourcesMenus.h"
#include "ResourcesBindings.h"


/* Menu */
type rtyp_Menu {
	cstring;										// Name
	array {
		switch {
			case Item:
				key byte = 1;
				cstring;				 			// Label
				longint;							// Command
				integer
					none	= 0,
					shift	= 1,
					control	= 4;					// Modifier keys
				byte noKey = 0;						// Command key
			case ColorItem:
				key byte = 2;
				cstring;							// Label
				longint;							// Command
				integer none = 0;					// Modifiers
				byte noKey = 0;						// Key
				byte; byte; byte;					// Color	red, green, blue
			case Separator:
				key byte = 3;
			case Submenu:
				key byte = 4;
				longint;							// Submenu id
		}
	};
	byte = 0;
};


/* MenuBar */
type rtyp_Mbar {
	array {
		integer;									// Menu id's
	};
};


/* Dialog */
type 'DLOG' {
	rect;											// Bounds
	cstring;										// Name
	longint
		B_TITLED_WINDOW		= 1,
		B_MODAL_WINDOW		= 3,
		B_DOCUMENT_WINDOW	= 11,
		B_BORDERED_WINDOW	= 20;
	longint
		NORMAL						= 0x00004042,
		B_WILL_ACCEPT_FIRST_CLICK	= 0x00000010;
	longint = $$countof(items);
	array items {
		switch {
			case Button:
				key longint = 'btn ';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
				longint;							// Command
			case RadioButton:
				key longint = 'radb';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
			case CheckBox:
				key longint = 'chkb';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
			case Edit:
				key longint = 'edit';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
				cstring;							// Initial value
				cstring;							// Allowed chars
				integer;							// Max length
				integer;							// Label width
			case Caption:
				key longint = 'capt';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
			case PopupMenu:
				key longint = 'popu';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
				integer;							// Menu ID
				integer;							// Divider
			case List:
				key longint = 'list';
				rect;								// Bounds
				cstring;							// Name
			case OutlineList:
				key longint = 'olst';
				rect;								// Bounds
				cstring;							// Name
			case ColorControl:
				key longint = 'clct';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
			case ColorSquare:
				key longint = 'csqr';
				rect;								// Bounds
				cstring;							// Name
			case ColorSlider:
				key longint = 'csld';
				rect;								// Bounds
				cstring;							// Name
			case ColorDemo:
				key longint = 'cdmo';
				rect;								// Bounds
				cstring;							// Name
			case ListBox:
				key longint = 'lbox';
				rect;								// Bounds
				cstring;							// Name
			case GrepListBox:
				key longint = 'gbox';
				rect;								// Bounds
				cstring;							// Name
			case StringListBox:
				key longint = 'slbx';
				rect;								// Bounds
				cstring;							// Name
			case PathBox:
				key longint = 'pbox';
				rect;								// Bounds
				cstring;							// Name
			case StdErrBox:
				key longint = 'ebox';
				rect;								// Bounds
				cstring;							// Name
			case KeyCapture:
				key longint = 'keyc';
				rect;								// Bounds
				cstring;							// Name
			case Line:
				key longint = 'line';
				rect;								// Bounds
			case TabbedBook:
				key longint = 'tabb';
				rect;								// Bounds
				cstring;							// Name
			case TabbedBookEnd:
				key longint = 'tabe';
			case TabSheet:
				key longint = 'shet';
				cstring;							// Name
				cstring;							// Description
			case TabSheetEnd:
				key longint = 'shte';
			case Box:
				key longint = 'box ';
				rect;								// Bounds
				cstring;							// Name
			case Line:
				key longint = 'line';
				rect;								// Bounds of the line
			case Slider:
				key longint = 'sldr';
				rect;								// Bounds
				cstring;							// Name
				cstring;							// Label
				longint;							// Msg
				longint;							// Min
				longint;							// Max
				longint
					block		= 0,
					triangle	= 1;				// Thumb
			case BoxEnd:
				key longint = 'boxe';
		};
	};
};


/* MiniIcon + Tolbar Button Image */
type 'MICN' {
	hexstring;
};


/* Cursor */
type 'CURS' {
	byte;											// Size
	byte;											// Depth
	array {
		byte;
	};
	array {
		byte;										// Image
	};
	array {
		byte;										// Mask
	}
};


/* Command */
type rtyp_Cmnd {
	longint = $$CountOf(binding);
	array binding {
		longint;									// Command
		cstring;									// Description
	}
};


/* Bindings */
type rtyp_Bind {
	longint = $$CountOf(cmdbinding);				// Number of Bindings
	array cmdbinding {

		// The key combination
		integer
			Shift	= 0x0001,
			Cmd		= 0x0002,
			Ctrl	= 0x0004,
			Opt		= 0x0040,
			Menu	= 0x0080;						// Modifiers
		char = 0;									// Raw Character
		char;										// Keycode

		// The Prefix for this combination
		integer
			Shift	= 0x0001,
			Cmd		= 0x0002,
			Ctrl	= 0x0004,
			Opt		= 0x0040,
			Menu	= 0x0080;						// Modifiers
		char = 0;									// Raw Character
		char;										// Keycode

		// The command
		longint;									// Command
	};
};
