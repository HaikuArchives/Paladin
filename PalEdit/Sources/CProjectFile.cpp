/*	$Id: CProjectFile.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

/*
 * CProjectFile is an abstraction of Projects, i.e. a set of
 * named file-groups, each of which contains a set of files.
 * This abstraction is implemented to give support for
 * different kinds of "real world" project-files (i.e. Makefiles
 * and Jamfiles).
 * In other words: Pe supports parsing a Makefile/Jamfile and displaying
 * the relevant parts of it in a GUI (see PProjectWindow.cpp).
 * You can add/remove files, too (which will change the contents of
 * the Makefile/Jamfile accordingly).
 *
 * Concrete implementations of the CProjectFile interface live in 
 * CProjectMakeFile.* and CProjectJamFile.*.
 */


#include "pe.h"
#include "Utils.h"

#include <Autolock.h>

#include "CProjectFile.h"
#include "CProjectRoster.h"

/*
 * CProjectItem()
 */
CProjectItem::CProjectItem()
{
}

/*
 * 
 */
CProjectItem::CProjectItem(const char* path, const char* leafName)
{
	SetTo(path, leafName);
}

/*
 * 
 */
CProjectItem::~CProjectItem()
{
}

/*
 * 
 */
status_t CProjectItem::SetTo(const char* path, const char* leafName)
{
	fParentPath.SetTo(path);
	fLeafName.SetTo(leafName);
	DisplayName(leafName);
	return B_OK;
}

/*
 * 
 */
status_t CProjectItem::SetTo(const char* path)
{
	BString fullPath(path);
	if (!fullPath.Length())
		return B_BAD_VALUE;
	int32 slashPos = fullPath.FindLast('/');
	if (slashPos < 0)
		return SetTo("", fullPath.String());
	BString parentPath(fullPath.String(), slashPos);
	return SetTo(parentPath.String(), fullPath.String()+slashPos+1);
}

void CProjectItem::DisplayName(const BString& dn)
{
	fDisplayName = dn;
	fDisplayName.RemoveSet("\\");
		// remove escaping characters for improved readability
}

/*
 * 
 */
void CProjectItem::SerializeTo( CProjectSerializer* serializer) const
{
	serializer->SerializeItem(this); 
}



/*
 * 
 */
CProjectGroupItem::CProjectGroupItem()
{
}

/*
 * 
 */
CProjectGroupItem::CProjectGroupItem(const char* path, const char* leafName,
												 const char* groupName)
	:	inherited(path, leafName)
	,	fGroupName(groupName)
	,	fIsDirty(false)
{
	if (fGroupName.Length())
		// build display-name for a group that has a name specified:
		DisplayName(fDisplayName << ' ' << fGroupName);
}

/*
 * 
 */
CProjectGroupItem::~CProjectGroupItem()
{
	list<CProjectItem*>::iterator subiter;
	for( subiter = fItems.begin(); 
		  subiter != fItems.end(); ++subiter)
		delete *subiter;
}

/*
 * 
 */
bool CProjectGroupItem::ContainsFile(const entry_ref* fileRef) const
{
	CProjectItem* child;
	BString fullPath;
	entry_ref eref;
	status_t res;
	list<CProjectItem*>::const_iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter) {
		child = *iter;
		fullPath.Truncate(0);
		fullPath << child->ParentPath() << '/' << child->LeafName();
		res = get_ref_for_path(fullPath.String(), &eref);
		if (res == B_OK && eref == *fileRef)
			return true;
		if (child->IsSubordinate()) {
			// do not descend into other project-files (as these will be
			// scanned separately):
			return child->ContainsFile(fileRef);
		}
	}
	return false;
}

/*
 * 
 */
int32 CProjectGroupItem::AddItem(CProjectItem* item, bool sorted)
{
	int32 pos = 0;
	fIsDirty = true;
	if (sorted) {
		list<CProjectItem*>::iterator iter;
		for( iter = fItems.begin(); iter != fItems.end(); ++iter, ++pos) {
			if (item->LeafName().ICompare((*iter)->LeafName()) < 0) {
				fItems.insert(iter, item);
				return pos;
			}
		}
	}
	pos = fItems.size();
	fItems.push_back(item);
	return pos;
}

/*
 * 
 */
void CProjectGroupItem::RemoveItem(CProjectItem* item)
{
	list<CProjectItem*>::iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ) {
		CProjectItem* child = *iter;
		CProjectGroupItem* groupChild = dynamic_cast<CProjectGroupItem*>(child);
		if (child == item) {
			++iter;
			fItems.remove(child);
			delete child;
		} else {
			++iter;
			if (groupChild)
				groupChild->RemoveItem(item);
		}
	}
	fIsDirty = true;
}

/*
 * 
 */
void CProjectGroupItem::SerializeTo( CProjectSerializer* serializer) const
{
	serializer->SerializeGroupItem(this);
	fIsDirty = false;
}



/*
 * 
 */
CProjectFile::CProjectFile()
	: CDoc(NULL, NULL, NULL)
	, fActivationTime(0)
{
	ProjectRoster->AddProject(this);
}

/*
 * 
 */
CProjectFile::~CProjectFile()
{
	ProjectRoster->RemoveProject(this);
}

/*
 * 
 */
void CProjectFile::GetIncludePaths(vector<BString>& inclPathVect) const
{
	inclPathVect.insert(inclPathVect.end(), fIncludePaths.begin(), 
							  fIncludePaths.end());
}

/*
 * 
 */
bool CProjectFile::IsDirty() const
{
	list<CProjectItem*>::const_iterator iter;
	for( iter = fItems.begin(); iter != fItems.end(); ++iter) {
		if ((*iter)->IsDirty())
			return true;
	}
	return false;
}

/*
 * 
 */
void CProjectFile::SerializeTo( CProjectSerializer* serializer) const
{
	serializer->SerializeFile(this); 
}

/*status_t 
CProjectFile::WriteToFile(BPositionIO* file, const BString& contents,
								  const char* mimetype) const
{
	BFile prjFile;
	if (!file) {
		BPath path( fParentPath.String(), fLeafName.String());
		status_t res 
			= prjFile.SetTo(path.Path(), 
								 B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		if (res != B_OK)
			return res;

		BNodeInfo nodeInfo(&prjFile);
		nodeInfo.SetType(mimetype);
		file = &prjFile;
	}
	ssize_t sz = file->Write(contents.String(), contents.Length());
	if (sz != contents.Length())
		return sz < 0 ? sz : B_IO_ERROR;
	else
		return B_OK;
}
*/

void CProjectFile::CollectSettings(BMessage& settingsMsg) const
{
}

void CProjectFile::ApplySettings(const BMessage& settingsMsg)
{
}

void CProjectFile::ReadAttr(BFile& file, BMessage& settingsMsg)
{
}

void CProjectFile::WriteAttr(BFile& file, const BMessage& settingsMsg)
{
}
