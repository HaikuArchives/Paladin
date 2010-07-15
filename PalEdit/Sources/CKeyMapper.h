/*	$Id: CKeyMapper.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 10/07/97 21:49:30
*/

#ifndef CKEYMAPPER_H
#define CKEYMAPPER_H


using std::set;

class PText;

struct KeyShortcut
{
	long combo;
	long prefix;

	KeyShortcut() : combo(0), prefix(0) {};
	
	inline bool operator<(const KeyShortcut& ks) const;
	inline bool operator==(const KeyShortcut& ks) const;
};

typedef std::map<KeyShortcut,int> keymap;

class CKeyMapper
{
  public:
	static void Init();
  	
	static CKeyMapper& Instance()
	{
		if (sfInstance == NULL)
			Init();
		return *sfInstance;
	}

	int GetCmd(PText *txt, int modifiers, int rawchar, int key);

	void ReadKeymap(keymap& kmap);
	void WriteKeymap(keymap& kmap);
	const keymap& Keymap()			{ return fKeybindings; }

  private:
	CKeyMapper();
	~CKeyMapper();
	
	void InitKeymap();

	static CKeyMapper *sfInstance;
	int fPrefix;
	bool fNrArgument;
	keymap fKeybindings;
	set<int> fPrefixSet;
};

inline bool KeyShortcut::operator<(const KeyShortcut& ks) const
{
	if (prefix > ks.prefix)
		return false;
	else if (prefix == ks.prefix)
		return combo < ks.combo;
	else
		return true;
} /* KeyShortcut::operator< */

inline bool KeyShortcut::operator==(const KeyShortcut& ks) const
{
	return prefix == ks.prefix && combo == ks.combo;
} /* KeyShortcut::operator== */

#define MODIFIERMASK	(B_CONTROL_KEY | B_COMMAND_KEY | B_OPTION_KEY | B_SHIFT_KEY | B_MENU_KEY)
#define BINDINGMASK ((MODIFIERMASK << 16) | 0x000000ff)

#endif // CKEYMAPPER_H
