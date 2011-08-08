#ifndef CINTERFACE_H
#define CINTERFACE_H

/* -------------------------------------------------------------------------------------
	This file contains all of the C bindings to the PDesigner object system. It is not
	pretty, but C is not well-suited to anything resembling OOP.
   ------------------------------------------------------------------------------------- */

#include <stdint.h>
#include <sys/types.h>
#include <TypeConstants.h>

#ifdef __cplusplus
	extern "C" {
#else
	#ifndef SWIG
		typedef unsigned char bool;
	#endif
#endif

enum
{
	B_LIST_TYPE = B_MESSAGE_TYPE
};

typedef int (*MethodFunction)(void *pobject, void *inArgs, void *outArgs, void *ptr);

int32_t					add_parg_char(void *args, const char *name, char arg);
int32_t					add_parg_rect(void *args, const char *name, float left,
									float top, float right, float bottom);
int32_t					add_parg_point(void *args, const char *name, float x, float y);
int32_t					add_parg_string(void *args, const char *name, const char *arg);
int32_t					add_parg_int8(void *args, const char *name, int8_t arg);
int32_t					add_parg_int16(void *args, const char *name, int16_t arg);
int32_t					add_parg_int32(void *args, const char *name, int32_t arg);
int32_t					add_parg_int64(void *args, const char *name, int64_t arg);

int32_t					add_parg_bool(void *args, const char *name, bool arg);
int32_t					add_parg_float(void *args, const char *name, float arg);
int32_t					add_parg_double(void *args, const char *name, double arg);

int32_t					add_parg_pointer(void *args, const char *name, void *ptr);
// No BMessenger version
int32_t					add_parg_list(void *args, const char *name, void *list);
// No Ref version
int32_t					add_parg_color(void *args, const char *name, unsigned char red,
									unsigned char green, unsigned char blue,
									unsigned char alpha);

int32_t					find_parg(void *args, const char *name, int32_t type,
								const void **data, int32_t *size);
int32_t					find_parg_index(void *args, const char *name, int32_t type,
								const void **data, int32_t *size, int32_t index);
int32_t					find_parg_int8(void *args, const char *name, int8_t *out);
int32_t					find_parg_int8_index(void *args, const char *name, int8_t *out,
											int32_t index);
int32_t					find_parg_int16(void *args, const char *name, int16_t *out);
int32_t					find_parg_int16_index(void *args, const char *name, int16_t *out,
											int32_t index);
int32_t					find_parg_int32(void *args, const char *name, int32_t *out);
int32_t					find_parg_int32_index(void *args, const char *name, int32_t *out,
											int32_t index);
int32_t					find_parg_int64(void *args, const char *name, int64_t *out);
int32_t					find_parg_int64_index(void *args, const char *name, int64_t *out,
											int32_t index);

int32_t					find_parg_float(void *args, const char *name, float *out);
int32_t					find_parg_float_index(void *args, const char *name, float *out,
											int32_t index);
int32_t					find_parg_double(void *args, const char *name, double *out);
int32_t					find_parg_double_index(void *args, const char *name, double *out,
											int32_t index);

int32_t					find_parg_bool(void *args, const char *name, bool *out);
int32_t					find_parg_bool_index(void *args, const char *name, bool *out,
											int32_t index);
int32_t					find_parg_char(void *args, const char *name, char *out);
int32_t					find_parg_char_index(void *args, const char *name, char *out,
											int32_t index);
int32_t					find_parg_string(void *args, const char *name, const char **arg);
int32_t					find_parg_string_index(void *args, const char *name, const char **arg,
											int32_t index);

int32_t					find_parg_point(void *args, const char *name, float *x,
									float *y);
int32_t					find_parg_point_index(void *args, const char *name, float *x,
									float *y, int32_t index);
int32_t					find_parg_rect(void *args, const char *name, float *left,
									float *top, float *right, float *bottom);
int32_t					find_parg_rect_index(void *args, const char *name, float *left,
									float *top, float *right, float *bottom, int32_t index);
int32_t					find_parg_color(void *args, const char *name, unsigned char *red,
									unsigned char *green, unsigned char *blue, unsigned char *alpha);
int32_t					find_parg_color_index(void *args, const char *name, unsigned char *red,
									unsigned char *green, unsigned char *blue,
									unsigned char *alpha, int32_t index);
int32_t					find_parg_pointer(void *args, const char *name, void **ptr);
int32_t					find_parg_pointer_index(void *args, const char *name, void **ptr,
											int32_t index);


int32_t					replace_parg_char(void *args, const char *name, char arg);
int32_t					replace_parg_rect(void *args, const char *name, float left,
										float top, float right, float bottom);
int32_t					replace_parg_point(void *args, const char *name, float x, float y);
int32_t					replace_parg_string(void *args, const char *name, const char *arg);
int32_t					replace_parg_int8(void *args, const char *name, int8_t arg);
int32_t					replace_parg_int16(void *args, const char *name, int16_t arg);
int32_t					replace_parg_int32(void *args, const char *name, int32_t arg);
int32_t					replace_parg_int64(void *args, const char *name, int64_t arg);
int32_t					replace_parg_bool(void *args, const char *name, bool arg);
int32_t					replace_parg_float(void *args, const char *name, float arg);
int32_t					replace_parg_double(void *args, const char *name, double arg);
int32_t					replace_parg_pointer(void *args, const char *name, void *ptr);
// No BMessenger version
int32_t					replace_parg_list(void *args, const char *name, void *list);
// No Ref version
int32_t					replace_parg_color(void *args, const char *name, unsigned char red,
										unsigned char green, unsigned char blue,
										unsigned char alpha);
#if defined(__cplusplus)
	}
#endif

#endif
