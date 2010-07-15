/*	$Id: Types.r,v 1.1 2008/12/20 23:35:48 darkwyrm Exp $
	
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

type 'MENU' {
	cstring;						// name
	array {
		switch {
			case Item:
				key byte = 1;
				cstring;				 	// label
				longint;					// command
				integer none = 0,			// modifier keys
						shift = 1,
						control = 4;
				byte noKey = 0;				// command key
			case ColorItem:
				key byte = 2;
				cstring;					// label
				longint;					// command
				integer none = 0;			// modifiers
				byte noKey = 0;				// key
				byte;						// color red
				byte;						//		 green
				byte;						//		 blue
			case Separator:
				key byte = 3;
			case Submenu:
				key byte = 4;
				longint;					// submenuid
		}
	};
	byte = 0;
};

type 'MBAR' {
	array {
		integer;		// menu id's
	};
};

type 'DLOG' {
	rect;										// bounds
	cstring;									// name
	longint B_TITLED_WINDOW = 1, B_MODAL_WINDOW = 3,
		B_DOCUMENT_WINDOW = 11, B_BORDERED_WINDOW = 20;
	longint NORMAL = 0x00004042;
	longint = $$countof(items);
	array items {
		switch {
			case Button:
				key longint = 'btn ';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
				longint;						// command
			case RadioButton:
				key longint = 'radb';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
			case CheckBox:
				key longint = 'chkb';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
			case Edit:
				key longint = 'edit';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
				cstring;						// Initial value
				cstring;						// allowed chars
				integer;						// max length
				integer;						// label width
			case Caption:
				key longint = 'capt';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
			case PopupMenu:
				key longint = 'popu';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
				integer;						// Menu ID
				integer;						// divider
			case ColorControl:
				key longint = 'clct';
				rect;							// bounds
				cstring;						// name
				cstring;						// label
			case ColorSquare:
				key longint = 'csqr';
				rect;							// bounds
				cstring;						// name
			case ColorSlider:
				key longint = 'csld';
				rect;							// bounds
				cstring;						// name
			case ColorDemo:
				key longint = 'cdmo';
				rect;							// bounds
				cstring;						// name
			case ListBox:
				key longint = 'lbox';
				rect;							// bounds
				cstring;						// name
			case StringListBox:
				key longint = 'slbx';
				rect;							// bounds
				cstring;						// name
			case PathBox:
				key longint = 'pbox';
				rect;							// bounds
				cstring;						// name
			case Line:
				key longint = 'line';
				rect;							// bounds
			case TabbedBook:
				key longint = 'tabb';
				rect;							// bounds
				cstring;						// name
			case TabbedBookEnd:
				key longint = 'tabe';
			case TabSheet:
				key longint = 'shet';
				cstring;						// name
				cstring;						// description
			case TabSheetEnd:
				key longint = 'shte';
			case Box:
				key longint = 'box ';
				rect;							// bounds
				cstring;						// name
			case BoxEnd:
				key longint = 'boxe';
		};
	};
};
