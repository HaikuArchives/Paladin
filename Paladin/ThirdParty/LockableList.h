#ifndef LOCKABLE_LIST_H
#define LOCKABLE_LIST_H

#include "ObjectList.h"
#include <Locker.h>

template<class T>
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
class LockableList : public BObjectList<T, true>, public BLocker
#else
class LockableList : public BObjectList<T>, public BLocker
#endif
{
public:
	LockableList(int32 itemsPerBlock = 20);
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	LockableList(const BObjectList<T, true> &list);
#else
	LockableList(const BObjectList<T> &list);
#endif
};

template<class T>
LockableList<T>::LockableList(int32 itemsPerBlock)
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	:	BObjectList<T, true>(itemsPerBlock)
#else
	:	BObjectList<T>(itemsPerBlock, true)
#endif
{
}

template<class T>
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
LockableList<T>::LockableList(const BObjectList<T, true> &list)
	:	BObjectList<T, true>(list)
#else
LockableList<T>::LockableList(const BObjectList<T> &list)
	:	BObjectList<T>(list)
#endif
{
}

#endif
