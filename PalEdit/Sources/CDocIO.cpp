/*
	Copyright 2005 Oliver Tappe

	Distributed under the MIT License
*/

#include "pe.h"

#include <NodeMonitor.h>
#include <fs_attr.h>
#include <String.h>

#include "CDoc.h"
#include "CDocIO.h"
#include "HError.h"
#include "HPreferences.h"
#include "MAlert.h"
#include "Prefs.h"

static void ConvertLinebreaks(BString& str, int fromType, int toType)
{
	const char* lbStr[] = {
		"\n", "\r", "\r\n"
	};
	if (fromType == kle_UNKNOWN)
		fromType = kle_LF;
	if (toType == kle_UNKNOWN)
		toType = kle_LF;
	const char* fromStr = lbStr[fromType];
	const char* toStr = lbStr[toType];
	if (fromType != toType)
	{
		vector<int> posVect;
		for( int32 pos = 0; (pos = str.FindFirst(fromStr, pos)) >= B_OK; ++pos)
			posVect.push_back(pos);
		int32 szDiff = (int32)strlen(toStr)-(int32)strlen(fromStr);
		int32 newLen = str.Length() + szDiff * (int32)posVect.size();
		if (newLen <= 0)
			str.Truncate(0);
		else
		{
			BString dest;
			const char* oldAdr = str.String();
			char* destBuf = dest.LockBuffer(newLen);
			char* newAdr = destBuf;
			FailNil(newAdr);
			int32 pos, len;
			int32 lastPos = 0;
			int fromLen = strlen(fromStr);
			int toLen = strlen(toStr);
			for( uint32 i=0; i<posVect.size(); ++i)
			{
				pos = posVect[i];
				len = pos - lastPos;
				if (len > 0)
				{
					memcpy(newAdr, oldAdr, len);
					oldAdr += len;
					newAdr += len;
				}
				memcpy(newAdr, toStr, toLen);
				oldAdr += fromLen;
				newAdr += toLen;
				lastPos = pos+fromLen;
			}
			len = str.Length() + 1 - lastPos;
			if (len > 0)
				memcpy(newAdr, oldAdr, len);
			dest.UnlockBuffer(newAdr-destBuf+len-1);
			str.Adopt(dest);
		}
	}
}

static int32 DetermineLineEndType(const BString& str)
{
	for( const char* s = str.String(); *s; ++s)
	{
		if (*s == '\n')
			return kle_LF;
		if (*s == '\r')
			return (*(s+1) == '\n') ? kle_CRLF : kle_CR;
	}
	return kle_LF;
}

static int32 DetermineEncoding(const BString& str)
{
	/* HACK: Get the first line and see if there's something like
	  "[PE:ENC=<encoding>]" in there. <encoding> is supported encodings
	  with spaces replaced by "-", eg.: "ISO-8859-15" */
	int32			pos;
	BString			line;
	int32			enc_id = -1;
	BString			enc_name;
	CEncodingRoster	enc_roster;

	// Get the first line
	if ((pos = str.FindFirst('\n')) != B_ERROR ||
	    (pos = str.FindFirst('\r')) != B_ERROR)
	{
		str.CopyInto(line, 0, pos);
		// Cut down to begin of magic identifier, if there
		if ((pos = line.IFindFirst("[PE:")) != B_ERROR)
		{
			line.Remove(0, pos+4);
			// Find end of settings and cut the rest off
			if ((pos = line.FindFirst(']')) != B_ERROR)
			{
				line.Remove(pos, line.Length()-pos);
				// Check supported encodings
				// (sofar no other settings allowed)
				while (enc_roster.IsSupportedEncoding(++enc_id))
				{
					enc_name = enc_roster.EncodingNameByIdx(enc_id);
					enc_name.ReplaceAll(' ', '-');
					enc_name.Prepend("ENC=");
					if (line.ICompare(enc_name) == 0)
					{
						return enc_id;
					}
				}
			}
		}
	}
	return B_UNICODE_UTF8;
}

static void CopyAttributes(BFile& from, BFile& to)
{
	try
	{
		char name[NAME_MAX];

		while (from.GetNextAttrName(name) == B_OK)
		{
			attr_info ai;

			if (strcmp(name, "_trk/_clipping_file_") == 0)
				continue;

			FailOSErr(from.GetAttrInfo(name, &ai));

			char *buf = new char [ai.size];
			from.ReadAttr(name, ai.type, 0, buf, ai.size);
			to.WriteAttr(name, ai.type, 0, buf, ai.size);
			delete [] buf;
		}

		mode_t perm;
		from.GetPermissions(&perm);
		to.SetPermissions(perm);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
}

static bool CopyFile(BEntry& srcEntry, BEntry& dstEntry)
{
	try
	{
		char srcName[B_FILE_NAME_LENGTH];

		BFile srcFile;
		BFile dstFile;
		BDirectory dir;
		const int bufSize = 1024;
		char buf[bufSize];
		ssize_t bytesRead;
		time_t created;

		// Create destination file
		FailOSErr(srcEntry.GetCreationTime(&created));
		FailOSErr(srcEntry.GetName(srcName));
		string dstName(srcName);
		dstName += '~';
		FailOSErr(srcEntry.GetParent(&dir));
		FailOSErr(dstEntry.SetTo(&dir, dstName.c_str(), true));
		FailOSErr(dstFile.SetTo(&dstEntry, B_WRITE_ONLY|B_CREATE_FILE|B_ERASE_FILE));

		// Copy file data, attributes and time
		FailOSErr(srcFile.SetTo(&srcEntry, B_READ_ONLY));
		while ((bytesRead = srcFile.Read((void *)buf, bufSize)) > 0)
		{
			dstFile.Write(buf, bytesRead);
		}
		CopyAttributes(srcFile, dstFile);
		FailOSErr(dstFile.SetCreationTime(created));
		dstFile.Sync();
	}
	catch (HErr& e)
	{
		e.DoError();
		return false;
	}
	return true;
}

// #pragma mark - CDocIO

CDocIO::CDocIO(CDoc* doc)
	: fDoc(doc)
	, fLastSaved(0)
{
}

CDocIO::~CDocIO()
{
}

bool CDocIO::VerifyFile()
{
	return true;
}

const entry_ref* CDocIO::EntryRef() const
{
	return NULL;
}

void CDocIO::SetEntryRef(const entry_ref* ref)
{
}

BLooper* CDocIO::Target()
{
	return NULL;
}

void CDocIO::StartWatchingFile()
{
}

void CDocIO::StopWatchingFile(bool stopDirectory)
{
}

void CDocIO::HandleNodeMonitorMsg(BMessage* msg)
{
}

bool CDocIO::MatchesNodeMonitorMsg(BMessage* msg)
{
	return false;
}

bool CDocIO::DoPreEditTextConversions(BString& docText)
{
	// convert from document's native encoding to internal type (UTF-8):
	fConv.SetTo(fDoc->Encoding());
	FailOSErr(fConv.ConvertToUtf8(docText));
	if (fConv.HadToSubstitute() && fConv.ErrorPos() >= 0)
	{
		BString err;
		if (fDoc->Encoding() != B_UNICODE_UTF8)
			err = BString("An error occurred when converting the document ")
					<< "from its native encoding to UTF-8. The first "
					<< "problematic character is highlighted.\n"
					<< "Please use the 'File Options'-dialog to select "
					<< "the correct source encoding.";
		else
			err = BString("An error occurred when checking the document's ")
					<< "UTF-8 encoding. The first problematic character is "
					<< "highlighted.\n"
					<< "Please use the 'File Options'-dialog to select "
					<< "the correct source encoding.";
		fDoc->SetErrorMsg(err.String());
		return false;
	}

	// convert from document's native linebreaks to internal type (LF):
	ConvertLinebreaks(docText, fDoc->LineEndType(), kle_LF);

	return true;
}

bool CDocIO::DoPostEditTextConversions(BString& docText)
{
	if (fDoc->Encoding() != B_UNICODE_UTF8)
	{
		// convert from internal encoding (UTF-8) to native type:
		fConv.SetTo(fDoc->Encoding());
		FailOSErr(fConv.ConvertFromUtf8(docText));
		if (fConv.HadToSubstitute() && fConv.ErrorPos() >= 0)
		{
			fDoc->HighlightErrorPos(fConv.ErrorPos());
			BString err =
				BString("An error occurred when converting the document ")
					<< "to the requested destination encoding. The first "
					<< "problematic character is highlighted.\n"
					<< "Please use the 'File Options'-dialog to select "
					<< "another destination encoding.";
			MInfoAlert a(err.String(), "I See");
			a.Go();
			return false;
		}
	}

	if (gPrefs->GetPrefInt(prf_I_EndWithNewline, 1) && docText.Length() > 0
		&& docText[docText.Length() - 1] != '\n')
	{
		docText << "\n";
	}

	// convert from internal linebreaks (LF) to native type:
	ConvertLinebreaks(docText, kle_LF, fDoc->LineEndType());

	return true;
}

// #pragma mark - CLocalDocIO

BLocker CLocalDocIO::sfDocListLock("DocListLock");

CLocalDocIO::CLocalDocIO(CDoc* doc, const entry_ref* eref, BLooper* target)
	: CDocIO(doc)
	, fEntryRef(NULL)
	, fTarget(target)
{
	if (eref)
		fEntryRef = new entry_ref(*eref);
}

CLocalDocIO::~CLocalDocIO()
{
	delete fEntryRef;
}

bool CLocalDocIO::IsLocal() const
{
	return true;
}

const char* CLocalDocIO::Name() const
{
	return fEntryRef ? fEntryRef->name : fDoc->DefaultName();
}

const entry_ref* CLocalDocIO::EntryRef() const
{
	return fEntryRef;
}

bool CLocalDocIO::ReadDoc(bool readAttributes)
{
	BFile file;
	FailOSErr(file.SetTo(fEntryRef, B_READ_ONLY));

	BMessage settingsMsg;
	if (readAttributes)
		fDoc->ReadAttr(file, settingsMsg);

	off_t size;
	FailOSErr(file.GetSize(&size));

	BString docText;

	if (size > 0)
	{
		char* buf = docText.LockBuffer(size);
		FailNil(buf);
		int bytesRead = file.Read(buf, size);
		docText.UnlockBuffer(size);
		if (bytesRead < size)
			THROW(("A read error occurred: %s", strerror(errno)));
	}

	if (readAttributes)
	{
		// determine and set the type of linebreaks:
		int32 lineEndType;
		if (settingsMsg.FindInt32("line breaks", &lineEndType) != B_OK)
			lineEndType = DetermineLineEndType(docText);
		fDoc->SetLineEndType(lineEndType);

		// set the encoding:
		int32 encoding;
		if (settingsMsg.FindInt32("encoding", &encoding) != B_OK)
			encoding = DetermineEncoding(docText);
		// [zooey]: For a short period of time (one of my bugs),
		// 			Pe used -1 as UTF-8, we compensate:
		if (encoding < 0)
			encoding = B_UNICODE_UTF8;
		fDoc->SetEncoding(encoding);
	}

	bool result = DoPreEditTextConversions(docText);

	fDoc->SetText(docText);
	fDoc->SetDirty(false);

	if (readAttributes)
		fDoc->ApplySettings(settingsMsg);

	if (!result && fConv.ErrorPos() >= 0)
		fDoc->HighlightErrorPos(fConv.ErrorPos());

	FailOSErr(BEntry(fEntryRef).GetModificationTime(&fLastSaved));

	return result;
}

bool CLocalDocIO::WriteDoc()
{
	char name[B_FILE_NAME_LENGTH];
	BEntry e(fEntryRef, true);
	BEntry backup;
	bool existed = e.Exists();

	try
	{
		BMessage settingsMsg;
		fDoc->CollectSettings(settingsMsg);
		BString docText;
		fDoc->GetText(docText);
		if (!DoPostEditTextConversions(docText))
			return false;

		FailOSErr(e.GetName(name));

		if (existed)
		{	// Create a backup file
			if (!CopyFile(e, backup))
				return false;
		}

		// Write document
		BFile file;
		BDirectory dir;
		FailOSErr(e.GetParent(&dir));
		FailOSErr(dir.CreateFile(name, &file, false));
		CheckedWrite(file, docText.String(), docText.Length());
		fDoc->WriteAttr(file, settingsMsg);
		file.Sync();

		// Remove backup file
		if (existed && !gPrefs->GetPrefInt(prf_I_MakeBackup))
			FailOSErr(backup.Remove());

		// Update MIME type info
		e.SetTo(&dir, name);
		BPath path(&e);
		if (!strlen(fDoc->MimeType()) && path.InitCheck() == B_OK
			&& update_mime_info(path.Path(), false, true, false) == B_OK)
		{
			// takeover MIME type from file
			char s[NAME_MAX];
			if (BNodeInfo(&file).GetType(s) == B_OK)
				fDoc->SetMimeType(s, false);
		}
		else
			BNodeInfo(&file).SetType(fDoc->MimeType());

		time(&fLastSaved);
	}
	catch (HErr& err)
	{
		err.DoError();
		return false;
	}
	return true;
}

void CLocalDocIO::SetEntryRef(const entry_ref* ref)
{
	delete fEntryRef;
	if (ref)
		fEntryRef = new entry_ref(*ref);
	else
		fEntryRef = NULL;
}

BLooper* CLocalDocIO::Target()
{
	return fTarget;
}

void CLocalDocIO::StartWatchingFolder()
{
	node_ref directoryNodeRef;
	BEntry entry(fEntryRef);
	BNode node;
	if (entry.GetParent(&entry) == B_OK
		&& node.SetTo(&entry) == B_OK
		&& node.GetNodeRef(&directoryNodeRef) == B_OK)
	{
		if (sfDocListLock.Lock())
		{
			if (sfWatchedFolderMap[directoryNodeRef]++ == 0)
				watch_node(&directoryNodeRef, B_WATCH_DIRECTORY, be_app);
			sfDocListLock.Unlock();
		}
	}
}

void CLocalDocIO::StopWatchingFolder()
{
	node_ref directoryNodeRef;
	BEntry entry(fEntryRef);
	BNode node;
	if (entry.GetParent(&entry) == B_OK
		&& node.SetTo(&entry) == B_OK
		&& node.GetNodeRef(&directoryNodeRef) == B_OK)
	{
		if (sfDocListLock.Lock())
		{
			if (sfWatchedFolderMap[directoryNodeRef]-- == 1)
				watch_node(&directoryNodeRef, B_STOP_WATCHING, be_app);
			sfDocListLock.Unlock();
		}
	}
}

void CLocalDocIO::StartWatchingFile()
{
	if (fEntryRef == NULL || fTarget == NULL)
		return;

	// start monitoring this file for changes
	BNode node(fEntryRef);
	if (node.GetNodeRef(&fNodeRef) == B_OK)
		watch_node(&fNodeRef, B_WATCH_NAME | B_WATCH_STAT, fTarget);


	StartWatchingFolder();
}

void CLocalDocIO::StopWatchingFile(bool stopDirectory)
{
	if (fEntryRef == NULL || fTarget == NULL)
		return;

	watch_node(&fNodeRef, B_STOP_WATCHING, fTarget);

	// if we get late messages, we don't want to deal with them
	fNodeRef.device = -1;
	fNodeRef.node = -1;

	if (stopDirectory)
		StopWatchingFolder();
}

void CLocalDocIO::HandleNodeMonitorMsg(BMessage* msg)
{
	int32 opcode;
	if (fEntryRef == NULL || msg->FindInt32("opcode", &opcode) != B_OK)
		return;

	const char *name;
	if ((opcode == B_ENTRY_CREATED
			|| opcode == B_ENTRY_MOVED && fNodeRef.node == -1)
		&& msg->FindString("name", &name) == B_OK
		&& !strcmp(name, fEntryRef->name))
	{
		if (!VerifyFile())
			ReadDoc(false);
	}
	else if (msg->FindInt64("node") != fNodeRef.node)
	{
		// filter out other stuff that comes from watching the directory
		return;
	}

	switch (opcode)
	{
		case B_ENTRY_MOVED:
		{
			int64 directory;
			if (msg->FindInt64("to directory", &directory) == B_OK)
				fEntryRef->directory = directory;

			const char *name;
			if (msg->FindString("name", &name) == B_OK)
				fEntryRef->set_name(name);

			fDoc->NameChanged();
			break;
		}
		case B_ENTRY_REMOVED:
		{
			StopWatchingFile(false);
				// We don't want to stop monitoring the directory; BTW, it
				// will get automatically updated on next save, the monitoring
				// slot is not lost
			fDoc->SetDirty(true);
			break;
		}
		case B_STAT_CHANGED:
			if (!VerifyFile())
				ReadDoc(false);
			break;
	}
}

bool CLocalDocIO::MatchesNodeMonitorMsg(BMessage* msg)
{
	int32 opcode = msg->FindInt32("opcode");

	if (opcode == B_ENTRY_REMOVED)
	{
		// the message is for us if it refers to our node-ref:
		node_ref nref;
		if (msg->FindInt64("node", &nref.node) == B_OK
			&& msg->FindInt32("device", &nref.device) == B_OK
			&& nref == fNodeRef)
			return true;
	}
	else if (opcode == B_ENTRY_CREATED || opcode == B_ENTRY_MOVED)
	{
		// the message is for us if it refers to our parent folder and carries
		// our name:
		node_ref pref;
		BString name;
		if (fEntryRef != NULL
			&& (msg->FindInt64("directory", &pref.node) == B_OK
				|| msg->FindInt64("to directory", &pref.node) == B_OK)
			&& msg->FindInt32("device", &pref.device) == B_OK
			&& msg->FindString("name", &name) == B_OK
			&& pref.device == fEntryRef->device
			&& pref.node == fEntryRef->directory
			&& name == fEntryRef->name)
			return true;
	}
		
	return false;
}

bool CLocalDocIO::VerifyFile()
{
	bool result = true;
	try
	{
		if (gPrefs->GetPrefInt(prf_I_VerifyOpenFiles, 1))
		{
			BFile file;
			FailOSErr(file.SetTo(fEntryRef, B_READ_ONLY));

			time_t t;
			FailOSErr(file.GetModificationTime(&t));
			if (fLastSaved && t > fLastSaved + 1)
			{
				char s[PATH_MAX + 20];
				sprintf(s, "File %s was modified by another application, reload it?", fEntryRef->name);
				MInfoAlert a(s, "Reload", "Cancel");

				if (a.Go() == 1)
				{
					result = false;
					// restart watching, the file may have changed:
					StopWatchingFile();
					StartWatchingFile();
				}
			}

			fLastSaved = std::max(fLastSaved, time(NULL));
				// if more than one update request was issued
				// in the mean time, only the first one is
				// considered
				// the max() takes care of mod-times in the future
				// (cvs may cause that kind of thing).
		}
	}
	catch (HErr& e)
	{
		// file seems to be gone, but it doesn't really matter...
		// (we could only run into problems if the directory is gone)
	}
	return result;
}
