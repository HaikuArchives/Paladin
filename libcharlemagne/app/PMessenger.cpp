#include "PMessenger.h"

#include <Messenger.h>
#include <stdio.h>

#include "MiscProperties.h"
#include "PArgs.h"
#include "PObjectBroker.h"
#include "PMethod.h"

int32_t PMessengerLockTarget(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PMessengerSendMessage(void *pobject, void *in, void *out, void *ptr = NULL);


PMessenger::PMessenger(void)
{
	fType = "PMessenger";
	fFriendlyType = "Messenger";
	AddInterface("PMessenger");
	
	// This constructor starts with an empty PMessenger, so we need to
	// initialize it with some properties
	InitProperties();
}


PMessenger::PMessenger(BMessage *msg)
	:	PObject(msg)
{
	fType = "PMessenger";
	fFriendlyType = "Messenger";
	AddInterface("PMessenger");
}


PMessenger::PMessenger(const char *name)
	:	PObject(name)
{
	fType = "PMessenger";
	fFriendlyType = "Messenger";
	AddInterface("PMessenger");
}


PMessenger::PMessenger(const PMessenger &from)
	:	PObject(from)
{
	fType = "PMessenger";
	fFriendlyType = "Messenger";
	AddInterface("PMessenger");
}


PMessenger::~PMessenger(void)
{
}


BArchivable *
PMessenger::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PMessenger"))
		return new PMessenger(data);
	
	return NULL;
}


PObject *
PMessenger::Create(void)
{
	return new PMessenger();
}


PObject *
PMessenger::Duplicate(void) const
{
	return new PMessenger(*this);
}

status_t
PMessenger::GetProperty(const char *name, PValue *value, const int32 &index) const
{
/*
	Properties
		IsValid
		TargetLocal
		Team
*/
	
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	// The Target property is handled by the parent version because it's
	// just the ID of the appropriate PHandler and, as such, is just a
	// standard-issue property which doesn't need a backend method call.
	
	if (str.ICompare("IsValid") == 0)
		((BoolProperty*)prop)->SetValue(fMessenger.IsValid());
	else if (str.ICompare("TargetLocal") == 0)
		((BoolProperty*)prop)->SetValue(fMessenger.IsTargetLocal());
	else if (str.ICompare("Team") == 0)
		((IntProperty*)prop)->SetValue(fMessenger.Team());
	else
		return PObject::GetProperty(name,value,index);
	
	return B_OK;
}


void
PMessenger::InitProperties(void)
{
	AddProperty(new BoolProperty("IsValid",true,
								"Set to true if the messenger's target is valid."),
								PROPERTY_HIDE_IN_EDITOR | PROPERTY_READ_ONLY);
	AddProperty(new StringProperty("Description","A message-sending object."));
	AddProperty(new IntProperty("TargetLocal", true, "True if the messenger's target is in the same application"),
								PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("Team",-1,"ID for this instance of the app."),
								PROPERTY_READ_ONLY);
}


void
PMessenger::InitMethods(void)
{
/*
	Methods
		LockTarget
		SendMessage
*/
	PMethodInterface pmi;
	pmi.AddReturnValue("success", B_BOOL_TYPE, "True if the target could be locked.");
	AddMethod(new PMethod("LockTarget", PMessengerLockTarget, &pmi));
	pmi.MakeEmpty();
	
	
}


int32_t
PMessengerLockTarget(void *pobject, void *in, void *out, void *ptr)
{
}


int32_t
PMessengerSendMessage(void *pobject, void *in, void *out, void *ptr)
{
	// SendMessage(command, BMessage reply, deliveryTimeout, replyTimeout)
	// SendMessage(
}

