/*	$Id: PDoc.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 10/25/97 08:57:11 by Maarten Hekkelman
*/

#ifndef PDOC_H
#define PDOC_H

#include <algorithm>
#include <typeinfo>

#include "CDocWindow.h"
#include "HDialog.h"

#include "PTextBuffer.h"

class PText;
class PStatus;
class HDialog;
class HButtonBar;
class PToolBar;
class HDialog;
class BMenu;
class BStringView;

using std::vector;

template <class T>
class PDialog 
{
  public:
  	bool operator() (HDialog*& d)
	  	{	return typeid(*d) == typeid(T);	}
};

class PDoc : public CDocWindow
{
			typedef CDocWindow inherited;
public:
			PDoc(const entry_ref *ref = NULL, bool show = true);
virtual		~PDoc();
		
virtual		const char* DocWindowType();

virtual		void MessageReceived(BMessage *msg);
virtual		bool QuitRequested();

static		void Stack();
static		void Tile();
static 		void UpdateShortCuts();

			PText* TextView() const;
			PStatus* Status() const;
			HButtonBar* ButtonBar() const;
			PToolBar* ToolBar() const;
	
			void MakeWorksheet();
			bool IsWorksheet();
static		PDoc* GetWorksheet();
			
			bool IsSourceFile();
			bool IsHeaderFile();
			void OpenPartner();

static		PDoc* TopWindow();

static		void LoadAddOns();
static		void BuildExtensionsMenu(BMenu *addOnMenu);

virtual		void SetDirty(bool dirty);

virtual 	BHandler* ResolveSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property);
virtual 	status_t GetSupportedSuites(BMessage *data);
			void SetError(const char *errtxt, rgb_color color);

virtual		void WindowActivated(bool active);

			void ChangeSourceEncoding(int encoding);

			template <class T>
			void GetDialog(T*& dlog)
			{
				vector<HDialog*>::iterator i = 
					find_if(fDialogs.begin(), fDialogs.end(), PDialog<T>());
				if (i != fDialogs.end())
				{
					(*i)->SetWorkspaces(1 << current_workspace());
					(*i)->Activate();
					dlog = static_cast<T*>(*i);
				}
				else
				{
					MakeDialog(this, dlog);
					dlog->Show();
				}
			}

protected:

virtual 	void GetText(BString &docText) const;
virtual 	void SetText(const BString& docText);
virtual 	void CollectSettings(BMessage& settingsMsg) const;
virtual 	void ApplySettings(const BMessage& settingsMsg);

virtual		void ReadAttr(BFile& file, BMessage& settingsMsg);
virtual		void WriteAttr(BFile& file, const BMessage& settingsMsg);

virtual		void CreateFilePanel();
virtual		void SaveRequested(entry_ref& directory, const char *name);

virtual 	void NameChanged();
virtual 	void HasBeenSaved();
virtual		void HighlightErrorPos(int errorPos);

private:
			void InitWindow(const char *name);

virtual		void MenusBeginning();
			
			void OpenSelection();
public:
			void OpenInclude(const char *incl);
			static void IDEBringToFront();
			static void IDEForceRebuild();
			static void IDEProject2Group();

private:
			void ResetMenuShortcuts();
			void PerformExtension(int nr);
			void PerformExtension(const char *ext);
			void SearchAlternativeSuffix(const BDirectory& directory,
				const char* name, BEntry& entry, const char* suffix, ...);

			void InstantiateAddOns();
			void DeleteAddOns();

			void IDEAddFile();
			void IDERemoveFile();
			void IDEMake();
			void IDERun();
			void IDERunInTerminal();
			void IDERunInDebugger();
			void IDEToggleErrors();
//			bool IDEOpenSourceHeader(entry_ref& ref);

			void ShowRecentMenu(BPoint where, bool showalways);

			HButtonBar *fButtonBar;
			PText *fText;
			PTextBuffer fTextBuffer;
			PToolBar *fToolBar;
			BMenu *fMBar;
			BMenu *fRecent;
			BMenu *fWindows;
			BStringView *fError;
			int fShortcut;
			PStatus *fStatus;
			bool fIsWorksheet;
			int fWindowMenuLength;
};

inline PText* PDoc::TextView() const
{
	return fText;
} /* PDoc::TextView */

inline PStatus* PDoc::Status() const
{
	return fStatus;
} /* PDoc::Status */

inline HButtonBar* PDoc::ButtonBar() const
{
	return fButtonBar;
} /* PDoc::ButtonBar */

inline PToolBar* PDoc::ToolBar() const
{
	return fToolBar;
} /* PDoc::ToolBar */

inline bool PDoc::IsWorksheet()
{
	return fIsWorksheet;
} /* PDoc::IsWorksheet */

#endif // PDOC_H
