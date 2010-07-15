/*	$Id: prefix.r,v 1.1 2008/12/20 23:35:48 darkwyrm Exp $
	
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

resource 'DLOG' (1, "dialog") {
	{ 0, 0, 250, 150 },
	"Prefix/Suffix",
	B_MODAL_WINDOW,
	NORMAL,
	{
		Caption			{{  10,  10, 100,  26 }, "", "Insert" },
		Edit			{{  10,  32, 240,  48 }, "text", "", ">", "", 1024, 0 },
		RadioButton		{{  10,  54, 110,  70 }, "before", "Before each line" },
		RadioButton		{{  10,  74, 110,  90 }, "after", "After each line" },
		CheckBox		{{  10, 100, 110, 116 }, "selection", "Selection only" },
		Button			{{ 180, 120, 240, 140 }, "ok", "OK", 'ok  ' },
		Button			{{ 110, 120, 170, 140 }, "cancel", "Cancel", 'cncl' }
	}
};
