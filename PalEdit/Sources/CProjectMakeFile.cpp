/*	$Id: CProjectMakeFile.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

	Copyright 2005 Oliver Tappe

	Distributed under the MIT License
*/

/*
 * CProjectMakeFile implements the CProjectFile-interface for Makefiles.
 *
 * In a file with mimetype 'text/x-makefile', Pe searches for special
 * markup comments:
 *		#%{
 * 	#%}
 * and interpretes anything between those comments as a list of named
 * groups of source-files. Instead of opening such files as text, Pe
 * opens a project-window, containing all source-files found.
 * This project-window can then be used to easily open the sources, and
 * add/remove sources, too.
 *
 * Here's an example of a relevant Makefile fragment:
 *
 * 	#%{
 * 	SRCS = sourcefile1.cpp sourcefile2.cpp
 * 	RSRCS = resfile.rsrc
 * 	#%}
 *
 * A second markup format (from Eddie) is support as well:
 *
 * 	# @src->@
 * 	SRCS = sourcefile1.cpp sourcefile2.cpp
 * 	RSRCS = resfile.rsrc
 * 	# @<-src@
 *
 * Complete examples can be found in the Be-provided sample code, e.g:
 * 	/boot/optional/sample-code/network_kit/FtpClient/makefile
 *
 */

#include "pe.h"
#include "Utils.h"
#include "CProjectMakeFile.h"
#include "HPreferences.h"
#include "Prefs.h"
#include <cctype>
#include <Entry.h>

/*
 * CProjectMakeSerializer
 *		a struct that implements the serialization of Makefile-items.
 */
struct CProjectMakeSerializer : public CProjectSerializer
{
	CProjectMakeSerializer( BString& result, const char* startPath);
	virtual ~CProjectMakeSerializer();
	virtual void SerializeItem(const CProjectItem* item);
	virtual void SerializeGroupItem(const CProjectGroupItem* item);
	virtual void SerializeFile(const CProjectFile* item);
	BString& fResult;
	BPath fStartPath;
};

CProjectMakeSerializer::CProjectMakeSerializer( BString& result, const char* startPath)
	:	fResult(result)
	,	fStartPath(startPath)
{
}

CProjectMakeSerializer::~CProjectMakeSerializer()
{
}

void CProjectMakeSerializer::SerializeItem(const CProjectItem* item)
{
	char path[PATH_MAX];
	BPath itemPath( item->ParentPath().String(), item->LeafName().String());
	RelativePath(fStartPath, itemPath, path);
	char quot = (strchr(path, ' ') != NULL ? '"' : ' ');
	if (path[0]=='.' && path[1]=='/')
		fResult << "\t" << quot << path+2 << quot << " \\\n";
	else
		fResult << "\t" << quot << path << quot << " \\\n";
}

void CProjectMakeSerializer::SerializeGroupItem(const CProjectGroupItem* item)
{
	fResult << item->GroupHeader() << item->LeafName() << " = \\\n";
	list<CProjectItem*>::const_iterator iter;
	for( iter = item->begin(); iter != item->end(); ++iter)
		(*iter)->SerializeTo(this);
	fResult << "\n";
}

void CProjectMakeSerializer::SerializeFile(const CProjectFile* item)
{
	// each CProjectFile lives in a file of its own right, so we write
	// this item to its own file:
	if (item->HasBeenParsed())
		(const_cast<CProjectFile*>(item))->Save();
}



CProjectMakeFile::CProjectMakeFile()
	:	fHaveProjectInfo(false)
{
}

CProjectMakeFile::CProjectMakeFile(const char* path)
	:	fHaveProjectInfo(false)
{
	SetTo(path);
}

CProjectMakeFile::~CProjectMakeFile()
{
}

static const char *skip_to(const char *t, char c)
{
	while (*t && *t != c)
		t++;
	return t;
} // skip_to

static const char *skip_white(const char *t)
{
	while (*t)
	{
		if (*t == '\\' && t[1] == '\n')
			t += 2;
		if (*t == ' ' || *t == '\t')
			t++;
		else if (*t == '#') {
			t = skip_to(t + 1, '\n');
			if (*t == '\n')
				t++;
		} else
			break;
	}

	return t;
} // skip_white

static const char* skipback_over(const char* ptr, const char* start,
											const char* skipChars)
{
	if (!ptr)
		return ptr;
	do {
		ptr--;
	} while(ptr>=start && strchr(skipChars, *ptr));
	ptr++;
	return ptr;
}

static const char *next_path(const char *t, const char *& p, int& pl)
{
	if (*t == '"')
	{
		p = ++t;
		t = strchr(t, '"');
		if (t)
		{
			pl = t - p;
			t++;
		}
		else
			p = NULL;
	}
	else if (*t == '\'')
	{
		p = ++t;
		t = strchr(t, '\'');
		if (t)
		{
			pl = t - p;
			t++;
		}
		else
			p = NULL;
	}
	else if (*t)
	{
		p = t;
		while (*t && ! isspace(*t))
			t++;
		pl = t - p;
	}
	else
		p = NULL;

	return t;
} // next_path

static const char* groupStart;

const char *CProjectMakeFile::_AddGroup(const char *t)
{
	if (!groupStart)
		groupStart = t;
	t = skip_white(t);

	if (isalnum(*t))
	{
		const char *n = t;

		BString groupHeader(groupStart, n-groupStart);

		while (isalnum(*t) || *t == '_')
			t++;

		int nl = t - n;

		t = skip_white(t);

		if (*t == ':')
			t++;
		if (*t != '=')
			return skip_to(t, '\n');

		t++;

		BString s(n, nl);

		CProjectGroupItem *group
			= new CProjectGroupItem(fParentPath.String(), s.String());
		group->GroupHeader(groupHeader);
		fItems.push_back(group);

		groupStart = NULL;

		while (true)
		{
			const char *p;
			int pl;

			t = skip_white(t);

			t = next_path(t, p, pl);
			if (p == NULL)
				break;

			s.SetTo(p, pl);

			entry_ref ref;
			BEntry e;

			if (s.Length())
				group->AddItem(new CProjectItem(fParentPath.String(), s.String()),
									gPrefs->GetPrefInt(prf_I_SortProjectFiles, 1) != 0);

			t = skip_white(t);

			if (*t == '\n')
				break;
		}
	}

	if (*t)
	{
		t = skip_to(t, '\n');
		if (t) t++;
	}

	return t;
}

#define RET_FAIL(c,s) \
	do { \
		fErrorMsg \
			<< "No project-info could be extracted from the jamfile\n\n" \
			<< "Error: " << s; \
		return c; \
} while(0)

status_t CProjectMakeFile::Parse(const BString& contents)
{
	const char* t = contents.String();
	int32 size = contents.Length();
	const char *s, *e;

	s = strstr(t, "%{");
	if (s) {
		s = skipback_over(s, t, " \t#");
		// Pe format
		e = s ? strstr(s, "%}") : s;
		e = skipback_over(e, s, " \t#");
	} else {
		// Eddie format
		s = strstr(t, "@src->@");
		s = skipback_over(s, t, " \t#");
		e = s ? strstr(s, "@<-src@") : s;
		e = skipback_over(e, t, " \t#");
	}

	if (s < e)
	{
		s = skip_to(s + 3, '\n') + 1;

		int l = s - t;
		fHeader.SetTo(t, l);

		l = t + size - e;
		fFooter.SetTo(e, l);

		while (s < e)
			s = _AddGroup(s);

		if (groupStart && s>groupStart)
			fFooter.Prepend(groupStart, s-groupStart);

	}
	if (fItems.empty()) {
		fErrorMsg
			<< "No project-items could be found in the makefile\n\n"
			<< "Maybe you have forgotten to add\n"
			<< "   # @src->@ and # @src<-@\n"
			<< "or\n"
			<< "   # %{ and # %}\n"
			<< "comments?";
	} else
		fHaveProjectInfo = true;

	return B_OK;
}

void CProjectMakeFile::GetText(BString& docText) const
{
	docText.SetTo(fHeader);

	BPath path( fParentPath.String(), fLeafName.String());
	CProjectMakeSerializer serializer(docText, path.Path());

	list<CProjectItem*>::const_iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter)
		(*iter)->SerializeTo(&serializer);

	docText << fFooter;
}

void CProjectMakeFile::SetText(const BString& docText)
{
	FailOSErrMsg(Parse(docText), ErrorMsg().String());
}
