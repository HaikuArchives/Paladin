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
PMethod::SetInterface(PArgs &in)
{
	fInterface = in;
}


void
PMethod::GetInterface(PArgs &out)
{
	out = fInterface;
}

	
void
PMethod::SetDescription(const char *string)
{
	fDescription = string;
}


void
PMethod::SetDescription(const BString &string)
{
	SetDescription(string.String());
}


BString
PMethod::GetDescription(void) const
{
	return fDescription;
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
PMethod::Run(PObject *object, PArgList &in, PArgList &out)
{
	return fFunction(object, &in, &out);
}

