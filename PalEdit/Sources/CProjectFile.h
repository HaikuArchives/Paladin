/*	$Id: CProjectFile.h,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
	Copyright 2005 Oliver Tappe
	
	Distributed under the MIT License
*/

#ifndef CPROJECTFILE_H
#define CPROJECTFILE_H

#include <list>
#include <vector>

#include <Path.h>
#include <String.h>

#include "CDoc.h"

class BPositionIO;
class CProjectSerializer;

using std::list;

/*
 * CProjectItem
 *		represents a source-file in a project (displayed in a PProjectWindow).
 * 	It basically just stores the path of the parent and the leaf-name.
 */
class CProjectItem
{
public:
	CProjectItem();
	CProjectItem(const char* path, const char* leafName);
	virtual ~CProjectItem();
	//
	status_t SetTo(const char* path, const char* leafName);
	status_t SetTo(const char* path);
	//
	virtual bool CanBeAddedTo() const	{ return false; }
	virtual bool CanBeRemoved() const	{ return true; }
	//
	virtual bool IsDirty() const			{ return false; }
	virtual void SerializeTo( CProjectSerializer* serializer) const;
	//
	virtual bool ContainsFile(const entry_ref* fileRef) const
													{ return false; }
	virtual bool IsSubordinate() const	{ return true; }
	//
	const BString& ParentPath() const	{ return fParentPath; }
	const BString& LeafName() const		{ return fLeafName; }
	const BString& DisplayName() const	{ return fDisplayName; }
	void DisplayName(const BString& dn);

protected:
	BString fParentPath;
	BString fLeafName;
	BString fDisplayName;
};



/*
 * CProjectGroupItem
 *		represents a group inside a project; each group can have multiple
 *		items (sources). The group itself is typically the name of the jam 
 *		target that depends on the sources it contains
 */
class CProjectGroupItem : public CProjectItem
{
	typedef CProjectItem inherited;
public:
	CProjectGroupItem();
	CProjectGroupItem(const char* path, const char* ruleName, 
							const char* groupName = NULL);
	virtual ~CProjectGroupItem();
	//
	list<CProjectItem*>::const_iterator begin() const
													{ return fItems.begin(); }
	list<CProjectItem*>::const_iterator end() const
													{ return fItems.end(); }
	int32 AddItem(CProjectItem* item, bool sorted);
	void RemoveItem(CProjectItem* item);
	//
	virtual bool CanBeAddedTo() const	{ return true; }
	virtual bool CanBeRemoved() const	{ return false; }
	//
	virtual bool IsDirty() const			{ return fIsDirty; }
	virtual void SerializeTo( CProjectSerializer* serializer) const;
	//
	virtual bool ContainsFile(const entry_ref* fileRef) const;
	//
	const BString& GroupHeader() const	{ return fGroupHeader; }
	const BString& GroupFooter() const	{ return fGroupFooter; }
	const BString& GroupName() const		{ return fGroupName; }
	//
	void GroupHeader( const BString& gh)
													{ fGroupHeader = gh; }
	void GroupFooter( const BString& gf)
													{ fGroupFooter = gf; }

protected:

	list<CProjectItem*> fItems;
	BString fGroupHeader;
	BString fGroupFooter;
	BString fGroupName;
	mutable bool fIsDirty;
};



/*
 * CProjectFile
 *		abstracts a project, i.e. a file-hierarchy that has been parsed from a
 *		Makefile/Jamfile. Files can be added/removed and the hierarchy can be 
 *		written back to the Makefile/Jamfile.
 *		Additionally, the notion of a list of include-paths is supported (which
 *		is taken from the Makefile/Jamfile, too).
 */
class CProjectFile : public CProjectGroupItem, public CDoc
{
	typedef CProjectGroupItem inherited;
public:
	CProjectFile();
	virtual ~CProjectFile();
	
	virtual status_t Parse(const BString& contents) = 0;
	virtual bool HasBeenParsed() const = 0;
	virtual bool HaveProjectInfo() const = 0;
	//
	virtual bool CanBeAddedTo() const	{ return false; }
	virtual bool CanBeRemoved() const	{ return false; }
	//
	virtual bool IsDirty() const;
	virtual void SerializeTo(CProjectSerializer* serializer) const;
	status_t WriteToFile(BPositionIO* file, const BString& contents,
								const char* mimetype) const;
	//
	time_t ActivationTime() const			{ return fActivationTime; }
	void ActivationTime(time_t at)		{ fActivationTime = at; }
	//
	const BString& ErrorMsg() const		{ return fErrorMsg; }
	//
	void GetIncludePaths(vector<BString>& inclPathVect) const;
	virtual bool IsSubordinate() const	{ return false; }
protected:
	virtual void CollectSettings(BMessage& settingsMsg) const;
	virtual void ApplySettings(const BMessage& settingsMsg);
	virtual	void ReadAttr(BFile& file, BMessage& settingsMsg);
	virtual	void WriteAttr(BFile& file, const BMessage& settingsMsg);

	void _AddIncludePath(const BString& inclPath)
													{ fIncludePaths.push_back(inclPath); }
	BString fErrorMsg;
private:
	vector<BString> fIncludePaths;
	time_t fActivationTime;
};



/*
 * CProjectSerializer
 *		a struct that encapsulates the serialization of items.
 */
struct CProjectSerializer
{
	virtual ~CProjectSerializer()			{}
	virtual void SerializeItem(const CProjectItem* item) = 0;
	virtual void SerializeGroupItem(const CProjectGroupItem* item) = 0;
	virtual void SerializeFile(const CProjectFile* item) = 0;
};


#endif
