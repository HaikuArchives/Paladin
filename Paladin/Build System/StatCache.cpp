#include "StatCache.h"

#include <Path.h>
#include <stdio.h>

StatCache::StatCache(void)
	:	fList(20,true),
		fMaxItems(79)
{
	// 79 is the approximate number of stat structures that will fit into 4K, which
	// seems like more than enough for most uses
	
}


StatCache::~StatCache(void)
{
}


void
StatCache::SetRAMLimit(uint32 size)
{
	fMaxItems = size / sizeof(struct stat);
	while (fList.CountItems() > fMaxItems)
		delete fList.RemoveItemAt(fList.CountItems() - 1);
}


uint32
StatCache::GetRAMLimit(void) const
{
	return fMaxItems * sizeof(struct stat);
}


struct stat *
StatCache::StatFor(entry_ref ref)
{
	statdata *item = NULL;
	for (int32 i = 0; i < fList.CountItems(); i++)
	{
		item = fList.ItemAt(i);
		if (item->ref == ref)
		{
			// we have a cache hit. Move the item to the front of the list
			// and return it.
			fList.RemoveItemAt(i);
			fList.AddItem(item,0);
			return &item->statinfo;
		}
	}
	
	// If we got this far, then we have a cache miss. Create a new statdata item and
	// add it to the list or reuse the last item in the cache if it has the max number
	// of items
	if (fList.CountItems() == fMaxItems)
		item = fList.RemoveItemAt(fList.CountItems() - 1);
	else
		item = new statdata;
	fList.AddItem(item,0);
	
	BPath path(&ref);
	if (stat(path.Path(),&item->statinfo) != 0)
	{
		delete fList.RemoveItemAt(0L);
		return NULL;
	}
	
	item->ref = ref;
	return &item->statinfo;
}


struct stat *
StatCache::StatFor(const char *path)
{
	entry_ref ref;
	BEntry entry(path);
	if (entry.InitCheck() != B_OK || !entry.Exists())
		return NULL;
	
	entry.GetRef(&ref);
	return StatFor(ref);
}


void
StatCache::MakeEmpty(void)
{
	fList.MakeEmpty();
}

