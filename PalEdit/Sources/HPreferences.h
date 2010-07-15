/*	$Id: HPreferences.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

#ifndef HPREFERENCES_H
#define HPREFERENCES_H

#include "HLibHekkel.h"

#include "pe.h"
#include "HError.h"

#include <string>

using std::string;

typedef std::map<string, char*> PrefMap;

class IMPEXP_LIBHEKKEL HPreferences {
public:
	HPreferences(const char *preffilename);
	virtual ~HPreferences();

	const char *GetPrefString(const char *name, const char *def = NULL);
	const char *GetIxPrefString(const char *name, int ix);
	
	int GetPrefInt(const char *name, int def = 0);
	double GetPrefDouble(const char *name, double def = 0.0);
	rgb_color GetPrefColor(const char *name, rgb_color def);
	BRect GetPrefRect(const char *name, BRect def);

	void SetPrefString(const char *name, const char *value);
	void SetIxPrefString(const char *name, int ix, const char *value);
	
	void SetPrefInt(const char *name, int value);
	void SetPrefDouble(const char *name, double value);
	void SetPrefColor(const char *name, rgb_color value);
	void SetPrefRect(const char *name, BRect value);
	
	void RemovePref(const char *name);
	
	void InitTextFont(BFont* font);

	void ReadPrefFile();
	void WritePrefFile();
	void ResetAll();

protected:
	void Clear();

	PrefMap fPrefs;
	
	char *fFile;
	BLocker fLock;
};

extern IMPEXP_LIBHEKKEL HPreferences *gPrefs;

#endif // HPREFERENCES_H
