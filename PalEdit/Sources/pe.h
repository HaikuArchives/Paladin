/*	$Id: pe.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#ifndef PE_H
#define PE_H

#include <typeinfo>

#if __INTEL__	
// Include all of appkit

#include <AppKit.h>

	// include only subset of interfacekit
#include <Alert.h>
#include <Box.h>
#include <Bitmap.h>
#include <Button.h>
#include <CheckBox.h>
#include <Font.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <InterfaceDefs.h>
#include <ListItem.h>
#include <ListView.h>
#include <OutlineListView.h>
#include <RadioButton.h>
#include <Region.h>
#include <Screen.h>
#include <ScrollView.h>
#include <StringView.h>
#include <TextControl.h>
#include <Box.h>
/* Note that <UnicodeBlockObjects.h> is not included on purpose. */
#include <Window.h>

	// only subset of storage kit
#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <FindDirectory.h>
#include <NodeInfo.h>
#include <Path.h>
#include <Resources.h>

	// and only a subset of the support kit
#include <Archivable.h>
#include <Autolock.h>
#include <Beep.h>
#include <ByteOrder.h>
#include <Debug.h>
#include <StopWatch.h>
#include <TypeConstants.h>
#include <UTF8.h>


#include <cstdlib>
#include <cstring>
#include <cstdio>

//#endif // __INTEL__

#include <cstdarg>
#include <ctime>
#include <cerrno>

#include <vector>
#include <set>
#include <map>
#include <string>

#endif // __INTEL__

#endif // PE_H
