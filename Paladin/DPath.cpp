/*
	DPath:	A class for handling the various parts of a string-based file path
	Copyright 2008 DarkWyrm
	Released under the MIT license
*/
#include "DPath.h"
#include <Path.h>

DPath::DPath(const char *string)
{
	SetTo(string);
}


DPath::DPath(const BString &string)
{
	SetTo(string);
}


DPath::DPath(const DPath &path)
{
	SetTo(path);
}


DPath::DPath(const entry_ref &ref)
{
	SetTo(ref);
}


DPath::DPath(void)
	:	fFileNamePos(-1),
		fExtensionPos(-1)
{
}


DPath::~DPath(void)
{
}


DPath &
DPath::operator =(const DPath &path)
{
	SetTo(path);
	return *this;
}


DPath &
DPath::operator =(const BString &string)
{
	SetTo(string.String());
	return *this;
}


DPath &
DPath::operator =(const char *string)
{
	SetTo(string);
	return *this;
}


void
DPath::SetTo(const char *string)
{
	fFullPath = string;
	
	if (fFullPath.FindLast("/.") == fFullPath.Length() - 2)
		fFullPath.RemoveLast("/.");
	
	if (!string)
	{
		fBaseName = string;
		fFolder = string;
		fExtensionPos = -1;
		fFileNamePos = -1;
	}
	else
	{
		fFileNamePos = fFullPath.FindLast("/") + 1;
		if (fFileNamePos > 0)
		{
			fBaseName = fFullPath.String() + fFileNamePos;
			fFolder = fFullPath;
			fFolder.Truncate(fFileNamePos - 1);
		}
		else
		{
			fFileNamePos = 0;
			fBaseName = fFullPath;
			fFolder = "";
		}
		
		fExtensionPos = fFullPath.FindLast(".") + 1;
		if (fExtensionPos < fFileNamePos)
			fExtensionPos = 0;
		
		// if the period is the first character, then we don't really have
		// an extension. .profile is an example of this. As a result, we look
		// for the period as the second character or later.
		if (fExtensionPos > 1)
			fBaseName.Truncate(fExtensionPos - fFileNamePos - 1);
	}
}


void
DPath::SetTo(const DPath &path)
{
	fFullPath = path.fFullPath;
	fBaseName = path.fBaseName;
	fFolder = path.fFolder;
	fFileNamePos = path.fFileNamePos;
	fExtensionPos = path.fExtensionPos;
}


void
DPath::SetTo(const BString &string)
{
	SetTo(string.String());
}


void
DPath::SetTo(const entry_ref &ref)
{
	SetTo(BPath(&ref).Path());
}


const char *
DPath::GetFullPath(void) const
{
	return (fFullPath.CountChars() > 0) ? fFullPath.String() : NULL;
}


const char *
DPath::GetFolder(void) const
{
	return (fFolder.CountChars() > 0) ? fFolder.String() : NULL;
}


const char *
DPath::GetFileName(void) const
{
	int32 count = fFullPath.CountChars();
	return (count > 0 && fFileNamePos != count) ? fFullPath.String() + fFileNamePos : NULL;
}


const char *
DPath::GetBaseName(void) const
{
	return (fBaseName.CountChars() > 0) ? fBaseName.String() : NULL;
}


const char *
DPath::GetExtension(void) const
{
	return (fExtensionPos > 0) ? fFullPath.String() + fExtensionPos : NULL;
}


DPath &
DPath::operator <<(const char *string)
{
	Append(string);
	return *this;
}


DPath &
DPath::operator <<(const BString &string)
{
	Append(string);
	return *this;
}


void
DPath::Append(const char *string)
{
	if (!string)
		return;
	
	int32 count = fFullPath.CountChars();
	bool base = false;
	
	if (count > 0 && fFullPath[count - 1] == '/')
		base = true;
	
	if (base)
	{
		if (string[0] == '/')
			fFullPath << (string + 1);
		else
			fFullPath << string;
	}
	else
	{
		if (string[0] == '/')
			fFullPath << string;
		else
			fFullPath << "/" << string;
	}
	
	BString s(fFullPath);
	SetTo(s.String());
}


void
DPath::Append(const BString &string)
{
	Append(string.String());
}


bool
DPath::IsEmpty(void) const
{
	return fFullPath.CountChars() < 1;
}

