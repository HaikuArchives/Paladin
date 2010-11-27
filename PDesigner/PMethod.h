#ifndef PMETHOD_H
#define PMETHOD_H

#include <Message.h>
#include <String.h>

class PObject;

typedef status_t (*MethodFunction)(PObject *object, BMessage &in, BMessage &out);

status_t	NullPMethod(PObject *object, BMessage &in, BMessage &out);

enum
{
	METHOD_SHOW_IN_EDITOR		= 0x00000001
};

class PMethod
{
public:
							PMethod(const char *name = NULL,
									MethodFunction func = NullPMethod,
									const int32 &flags = 0);
	virtual					~PMethod(void);
			PMethod &		operator=(const PMethod &from);
			
	virtual	void			SetName(const char *name);
	virtual void			SetName(const BString &name);
			BString			GetName(void) const;
	
	virtual	void			SetFunction(MethodFunction func);
			MethodFunction	GetFunction(void) const;
	
	virtual	status_t		Run(PObject *object, BMessage &in, BMessage &out);
	
private:
			BString			fName;
			MethodFunction	fFunction;
			uint32			fFlags;
};


#endif
