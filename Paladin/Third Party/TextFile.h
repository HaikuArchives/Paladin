/*
	TextFile.h: Buffered line-based access to text files
	Written by DarkWyrm, Copyright 2007
	Released under the MIT license.
*/
#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <File.h>

class TextFile : public BFile
{
public:
					TextFile(const char *path, const uint32 &openmode);
					TextFile(const entry_ref &ref, const uint32 &openmode);
					~TextFile(void);
	const char *	ReadLine(void);
	
private:
	void			InitObject(void);
	
	char			*fBuffer;
	off_t			fBufferSize;
	char			*fReadBuffer;
	int32			fReadBufferSize;
};

#endif
