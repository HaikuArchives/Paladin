#ifndef PARGS_H
#define PARGS_H

#include <SupportDefs.h>
#include "CInterface.h"

class PArgs
{
public:
						PArgs(void);
						~PArgs(void);
	
	int32				AddItem(const char *name, void *arg, size_t argsize,
								PArgType type);
	int32				AddItem(const char *name, int8 arg);
	int32				AddItem(const char *name, int16 arg);
	int32				AddItem(const char *name, int32 arg);
	int32				AddItem(const char *name, int64 arg);
	int32				AddItem(const char *name, float arg);
	int32				AddItem(const char *name, double arg);
	int32				AddItem(const char *name, char arg);
	int32				AddItem(const char *name, const char *arg);
	int32				RemoveItem(PArgListItem *item);
	
	PArgListItem *		FindItem(const char *name, int32 index = 0);
	PArgListItem *		ItemAt(int32 index);
	
private:
	PArgList	*fArgList;
};

#endif
