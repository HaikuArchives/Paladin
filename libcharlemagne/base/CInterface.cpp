#include "CInterface.h"
#include "PArgs.h"

#ifdef __cplusplus
	extern "C" {
#endif

int32_t
add_parg_char(void *args, const char *name, char arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddChar(name, arg);
}


int32_t
add_parg_rect(void *args, const char *name, float left,
									float top, float right, float bottom)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	return pargs->AddRect(name, BRect(left, top, right, bottom));
}


int32_t
add_parg_point(void *args, const char *name, float x, float y)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddPoint(name, BPoint(x, y));
}


int32_t
add_parg_string(void *args, const char *name, const char *arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddString(name, arg);
}


int32_t
add_parg_int8(void *args, const char *name, int8_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddInt8(name, arg);
}


int32_t
add_parg_int16(void *args, const char *name, int16_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddInt16(name, arg);
}


int32_t
add_parg_int32(void *args, const char *name, int32_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddInt32(name, arg);
}


int32_t
add_parg_int64(void *args, const char *name, int64_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddInt64(name, arg);
}


int32_t
add_parg_bool(void *args, const char *name, bool arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddBool(name, arg);
}


int32_t
add_parg_float(void *args, const char *name, float arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddFloat(name, arg);
}


int32_t
add_parg_double(void *args, const char *name, double arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddDouble(name, arg);
}


int32_t
add_parg_pointer(void *args, const char *name, void *ptr)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->AddPointer(name, ptr);
}


int32_t
add_parg_list(void *args, const char *name, void *list)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	PArgs *arg = static_cast<PArgs*>(list);
	
	return pargs->AddPArg(name, *arg);
}


int32_t
add_parg_color(void *args, const char *name, unsigned char red,
			unsigned char green, unsigned char blue, unsigned char alpha)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	rgb_color c = { red, green, blue, alpha };
	return pargs->AddColor(name, c);
}


#pragma mark - Find functions


int32_t
find_parg(void *args, const char *name, int32_t type, const void **data,
			int32_t *size)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindData(name, type, data, size);
}


int32_t
find_parg_index(void *args, const char *name, int32_t type, const void **data,
			int32_t *size, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindData(name, type, data, size, index);
}


int32_t
find_parg_int8(void *args, const char *name, int8_t *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt8(name, out);
}


int32_t
find_parg_int8_index(void *args, const char *name, int8_t *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt8(name, out, index);
}


int32_t
find_parg_int16(void *args, const char *name, int16_t *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt16(name, out);
}


int32_t
find_parg_int16_index(void *args, const char *name, int16_t *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt16(name, out, index);
}


int32_t
find_parg_int32(void *args, const char *name, int32_t *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt32(name, (int32*)out);
}


int32_t
find_parg_int32_index(void *args, const char *name, int32_t *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt32(name, (int32*)out, index);
}


int32_t
find_parg_int64(void *args, const char *name, int64_t *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt64(name, out);
}


int32_t
find_parg_int64_index(void *args, const char *name, int64_t *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindInt64(name, out, index);
}


int32_t
find_parg_float(void *args, const char *name, float *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindFloat(name, out);
}


int32_t
find_parg_float_index(void *args, const char *name, float *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindFloat(name, out, index);
}


int32_t
find_parg_double(void *args, const char *name, double *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindDouble(name, out);
}


int32_t
find_parg_double_index(void *args, const char *name, double *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindDouble(name, out, index);
}


int32_t
find_parg_bool(void *args, const char *name, bool *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindBool(name, out);
}


int32_t
find_parg_bool_index(void *args, const char *name, bool *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindBool(name, out, index);
}


int32_t
find_parg_char(void *args, const char *name, char *out)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindChar(name, out);
}


int32_t
find_parg_char_index(void *args, const char *name, char *out, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindChar(name, out, index);
}


int32_t
find_parg_string(void *args, const char *name, const char **arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindString(name, arg);
}


int32_t
find_parg_string_index(void *args, const char *name, const char **arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindString(name, arg, index);
}


int32_t
find_parg_point(void *args, const char *name, float *x, float *y)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	BPoint pt;
	status_t status = pargs->FindPoint(name, &pt);
	*x = pt.x;
	*y = pt.y;
	
	return status;
}


int32_t
find_parg_point_index(void *args, const char *name, float *x, float *y,
					int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	BPoint pt;
	status_t status = pargs->FindPoint(name, &pt, index);
	
	*x = pt.x;
	*y = pt.y;
	
	return status;
}


int32_t
find_parg_rect(void *args, const char *name, float *left,
									float *top, float *right, float *bottom)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	BRect r;
	status_t status = pargs->FindRect(name, &r);
	
	*left = r.left;
	*top = r.top;
	*right = r.right;
	*bottom = r.bottom;
	
	return status;
}


int32_t
find_parg_rect_index(void *args, const char *name, float *left,
					float *top, float *right, float *bottom, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	BRect r;
	status_t status = pargs->FindRect(name, &r, index);
	
	*left = r.left;
	*top = r.top;
	*right = r.right;
	*bottom = r.bottom;
	
	return status;
}


int32_t
find_parg_color(void *args, const char *name, unsigned char *red,
				unsigned char *green, unsigned char *blue, unsigned char *alpha)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	rgb_color c;
	status_t status = pargs->FindColor(name, &c);
	
	*red = c.red;
	*green = c.green;
	*blue = c.blue;
	*alpha = c.alpha;
	
	return status;
}


int32_t
find_parg_color_index(void *args, const char *name, unsigned char *red,
				unsigned char *green, unsigned char *blue, unsigned char *alpha,
				int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	rgb_color c;
	status_t status = pargs->FindColor(name, &c, index);
	
	*red = c.red;
	*green = c.green;
	*blue = c.blue;
	*alpha = c.alpha;
	
	return status;
}


int32_t
find_parg_pointer(void *args, const char *name, void **ptr)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindPointer(name, ptr);
}


int32_t
find_parg_pointer_index(void *args, const char *name, void **ptr, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->FindPointer(name, ptr, index);
}

#pragma mark - Replacement functions


int32_t
replace_parg_char(void *args, const char *name, char arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceChar(name, arg);
}


int32_t
replace_parg_char_index(void *args, const char *name, char arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceChar(name, arg, index);
}


int32_t
replace_parg_rect(void *args, const char *name, float left, float top, float right,
				float bottom)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	BRect r(left, top, right, bottom);
	return pargs->ReplaceRect(name, r);
}


int32_t
replace_parg_rect_index(void *args, const char *name, float left, float top,
				float right, float bottom, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	BRect r(left, top, right, bottom);
	return pargs->ReplaceRect(name, r, index);
}


int32_t
replace_parg_point(void *args, const char *name, float x, float y)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	BPoint pt(x, y);
	return pargs->ReplacePoint(name, pt);
}


int32_t
replace_parg_point_index(void *args, const char *name, float x, float y, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	BPoint pt(x, y);
	return pargs->ReplacePoint(name, pt, index);
}


int32_t
replace_parg_string(void *args, const char *name, const char *arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceString(name, arg);
}


int32_t
replace_parg_string_index(void *args, const char *name, const char *arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceString(name, arg, index);
}


int32_t
replace_parg_int8(void *args, const char *name, int8_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt8(name, arg);
}


int32_t
replace_parg_int8_index(void *args, const char *name, int8_t arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt8(name, arg, index);
}


int32_t
replace_parg_int16(void *args, const char *name, int16_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt16(name, arg);
}


int32_t
replace_parg_int16_index(void *args, const char *name, int16_t arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt16(name, arg, index);
}


int32_t
replace_parg_int32(void *args, const char *name, int32_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt32(name, arg);
}


int32_t
replace_parg_int32_index(void *args, const char *name, int32_t arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt32(name, arg, index);
}


int32_t
replace_parg_int64(void *args, const char *name, int64_t arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt64(name, arg);
}


int32_t
replace_parg_int64_index(void *args, const char *name, int64_t arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceInt64(name, arg, index);
}


int32_t
replace_parg_bool(void *args, const char *name, bool arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceBool(name, arg);
}


int32_t
replace_parg_bool_index(void *args, const char *name, bool arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceBool(name, arg, index);
}


int32_t
replace_parg_float(void *args, const char *name, float arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceFloat(name, arg);
}


int32_t
replace_parg_float_index(void *args, const char *name, float arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceFloat(name, arg, index);
}


int32_t
replace_parg_double(void *args, const char *name, double arg)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceDouble(name, arg);
}


int32_t
replace_parg_double_index(void *args, const char *name, double arg, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplaceDouble(name, arg, index);
}


int32_t
replace_parg_pointer(void *args, const char *name, void *ptr)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplacePointer(name, ptr);
}


int32_t
replace_parg_pointer_index(void *args, const char *name, void *ptr, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	return pargs->ReplacePointer(name, ptr, index);
}


int32_t
replace_parg_list(void *args, const char *name, void *list)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	PArgs *arg = static_cast<PArgs*>(list);
	if (!arg)
		return B_BAD_DATA;
	
	return pargs->ReplacePArg(name, *arg);
}


int32_t
replace_parg_list_index(void *args, const char *name, void *list, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	PArgs *arg = static_cast<PArgs*>(list);
	if (!arg)
		return B_BAD_DATA;
	
	return pargs->ReplacePArg(name, *arg, index);
}


int32_t
replace_parg_color(void *args, const char *name, unsigned char red,
					unsigned char green, unsigned char blue,
					unsigned char alpha)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	rgb_color c;
	c.red = red;
	c.green = green;
	c.blue = blue;
	c.alpha = alpha;
	return pargs->ReplaceColor(name, c);
}


int32_t
replace_parg_color_index(void *args, const char *name, unsigned char red,
						unsigned char green, unsigned char blue,
						unsigned char alpha, int32_t index)
{
	PArgs *pargs = static_cast<PArgs*>(args);
	if (!pargs)
		return B_BAD_DATA;
	
	rgb_color c;
	c.red = red;
	c.green = green;
	c.blue = blue;
	c.alpha = alpha;
	return pargs->ReplaceColor(name, c, index);
}





#ifdef __cplusplus
	}
#endif
