#include "TypedRefFilter.h"

#ifdef __HAIKU__
#include <compat/sys/stat.h>
#endif

TypedRefFilter::TypedRefFilter(void)
  : BRefFilter()
{
}


TypedRefFilter::TypedRefFilter(const char *file_type, const uint32 &node_type)
  : BRefFilter(),
  	fFileType(file_type),
  	fNodeType(node_type)
{
}


TypedRefFilter::~TypedRefFilter(void)
{
}

	
const char *
TypedRefFilter::FileType(void) const
{
	return fFileType.String();
}


void
TypedRefFilter::SetFileType(const char *type)
{
	fFileType = type;
}


uint32
TypedRefFilter::NodeType(void) const
{
	return fNodeType;
}


void
TypedRefFilter::SetNodeType(const uint32 &node_type)
{
	fNodeType = node_type;
}


#ifdef __HAIKU__
bool
TypedRefFilter::Filter(const entry_ref *ref, BNode *node, struct stat_beos *st,
						const char *filetype)
#else
bool
TypedRefFilter::Filter(const entry_ref *ref, BNode *node, struct stat *st,
						const char *filetype)
#endif
{
	if (B_DIRECTORY_NODE & NodeType())
		if (S_ISDIR(st->st_mode))
			return true;
	
	if (B_SYMLINK_NODE & NodeType())
	{
		if (S_ISLNK(st->st_mode))
		{
			// Check the type of the referred object;
			
			BEntry entry(ref,true);
			if (!entry.InitCheck())
				return true;
			
			BNode node(&entry);
			BString type;
			node.ReadAttrString("BEOS:TYPE",&type);
			if (type.CountChars() < 1)
				return false;
			
			return (type == fFileType);
		}
	}
	
	if (!(B_FILE_NODE & NodeType()))
		return false;
	
	// An empty file type means any file type
	if (fFileType.Length() < 1)
		return true;
	
	if (fFileType == filetype)
		return true;
	
	return false;
}
