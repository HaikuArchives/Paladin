/*	$Id: CProjectMakeFile.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTMAKEFILE_H
#define CPROJECTMAKEFILE_H

#include "CProjectFile.h"

class CProjectMakeFile : public CProjectFile
{
	typedef CProjectFile inherited;
public:
	CProjectMakeFile();
	CProjectMakeFile(const char* path);
	virtual ~CProjectMakeFile();
	
	virtual status_t Parse(const BString& contents);
	virtual bool HasBeenParsed() const	{ return true; }
	virtual bool HaveProjectInfo() const
													{ return fHaveProjectInfo; }

protected:
	virtual void GetText(BString &docText) const;
	virtual void SetText(const BString& docText);
private:
	const char* _AddGroup(const char* name);
	
	BString fHeader;
	BString fFooter;
	bool fHaveProjectInfo;
};

#endif
