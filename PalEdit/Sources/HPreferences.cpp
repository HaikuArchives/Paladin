/*	$Id: HPreferences.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#include "pe.h"
#include "HPreferences.h"
#include "HError.h"
#include "Prefs.h"

#define BLOCK		BAutolock lock(fLock); if (! lock.IsLocked()) return

HPreferences *gPrefs = NULL;

HPreferences::HPreferences(const char *preffilename)
{
	BPath settings;
	
	FailOSErr(find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true));

	char p[PATH_MAX];
	strcpy(p, settings.Path());
	strcat(p, "/");
	strcat(p, preffilename);
	fFile = strdup(p);
	FailNil(fFile);
} /* HPreferences::HPreferences */

HPreferences::~HPreferences()
{
	PrefMap::iterator pi;
	
	for (pi = fPrefs.begin(); pi != fPrefs.end(); pi++)
		free((*pi).second);
	
	fPrefs.clear();

	free(fFile);
} /* HPreferences::~HPreferences */

const char *HPreferences::GetPrefString(const char *name, const char *def)
{
	PrefMap::iterator pi = fPrefs.find(name);
	
	if (pi != fPrefs.end())
		return (*pi).second;
	else
	{
		fPrefs[name] = strdup(def);
		return def;
	}
} /* HPreferences::GetPrefString */

const char *HPreferences::GetIxPrefString(const char *name, int ix)
{
	char n[32];
	ASSERT(strlen(name) < 28);
	
	sprintf(n, "%s%d", name, ix);
	
	PrefMap::iterator pi = fPrefs.find(n);
	
	if (pi != fPrefs.end())
		return (*pi).second;
	else
		return NULL;
} /* HPreferences::GetIxPrefString */

int HPreferences::GetPrefInt(const char *name, int def)
{
	PrefMap::iterator pi = fPrefs.find(name);
	
	if (pi != fPrefs.end())
		return atoi((*pi).second);
	else
	{
		SetPrefInt(name, def);
		return def;
	}
} /* HPreferences::GetPrefInt */

double HPreferences::GetPrefDouble(const char *name, double def)
{
	PrefMap::iterator pi = fPrefs.find(name);
	
	if (pi != fPrefs.end())
		return atof((*pi).second);
	else
	{
		SetPrefDouble(name, def);
		return def;
	}
} /* HPreferences::GetPrefDouble */

rgb_color HPreferences::GetPrefColor(const char *name, rgb_color def)
{
	PrefMap::iterator pi = fPrefs.find(name);
	
	if (pi != fPrefs.end())
	{
		rgb_color c;
		char s[4], *p;
		const char *v = (*pi).second;
		
		s[2] = 0;
		
		strncpy(s, v + 1, 2);
		c.red = strtoul(s, &p, 16);
		
		strncpy(s, v + 3, 2);
		c.green = strtoul(s, &p, 16);
		
		strncpy(s, v + 5, 2);
		c.blue = strtoul(s, &p, 16);
		
		return c;
	}
	else
	{
		SetPrefColor(name, def);
		return def;
	}
} /* HPreferences::GetPrefColor */

BRect HPreferences::GetPrefRect(const char *name, BRect def)
{
	PrefMap::iterator pi = fPrefs.find(name);
	
	if (pi != fPrefs.end())
	{
		BRect r(1, 1, 0, 0);
		const char *v = (*pi).second;
		char *p;
		
		r.left = strtod(v, &p);
		p++;
		r.top = strtod(p, &p);
		p++;
		r.right = strtod(p, &p);
		p++;
		r.bottom = strtod(p, &p);
		
		return r.IsValid() ? r : def;
	}
	else
	{
		SetPrefRect(name, def);
		return def;
	}
} /* HPreferences::GetPrefRect */

void HPreferences::SetPrefString(const char *name, const char *value)
{
	BLOCK;

	if (name && value)
	{
		PrefMap::iterator pi = fPrefs.find(name);
		
		if (pi != fPrefs.end())
		{
			free((*pi).second);
			(*pi).second = strdup(value);
		}
		else
			fPrefs[name] = strdup(value);
	}
} /* HPreferences::SetPrefString */

void HPreferences::SetIxPrefString(const char *name, int ix, const char *value)
{
	ASSERT(strlen(name) < 28);
	char n[32];
	sprintf(n, "%s%d", name, ix);
	
	SetPrefString(n, value);
} /* HPreferences::SetIxPrefString */

void HPreferences::SetPrefInt(const char *name, int value)
{
	char s[10];
	sprintf(s, "%d", value);
	SetPrefString(name, s);
} /* HPreferences::SetPrefInt */

void HPreferences::SetPrefDouble(const char *name, double value)
{
	char s[20];
	sprintf(s, "%g", value);
	SetPrefString(name, s);
} /* HPreferences::SetPrefDouble */

void HPreferences::SetPrefColor(const char *name, rgb_color value)
{
	char c[10];
	sprintf(c, "#%2.2x%2.2x%2.2x", value.red, value.green, value.blue);
	SetPrefString(name, c);
} /* HPreferences::SetPrefColor */

void HPreferences::SetPrefRect(const char *name, BRect value)
{
	char s[64];
	sprintf(s, "%g,%g,%g,%g", value.left, value.top, value.right, value.bottom);
	SetPrefString(name, s);
} /* HPreferences::SetPrefRect */

void HPreferences::RemovePref(const char *name)
{
	PrefMap::iterator pi = fPrefs.find(name);
	
	if (pi != fPrefs.end())
	{
		free((*pi).second);
		fPrefs.erase(pi);
	}
	
	if (strlen(name) < 28)
	{
		int ix = 0;
		char n[32];
		
		do
		{
			sprintf(n, "%s%d", name, ix++);
			
			pi = fPrefs.find(n);
	
			if (pi != fPrefs.end())
			{
				free((*pi).second);
				fPrefs.erase(pi);
			}
		}
		while (pi != fPrefs.end());
	}
} /* HPreferences::RemovePref */

void HPreferences::ReadPrefFile()
{
	BLOCK;

	FILE *f;
	
	f = fopen(fFile, "r");
	
	if (!f)
		return;
	
	char s[2048];
	
	while (fgets(s, 2047, f))
	{
		char *n, *v;
		
		n = s;
		v = strchr(s, '=');
		if (!v)
			continue;
		
		*v++ = 0;
		v[strlen(v) - 1] = 0;
		
		char *p = v;
		
		while ((p = strchr(p, 0x1b)) != 0)
		{
			if (p[1] == 'n')
			{
				memmove(p + 1, p + 2, strlen(p + 2) + 1);
				*p = '\n';
			}
			else if (p[1] == 0x1b)
				p++;
			p++;
		}
		
		fPrefs[n] = strdup(v);
	}
	
	fclose(f);
} /* HPreferences::ReadPrefFile */

void HPreferences::WritePrefFile()
{
	BLOCK;

	FILE *f;
	
	f = fopen(fFile, "w");
	
	if (!f)
		throw HErr("Could not create settings file");
	
	PrefMap::iterator pi;
	
	for (pi = fPrefs.begin(); pi != fPrefs.end(); pi++)
	{
		if (!(*pi).second)
			continue;
		else if (strchr((*pi).second, '\n') == NULL)
			fprintf(f, "%s=%s\n", (*pi).first.c_str(), (*pi).second);
		else
		{
			char *buf = (char *)malloc(2 * strlen((*pi).second));
			FailNil(buf);
			
			char *a, *b;
			a = buf, b = (*pi).second;
			
			while (*b)
			{
				if (*b == '\n')
				{
					*a++ = 0x1b;
					*a++ = 'n';
					b++;
				}
				else if (*b == 0x1b)
				{
					*a++ = 0x1b;
					*a++ = 0x1b;
					b++;
				}
				else
					*a++ = *b++;
			}

			*a = 0;

			fprintf(f, "%s=%s\n", (*pi).first.c_str(), buf);
			free(buf);
		}
	}
	
	fclose(f);
} /* HPreferences::WritePrefFile */

void HPreferences::ResetAll()
{
	BLOCK;

	fPrefs.erase(fPrefs.begin(), fPrefs.end());
} // HPreferences::ResetAll

void HPreferences::InitTextFont(BFont* font) {
	font_family ff;
	font_style fs;
	be_fixed_font->GetFamilyAndStyle(&ff, &fs);
	
	strcpy(ff, GetPrefString(prf_S_FontFamily, ff));
	strcpy(fs, GetPrefString(prf_S_FontStyle, fs));
	
	font->SetFamilyAndStyle(ff, fs);
	font->SetSize(GetPrefDouble(prf_D_FontSize, be_plain_font->Size()));
}	
