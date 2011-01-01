#ifndef PMETHOD_H
#define PMETHOD_H

#include <Message.h>
#include <String.h>

#include "CInterface.h"
#include "ObjectList.h"
#include "PArgs.h"

class PObject;

enum
{
	METHOD_SHOW_IN_EDITOR		= 0x00000001
};


class PMethodInterfaceData
{
public:
							PMethodInterfaceData(void)
							{
								type = PARG_END;
							}
							PMethodInterfaceData(PMethodInterfaceData &from)
							{
								name = from.name;
								type = from.type;
								description = from.description;
							}
							PMethodInterfaceData(const char *inName,
												const PArgType &inType,
												const char *inDescription)
							{
								name = inName;
								type = inType;
								description = inDescription;
							}
							
			BString			name;
			PArgType		type;
			BString			description;
};


class PMethodInterface
{
public:
							PMethodInterface(void);
							PMethodInterface(PMethodInterface &from);
	virtual					~PMethodInterface(void);
	
			PMethodInterface &	operator=(const PMethodInterface &from);
			void			SetTo(const PMethodInterface &from);
			void			MakeEmpty(void);
			
	virtual	status_t		SetArg(const int32 &index, const char *name,
									const PArgType &type,
									const char *description = NULL);
	virtual	status_t		AddArg(const char *name, const PArgType &type,
									const char *description = NULL);
	virtual	status_t		RemoveArg(const int32 &index);
	
			BString			ArgNameAt(const int32 &index);
			PArgType		ArgTypeAt(const int32 &index);
			BString			ArgDescAt(const int32 &index);
			int32			CountArgs(void) const;
			int32			FindArg(const char *name) const;
			
	virtual	status_t		SetReturnValue(const int32 &index, const char *name,
									const PArgType &type,
									const char *description = NULL);
	virtual	status_t		AddReturnValue(const char *name, const PArgType &type,
									const char *description = NULL);
	virtual	status_t		RemoveReturnValue(const int32 &index);
			BString			ReturnNameAt(const int32 &index);
			PArgType		ReturnTypeAt(const int32 &index);
			BString			ReturnDescAt(const int32 &index);
			int32			CountReturnValues(void) const;
			int32			FindReturnValue(const char *name) const;
			
private:
	BObjectList<PMethodInterfaceData>	fIn,
										fOut;
};

class PMethod
{
public:
							PMethod(const char *name = NULL,
									MethodFunction func = NULL,
									PMethodInterface *interface = NULL,
									const int32 &flags = 0);
	virtual					~PMethod(void);
			PMethod &		operator=(const PMethod &from);
			
	virtual	void			SetName(const char *name);
			void			SetName(const BString &name);
			BString			GetName(void) const;
	
	virtual	void			SetInterface(PMethodInterface &interface);
			void			GetInterface(PMethodInterface &interface);
	
	virtual	void			SetDescription(const char *string);
			void			SetDescription(const BString &string);
			BString			GetDescription(void) const;
	
	virtual	void			SetFunction(MethodFunction func);
			MethodFunction	GetFunction(void) const;
	
	virtual	status_t		Run(PObject *object, PArgList &in, PArgList &out);
	
private:
			BString				fName,
								fDescription;
			MethodFunction		fFunction;
			uint32				fFlags;
			PMethodInterface	fInterface;
};


#endif
