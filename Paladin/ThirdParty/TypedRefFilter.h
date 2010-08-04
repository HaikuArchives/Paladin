#ifndef TYPED_REF_FILTER
#define TYPED_REF_FILTER

#include <FilePanel.h>
#include <String.h>

/*
	This utility class is for filtering 
*/
class TypedRefFilter : public BRefFilter
{
public:
							TypedRefFilter(void);
							TypedRefFilter(const char *file_type,
											const uint32 &node_type = B_FILE_NODE |
																	B_DIRECTORY_NODE |
																	B_SYMLINK_NODE);
	virtual 				~TypedRefFilter(void);
	
			const char *	FileType(void) const;
			void			SetFileType(const char *type);
	
			uint32			NodeType(void) const;
			void			SetNodeType(const uint32 &node_type);
	
	// This is because of a source-incompatible change in Haiku's BRefFilter implementation
	// related to POSIX compatibility and the stat structure. *sigh*
#ifdef __HAIKU__
	virtual bool			Filter(const entry_ref *ref, BNode *node,
									struct stat_beos *st, const char *filetype);
#else
	virtual bool			Filter(const entry_ref *ref, BNode *node,
									struct stat *st, const char *filetype);
#endif
	
private:
			BString			fFileType;
			uint32			fNodeType;
};

#endif
