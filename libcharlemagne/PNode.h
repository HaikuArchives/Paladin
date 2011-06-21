#ifndef PNODE_H
#define PNODE_H

#include "PObject.h"
#include <String.h>

class BNode;


/*
	PNode Object
	
	Properties:
		Locked
		Path (NOT read-only!)
		Status (read-only)
	
	Methods:
		GetAttrInfo
		GetAttrNames
		ReadAttr
		RemoveAttr
		RenameAttr
		Sync
		WriteAttr
*/

class PNode : public PObject
{
public:
							PNode(void);
							PNode(BMessage *msg);
							PNode(const char *name);
							PNode(const PNode &from);
							~PNode(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BNode *			GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

			BNode			*fBackend;
			BString			fPath;
			bool			fIsLocked;
};

#endif

