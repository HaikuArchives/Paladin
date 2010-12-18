#ifndef CINTERFACE_H
#define CINTERFACE_H

#include <stdint.h>
#include <sys/types.h>

#if defined(__cplusplus)
	extern "C" {
#endif

/*
	Doubly-linked list implementation for passing arguments into and out of PObject methods.
	Do not use the node objects directly. Instead, use the C++-like *_parglist function calls
*/

enum parg_type_t
{
	PARG_RAW = 0,
	
	PARG_INT8,
	PARG_INT16,
	PARG_INT32,
	PARG_INT64,
	
	PARG_FLOAT,
	PARG_DOUBLE,
	
	PARG_CHAR,
	PARG_STRING,
	
	PARG_END
};

typedef enum parg_type_t PArgType;


struct p_arg_list_node
{
	void		*data;
	size_t		datasize;
	PArgType	type;
	char		*name;
	
	struct p_arg_list_node *next;
	struct p_arg_list_node *prev;
};

typedef struct p_arg_list_node PArgListItem;


struct p_arg_list
{
	PArgListItem	*head,
					*tail;
	
	int32_t			itemcount;
};

typedef struct p_arg_list PArgList;


/* -------------------------------------------------------------------------------------
	Argument-passing code
	
	The node functions are used internally and should not be called directly. Rather,
	use the *_parglist() functions. These calls exist to provide a flexible way of passing
	information into and out of PObject methods via a C interface.
   ------------------------------------------------------------------------------------- */

PArgListItem *		create_pargitem(void);
void				destroy_pargitem(PArgListItem *node);
void				set_pargitem_name(PArgListItem *node, const char *name);
void				print_pargitem(PArgListItem *node);

PArgList *			create_parglist(void);
void				destroy_parglist(PArgList *list);
void				print_parglist(PArgList *list);

int32_t				add_parg(PArgList *list, const char *name, void *arg, size_t argsize,
							PArgType type);
int32_t				remove_parg(PArgList *list, PArgListItem *node);
PArgListItem *		find_parg(PArgList *list, const char *name, PArgListItem *startItem);
PArgListItem *		find_parg_index(PArgList *list, int32_t index);
void				set_parg(PArgListItem *node, void *arg, size_t argsize,
							PArgType type);

int32_t				add_parg_int8(PArgList *list, const char *name, int8_t arg);
int32_t				add_parg_int16(PArgList *list, const char *name, int16_t arg);
int32_t				add_parg_int32(PArgList *list, const char *name, int32_t arg);
int32_t				add_parg_int64(PArgList *list, const char *name, int64_t arg);

int32_t				add_parg_float(PArgList *list, const char *name, float arg);
int32_t				add_parg_double(PArgList *list, const char *name, double arg);

int32_t				add_parg_char(PArgList *list, const char *name, char arg);
int32_t				add_parg_string(PArgList *list, const char *name, const char *arg);


#if defined(__cplusplus)
	}
#endif

#endif
