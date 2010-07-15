#ifndef STAT_CACHE_H
#define STAT_CACHE_H

#include <sys/stat.h>
#include <Entry.h>

#include "ObjectList.h"

typedef struct
{
	struct stat statinfo;
	entry_ref	ref;
} statdata;

class StatCache
{
public:
					StatCache(void);
					~StatCache(void);
			
	void			SetRAMLimit(uint32 size);
	uint32			GetRAMLimit(void) const;
	
	struct stat	*	StatFor(entry_ref ref);
	struct stat	*	StatFor(const char *path);
	
	void			MakeEmpty(void);
	
private:
	BObjectList<statdata>	fList;
	int32					fMaxItems;
};

#endif
