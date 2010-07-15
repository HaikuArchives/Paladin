/*	$Id: CDoc.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 09/10/97 13:20:21
*/

#ifndef CDOC_H
#define CDOC_H

#include <list>
#include <string>
#include <vector>

class BFile;
class BFilePanel;
class CDoc;

using std::string;
using std::vector;

typedef std::list<CDoc*> doclist;

// types of linebreaks
enum {
	kle_UNKNOWN = -1,
	kle_LF, 
	kle_CR, 
	kle_CRLF
};

class CDocIO;

class CDoc {

public:
			CDoc(const char* mimetype, BLooper *target, const entry_ref *doc = NULL);
	virtual	~CDoc();

			void Read(bool readAttributes = true);
	virtual	void Save();
	virtual	void SaveAs();

	virtual	const char* DefaultName() const;
			const char* Name() const;
		
			int  Encoding() const;
			void SetEncoding(int encoding);
			int  LineEndType() const;
			void SetLineEndType(int lineEndType);

			bool IsDirty() const;
	virtual	void SetDirty(bool dirty);
		
			bool IsReadOnly() const;
			void SetReadOnly(bool readOnly);

	virtual void GetText(BString &docText) const = 0;
	virtual void SetText(const BString& docText) = 0;
	virtual void CollectSettings(BMessage& settingsMsg) const = 0;
	virtual void ApplySettings(const BMessage& settingsMsg) = 0;
	virtual	void ReadAttr(BFile& file, BMessage& settingsMsg) = 0;
	virtual	void WriteAttr(BFile& file, const BMessage& settingsMsg) = 0;

	virtual void HighlightErrorPos(int errorPos);

	static	CDoc* FindDoc(const entry_ref& doc);
	static	CDoc* FindDoc(const char* name);
	static	CDoc* FirstDoc();
	static	int CountDocs();
	static	const doclist& DocList();
	static	void InvalidateAll();
	static	void PostToAll(unsigned long msg, bool async);
	static  void HandleFolderNodeMonitorMsg(BMessage* msg);

	static	void AddRecent(const char *path);
	static	bool GetNextRecent(char *path, int& indx);

			void SetMimeType(const char *type, bool updateOnDisk=true);
			const char *MimeType() const;

			const entry_ref* EntryRef() const;
			void SetEntryRef(const entry_ref* ref);
			
			bool HadError() const;
			const char* ErrorMsg() const;
			void SetErrorMsg(const char*);
			void ClearErrorMsg();

			void SetDocIO( CDocIO* docIO);

	virtual status_t InitCheck() const;

	// hook methods
	virtual void NameChanged();
	virtual void HasBeenSaved();

protected:

	virtual	void CreateFilePanel();
	virtual	void SaveRequested(entry_ref& directory, const char *name);
			void SaveACopy();
			void DoSaveACopy(entry_ref& directory, const char *name);
			void Revert();

	static	doclist sfDocList;
	static	vector<char*> sfTenLastDocs;

			void StartWatchingFile(void);
			void StopWatchingFile(bool stopDirectory = true);
			
protected:
			CDocIO* fDocIO;
			BFilePanel *fSavePanel;
private:
			CDoc();

			string fMimeType;
			bool fDirty;
			bool fReadOnly;
			int fEncoding;
			int fLineEndType;
			string fErrorMsg;
};

inline bool CDoc::IsDirty() const
{
	return fDirty;
} /* CDoc::IsDirty */

inline bool CDoc::IsReadOnly() const
{
	return fReadOnly;
} /* CDoc::IsReadOnly */

inline int CDoc::Encoding() const
{
	return fEncoding;
}

inline void CDoc::SetEncoding(int encoding)
{
	fEncoding = encoding;
}

inline int CDoc::LineEndType() const
{
	return fLineEndType;
}

inline void CDoc::SetLineEndType(int lineEndType)
{
	fLineEndType = lineEndType;
}

inline bool CDoc::HadError() const
{
	return fErrorMsg.length() > 0;
}

inline const char* CDoc::ErrorMsg() const
{
	return fErrorMsg.c_str();
}

inline void CDoc::SetErrorMsg(const char* errorMsg)
{
	fErrorMsg = errorMsg;
}

inline void CDoc::ClearErrorMsg()
{
	fErrorMsg.erase();
}

inline const char *CDoc::MimeType() const
{
	return fMimeType.c_str();
} /* CDoc::MimeType */

inline int CDoc::CountDocs()
{
	return sfDocList.size();
} /* CDoc::CountDocs */

inline CDoc* CDoc::FirstDoc()
{
	if (sfDocList.size())
		return sfDocList.front();
	else
		return NULL;
} /* CDoc::FirstDoc */

inline const doclist& CDoc::DocList()
{
	return sfDocList; 
}

#endif // CDOC_H
