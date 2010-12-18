#include "CInterface.h"

#include <Errors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


PArgListItem *
create_pargitem(void)
{
	PArgListItem *node = (PArgListItem *)malloc(sizeof(PArgListItem));
	
	node->data = NULL;
	node->datasize = 0;
	
	node->type = PARG_END;
	node->name = NULL;
	
	node->next = NULL;
	node->prev = NULL;
	
	return node;
}


void
destroy_pargitem(PArgListItem *node)
{
	if (!node)
		return;
	
	if (node->prev && node->prev->next == node)
		node->prev->next = node->next;
	
	if (node->next && node->next->prev == node)
		node->next->prev = node->prev;
	
	free(node->name);
	
	free(node);
}


void
set_pargitem_name(PArgListItem *node, const char *name)
{
	if (!node)
		return;
	
	if (node->name)
		free(node->name);
	
	node->name = (name != NULL) ? strdup(name) : NULL;
}


void
print_pargitem(PArgListItem *node)
{
	printf("Name: %s\t", node->name);
	switch (node->type)
	{
		case PARG_RAW:
		{
			printf("Type: Raw\t(raw data)\n");
			break;
		}
		case PARG_INT8:
		{
			printf("Type: int8\t%d\n",*((int8_t*)node->data));
			break;
		}
		case PARG_INT16:
		{
			printf("Type: int16\t%d\n",*((int16_t*)node->data));
			break;
		}
		case PARG_INT32:
		{
			printf("Type: int32\t%d\n",*((int32_t*)node->data));
			break;
		}
		case PARG_INT64:
		{
			printf("Type: int64\t%lld\n",*((int64_t*)node->data));
			break;
		}
		case PARG_FLOAT:
		{
			printf("Type: float\t%f\n",*((float*)node->data));
			break;
		}
		case PARG_DOUBLE:
		{
			printf("Type: float\t%g\n",*((double*)node->data));
			break;
		}
		case PARG_CHAR:
		{
			printf("Type: char\t%c\n",*((char*)node->data));
			break;
		}
		case PARG_STRING:
		{
			printf("Type: string\t\"%s\"\n",(char*)node->data);
			break;
		}
		default:
			break;
	}
}


PArgList *
create_parglist(void)
{
	PArgList *list = (PArgList *)malloc(sizeof(PArgList));
	
	if (list)
	{
		list->head = list->tail = NULL;
		list->itemcount = 0;
	}
	
	return list;
}


void
destroy_parglist(PArgList *list)
{
	if (!list || list->itemcount == 0)
		return;
	
	PArgListItem *node = list->head;
	while (node)
	{
		PArgListItem *next = node->next;
		destroy_pargitem(node);
		node = next;
	}
}


int32_t
add_parg(PArgList *list, const char *name, void *arg, size_t argsize, PArgType type)
{
	/* Appends an argument to the list */
	if (!list || !arg || argsize < 1 || type >= PARG_END)
		return B_ERROR;
	
	PArgListItem *node = create_pargitem();
	set_parg(node, arg, argsize, type);
	set_pargitem_name(node, name);
	
	if (list->itemcount > 0)
	{
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	}
	else
	{
		list->head = list->tail = node;
		node->prev = node->next = NULL;
	}
	list->itemcount++;
	
	return B_OK;
}


int32_t
remove_parg(PArgList *list, PArgListItem *node)
{
	/* Removes an argument from the list */
	if (!list || !node)
		return B_ERROR;
	
	if (list->head == node)
		list->head = node->next;
	
	if (list->tail == node)
		list->tail = node->prev;
	
	if (node->prev && node->prev->next == node)
		node->prev->next = node->next;
	
	if (node->next && node->next->prev == node)
		node->next->prev = node->prev;
	
	return B_OK;
}


PArgListItem *
find_parg(PArgList *list, const char *name, PArgListItem *startItem)
{
	/* Searches for an item with the given name, starting from item start */
	if (!list || !name)
		return NULL;
	
	PArgListItem *current = startItem ? startItem : list->head;
	while (current)
	{
		if (current->name && strcmp(current->name, name) == 0)
			return current;
		
		current = current->next;
	}
	
	return NULL;
}


PArgListItem *
find_parg_index(PArgList *list, int32_t index)
{
	/* Returns an item at the specified index or NULL if the index is out of range */
	if (!list || index < 0 || index >= list->itemcount)
		return NULL;
	
	int32_t i = 0;
	PArgListItem *current = list->head;
	while (i < index)
	{
		if (!current->next)
			return NULL;
		else
		{
			current = current->next;
			i++;
		}
	}
	return current;
}


void
print_parglist(PArgList *list)
{
	/* Prints each item in the list */
	if (!list)
	{
		printf("print_parglist(): NULL list\n");
		return;
	}
	
	PArgListItem *current = list->head;
	while (current)
	{
		print_pargitem(current);
		current = current->next;
	}
	
}


void
set_parg(PArgListItem *node, void *arg, size_t argsize, PArgType type)
{
	/*
		Assign a value to an existing PArgListItem. This handles all of the
		memory management and type handling hassles for us.
	*/
	
	if (!node)
		return;
	
	if (!arg || argsize < 1 || type >= PARG_END)
	{
		free(node->data);
		node->data = NULL;
		node->datasize = 0;
		node->type = PARG_END;
		return;
	}
	
	if (node->data)
		node->data = realloc(node->data, argsize);
	else
		node->data = malloc(argsize);
	
	switch (type)
	{
		case PARG_RAW:
		{
			memcpy(node->data, arg, argsize);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_INT8:
		{
			*((int8_t*)node->data) = *((int8_t*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_INT16:
		{
			*((int16_t*)node->data) = *((int16_t*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_INT32:
		{
			*((int32_t*)node->data) = *((int32_t*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_INT64:
		{
			*((int64_t*)node->data) = *((int64_t*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_FLOAT:
		{
			*((float*)node->data) = *((float*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_DOUBLE:
		{
			*((double*)node->data) = *((double*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_CHAR:
		{
			*((char*)node->data) = *((char*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		case PARG_STRING:
		{
			strcpy((char*)node->data,(char*)arg);
			node->datasize = argsize;
			node->type = type;
			break;
		}
		default:
			break;
	}
	
}


int32_t
add_parg_int8(PArgList *list, const char *name, int8_t arg)
{
	return add_parg(list, name, &arg, sizeof(int8_t), PARG_INT8);
}


int32_t
add_parg_int16(PArgList *list, const char *name, int16_t arg)
{
	return add_parg(list, name, &arg, sizeof(int16_t), PARG_INT16);
}


int32_t
add_parg_int32(PArgList *list, const char *name, int32_t arg)
{
	return add_parg(list, name, &arg, sizeof(int32_t), PARG_INT32);
}


int32_t
add_parg_int64(PArgList *list, const char *name, int64_t arg)
{
	return add_parg(list, name, &arg, sizeof(int64_t), PARG_INT64);
}


int32_t
add_parg_float(PArgList *list, const char *name, float arg)
{
	return add_parg(list, name, &arg, sizeof(float), PARG_FLOAT);
}


int32_t
add_parg_double(PArgList *list, const char *name, double arg)
{
	return add_parg(list, name, &arg, sizeof(double), PARG_DOUBLE);
}


int32_t
add_parg_char(PArgList *list, const char *name, char arg)
{
	return add_parg(list, name, &arg, sizeof(char), PARG_CHAR);
}


int32_t
add_parg_string(PArgList *list, const char *name, const char *arg)
{
	if (!arg)
		return B_ERROR;
	
	return add_parg(list, name, (void*)arg, strlen(arg) + 1, PARG_STRING);
}


