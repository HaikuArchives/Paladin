#ifndef CINTERFACE_H
#define CINTERFACE_H

/* -------------------------------------------------------------------------------------
	This file contains all of the C bindings to the PDesigner object system. It is not
	pretty, but C is not well-suited to anything resembling OOP.
   ------------------------------------------------------------------------------------- */

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
	extern "C" {
#else
	#ifndef SWIG
		typedef unsigned char bool;
	#endif
#endif

/*
	Doubly-linked list implementation for passing arguments into and out of PObject methods.
	Do not use the node objects directly unless you really like pain. Instead, use the
	C++-like *_parglist function calls for list manipulation.
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
	
	PARG_BOOL,
	PARG_CHAR,
	PARG_STRING,
	
	PARG_RECT,
	PARG_POINT,
	PARG_COLOR,
	PARG_POINTER,
	
	PARG_LIST,
	
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
	
	int			itemcount;
};

typedef struct p_arg_list PArgList;

typedef int (*MethodFunction)(void *pobject, PArgList *in, PArgList *out, void *ptr);

/* -------------------------------------------------------------------------------------
	Argument-passing code
	
	The node functions are used internally and should not be called directly. Rather,
	use the *_parglist() functions. These calls exist to provide a flexible way of passing
	information into and out of PObject methods via a C interface.
   ------------------------------------------------------------------------------------- */

PArgListItem *		create_pargitem(void);
void				destroy_pargitem(PArgListItem *node);
void				copy_pargitem(PArgListItem *from, PArgListItem *to);
void				set_pargitem_name(PArgListItem *node, const char *name);
void				set_pargitem_data(PArgListItem *node, void *inData,
									size_t inSize, PArgType inType);
void				print_pargitem(PArgListItem *node);

PArgList *			create_parglist(void);
void				destroy_parglist(PArgList *list);
void				empty_parglist(PArgList *list);
void				copy_parglist(const PArgList *from, PArgList *to);
void				print_parglist(PArgList *list);

int					add_pargitem(PArgList *list, PArgListItem *item);
int					add_parg(PArgList *list, const char *name, void *arg, size_t argsize,
							PArgType type);
int					remove_parg(PArgList *list, PArgListItem *node);
void				set_parg(PArgListItem *node, const void *arg, size_t argsize,
							PArgType type);
					
					// Expensive call because this is a doubly-linked list. Cache the
					// value returned when possible.
int					count_pargs(PArgList *list);

int					add_parg_int8(PArgList *list, const char *name, int8_t arg);
int					add_parg_int16(PArgList *list, const char *name, int16_t arg);
int					add_parg_int32(PArgList *list, const char *name, int arg);
int					add_parg_int64(PArgList *list, const char *name, int64_t arg);

int					add_parg_float(PArgList *list, const char *name, float arg);
int					add_parg_double(PArgList *list, const char *name, double arg);

int					add_parg_bool(PArgList *list, const char *name, bool arg);
int					add_parg_char(PArgList *list, const char *name, char arg);
int					add_parg_string(PArgList *list, const char *name, const char *arg);

int					add_parg_point(PArgList *list, const char *name, float x,
									float y);
int					add_parg_rect(PArgList *list, const char *name, float left,
								float top, float right, float bottom);
int					add_parg_color(PArgList *list, const char *name, unsigned char red,
									unsigned char green, unsigned char blue, unsigned char alpha);
int					add_parg_pointer(PArgList *list, const char *name, void *ptr);
int					add_parg_list(PArgList *list, const char *name, const PArgList *childlist);

PArgListItem *		get_parg_first(PArgList *list);
PArgListItem *		get_parg_last(PArgList *list);
PArgListItem *		get_parg_next(PArgList *list, PArgListItem *item);
PArgListItem *		get_parg_previous(PArgList *list, PArgListItem *item);
PArgListItem *		get_parg_at(PArgList *list, int index);

PArgListItem *		find_parg(PArgList *list, const char *name, PArgListItem *startItem);
PArgListItem *		find_parg_index(PArgList *list, int index);
int					find_parg_int8(PArgList *list, const char *name, int8_t *out);
int					find_parg_int16(PArgList *list, const char *name, int16_t *out);
int					find_parg_int32(PArgList *list, const char *name, int *out);
int					find_parg_int64(PArgList *list, const char *name, int64_t *out);

int					find_parg_float(PArgList *list, const char *name, float *out);
int					find_parg_double(PArgList *list, const char *name, double *out);

int					find_parg_bool(PArgList *list, const char *name, bool *out);
int					find_parg_char(PArgList *list, const char *name, char *out);
int					find_parg_string(PArgList *list, const char *name, char **arg);

int					find_parg_point(PArgList *list, const char *name, float *x,
									float *y);
int					find_parg_rect(PArgList *list, const char *name, float *left,
									float *top, float *right, float *bottom);
int					find_parg_color(PArgList *list, const char *name, unsigned char *red,
									unsigned char *green, unsigned char *blue, unsigned char *alpha);
int					find_parg_pointer(PArgList *list, const char *name, void **ptr);
int					find_parg_list(PArgList *list, const char *name, PArgList **out);

/* -------------------------------------------------------------------------------------
	PValue-related definitions
   ------------------------------------------------------------------------------------- */
bool				pvalue_accepts_type(void *pval, char *type);
bool				pvalue_returns_type(void *pval, char *type);
int					pvalue_copy_value(void *from, void *to);
void				pvalue_get_type(void *pval, char **out);

/* -------------------------------------------------------------------------------------
	PProperty-related definitions
   ------------------------------------------------------------------------------------- */
void *				pproperty_create(const char *type);
void				pproperty_destroy(void *prop);
void *				pproperty_duplicate(void *prop);
void				pproperty_copy(void *from, void *to);
void				pproperty_set_name(void *prop, const char *name);
void				pproperty_get_name(void *prop, char **out);
bool				pproperty_is_read_only(void *prop);
void				pproperty_set_read_only(void *prop, bool value);
void				pproperty_set_enabled(void *prop, bool value);
bool				pproperty_is_enabled(void *prop);
void				pproperty_set_type(void *prop, const char *type);
void				pproperty_get_type(void *prop, char **out);
int					pproperty_set_value(void *prop, void *pvalue);
int					pproperty_get_value(void *prop, void *pvalue);
void				pproperty_get_value_as_string(void *prop, char **out);
void				pproperty_set_description(void *prop, const char *desc);
void				pproperty_get_description(void *prop, char **out);

/* -------------------------------------------------------------------------------------
	PData-related definitions
	
	PObject is a kind of PData, so all of these calls are valid for both.
   ------------------------------------------------------------------------------------- */
void *				pdata_create(void);
void				pdata_destroy(void *pdata);
void *				pdata_duplicate(void *pdata);
void				pdata_copy(void *from, void *to);

int					pdata_count_properties(void *pdata, const char *name);
void *				pdata_property_at(void *pdata, int index);
int					pdata_index_of_property(void *pdata, void *property);
void *				pdata_find_property(void *pdata, const char *name);

bool				pdata_add_property(void *pdata, void *prop, unsigned int flags,
										int index);
void *				pdata_remove_property_at(void *pdata, int index);
void				pdata_remove_property(void *pdata, void *prop);
unsigned int		pdata_property_flags_at(void *pdata, int index);
void				pdata_set_flags_for_property(void *pdata, void *prop, int flags);
unsigned int		pdata_flags_for_property(void *pdata, void *prop);

int					pdata_set_value_for_property(void *pdata, const char *name,
													void *pvalue);
int					pdata_get_value_for_property(void *pdata, const char *name,
													void *pvalue);
int					pdata_set_string_property(void *pdata, const char *name,
											const char *value);
int					pdata_set_int_property(void *pdata, const char *name,
											int64_t value);
int					pdata_set_bool_property(void *pdata, const char *name, bool value);
int					pdata_set_float_property(void *pdata, const char *name,
											float value);
int					pdata_set_rect_property(void *pdata, const char *name, float left,
											float top, float right, float bottom);
int					pdata_set_point_property(void *pdata, const char *name, float x,
											float y);
int					pdata_set_color_property(void *pdata, const char *name, unsigned char red,
											unsigned char green, unsigned char blue, unsigned char alpha);

int					pdata_get_string_property(void *pdata, const char *name,
											char **out);
int					pdata_get_int_property(void *pdata, const char *name,
												int64_t *value);
int					pdata_get_bool_property(void *pdata, const char *name,
											bool *value);
int					pdata_get_float_property(void *pdata, const char *name,
											float *value);
int					pdata_get_rect_property(void *pdata, const char *name,
											float *left, float *top,
											float *right, float *bottom);
int					pdata_get_point_property(void *pdata, const char *name, float *x,
											float *y);
int					pdata_get_color_property(void *pdata, const char *name,
											unsigned char *red, unsigned char *green,
											unsigned char *blue, unsigned char *alpha);
void				pdata_get_type(void *pdata, char **out);
void				pdata_get_friendly_type(void *pdata, char **out);
void				pdata_print_to_stream(void *pdata);


/* -------------------------------------------------------------------------------------
	PObject-related definitions
	
   ------------------------------------------------------------------------------------- */
void				pobjectspace_init(void);
void				pobjectspace_shutdown(void);
	
int					pobjectspace_count_types(void);
void				pobjectspace_type_at(int index, char **out);
void				pobjectspace_friendly_type_at(int index, char **out);
void *				pobjectspace_find_object(uint64_t id);

/* This method saves a load of code just to run the PApplication object */
int					run_app(const char *signature, MethodFunction setupFunc);

void *				pobject_create(const char *type);
void *				pobject_duplicate(void *pobj);
void				pobject_copy(void *from, void *to);
void				pobject_delete(void *pobj);
unsigned long		pobject_get_id(void *pobj);
int					pobject_run_method(void *pobj, const char *name, PArgList *in,
										PArgList *out, void *ptr);
void *				pobject_find_method(void *pobj, const char *name);
void *				pobject_method_at(void *pobj, int index);
int					pobject_count_methods(void *pobj);

bool				pobject_uses_interface(void *pobj, const char *name);
void				pobject_interface_at(void *pobj, int index,
										char **out);
int					pobject_count_interfaces(void *pobj);
void				pobject_print_to_stream(void *pobj);

void *				pobject_find_event(void *pobj, const char *name);
void *				pobject_event_at(void *pobj, int index);
int					pobject_count_events(void *pobj);
int					pobject_run_event(void *pobj, const char *name, PArgList *in,
										PArgList *out, void *ptr);
int					pobject_connect_event(void *pobj, const char *name,
										MethodFunction func, void *extraData);
void				event_set_code(void *event, const char *code);
void				event_get_code(void *event, const char **out);

/* -------------------------------------------------------------------------------------
	PMethod-related definitions
	
   ------------------------------------------------------------------------------------- */
void *				pmethodinterface_create(void);
void				pmethodinterface_destroy(void *pmethodinterface);
void				pmethodinterface_set_arg(void *pmi, int index, const char *name,
											PArgType type, const char *description);
void				pmethodinterface_add_arg(void *pmi, const char *name,
											PArgType type, const char *description);
void				pmethodinterface_remove_arg(void *pmi, int index);
void				pmethodinterface_arg_name_at(void *pmi, int index, char **out);
PArgType			pmethodinterface_arg_type_at(void *pmi, int index);
void				pmethodinterface_arg_desc_at(void *pmi, int index, char **out);
int					pmethodinterface_count_args(void *pmi);
int					pmethodinterface_find_arg(void *pmi, const char *name);

void				pmethodinterface_set_rval(void *pmi, int index, const char *name,
											PArgType type, const char *description);
void				pmethodinterface_add_rval(void *pmi, const char *name,
											PArgType type, const char *description);
void				pmethodinterface_remove_rval(void *pmi, int index);
void				pmethodinterface_rval_name_at(void *pmi, int index, char **out);
PArgType			pmethodinterface_rval_type_at(void *pmi, int index);
void				pmethodinterface_rval_desc_at(void *pmi, int index, char **out);
int					pmethodinterface_count_rvals(void *pmi);
int					pmethodinterface_find_rval(void *pmi, const char *name);


void *				pmethod_create(void);
void				pmethod_destroy(void *pmethod);
void				pmethod_set_name(void *pmethod, const char *name);
void				pmethod_get_name(void *pmethod, char **out);
void				pmethod_set_interface(void *pmethod, void *pmi);
void				pmethod_get_interface(void *pmethod, void *pmi);
void				pmethod_set_desc(void *pmethod, const char *name);
void				pmethod_get_desc(void *pmethod, char **out);
void				pmethod_set_function(void *pmethod, MethodFunction func);
MethodFunction		pmethod_get_function(void *pmethod);
void				pmethod_set_code(void *pmethod, const char *name);
void				pmethod_get_code(void *pmethod, char **out);
int					pmethod_run(void *pmethod, void *pobject, PArgList *in, PArgList *out,
								void *extraData);

#if defined(__cplusplus)
	}
#endif

#endif
