#ifndef LOCKABLE_LIST_H
#define LOCKABLE_LIST_H

#include "ObjectList.h"
#include <Locker.h>

template<class T>
class LockableList : public BObjectList<T>, public BLocker
{
public:
	LockableList(int32 itemsPerBlock = 20, bool owning = false);
	LockableList(const BObjectList<T> &list);
};

template<class T>
LockableList<T>::LockableList(int32 itemsPerBlock, bool owning)
	:	BObjectList<T>(itemsPerBlock, owning)
{
}

template<class T>
LockableList<T>::LockableList(const BObjectList<T> &list)
	:	BObjectList<T>(list)
{
}

#endif
