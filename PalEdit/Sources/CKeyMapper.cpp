/*	$Id: CKeyMapper.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 10/07/97 21:53:17
*/

#include "pe.h"

#include "HStream.h"
#include "HAppResFile.h"
#include "CKeyMapper.h"
#include "PMessages.h"
#include "PApp.h"
#include "KeyBindings.h"
#include "PText.h"
#include "HError.h"
#include "ResourcesBindings.h"
#include "ResourcesUtf.h"

CKeyMapper* CKeyMapper::sfInstance = NULL;

void CKeyMapper::Init()
{
	sfInstance = new CKeyMapper();
} // CKeyMapper::Init

CKeyMapper::CKeyMapper()
{
	fPrefix = 0;
	fNrArgument = false;
	
	InitKeymap();
	
	sfInstance = this;
} // CKeyMapper::CKeyMapper

void CKeyMapper::InitKeymap()
{
	ReadKeymap(fKeybindings);

	keymap::iterator ki;
	for (ki = fKeybindings.begin(); ki != fKeybindings.end(); ki++)
	{
		if ((*ki).first.prefix)
			fPrefixSet.insert((*ki).first.prefix);
	}	
} /* CKeyMapper::CKeyMapper */

int CKeyMapper::GetCmd(PText *txt, int modifiers, int rawchar, int key)
{
	int modcode = (modifiers & MODIFIERMASK);
	if (key == 0x37 || key == 0x38 || key == 0x39 
		|| key == 0x48 || key == 0x49 || key == 0x4a	
		|| key == 0x58 || key == 0x59 || key == 0x5a 
		|| key == 0x64 || key == 0x65)
		// it's a keypad key which may have two meanings, depending on the
		// numlock-state. In order to find out, we add B_NUM_LOCK to the 
		// modifier-mask:
		modcode = (modifiers & (MODIFIERMASK | B_NUM_LOCK));
	else
		// default behaviour, we don't care about B_NUM_LOCK:
		modcode = (modifiers & MODIFIERMASK);
	
	int keycode = (modcode << 16) | key;
	KeyShortcut ks;
	int cmd = msg_Nothing;
	
	if (txt && txt->IsIncSearching() && rawchar == B_ESCAPE)
		; // pass this on
	else if (txt && fNrArgument)
	{
		fNrArgument = false;
		if (isdigit(rawchar))
		{
			BMessage msg(kmsg_NrArgument);
			msg.AddInt32("Nr Argument", rawchar - '0');
			BMessenger(txt).SendMessage(&msg);
		}
		else
			beep();
	}
	else if (fPrefix)
	{
		ks.prefix = fPrefix;
		ks.combo = keycode;
		fPrefix = 0;

		if (fKeybindings.find(ks) != fKeybindings.end())
			cmd = fKeybindings[ks];
		else
		{
			beep();
			cmd = msg_Nothing;
		}
	}
	else
	{
		ks.prefix = 0;
		ks.combo = keycode;
		
		if (fPrefixSet.count(keycode))
			fPrefix = keycode;

		if (fKeybindings.count(ks))
			cmd = fKeybindings[ks];
		
		if (cmd == kmsg_NrArgument)
		{
			fNrArgument = true;
			cmd = msg_Nothing;
		}
	}
	
	if (txt && txt->Anchor() == txt->Caret() && rawchar == B_TAB &&
		(cmd == msg_ShiftLeft || cmd == msg_ShiftRight))
	{
		cmd = msg_Nothing;
	}
	
	return cmd;
} /* CKeyMapper::GetCmd */

/*
		Keymap IO functions
*/

void CKeyMapper::ReadKeymap(keymap& kmap)
{
	static sem_id kmsem = create_sem(1, "reading keymap");
	if (kmsem < 0) THROW(("Error creating semaphore"));
	
	FailOSErr(acquire_sem(kmsem));

	BFile file;

	if (!gPrefsDir.Contains("keybindings"))
	{
		FailOSErr(gPrefsDir.CreateFile("keybindings", &file));

		long resID = rid_Bind_Editing, cnt = 0;
		BMallocIO b;
		
		b << cnt;
		
		while (true)
		{
			size_t size;
			const void *p;
			
			if ((p = HResources::GetResource(rtyp_Bind, resID++, size)) == NULL)
				break;
			
			BMemoryIO buf(p, size);
			
			long t;
			
			buf >> t;
			cnt += t;
			
			while (t--)
			{
				short modifiers;
				char rawchar, key;
				long cmd;
			
				buf >> modifiers >> rawchar >> key;
				
				long k = (modifiers << 16) | key;
				b << k;
				buf >> modifiers >> rawchar >> key;
				k = (modifiers << 16) | key;
				b << k;
				
				buf >> cmd;
				b << cmd;
			}
		}
		
		b.Seek(0, SEEK_SET);
		b << cnt;
		
		if (file.Write(b.Buffer(), b.BufferLength()) != b.BufferLength())
			THROW(("Error writing keybindings"));
	}
	else
		FailOSErr(file.SetTo(&gPrefsDir, "keybindings", B_READ_ONLY));
	
	long cnt, cmd, size;

	size = file.Seek(0, SEEK_END);
	file.Seek(0, SEEK_SET);
	file >> cnt;
	
	if (cnt != size / (sizeof(long) * 3))
	{
		cnt = __swap_int32(cnt);
		if (cnt != size / (sizeof(long) * 3))
			THROW(("Invalid keybinding file!"));
		
		while (cnt--)
		{
			KeyShortcut ks;
			
			int a, b, c;
			
			file >> a >> b >> c;
			
			ks.combo = __swap_int32(a);
			ks.prefix = __swap_int32(b);
			cmd = __swap_int32(c);
			
			kmap[ks] = cmd;
		}
	}
	else
		while (cnt--)
		{
			KeyShortcut ks;
			file >> ks.combo >> ks.prefix >> cmd;
			kmap[ks] = cmd;
		}
	
	FailOSErr(release_sem(kmsem));
} /* CKeyMapper::ReadKeymap */

void CKeyMapper::WriteKeymap(keymap& kmap)
{
	BFile file;

	if (gPrefsDir.Contains("keybindings"))
	{
		BEntry e;
		
		FailOSErr(gPrefsDir.FindEntry("keybindings", &e, true));
		FailOSErr(e.Remove());
	}

	FailOSErr(gPrefsDir.CreateFile("keybindings", &file));

	file << kmap.size();

	keymap::iterator ki;
	for (ki = kmap.begin(); ki != kmap.end(); ki++)
	{
		file << (*ki).first.combo << (*ki).first.prefix << (long)(*ki).second;
	}
	
	InitKeymap();
} /* CKeyMapper::WriteKeymap */
