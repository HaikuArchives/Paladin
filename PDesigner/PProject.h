#ifndef PPROJECT_H
#define PPROJECT_H

#include "PObject.h"
#include "PProperty.h"

class PProject : public PObject
{
public:
							PProject(void);
							PProject(BMessage *msg);
							PProject(const char *name);
							PProject(const PProject &proj);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	static	BArchivable *	Instantiate(BMessage *data);
	
private:
			void			InitProperties(void);
};

#endif
