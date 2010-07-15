#ifndef OBJECTITEM_H
#define OBJECTITEM_H

#include <ListItem.h>

class PObject;

class ObjectItem : public BStringItem
{
public:
				ObjectItem(PObject *obj, const char *namebase = NULL);
	void		SetName(const char *namebase);
	PObject *	GetObject(void);

private:
	PObject		*fObject;
};

#endif
