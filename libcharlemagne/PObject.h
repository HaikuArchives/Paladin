#ifndef POBJECT_H
#define POBJECT_H

#include <Archivable.h>
#include <Message.h>
#include <String.h>

#include "CInterface.h"
#include "ObjectList.h"
#include "PArgs.h"
#include "PData.h"
#include "PMethod.h"
#include "PProperty.h"

class MethodData
{
public:
	MethodData(const char *n, uint32 f) { name = n; flags = f; }
	
	BString		name;
	uint32 		flags;
};


class EventData
{
public:
	EventData(const char *n, const char *d, PMethodInterface *pmi = NULL);
	
	BString				name,
						description;
	
	PMethodInterface	interface;
	MethodFunction		hook;
};


void InitObjectSystem(void);
void ShutdownObjectSystem(void);


class PObject : public PData
{
public:
							PObject(void);
							PObject(BMessage *msg);
							PObject(const char *name);
							PObject(const PObject &from);
			PObject &		operator=(const PObject &from);
			PProperty *		operator[](const char *name);
			PProperty *		operator[](const BString &name);
	virtual					~PObject(void);
	
	// Create() is needed for the object broker's use. Always uses a class' default constructor
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	
			uint64			GetID(void) const;
	
	virtual	status_t		RunMethod(const char *name, PArgList &in, PArgList &out);
			PMethod *		FindMethod(const char *name);
			PMethod *		MethodAt(const int32 &index) const;
			int32			CountMethods(void) const;
			
	virtual	status_t		RunInheritedMethod(const char *name, PArgList &in, PArgList &out);
			PMethod *		FindInheritedMethod(const char *name);
			PMethod *		InheritedMethodAt(const int32 &index) const;
			int32			CountInheritedMethods(void) const;
			
			BString			GetType(void) const;
			BString			GetFriendlyType(void) const;
			
			bool			UsesInterface(const char *name);
			bool			UsesInterface(const BString &name);
			BString			InterfaceAt(const int32 &index) const;
			int32			CountInterfaces(void) const;
			
			EventData *		EventAt(const int32 &index) const;
			int32			CountEvents(void) const;
			EventData *		FindEvent(const char *name);
			status_t		RunEvent(const char *name, PArgList &in, PArgList &out);
	virtual	status_t		RunEvent(EventData *data, PArgList &in, PArgList &out);
			status_t		ConnectEvent(const char *name, MethodFunction func);
			
	virtual	void			PrintToStream(void);
	
			void			ConvertMsgToArgs(BMessage &in, PArgList &out);
			void			ConvertArgsToMsg(PArgList &in, BMessage &out);
			
protected:
			void			AddInterface(const char *name);
			void			RemoveInterface(const char *name);
	
	virtual	status_t		AddMethod(PMethod *method);
	virtual	status_t		RemoveMethod(const char *name);
	virtual	status_t		ReplaceMethod(const char *old, PMethod *newMethod);
	
	virtual	status_t		AddInheritedMethod(PMethod *method);
	
	virtual	status_t		AddEvent(const char *name, const char *description,
									PMethodInterface *interface = NULL);
	virtual	status_t		RemoveEvent(const char *name);
	
	
	BString					fType;
	BString					fFriendlyType;
	
private:
	friend class PObjectBroker;
	uint64						fObjectID;
	BObjectList<PropertyData>	*fPropertyList;
	BObjectList<PMethod>		*fMethodList;
	BObjectList<PMethod>		*fInheritedList;
	BObjectList<BString>		*fInterfaceList;
	BObjectList<EventData>		*fEventList;
};

// Convenience functions
PObject *			MakeObject(const char *type);
PObject *			UnflattenObject(BMessage *msg);

#endif
