/*	$Id: CDiffWindow.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 03/29/98 20:08:05
*/

#ifndef CDIFFWINDOW_H
#define CDIFFWINDOW_H

class PToolBar;
class HButtonBar;
class PDoc;

class CDiffItem : public BListItem {
public:
			CDiffItem(int a1, int a2, int b1, int b2)
				: BListItem(), fA1(a1), fA2(a2), fB1(b1), fB2(b2) {};

virtual	void DrawItem(BView *owner, BRect bounds, bool complete = false);

			int fA1, fA2, fB1, fB2;
};

class CDiffDItem : public BListItem {
public:
			CDiffDItem(const char *file, int where)
				: fFile(strdup(file)), fWhere(where) {};
			~CDiffDItem()
			{
				free(fFile);
			}

virtual	void DrawItem(BView *owner, BRect bounds, bool complete = false);

			char *fFile;
			int fWhere;
};

class CDiffWindow
	: public BWindow
{
public:
			CDiffWindow(BRect frame, const char *name);
		
virtual	void MessageReceived(BMessage *msg);

			~CDiffWindow();
		
			const entry_ref& File1() const;
			const entry_ref& File2() const;
		
private:
			void GetDocs(PDoc*& docA, PDoc*& docB, bool activate);
			
			void Work();
			void AddFile(int fileNr, BPoint where, bool showAlways, BPoint msgWhere);
			void HiliteDiffs();
			void ArrangeWindows();
			void Merge(int toFile);
			
			void WorkOnDirs();

			void CopyFiles();
			void CopyScript();
			bool FilesDiffer(const char *f, const char *d1, const char *d2);
			BListView *fScript;
			entry_ref fFile1, fFile2;
			HButtonBar *fButtonBar;
			PToolBar *fToolBar;
			BFilePanel *fPanel;
};

inline const entry_ref& CDiffWindow::File1() const
{
	return fFile1;
} /* CDiffWindow::File1 */

inline const entry_ref& CDiffWindow::File2() const
{
	return fFile2;
} /* CDiffWindow::File2 */

#endif // CDIFFWINDOW_H
