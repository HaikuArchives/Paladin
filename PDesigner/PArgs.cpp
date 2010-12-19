#include "PArgs.h"

#include <string.h>

PArgs::PArgs(void)
{
	fArgList = create_parglist();
}


PArgs::~PArgs(void)
{
	destroy_parglist(fArgList);
}


int32
PArgs::AddItem(const char *name, void *arg, size_t argsize,
							PArgType type)
{
	return add_parg(fArgList, name, arg, argsize, type);
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
PArgs::RemoveItem(PArgListItem *item)
{
	return remove_parg(fArgList, item);
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

