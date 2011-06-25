#ifndef PSTATABLE_H
#define PSTATABLE_H

#include "PObject.h"
#include <String.h>

class BNode;


/*
	PStatable Object
	
	Properties:
		AccessTime
		CreationTime
		Group
		IsDirectory (read-only)
		IsFile (read-only)
		IsSymlink (read-only)
		ModificationTime
		Owner
		Permissions
		Size (read-only)
	
	Methods:
		GetNodeRef
		GetStat
*/

class PStatable : public PObject
{
public:
							PStatable(void);
							PStatable(BMessage *msg);
							PStatable(const char *name);
							PStatable(const PStatable &from);
							~PStatable(void);
							
	static	BArchivable *	Instantiate(BMessage *data);
							
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
							
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	BStatable *				GetBackend(void) const;
	
protected:
	virtual	void			InitBackend(void);
	
private:
			void			InitProperties(void);
			void			InitMethods(void);
			
			BStatable		*fBackend;
};

#endif
