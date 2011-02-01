#include "LockableVar.h"

LockableBool::LockableBool(const bool &value)
	:	fValue(value)
{
}


LockableBool::~LockableBool(void)
{
}


LockableBool &
LockableBool::operator=(const bool &from)
{
	SetValue(from);
	return *this;
}


bool
LockableBool::operator==(const bool &from)
{
	return GetValue() == from;
}


bool
LockableBool::operator!=(const bool &from)
{
	return GetValue() == from;
}


void
LockableBool::SetValue(const bool &value)
{
	fLock.Lock();
	fValue = value;
	fLock.Unlock();
}


bool
LockableBool::GetValue(void)
{
	bool value;
	fLock.Lock();
	value = fValue;
	fLock.Unlock();
	return value;
}
