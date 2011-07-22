#ifndef PHANDLER_H
#define PHANDLER_H

#include "PObject.h"

#include <map>

using std::map;

class HandlerData
{
public:
	HandlerData(void)
	{
		handler = NULL;
		extraData = NULL;
	}
	HandlerData(MethodFunction func, void *ptr = NULL)
	{
		handler = func;
		extraData = ptr;
	}
	
	MethodFunction	handler;
	void			*extraData;
	BString			code;
};

typedef std::map<int32, HandlerData> MsgHandlerMap;

class PHandler : public PObject
{
public:
							PHandler(void);
							PHandler(BMessage *msg);
							PHandler(const char *name);
							PHandler(const PHandler &from);
							~PHandler(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	void			SetMsgHandler(const int32 &constant, MethodFunction handler, void *ptr);
			MethodFunction	GetMsgHandler(const int32 &constant);
			void			RemoveMsgHandler(const int32 &constant);
			
	virtual	status_t		SetHandlerCode(const int32 &constant, const char *code);
			const char *	GetHandlerCode(const int32 &constant);
	
	virtual	status_t		RunMessageHandler(const int32 &constant, PArgList &args);
	
	virtual	status_t		SendMessage(BMessage *msg);
	
private:
	MsgHandlerMap			fMsgHandlerMap;
};

#endif
