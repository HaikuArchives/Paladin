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
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	BObjectList<statdata, true>	fList;
#else
	BObjectList<statdata>	fList;
#endif
	int32					fMaxItems;
};

#endif
