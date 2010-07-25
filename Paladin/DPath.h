/*
	DPath:	A class for handling the various parts of a string-based file path
	Copyright 2008 DarkWyrm
	Released under the MIT license
*/
#ifndef DPATH_H
#define	DPATH_H

#include <String.h>
#include <Entry.h>

class DPath
{
public:
							DPath(const char *string);
							DPath(const BString &string);
							DPath(const DPath &path);
							DPath(const entry_ref &ref);
							DPath(void);
	virtual					~DPath(void);
	
			DPath &			operator =(const DPath &path);
			DPath &			operator =(const BString &string);
			DPath &			operator =(const char *string);
			void			SetTo(const char *string);
			void			SetTo(const DPath &path);
			void			SetTo(const BString &string);
			void			SetTo(const entry_ref &ref);
	
			const char *	GetFullPath(void) const;
			const char *	GetFolder(void) const;
			const char *	GetFileName(void) const;
			const char *	GetBaseName(void) const;
			const char *	GetExtension(void) const;
			entry_ref		GetRef(void) const;
			
			DPath &			operator <<(const char *string);
			DPath &			operator <<(const BString &string);
			void			Append(const char *string);
			void			Append(const BString &string);
			bool			IsEmpty(void) const;
private:
	BString	fFullPath,
			fBaseName,
			fFolder;
			
	int32	fFileNamePos,
			fExtensionPos;
};

#endif
