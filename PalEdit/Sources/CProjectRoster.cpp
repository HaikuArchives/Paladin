/*	$Id: CProjectRoster.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#include "pe.h"
#include "Utils.h"

#include <Autolock.h>

#include "CProjectRoster.h"
#include "CProjectJamFile.h"
#include "CProjectMakeFile.h"

/*
 * 
 */
static CProjectRoster gProjectRoster;
CProjectRoster* ProjectRoster = &gProjectRoster;

CProjectRoster::CProjectRoster()
	:	fLocker("ProjectRosterLock")
{
}

/*
 * 
 */
void CProjectRoster::AddProject(CProjectFile* pf)
{
	BAutolock lock(&fLocker);
	fProjects.push_back(pf); 
}

/*
 * 
 */
void CProjectRoster::RemoveProject(CProjectFile* pf)
{
	BAutolock lock(&fLocker);
	fProjects.remove(pf); 
}

/*
 * 
 */
bool CProjectRoster::IsProjectType(const char* mimetype) const
{
	return (!strcmp(mimetype, "text/x-makefile")
			  || !strcmp(mimetype, "text/x-jamfile"));
}

/*
 * 
 */
CProjectFile* CProjectRoster::ParseProjectFile(const entry_ref* eref, 
															  const char* mt,
															  const BString& contents)
{
	BPath path;
	if (BEntry(eref, true).GetPath(&path) != B_OK)
		return NULL;
	CProjectFile* prjFile = NULL;
	if (!strcmp(mt,"text/x-makefile"))
		prjFile = new CProjectMakeFile(path.Path());
	else if (!strcmp(mt,"text/x-jamfile"))
		prjFile = new CProjectJamFile(path.Path());
	if (prjFile)
		prjFile->Parse(contents);
	return prjFile;
}

/* GetIncludePathsForFile()
 * 	tries to determine the project that contains the given file and,
 *		if such a project is found, passes back this projects include-paths.
 */
bool CProjectRoster::GetIncludePathsForFile(const entry_ref* fileRef, 
														  vector<BString>& inclPathVect) const
{
	inclPathVect.clear();
	BAutolock lock(&fLocker);
	list<CProjectFile*>::const_iterator iter;
	for( iter=fProjects.begin(); iter != fProjects.end(); ++iter) {
		if ((*iter)->ContainsFile(fileRef)) {
			(*iter)->GetIncludePaths(inclPathVect);
			return true;
		}
	}
	return false;
}

/* GetAllIncludePaths()
 * 	returns the include-paths of all projects, sorted in descending
 *		order of the project-windows last activation time (i.e. projects
 *		have been used recently will be searched first).
 */
struct ProjectActivationTimeSorter {
	bool operator() (const CProjectFile* pfl, const CProjectFile* pfr) const {
		return pfl->ActivationTime() >= pfr->ActivationTime();
	}
};
bool CProjectRoster::GetAllIncludePaths(vector<BString>& inclPathVect)
{
	inclPathVect.clear();
	BAutolock lock(&fLocker);
	ProjectActivationTimeSorter compFunc;
	fProjects.sort(compFunc);
	list<CProjectFile*>::const_iterator iter;
	for( iter=fProjects.begin(); iter != fProjects.end(); ++iter) {
		(*iter)->GetIncludePaths(inclPathVect);
	}
	return true;
}
