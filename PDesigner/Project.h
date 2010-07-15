#ifndef PROJECT_H
#define PROJECT_H

#include <String.h>

#include "ObjectList.h"
#include "PObject.h"
#include "PObjectBroker.h"

class Project
{
public:
							Project(void);
							~Project(void);
							
				void		SetName(const char *name);
				const char *GetName(void) const;
				
				void		AddObject(PObject *obj, const int32 &index = -1);
				void		RemoveObject(PObject *obj);
				PObject *	RemoveObject(const int32 &index);
				int32		CountObjects(void) const;
				PObject *	ObjectAt(const int32 &index);
				
private:
				BString		*fName;
	BObjectList<PObject>	*fList;
};

#endif
