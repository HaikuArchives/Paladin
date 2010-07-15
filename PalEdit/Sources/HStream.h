/*	$Id: HStream.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#ifndef HSTREAM_H
#define HSTREAM_H

#include "HLibHekkel.h"

#include "HError.h"

template <class S>
class HStream
{
	typedef S stream_type;

  public:
	HStream(stream_type& s)
		: fStream(s) {}

	template <class D>
	HStream& operator>> (D& d)
	{
		fStream.Read(&d, sizeof(D));
		return *this;
	}
	
	HStream& operator>> (char* s)
	{
		unsigned char t;
		fStream.Read(&t, 1);
		fStream.Read(s, t);
		s[t] = 0;
		return *this;
	}
	
	template <class D>
	HStream& operator<< (const D& d)
	{
		fStream.Write(&d, sizeof(D));
		return *this;
	}
	
	HStream& operator<< (const char* s)
	{
		unsigned char t = strlen(s);
		fStream.Write(&t, 1);
		fStream.Write(s, t);
		return *this;
	}
	
	void seekp (off_t position, uint32 seek_mode = SEEK_SET)
	{
		fStream.Seek(position, seek_mode);
	}
	
	off_t tellp ()
	{
		return fStream.Position();
	}
	
	off_t size ()
	{
		off_t size, pos;
		pos = fStream.Position();
		size = fStream.Seek(0, SEEK_END);
		fStream.Seek(pos, SEEK_SET);
		return size;
	}
	
  private:
	stream_type& fStream;
};

IMPEXP_LIBHEKKEL void ReadCString(BPositionIO& stream, int maxLen, char *s);

template<class D>
inline IMPEXP_LIBHEKKEL BPositionIO& operator>>(BPositionIO& s, D& d)
{
	if (s.Read(&d, sizeof(D)) != sizeof(D))
		throw HErr("Error reading");
	return s;
} /* operator>> */

inline IMPEXP_LIBHEKKEL BPositionIO& operator>>(BPositionIO& stream, char *string)
{
	ReadCString(stream, 255, string);
	return stream;
} /* operator>> */

template<class D>
inline IMPEXP_LIBHEKKEL BPositionIO& operator<<(BPositionIO& s, const D& d)
{
	if (s.Write(&d, sizeof(D)) != sizeof(D))
		throw HErr("Error writing");
	return s;
} /* operator<< */

inline IMPEXP_LIBHEKKEL 
BPositionIO& operator<<(BPositionIO& stream, const char *string)
{
	int sl = strlen(string) + 1;
	if (stream.Write(string, sl) != sl)
		throw HErr("Error writing");
	return stream;
} /* operator<< */

#endif // HSTREAM_H
