#include "PHandler.h"

PHandler::PHandler(void)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::PHandler(BMessage *msg)
	:	PObject(msg)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::PHandler(const char *name)
	:	PObject(name)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::PHandler(const PHandler &from)
	:	PObject(from)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::~PHandler(void)
{
}


BArchivable *
PHandler::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PHandler"))
		return new PHandler(data);

	return NULL;
}


PObject *
PHandler::Create(void)
{
	return new PHandler();
}


PObject *
PHandler::Duplicate(void) const
{
	return new PHandler(*this);
}


void
PHandler::SetMsgHandler(const int32 &constant, MethodFunction handler, void *ptr)
{
	fMsgHandlerMap[constant] = HandlerData(handler, ptr);
}


MethodFunction
PHandler::GetMsgHandler(const int32 &constant)
{
	MsgHandlerMap::iterator i = fMsgHandlerMap.find(constant);
	return (i == fMsgHandlerMap.end()) ? NULL : i->second.handler;
}


void
PHandler::RemoveMsgHandler(const int32 &constant)
{
	fMsgHandlerMap.erase(constant);
}


status_t
PHandler::SetHandlerCode(const int32 &constant, const char *code)
{
	MsgHandlerMap::iterator i = fMsgHandlerMap.find(constant);
	if (i == fMsgHandlerMap.end())
		return B_NAME_NOT_FOUND;
	
	i->second.code = code;
	return B_OK;
}


const char *
PHandler::GetHandlerCode(const int32 &constant)
{
	MsgHandlerMap::iterator i = fMsgHandlerMap.find(constant);
	return (i == fMsgHandlerMap.end()) ? NULL : i->second.code.String();
}


status_t
PHandler::RunMessageHandler(const int32 &constant, PArgs &args)
{
	MsgHandlerMap::iterator i = fMsgHandlerMap.find(constant);
	if (i == fMsgHandlerMap.end())
		return B_NAME_NOT_FOUND;
	
	PArgs out;
	return i->second.handler(this, &args, &out, NULL);
}


status_t
PHandler::SendMessage(BMessage *msg)
{
	// This message is meant to be implemented by child classes
	return B_OK;
}

