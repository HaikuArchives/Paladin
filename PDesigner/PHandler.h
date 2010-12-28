#ifndef PHANDLER_H
#define PHANDLER_H

#include "PObject.h"

#include <map>

using std::map;

typedef std::map<int32, MethodFunction> MsgHandlerMap;

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
	
	virtual	void			SetMsgHandler(const int32 &constant, MethodFunction handler);
			MethodFunction	GetMsgHandler(const int32 &constant);
			void			RemoveMsgHandler(const int32 &constant);
	
	virtual	status_t		RunMessageHandler(const int32 &constant, PArgList &args);
			void			ConvertMsgToArgs(BMessage &in, PArgList &out);
			void			ConvertArgsToMsg(PArgList &in, BMessage &out);
	
	virtual	status_t		SendMessage(BMessage *msg);
	
private:
	MsgHandlerMap			fMsgHandlerMap;
};

#endif
