/*	$Id: CProjectRoster.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef _CProjectRoster_h_
#define _CProjectRoster_h_

#include <vector>

#include <Locker.h>
#include <String.h>

#include "CProjectFile.h"

/*
 * CProjectRoster
 *		allows easy retrieval of currently open projects
 */
class CProjectRoster
{
public:
	CProjectRoster();
	//
	void AddProject(CProjectFile* pf);
	void RemoveProject(CProjectFile* pf);
	//
	bool IsProjectType(const char* mimetype) const;
	CProjectFile* ParseProjectFile(const entry_ref* eref, const char* mt,
											 const BString& contents);
	//
	bool GetIncludePathsForFile(const entry_ref* fileRef, 
										 vector<BString>& inclPathVect) const;
	bool GetAllIncludePaths(vector<BString>& inclPathVect);

private:
	list<CProjectFile*> fProjects;
	mutable BLocker fLocker;
};

extern CProjectRoster* ProjectRoster;


#endif
