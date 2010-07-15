#ifndef POBJECTBROKER_H
#define POBJECTBROKER_H

#include <String.h>

#include "ObjectList.h"
#include "PProperty.h"
#include "PObject.h"

typedef BArchivable * (*MakeFromArchiveFunc)(BMessage *msg);
typedef PObject *	(*MakeObjectFunc)(void);
typedef PProperty *	(*MakePropertyFunc)(void);

void InitObjectSystem(void);
void ShutdownObjectSystem(void);

class PObjectInfo
{
public:
	PObjectInfo(const char *typestr, const char *frtypestr, MakeFromArchiveFunc objfunc,
				MakeObjectFunc func)
	{	
		type = typestr;
		friendlytype = frtypestr;
		arcfunc = objfunc;
		createfunc = func; 
	}
	
	MakeFromArchiveFunc arcfunc;
	MakeObjectFunc createfunc;
	BString type;
	BString friendlytype;
};

class PPropertyInfo
{
public:
	PPropertyInfo(const char *typestr, MakeFromArchiveFunc objfunc,
				MakePropertyFunc func) { type = typestr; arcfunc = objfunc; createfunc = func; }
	
	MakeFromArchiveFunc arcfunc;
	MakePropertyFunc createfunc;
	BString type;
};

class PObjectBroker
{
public:
						PObjectBroker(void);
						~PObjectBroker(void);
	
	PObject *			MakeObject(const char *type, BMessage *msg = NULL);
	int32				CountTypes(void) const;
	BString				TypeAt(const int32 &index) const;
	BString				FriendlyTypeAt(const int32 &index) const;
	
	PObject *			FindObject(const uint64 &id);
	
	PProperty *			MakeProperty(const char *type, BMessage *msg = NULL) const;
	int32				CountProperties(void) const;
	BString				PropertyAt(const int32 &index) const;
	
	static	PObjectBroker *	GetBrokerInstance(void);
	static	void		RegisterObject(PObject *obj);
			void		UnregisterObject(PObject *obj);
	
private:
	
	BObjectList<PObject>		*fObjectList;
	BObjectList<PObjectInfo>	*fObjInfoList;
	BObjectList<PPropertyInfo>	*fPropertyList;
	
	PObjectInfo *		FindObjectInfo(const char *type);
	PPropertyInfo *		FindProperty(const char *type);
	
	bool						fQuitting;
};

#define BROKER PObjectBroker::GetBrokerInstance()

#endif
