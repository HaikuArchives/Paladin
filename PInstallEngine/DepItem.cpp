#include "DepItem.h"
#include <stdio.h>

DepItem::DepItem(void)
	:	fType(DEP_FILE)
{
}


const char *
DepItem::GetName(void) const
{
	return fName.String();
}


void
DepItem::SetName(const char *name)
{
	fName = name;
}


const char *
DepItem::GetFileName(void) const
{
	return fFileName.CountChars() > 0 ? fFileName.String() : fName.String();
}


void
DepItem::SetFileName(const char *name)
{
	fFileName = name;
}


PkgPath
DepItem::GetPath(void) const
{
	return fPath;
}


void
DepItem::SetPath(const char *path)
{
	fPath.SetTo(path);
}


void
DepItem::SetPath(const PkgPath &path)
{
	fPath = path;
}


void
DepItem::SetURL(const char *url)
{
	fURL = url;
}


const char *
DepItem::GetURL(void) const
{
	return fURL.String();
}


void
DepItem::SetType(const char *type)
{
	if (!type || strlen(type) < 1)
		return;
	
	BString str(type);
	if (str.ICompare("library") == 0 || str.ICompare("lib") == 0)
		fType = DEP_LIBRARY;
	else
		fType = DEP_FILE;
}


void
DepItem::SetType(int8 type)
{
	fType = type;
}


int8
DepItem::GetType(void) const
{
	return fType;
}


BString
DepItem::MakeInfo(void)
{
	BString out;
	out << "DEPENDENCY=" << GetName() << "\n";
	
	if (fFileName.CountChars() > 0)
		out << "\tFILENAME=" << fFileName << "\n";
	
	out << "\tTYPE=" << (fType == DEP_LIBRARY ? "library" : "file") << "\n";
	
	if (fType != DEP_LIBRARY)
		out << "\tPATH=" << fPath.Path() << "\n";
	if (fURL.CountChars() > 0)
		out << "\tDEPURL=" << fURL << "\n\n";
	return out;
}

void
DepItem::PrintToStream(int8 indent)
{
	BString tabstr;
	for (int8 i = 0; i < indent; i++)
		tabstr += "\t";
	
	BString out;
	out << tabstr << "Dependency: " << GetName() << "\n";
	tabstr << "\t";
	out << tabstr << "Filename: " << GetFileName() << "\n"
		<< tabstr << "Path: " << GetPath().Path() << "\n"
		<< tabstr << "Website: " << GetURL() << "\n"
		<< tabstr << "Type: " << (GetType() == DEP_LIBRARY ? "library" : "file") << "\n";
	printf(out.String());
}
