	#include "PArgs.h"

#include <string.h>

PArgs::PArgs(void)
	:	fFreeList(true)
{
	fArgList = create_parglist();
}


PArgs::PArgs(PArgs &from)
{
	fArgList = create_parglist();
	copy_parglist(fArgList, from.fArgList);
}


PArgs::PArgs(PArgList *from, bool own)
	:	fArgList(from),
		fFreeList(own)
{
	if (!from)
	{
		fArgList = create_parglist();
		fFreeList = true;
	}
}


PArgs::~PArgs(void)
{
	if (fFreeList)
		destroy_parglist(fArgList);
}


PArgs &
PArgs::operator=(const PArgs &from)
{
	copy_parglist(fArgList, from.fArgList);
	return *this;
}


void
PArgs::SetTo(const PArgs &from)
{
	copy_parglist(fArgList, from.fArgList);
}


void
PArgs::SetTo(PArgList *from, bool own)
{
	// If own is true, we copy the entire list, but it is false,
	// we just copy the pointer to the list
	if (fFreeList)
		empty_parglist(fArgList);
	else if (own)
	{
		// If we don't already own the list but we're going to,
		// then we will need to allocate a new list to hold the
		// new list items
		fArgList = create_parglist();
	}
	
	if (!from)
		return;
	
	if (own)
	{
		copy_parglist(from, fArgList);
		fFreeList = true;
	}
	else
	{
		destroy_parglist(fArgList);
		fArgList = from;
		fFreeList = false;
	}
}


void
PArgs::MakeEmpty(void)
{
	empty_parglist(fArgList);
}


int32
PArgs::AddItem(PArgListItem *item)
{
	return add_pargitem(fArgList, item);
}


int32
PArgs::AddItem(const char *name, void *arg, size_t argsize,
							PArgType type)
{
	return add_parg(fArgList, name, arg, argsize, type);
}


int32
PArgs::RemoveItem(PArgListItem *item)
{
	return remove_parg(fArgList, item);
}


int32
PArgs::CountItems(void)
{
	return count_pargs(fArgList);
}


int32
PArgs::AddInt8(const char *name, int8 arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(int8_t), PARG_INT8);
}


int32
PArgs::AddInt16(const char *name, int16 arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(int16_t), PARG_INT16);
}


int32
PArgs::AddInt32(const char *name, int32 arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(int32_t), PARG_INT32);
}


int32
PArgs::AddInt64(const char *name, int64 arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(int64_t), PARG_INT64);
}


int32
PArgs::AddFloat(const char *name, float arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(float), PARG_FLOAT);
}


int32
PArgs::AddDouble(const char *name, double arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(double), PARG_DOUBLE);
}


int32
PArgs::AddBool(const char *name, bool arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(bool), PARG_BOOL);
}


int32
PArgs::AddChar(const char *name, char arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(char), PARG_CHAR);
}


int32
PArgs::AddString(const char *name, const char *arg)
{
	return add_parg(fArgList, name, (void*)arg, strlen(arg) + 1, PARG_STRING);
}


int32
PArgs::AddPoint(const char *name, BPoint arg)
{
	return add_parg_point(fArgList, name, arg.x, arg.y);
}


int32
PArgs::AddRect(const char *name, BRect arg)
{
	return add_parg_rect(fArgList, name, arg.left, arg.top, arg.right, arg.bottom);
}


int32
PArgs::AddColor(const char *name, uint8 red, uint8 green, uint8 blue,
				uint8 alpha)
{
	return add_parg_color(fArgList, name, red, green, blue, alpha);
}


int32
PArgs::AddColor(const char *name, rgb_color color)
{
	return add_parg_rect(fArgList, name, color.red, color.green, color.blue,
						color.alpha);
}


int32
PArgs::AddPointer(const char *name, void *arg)
{
	return add_parg_pointer(fArgList, name, &arg);
}


int32
PArgs::AddList(const char *name, const PArgList *list)
{
	return add_parg_list(fArgList, name, list);
}


int32
PArgs::AddList(const char *name, const PArgs &list)
{
	return add_parg_list(fArgList, name, list.fArgList);
}


PArgListItem *
PArgs::FindItem(const char *name, int32 index)
{
	PArgListItem *item = find_parg_index(fArgList, index);
	if (!item)
		return NULL;
	
	return find_parg(fArgList, name, item);
}


int32
PArgs::FindInt8(const char *name, int8 *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((int8_t*)item->data);
	return B_OK;
}


int32
PArgs::FindInt16(const char *name, int16 *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((int16_t*)item->data);
	return B_OK;
}


int32
PArgs::FindInt32(const char *name, int32 *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((int32_t*)item->data);
	return B_OK;
}


int32
PArgs::FindInt64(const char *name, int64 *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((int64_t*)item->data);
	return B_OK;
}


int32
PArgs::FindFloat(const char *name, float *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((float*)item->data);
	return B_OK;
}


int32
PArgs::FindDouble(const char *name, double *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((double*)item->data);
	return B_OK;
}


int32
PArgs::FindBool(const char *name, bool *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((bool*)item->data);
	return B_OK;
}


int32
PArgs::FindChar(const char *name, char *out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = *((char*)item->data);
	return B_OK;
}


int32
PArgs::FindString(const char *name, char **out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = strdup((char*)item->data);
	return B_OK;
}


int32
PArgs::FindString(const char *name, BString *out)
{
	if (!name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = (char *)item->data;
	return B_OK;
}


int32
PArgs::FindPoint(const char *name, BPoint *out)
{
	return find_parg_point(fArgList, name, &out->x, &out->y);
}


int32
PArgs::FindRect(const char *name, BRect *out)
{
	return find_parg_rect(fArgList, name, &out->left, &out->top,
							&out->right, &out->bottom);
}


int32
PArgs::FindColor(const char *name, uint8 *red, uint8 *green, uint8 *blue,
				uint8 *alpha)
{
	return find_parg_color(fArgList, name, red, green, blue, alpha);
}


int32
PArgs::FindColor(const char *name, rgb_color *color)
{
	if (!color)
		return B_BAD_DATA;
	
	return find_parg_color(fArgList, name, &color->red, &color->green,
							&color->blue, &color->alpha);
}


int32
PArgs::FindPointer(const char *name, void **out)
{
	if (!name)
		return B_ERROR;
	
	PArgListItem *item = find_parg(fArgList, name, NULL);
	if (!item)
		return B_NAME_NOT_FOUND;
	
	*out = item->data;
	return B_OK;
}


int32
PArgs::FindList(const char *name, PArgList **list)
{
	if (!name)
		return B_ERROR;
	
	find_parg_list(fArgList, name, list);
	
	return (*list == NULL) ? B_NAME_NOT_FOUND : B_OK;
}


int32
PArgs::FindList(const char *name, PArgs &list, bool copy)
{
	if (!name)
		return B_ERROR;
	
	PArgList *ptr;
	status_t status = find_parg_list(fArgList, name, &ptr);
	if (status != B_OK)
		return status;
	
	SetTo(ptr, copy);
	return B_OK;
}


PArgListItem *
PArgs::GetFirstItem(void)
{
	return get_parg_first(fArgList);
}


PArgListItem *
PArgs::GetLastItem(void)
{
	return get_parg_last(fArgList);
}


PArgListItem *
PArgs::GetNextItem(PArgListItem *item)
{
	return get_parg_next(fArgList, item);
}


PArgListItem *
PArgs::GetPreviousItem(PArgListItem *item)
{
	return get_parg_previous(fArgList, item);
}


PArgListItem *
PArgs::ItemAt(int32 index)
{
	return find_parg_index(fArgList, index);
}


PArgList *
PArgs::List(void)
{
	return fArgList;
}


PArgList &
PArgs::ListRef(void)
{
	return *fArgList;
}

