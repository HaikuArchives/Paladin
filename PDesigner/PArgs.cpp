#include "PArgs.h"

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
PArgs::AddItem(const char *name, int8 arg)
{
	return add_parg(fArgList, name, (void*)&arg, sizeof(int8_t), PARG_INT8);
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


PArgListItem *
PArgs::ItemAt(int32 index)
{
	return find_parg_index(fArgList, index);
}

