/*	$Id: CProjectJamFile.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTJAMFILE_H
#define CPROJECTJAMFILE_H

#include "CProjectFile.h"

class CProjectJamFile : public CProjectFile
{
	typedef CProjectFile inherited;
public:
	CProjectJamFile();
	CProjectJamFile(const char* path);
	virtual ~CProjectJamFile();
	
	virtual status_t Parse(const BString& contents);
	virtual bool HasBeenParsed() const	{ return fHasBeenParsed; }
	virtual bool HaveProjectInfo() const
													{ return fHaveProjectInfo; }

protected:
	virtual void GetText(BString &docText) const;
	virtual void SetText(const BString& docText);
private:
	void _ParseSources(const BString& contents);
	bool _ParseIncludeStmt(const char*& t);
	bool _ParseIncludeBlock(const char* start, const char* end);
	void _ParseIncludes(const BString& contents);
	const char* _ParseJamPath( const char* t, BString& jamPath);
	void _ParseSubJamfiles(const BString& contents);
	const char* _ParseSubJamfile(const char* start);
	CProjectGroupItem* _AddGroup(const char* start, const char* end, 
										  const char* buftop);
	
	BString fHeader;
	BString fFooter;
	BString fJamTopVarName;
	BString fJamTopPath;
	bool fHaveProjectInfo;
	bool fHasBeenParsed;
};

#endif
