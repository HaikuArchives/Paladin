#ifndef DNODE_H
#define DNODE_H

#include <Directory.h>
#include <Entry.h>
#include <Node.h>

class DNode : public BNode
{
public:
					DNode(void);
					DNode(const entry_ref *ref);
					DNode(const BEntry *entry);
					DNode(const char *path);
					DNode(const BDirectory *dir, const char *path);
					DNode(const DNode &node);

		status_t	Set(const entry_ref *ref);
		status_t	Set(const BEntry *entry);
		status_t	Set(const char *path);
		status_t	Set(const BDirectory *dir, const char *path);
		
		ssize_t		WriteAttrString(const char *attr, const char *string);
		ssize_t		WriteAttrInt32(const char *attr, const int32 &data);
		ssize_t		ReadAttrInt32(const char *attr, int32 *data) const;
		
		ssize_t		WriteAttrInt64(const char *attr, const int64 &data);
		ssize_t		ReadAttrInt64(const char *attr, int64 *data) const;
		
		ssize_t		WriteAttrBool(const char *attr, const bool &data);
		ssize_t		ReadAttrBool(const char *attr, bool *data) const;
		
		entry_ref	GetRef(void) const;
		bool		Exists(void) const;
private:
		entry_ref	fRef;
};

#endif
