/*	$Id: PTextAddOn.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#ifndef PTEXTADDON_H
#define PTEXTADDON_H

/*	$Id: PTextAddOn.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright Hekkelman Programmatuur
	Maarten Hekkelman
	
	Created: 03/13/98 09:41:16
	
	This is an extended proxy for use with Pe addons. It offers extended functionality
	to addons like the ability to create new documents and report errors in other files.
	
	Since this interface is an extension to the BeIDE addon interface you should make
	sure you are passed a valid proxy. You can do this by using RTTI like this:
	
	long perform_edit(MTextAddOn *addon)
	{
		PTextAddOn pAddon = dynamic_cast<PTextAddOn*>(addon);
		
		if (pAddon != NULL)
		{
			// we are called with a valid PTextAddOn
			
			...	// do our stuff here
		}
		else
			ReportError();	// The addon is not a PTextAddOn, probably called by BeIDE or Pe 1.0
	} // perform_edit 
	
*/

#include "MTextAddOnImpl.h"

class PTextAddOn : public MTextAddOnImpl {
public:
			PTextAddOn(MIDETextView&	inTextView);
			PTextAddOn(MIDETextView&	inTextView, const char *extension);

			~PTextAddOn();

/*
	NewDocument will open a new window in Pe and returns a new proxy to the new document.
	
	OpenDocument will open the file pointed to by entry. If you pass NULL, a standard Open
	dialog is displayed allowing the user to choose a file. If the user canceled or the entry was
	not valid, NULL is returned. Otherwise the result will be a new proxy to the new document.
*/
virtual	PTextAddOn* NewDocument();
virtual	PTextAddOn* OpenDocument(entry_ref *entry);

/*
	AddError will open an error list window and add this error to the list.
	These are the parameters expected:
	-	file should be the full path to the file containing the error
	-	line is the line number on which the error occured
	-	start is the offset from the beginning of the line to the token that triggered the error
	-	length is the length of the token that triggered the error
	-	description is a verbal description of the error
	-	error_type defines what error icon should be used
*/
			enum {
				errInfo, errWarning, errError
			};

virtual	void AddError(const char *file, int line, int start, int length,
				const char *description, error_type err = errWarning);

/*
	These routines help you to store preferences inside Pe's preferences file.
	
	SetPrefxxx stores a preferences value.
	GetPrefxxx retrieves a preferences value, you pass in a default value and if
		this preference was not previously stored, the default value is stored and
		returned.
*/

virtual	void SetPrefInt(const char *name, int value);
virtual	int GetPrefInt(const char *name, int defValue);

virtual	void SetPrefString(const char *name, const char *value);
virtual	const char* GetPrefString(const char *name, const char *defValue);

};

#endif // PTEXTADDON_H
