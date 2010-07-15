/*	$Id: HAppResFile.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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
#include "HAppResFile.h"
#include "HError.h"
#include "HStream.h"
#include "ResourcesMenus.h"

using namespace HResources;

BResources *gAppResFile = NULL;

static void InitAppResFile()
{
	app_info ai;
	be_app->GetAppInfo(&ai);
	
	BEntry entry(&ai.ref);
	
	BFile *file = new BFile;
	FailOSErr(file->SetTo(&entry, B_READ_ONLY));
	
	gAppResFile = new BResources;
	FailOSErr(gAppResFile->SetTo(file));
} // InitAppResFile

BMenuBar* HResources::GetMenuBar(BRect r, int id)
{
	BMenuBar *mbar = new BMenuBar(r, "mbar");
	FailNil(mbar);
	
	size_t size;
	const short *lst = (short *)GetResource(rtyp_Mbar, id, size);
	FailNilRes(lst);
	
	for (uint32 i = 0; i < (size / 2); i++)
		mbar->AddItem(GetMenu(lst[i]));
	
	return mbar;
} /* GetMenuBar */

BMenu* HResources::GetMenu(int id, bool popup)
{
	size_t size;
	const char *m = (char *)GetResource(rtyp_Menu, id, size);
	if (!m) throw HErr("Could not find resource!");
	
	BMemoryIO buf(m, size);
	BPositionIO& data = buf;
	
	char s[256];
	data >> s;
	
	BMenu *menu = popup ? new BPopUpMenu(s) : new BMenu(s);
	char type, key;
	long l;
	short modifiers;
	
	buf >> type;
	while (type)
	{
		switch (type)
		{
			case 1:
				data >> s >> l >> modifiers >> key;
				menu->AddItem(new BMenuItem(s, new BMessage(l), key, modifiers));
				break;
//			case 2:
//				break;
			case 3:
				menu->AddSeparatorItem();
				break;
			case 4:
				buf >> l;
				menu->AddItem(GetMenu(l));
				break;
			default:
				break;
		}
		buf >> type;
	}
	
	return menu;
} /* GetMenu */

const void* HResources::GetResource(unsigned long type, int id)
{
	if (gAppResFile == NULL)
		InitAppResFile();
	size_t size;
	return gAppResFile->LoadResource(type, id, &size);
} // HGetResource

const void* HResources::GetResource(unsigned long type, int id, size_t& size)
{
	if (gAppResFile == NULL)
		InitAppResFile();
	return gAppResFile->LoadResource(type, id, &size);
} // HGetResource

const void* HResources::GetResource(unsigned long type, int id, size_t& size, const char **name)
{
	if (gAppResFile == NULL)
		InitAppResFile();
	
	gAppResFile->GetResourceInfo(type, id, name, &size);
	return gAppResFile->LoadResource(type, id, &size);
} // HGetResource

const void* HResources::GetNamedResource(unsigned long type, const char *name)
{
	if (gAppResFile == NULL)
		InitAppResFile();
	
	size_t size;
	return gAppResFile->LoadResource(type, name, &size);
} // HResources::GetNamedResource

