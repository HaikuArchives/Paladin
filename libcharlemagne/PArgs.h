#ifndef PARGS_H
#define PARGS_H

#include <Rect.h>
#include <InterfaceDefs.h>
#include <String.h>

#include "CInterface.h"

class PArgs
{
public:
						PArgs(void);
						PArgs(PArgs &from);
						PArgs(PArgList *from, bool own = false);
						~PArgs(void);
	
	PArgs &				operator=(const PArgs &from);
	void				SetTo(const PArgs &from);
	
	void				MakeEmpty(void);
	
	int32				AddItem(PArgListItem *item);
	int32				AddItem(const char *name, void *arg, size_t argsize,
								PArgType type);
	int32				RemoveItem(PArgListItem *item);
	
	int32				AddInt8(const char *name, int8 arg);
	int32				AddInt16(const char *name, int16 arg);
	int32				AddInt32(const char *name, int32 arg);
	int32				AddInt64(const char *name, int64 arg);
	int32				AddFloat(const char *name, float arg);
	int32				AddDouble(const char *name, double arg);
	int32				AddBool(const char *name, bool arg);
	int32				AddChar(const char *name, char arg);
	int32				AddString(const char *name, const char *arg);
	int32				AddPoint(const char *name, BPoint arg);
	int32				AddRect(const char *name, BRect arg);
	int32				AddColor(const char *name, uint8 red, uint8 green,
								uint8 blue, uint8 alpha = 255);
	int32				AddColor(const char *name, rgb_color color);
	int32				AddPointer(const char *name, void *arg);
	int32				AddList(const char *name, const PArgList *list);
	int32				AddList(const char *name, const PArgs &list);
	
	PArgListItem *		FindItem(const char *name, int32 index = 0);
	int32				FindInt8(const char *name, int8 *out);
	int32				FindInt16(const char *name, int16 *out);
	int32				FindInt32(const char *name, int32 *out);
	int32				FindInt64(const char *name, int64 *out);
	int32				FindFloat(const char *name, float *out);
	int32				FindDouble(const char *name, double *out);
	int32				FindBool(const char *name, bool *out);
	int32				FindChar(const char *name, char *out);
	int32				FindString(const char *name, char **out);
	int32				FindString(const char *name, BString *out);
	int32				FindPoint(const char *name, BPoint *arg);
	int32				FindRect(const char *name, BRect *arg);
	int32				FindColor(const char *name, uint8 *red, uint8 *green,
								uint8 *blue, uint8 *alpha = NULL);
	int32				FindColor(const char *name, rgb_color *color);
	int32				FindPointer(const char *name, void **arg);
	
	PArgListItem *		ItemAt(int32 index);
	
	PArgList *			List(void);
	PArgList &			ListRef(void);
	
private:
	PArgList			*fArgList;
	bool				fFreeList;
};

#endif
