#ifndef POBJECTBROKER_H
#define POBJECTBROKER_H

#include <Looper.h>
#include <String.h>

#include "ObjectList.h"
#include "PProperty.h"
#include "PObject.h"

typedef PObject *	(*MakeObjectFunc)(void);

void InitObjectSystem(void);
void ShutdownObjectSystem(void);

enum
{
	POBJECT_BROKER_DELETE_OBJECT = 'pdlo'
};

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


class PObjectBroker : public BLooper
{
public:
						PObjectBroker(void);
						~PObjectBroker(void);
	
	PObject *			MakeObject(const char *type, BMessage *msg = NULL);
	int32				CountTypes(void) const;
	BString				TypeAt(const int32 &index) const;
	BString				FriendlyTypeAt(const int32 &index) const;
	
	PObject *			FindObject(const uint64 &id);
	
	static	PObjectBroker *	GetBrokerInstance(void);
	static	void		RegisterObject(PObject *obj);
			void		UnregisterObject(PObject *obj);
	
			void		MessageReceived(BMessage *msg);
	
private:
	
	BObjectList<PObject>		*fObjectList;
	BObjectList<PObjectInfo>	*fObjInfoList;
	
	PObjectInfo *		FindObjectInfo(const char *type);
	
	bool				fQuitting;
	PObject				*pApp;
};

#define BROKER PObjectBroker::GetBrokerInstance()

#endif
