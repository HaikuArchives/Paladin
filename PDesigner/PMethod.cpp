#include "PMethod.h"

PMethod::PMethod(const char *name, MethodFunction func, const int32 &flags)
	:	fName(name),
		fFlags(flags)
{
	fFunction = func;
}


PMethod::~PMethod(void)
{
}


PMethod &
PMethod::operator=(const PMethod &from)
{
	fName = from.fName;
	
	return *this;
}


void
PMethod::SetName(const char *name)
{
	fName = name;
}


void
PMethod::SetName(const BString &name)
{
	SetName(name.String());
}


BString
PMethod::GetName(void) const
{
	return fName;
}


void
PMethod::SetFunction(MethodFunction func)
{
	fFunction = func;
}


MethodFunction
PMethod::GetFunction(void) const
{
	return fFunction;
}


status_t
PMethod::Run(PObject *object, BMessage &in, BMessage &out)
{
	return fFunction(object, in, out);
}

status_t
NullPMethod(PObject *object, BMessage &in, BMessage &out)
{
	return B_OK;
}
