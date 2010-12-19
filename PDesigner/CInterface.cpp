#include "CInterface.h"

#include <Errors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PData.h"
#include "PObject.h"
#include "PObjectBroker.h"
#include "PProperty.h"
#include "PValue.h"

#pragma mark - argument list item functions

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

#pragma mark - argument list functions

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


void
empty_parglist(PArgList *list)
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
	
	list->head = list->tail = NULL;
	list->itemcount = 0;
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


int32_t
add_parg_point(PArgList *list, const char *name, float x, float y)
{
	float data[2];
	data[0] = x;
	data[1] = y;
	
	return add_parg(list, name, data, sizeof(float) * 2, PARG_POINT);
}


int32_t
add_parg_rect(PArgList *list, const char *name, float left, float top,
			float right, float bottom)
{
	float data[4];
	data[0] = left;
	data[1] = top;
	data[2] = right;
	data[3] = bottom;
	
	return add_parg(list, name, data, sizeof(float) * 4, PARG_RECT);
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


int32_t
find_parg_int8(PArgList *list, const char *name, int8_t *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT8)
		return B_NAME_NOT_FOUND;
	
	*out = *((int8_t*)item->data);
	return B_OK;
}


int32_t
find_parg_int16(PArgList *list, const char *name, int16_t *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT16)
		return B_NAME_NOT_FOUND;
	
	*out = *((int16_t*)item->data);
	return B_OK;
}


int32_t
find_parg_int32(PArgList *list, const char *name, int32_t *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT32)
		return B_NAME_NOT_FOUND;
	
	*out = *((int32_t*)item->data);
	return B_OK;
}


int32_t
find_parg_int64(PArgList *list, const char *name, int64_t *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT64)
		return B_NAME_NOT_FOUND;
	
	*out = *((int64_t*)item->data);
	return B_OK;
}


int32_t
find_parg_float(PArgList *list, const char *name, float *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT64)
		return B_NAME_NOT_FOUND;
	
	*out = *((float*)item->data);
	return B_OK;
}


int32_t
find_parg_double(PArgList *list, const char *name, double *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT64)
		return B_NAME_NOT_FOUND;
	
	*out = *((double*)item->data);
	return B_OK;
}


int32_t
find_parg_char(PArgList *list, const char *name, char *out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT64)
		return B_NAME_NOT_FOUND;
	
	*out = *((char*)item->data);
	return B_OK;
}


int32_t
find_parg_string(PArgList *list, const char *name, char **out)
{
	if (!list || !name || !out)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_INT64)
		return B_NAME_NOT_FOUND;
	
	*out = strdup((char*)item->data);
	return B_OK;
}


int32_t
find_parg_point(PArgList *list, const char *name, float *x, float *y)
{
	if (!list || !name || !x || !y)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_POINT)
		return B_NAME_NOT_FOUND;
	
	float *args = (float*)item->data;
	*x = args[0];
	*y = args[1];
	
	return B_OK;
}


int32_t
find_parg_rect(PArgList *list, const char *name, float *left, float *top,
				float *right, float *bottom)
{
	if (!list || !name || !left || !top || !right || !bottom)
		return B_ERROR;
	
	PArgListItem *item = find_parg(list, name, NULL);
	if (!item || item->type != PARG_RECT)
		return B_NAME_NOT_FOUND;
	
	float *args = (float*)item->data;
	*left = args[0];
	*top = args[1];
	*right = args[2];
	*bottom = args[3];
	
	return B_OK;
}


#pragma mark - value functions



bool
pvalue_accepts_type(void *pval, void *testval)
{
	PValue *value = static_cast<PValue*>(pval);
	PValue *testvalue = static_cast<PValue*>(testval);
	return (value && testvalue) ? value->AcceptsType(testvalue) : false;
}


bool
pvalue_returns_type(void *pval, void *testval)
{
	PValue *value = static_cast<PValue*>(pval);
	PValue *testvalue = static_cast<PValue*>(testval);
	return (value && testvalue) ? value->ReturnsType(testvalue) : false;
}



int32_t
pvalue_copy_value(void *from, void *to)
{
	PValue *src = static_cast<PValue*>(from);
	PValue *dest = static_cast<PValue*>(to);
	return (src && dest) ? dest->SetValue(src) : B_BAD_DATA;
}


void
pvalue_get_type(void *pval, char **out)
{
	PValue *value = static_cast<PValue*>(pval);
	if (!value)
		return;
	if (value->type->CountChars() > 0)
		*out = strdup(value->type->String());
	else
		*out = NULL;
}


#pragma mark -	property functions

void *
pproperty_create(void)
{
	return new PProperty();
}


void
pproperty_destroy(void *prop)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
		delete p;
}


void *
pproperty_duplicate(void *prop)
{
	PProperty *p = static_cast<PProperty*>(prop);
	return p ? new PProperty(*p) : NULL;
}


void
pproperty_copy(void *from, void *to)
{
	PProperty *src = static_cast<PProperty*>(from);
	PProperty *dest = static_cast<PProperty*>(to);
	if (src && dest)
		*dest = *src;
}


void
pproperty_set_name(void *prop, const char *name)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
		p->SetName(name);
}


void
pproperty_get_name(void *prop, char **out)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
	{
		BString name(p->GetName());
		*out = name.CountChars() ? strdup(name.String()) : NULL;
	}
}



bool
pproperty_is_read_only(void *prop)
{
	PProperty *p = static_cast<PProperty*>(prop);
	return p ? p->IsReadOnly() : false;
}


void
pproperty_set_read_only(void *prop, bool value)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
		p->SetReadOnly(value);
}


void
pproperty_set_enabled(void *prop, bool value)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
		p->SetEnabled(value);
}


bool
pproperty_is_enabled(void *prop)
{
	PProperty *p = static_cast<PProperty*>(prop);
	return p ? p->IsEnabled() : false;
}



int32_t
pproperty_set_value(void *prop, void *pvalue)
{
	PProperty *p = static_cast<PProperty*>(prop);
	PValue *pv = static_cast<PValue*>(pvalue);
	return (p && pv) ? p->SetValue(pv) : B_BAD_DATA;
}


int32_t
pproperty_get_value(void *prop, void *pvalue)
{
	PProperty *p = static_cast<PProperty*>(prop);
	PValue *pv = static_cast<PValue*>(pvalue);
	return (p && pv) ? p->GetValue(pv) : B_BAD_DATA;
}


void
pproperty_get_value_as_string(void *prop, char **out)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
	{
		BString name(p->GetValueAsString());
		*out = name.CountChars() ? strdup(name.String()) : NULL;
	}
}


void
pproperty_set_description(void *prop, const char *desc)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
		p->SetDescription(desc);
}


void
pproperty_get_description(void *prop, char **out)
{
	PProperty *p = static_cast<PProperty*>(prop);
	if (p)
	{
		BString desc(p->GetValueAsString());
		*out = desc.CountChars() ? strdup(desc.String()) : NULL;
	}
}



#pragma mark - data functions

void *
pdata_create(void)
{
	return new PData();
}


void
pdata_destory(void *pdata)
{
	PData *pd = static_cast<PData*>(pdata);
	if (pd)
		delete pd;
}


void *
pdata_duplicate(void *pdata)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->Duplicate() : NULL;
}


void
pdata_copy(void *from, void *to)
{
	PData *src = static_cast<PData*>(from);
	PData *dest = static_cast<PData*>(to);
	if (src && dest)
		*dest = *src;
}



int32_t
pdata_count_properties(void *pdata, const char *name)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->CountProperties() : B_BAD_DATA;
}


void *
pdata_property_at(void *pdata, int32_t index)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->PropertyAt(index) : NULL;
}


int32_t
pdata_index_of_property(void *pdata, void *prop)
{
	PData *pd = static_cast<PData*>(pdata);
	PProperty *val = static_cast<PProperty*>(prop);
	return (pd && val) ? pd->IndexOfProperty(val) : B_BAD_DATA;
}


void *
pdata_find_property(void *pdata, const char *name)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->FindProperty(name) : NULL;
}


bool
pdata_add_property(void *pdata, void *prop, uint32_t flags, int32_t index)
{
	PData *pd = static_cast<PData*>(pdata);
	PProperty *val = static_cast<PProperty*>(prop);
	return (pd && val) ? pd->AddProperty(val, flags, index) : false;
}


void *
pdata_remove_property_at(void *pdata, int32_t index)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->RemoveProperty(index) : NULL;
}


void
pdata_remove_property(void *pdata, void *prop)
{
	PData *pd = static_cast<PData*>(pdata);
	PProperty *val = static_cast<PProperty*>(prop);
	if (pd && val)
		pd->RemoveProperty(val);
}


uint32_t
pdata_property_flags_at(void *pdata, int32_t index)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->PropertyFlagsAt(index) : 0;
}


void
pdata_set_flags_for_property(void *pdata, void *prop, int32_t flags)
{
	PData *pd = static_cast<PData*>(pdata);
	PProperty *val = static_cast<PProperty*>(prop);
	if (pd)
		pd->SetFlagsForProperty(val, flags);
}


uint32_t
pdata_flags_for_property(void *pdata, void *prop)
{
	PData *pd = static_cast<PData*>(pdata);
	PProperty *val = static_cast<PProperty*>(prop);
	return (pd && val) ? pd->FlagsForProperty(val) : 0;
}



int32_t
pdata_set_value_for_property(void *pdata, const char *name, void *pvalue)
{
	PData *pd = static_cast<PData*>(pdata);
	PValue *val = static_cast<PValue*>(pvalue);
	return pd ? pd->SetProperty(name, val) : B_BAD_DATA;
}


int32_t
pdata_get_value_for_property(void *pdata, const char *name, void **pvalue)
{
	// TODO: Test this sucker
	PData *pd = static_cast<PData*>(pdata);
	if (pd && static_cast<PValue*>(*pvalue))
		return pd->GetProperty(name, static_cast<PValue*>(*pvalue));
	else
		return B_BAD_DATA;
}


int32_t
pdata_set_string_property(void *pdata, const char *name, const char *value)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->SetStringProperty(name, value) : B_BAD_DATA;
}


int32_t
pdata_set_int_property(void *pdata, const char *name, int64_t value)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->SetIntProperty(name, value) : B_BAD_DATA;
}


int32_t
pdata_set_bool_property(void *pdata, const char *name, bool value)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->SetBoolProperty(name, value) : B_BAD_DATA;
}


int32_t
pdata_set_float_property(void *pdata, const char *name, float value)
{
	PData *pd = static_cast<PData*>(pdata);
	return pd ? pd->SetFloatProperty(name, value) : B_BAD_DATA;
}


int32_t
pdata_set_rect_property(void *pdata, const char *name, float left, float top,
						float right, float bottom)
{
	PData *pd = static_cast<PData*>(pdata);
	BRect r(left, top, right, bottom);
	return pd ? pd->SetRectProperty(name, r) : B_BAD_DATA;
}


int32_t
pdata_set_point_property(void *pdata, const char *name, float x, float y)
{
	PData *pd = static_cast<PData*>(pdata);
	BPoint pt(x,y);
	return pd ? pd->SetPointProperty(name, pt) : B_BAD_DATA;
}


int32_t
pdata_set_color_property(void *pdata, const char *name, uint8_t red,
						uint8_t green, uint8_t blue, uint8_t alpha)
{
	PData *pd = static_cast<PData*>(pdata);
	rgb_color color = { red, green, blue, alpha };
	return pd ? pd->SetColorProperty(name, color) : B_BAD_DATA;
}



int32_t
pdata_get_string_property(void *pdata, const char *name, char **out)
{
	PData *pd = static_cast<PData*>(pdata);
	if (pd || !out)
	{
		BString string;
		int32 status = pd->GetStringProperty(name, string);
		if (status != B_OK)
		{
			*out = NULL;
			return status;
		}
		
		*out = strdup(string.String());
		return status;
	}
	
	return B_BAD_DATA;
}


int32_t
pdata_get_int_property(void *pdata, const char *name, int64_t *value)
{
	PData *pd = static_cast<PData*>(pdata);
	return (pd && value) ? pd->GetIntProperty(name, *value) : B_BAD_DATA;
}


int32_t
pdata_get_bool_property(void *pdata, const char *name, bool *value)
{
	PData *pd = static_cast<PData*>(pdata);
	return (pd && value) ? pd->GetBoolProperty(name, *value) : B_BAD_DATA;
}


int32_t
pdata_get_float_property(void *pdata, const char *name, float *value)
{
	PData *pd = static_cast<PData*>(pdata);
	return (pd && value) ? pd->GetFloatProperty(name, *value) : B_BAD_DATA;
}


int32_t
pdata_get_rect_property(void *pdata, const char *name, float *left, float *top,
						float *right, float *bottom)
{
	PData *pd = static_cast<PData*>(pdata);
	if (pd && left && right && top && bottom)
	{
		BRect r;
		status_t status = pd->GetRectProperty(name, r);
		if (status != B_OK)
			return status;
		
		*left = r.left;
		*top = r.top;
		*right = r.right;
		*bottom = r.bottom;
		
		return status;
	}
	
	return B_BAD_DATA;
}


int32_t
pdata_get_point_property(void *pdata, const char *name, float *x, float *y)
{
	PData *pd = static_cast<PData*>(pdata);
	if (pd && x && y)
	{
		BPoint pt;
		status_t status = pd->GetPointProperty(name, pt);
		if (status != B_OK)
			return status;
		
		*x = pt.x;
		*y = pt.y;
		
		return status;
	}
	
	return B_BAD_DATA;
}


int32_t
pdata_get_color_property(void *pdata, const char *name, uint8_t *red,
						uint8_t *green, uint8_t *blue, uint8_t *alpha)
{
	PData *pd = static_cast<PData*>(pdata);
	if (pd && red && green && blue)
	{
		rgb_color color;
		status_t status = pd->GetColorProperty(name, color);
		if (status != B_OK)
			return status;
		
		*red = color.red;
		*green = color.green;
		*blue = color.blue;
		if (alpha)
			*alpha = color.alpha;
		
		return status;
	}
	
	return B_BAD_DATA;
}


void
pdata_get_type(void *pdata, char **out)
{
	PData *pd = static_cast<PData*>(pdata);
	if (!pd)
		return;
	
	BString string(pd->GetType());
	*out = string.CountChars() ? strdup(string.String()) : NULL;
}


void
pdata_get_friendly_type(void *pdata, char **out)
{
	PData *pd = static_cast<PData*>(pdata);
	if (!pd)
		return;
	
	BString string(pd->GetFriendlyType());
	*out = string.CountChars() ? strdup(string.String()) : NULL;
}


#pragma mark - object broker and object functions


void
pobjectspace_init(void)
{
	InitObjectSystem();
}


void
pobjectspace_shutdown(void)
{
	ShutdownObjectSystem();
}


	
int32_t
pobjectspace_count_types(void)
{
	return BROKER->CountTypes();
}


void
pobjectspace_type_at(int32_t index, char **out)
{
	BString string(BROKER->TypeAt(index));
	*out = string.CountChars() ? strdup(string.String()) : NULL;
}


void
pobjectspace_friendly_type_at(int32_t index, char **out)
{
	BString string(BROKER->FriendlyTypeAt(index));
	*out = string.CountChars() ? strdup(string.String()) : NULL;
}


void *
pobjectspace_find_object(uint64_t id)
{
	return BROKER->FindObject(id);
}


void *
pobject_create(const char *type)
{
	return BROKER->MakeObject(type);
}


void *
pobject_duplicate(void *pobj)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->Duplicate() : NULL;
}


void
pobject_copy(void *from, void *to)
{
	PObject *src = static_cast<PObject*>(from);
	PObject *dest = static_cast<PObject*>(to);
	*dest = *src;
}


void
pobject_delete(void *pobj)
{
	PObject *obj = static_cast<PObject*>(pobj);
	if (obj)
		delete obj;
}


uint64_t
pobject_get_id(void *pobj)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->GetID() : 0;
}


int32_t
pobject_run_method(void *pobj, const char *name, PArgList *in,
										PArgList *out)
{
	// TODO: Implement after converting PObect to PArgList
	return B_ERROR;
}


void *
pobject_find_method(void *pobj, const char *name)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->FindMethod(name) : NULL;
}


void *
pobject_method_at(void *pobj, int32_t index)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->MethodAt(index) : NULL;
}


int32_t
pobject_count_methods(void *pobj)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->CountMethods() : B_BAD_DATA;
}


bool
pobject_uses_interface(void *pobj, const char *name)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->UsesInterface(name) : false;
}


void
pobject_interface_at(void *pobj, int32_t index, char **out)
{
	PObject *obj = static_cast<PObject*>(pobj);
	if (!obj)
		return;
	
	BString string(obj->InterfaceAt(index));
	*out = string.CountChars() ? strdup(string.String()) : NULL;
}


int32_t
pobject_count_interfaces(void *pobj)
{
	PObject *obj = static_cast<PObject*>(pobj);
	return obj ? obj->CountInterfaces() : B_BAD_DATA;
}


void
pobject_print_to_stream(void *pobj)
{
	PObject *obj = static_cast<PObject*>(pobj);
	if (obj)
		obj->PrintToStream();
}

#pragma mark - PMethod-related

int32_t
NullPMethod(void *pobject, PArgList *in, PArgList *out)
{
	return B_OK;
}
