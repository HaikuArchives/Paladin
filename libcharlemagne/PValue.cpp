#include "PValue.h"
#include <stdlib.h>
#include <ClassInfo.h>
#include "PObjectBroker.h"

PValue::PValue(void)
{
	type = new BString();
}


PValue::~PValue(void)
{
	delete type;
}


BoolValue::BoolValue(void)
{
	value = new bool(false);
	*type = "bool";
}


BoolValue::BoolValue(const BoolValue &from)
{
	value =  new bool(*from.value);
	*type = "bool";
}


BoolValue::BoolValue(bool from)
{
	value = new bool(from);
	*type = "bool";
}


BoolValue::~BoolValue(void)
{
	delete value;
}


bool
BoolValue::AcceptsType(char *type)
{
	if (strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}


bool
BoolValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}

		
status_t
BoolValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*value = *b->value;
	}
	else if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*value = (*i->value == 0) ? false : true;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*value = (*f->value == 0.0) ? false : true;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*value = (s->value->CountChars() == 0) ? false : true;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
BoolValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*b->value = *value;
	}
	else if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*i->value = *value ? 1 : 0;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*f->value = *value ? 1.0 : 0.0;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = *value ? "true" : "false";
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
BoolValue::Duplicate(void) const
{
	return new BoolValue(*this);
}


StringValue::StringValue(const StringValue &from)
{
	value = new BString(*from.value);
	*type = "string";
}


StringValue::StringValue(BString from)
{
	value = new BString(from);
	*type = "string";
}


StringValue::StringValue(const char *from)
{
	value = new BString(from);
	*type = "string";
}


StringValue::~StringValue(void)
{
	delete value;
}


bool
StringValue::AcceptsType(char *type)
{
	if (strcasecmp(type, "string") == 0 ||
		strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "rect") == 0 ||
		strcasecmp(type, "point") == 0 ||
		strcasecmp(type, "color") == 0)
		return true;
	return false;
}


bool
StringValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "string") == 0 ||
		strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0)
		return true;
	return false;
}

		
status_t
StringValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*value = *s->value;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*value = *b->value ? "true" : "false";
	}
	else if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*value = "";
		*value << *i->value;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*value = "";
		*value << *f->value;
	}
	else if (pval->type->ICompare("rect") == 0)
	{
		RectValue *r = (RectValue*)pval;
		*value = "";
		*value << "(" << r->value->left << "," << r->value->top
			<< "," << r->value->right << "," << r->value->bottom << ")";
	}
	else if (pval->type->ICompare("point") == 0)
	{
		PointValue *p = (PointValue*)pval;
		*value = "";
		*value << "(" << p->value->x << "," << p->value->y << ")";
	}
	else if (pval->type->ICompare("color") == 0)
	{
		ColorValue *c = (ColorValue*)pval;
		*value = "";
		*value << "(" << (int)c->value->red << "," << (int)c->value->green << ","
			<< (int)c->value->blue << "," << (int)c->value->alpha << ")";
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
StringValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = *value;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*b->value = (value->CountChars() > 0);
	}
	else if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*i->value = atol(value->String());
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*f->value = atof(value->String());
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
StringValue::Duplicate(void) const
{
	return new StringValue(*this);
}


IntValue::IntValue(void)
{
	value = new int64(0LL);
	*type = "int";
}


IntValue::IntValue(const IntValue &from)
{
	value = new int64(*from.value);
	*type = "int";
}


IntValue::IntValue(int64 from)
{
	value = new int64(from);
	*type = "int";
}


IntValue::~IntValue(void)
{
	delete value;
}


bool
IntValue::AcceptsType(char *type)
{
	if (strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}


bool
IntValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}

		
status_t
IntValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*value = *i->value;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*value = *b->value ? 1 : 0;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*value = int64(*f->value);
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*value = atol(s->value->String());
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
IntValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*i->value = *value;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*b->value = (*value == 0) ? false : true;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*f->value = *value;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = "";
		*s->value << *value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
IntValue::Duplicate(void) const
{
	return new IntValue(*this);
}


FloatValue::FloatValue(void)
{
	value = new float();
	*type = "float";
}


FloatValue::FloatValue(const FloatValue &from)
{
	value =  new float(*from.value);
	*type = "float";
}


FloatValue::FloatValue(float from)
{
	value = new float(from);
	*type = "float";
}


FloatValue::~FloatValue(void)
{
	delete value;
}


bool
FloatValue::AcceptsType(char *type)
{
	if (strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}


bool
FloatValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "bool") == 0 ||
		strcasecmp(type, "int") == 0 ||
		strcasecmp(type, "float") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}

		
status_t
FloatValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*value = *f->value;
	}
	else if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*value = *i->value;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*value = *b->value ? 1 : 0;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*value = atof(s->value->String());
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
FloatValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*f->value = *value;
	}
	else if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*i->value = int64(*value);
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*b->value = (*value == 0.0) ? false : true;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = "";
		*s->value << *value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
FloatValue::Duplicate(void) const
{
	return new FloatValue(*this);
}


ColorValue::ColorValue(void)
{
	value = new rgb_color;
	
	*type = "color";
	value->red = 0;
	value->green = 0;
	value->blue = 0;
	value->alpha = 255;
}


ColorValue::ColorValue(const ColorValue &from)
{
	value = new rgb_color;
	*this = from;
}


ColorValue::ColorValue(rgb_color from)
{
	value = new rgb_color;
	*this = from;
}


ColorValue::ColorValue(uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	value = new rgb_color;
	*type = "color";
	SetValue(red,green,blue,alpha);
}


ColorValue::~ColorValue(void)
{
	delete value;
}


bool
ColorValue::AcceptsType(char *type)
{
	return (strcasecmp(type, "color") == 0);
}


bool
ColorValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "color") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}

		
status_t
ColorValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("color") == 0)
	{
		ColorValue *s = (ColorValue*)pval;
		*value = *s->value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


void
ColorValue::SetValue(uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	value->red = red;
	value->green = green;
	value->blue = blue;
	value->alpha = alpha;
}


status_t
ColorValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("color") == 0)
	{
		ColorValue *c = (ColorValue*)pval;
		*c->value = *value;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = "";
		*s->value << "(" << (int)value->red << "," << (int)value->green << ","
			<< (int)value->blue << "," << (int)value->alpha << ")";
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
ColorValue::Duplicate(void) const
{
	return new ColorValue(*this);
}


ColorValue &
ColorValue::operator=(const ColorValue &from)
{
	type = from.type;
	value->red = from.value->red;
	value->green = from.value->green;
	value->blue = from.value->blue;
	value->alpha = from.value->alpha;
	return *this;
}


ColorValue &
ColorValue::operator=(const rgb_color &from)
{
	*type = "color";
	value->red = from.red;
	value->green = from.green;
	value->blue = from.blue;
	value->alpha = from.alpha;
	return *this;
}


rgb_color
ColorValue::operator+(const rgb_color &from)
{
	rgb_color out = *value;
	out.red += from.red;
	out.green += from.green;
	out.blue += from.blue;
	out.alpha += from.alpha;
	return out;
}


rgb_color
ColorValue::operator+(const ColorValue &from)
{
	rgb_color out = *value;
	out.red += from.value->red;
	out.green += from.value->green;
	out.blue += from.value->blue;
	out.alpha += from.value->alpha;
	return out;
}


rgb_color
ColorValue::operator-(const rgb_color &from)
{
	rgb_color out = *value;
	out.red -= from.red;
	out.green -= from.green;
	out.blue -= from.blue;
	out.alpha -= from.alpha;
	return out;
}


rgb_color
ColorValue::operator-(const ColorValue &from)
{
	rgb_color out = *value;
	out.red -= from.value->red;
	out.green -= from.value->green;
	out.blue -= from.value->blue;
	out.alpha -= from.value->alpha;
	return out;
}


rgb_color
ColorValue::operator*(const rgb_color &from)
{
	rgb_color out = *value;
	out.red *= from.red;
	out.green *= from.green;
	out.blue *= from.blue;
	out.alpha *= from.alpha;
	return out;
}


rgb_color
ColorValue::operator*(const ColorValue &from)
{
	rgb_color out = *value;
	out.red *= from.value->red;
	out.green *= from.value->green;
	out.blue *= from.value->blue;
	out.alpha *= from.value->alpha;
	return out;
}


rgb_color
ColorValue::operator/(const rgb_color &from)
{
	rgb_color out = *value;
	out.red /= from.red;
	out.green /= from.green;
	out.blue /= from.blue;
	out.alpha /= from.alpha;
	return out;
}


rgb_color
ColorValue::operator/(const ColorValue &from)
{
	rgb_color out = *value;
	out.red /= from.value->red;
	out.green /= from.value->green;
	out.blue /= from.value->blue;
	out.alpha /= from.value->alpha;
	return out;
}


bool
ColorValue::operator==(const ColorValue &from)
{
	if (value->red == from.value->red &&
		value->green == from.value->green &&
		value->blue == from.value->blue &&
		value->alpha == from.value->alpha)
		return true;
	return false;
}


bool
ColorValue::operator==(const rgb_color &from)
{
	if (value->red == value->red &&
		value->green == value->green &&
		value->blue == value->blue &&
		value->alpha == value->alpha)
		return true;
	return false;
}


bool
ColorValue::operator!=(const ColorValue &from)
{
	if (value->red != from.value->red ||
		value->green != from.value->green ||
		value->blue != from.value->blue ||
		value->alpha != from.value->alpha)
		return true;
	return false;
}


bool
ColorValue::operator!=(const rgb_color &from)
{
	if (value->red != from.red ||
		value->green != from.green ||
		value->blue != from.blue ||
		value->alpha != from.alpha)
		return true;
	return false;
}


RectValue::RectValue(void)
{
	value = new BRect(0,0,0,0);
	*type = "rect";
}


RectValue::RectValue(const RectValue &from)
{
	value =  new BRect(*from.value);
	*type = "rect";
}


RectValue::RectValue(BRect from)
{
	value = new BRect(from);
	*type = "rect";
}


RectValue::~RectValue(void)
{
	delete value;
}


bool
RectValue::AcceptsType(char *type)
{
	return (strcasecmp(type, "rect") == 0);
}


bool
RectValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "rect") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}

		
status_t
RectValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("rect") == 0)
	{
		RectValue *r = (RectValue*)pval;
		*value = *r->value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
RectValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("rect") == 0)
	{
		RectValue *r = (RectValue*)pval;
		*r->value = *value;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = "";
		*s->value << "(" << value->left << "," << value->top
			<< "," << value->right << "," << value->bottom << ")";
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
RectValue::Duplicate(void) const
{
	return new RectValue(*this);
}



PointValue::PointValue(void)
{
	value = new BPoint(0,0);
	*type = "point";
}

PointValue::PointValue(const PointValue &from)
{
	value =  new BPoint(*from.value);
	*type = "point";
}


PointValue::PointValue(BPoint from)
{
	value = new BPoint(from);
	*type = "point";
}


PointValue::~PointValue(void)
{
	delete value;
}


bool
PointValue::AcceptsType(char *type)
{
	return (strcasecmp(type, "point") == 0);
}


bool
PointValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "point") == 0 ||
		strcasecmp(type, "string") == 0)
		return true;
	return false;
}

		
status_t
PointValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("point") == 0)
	{
		PointValue *p = (PointValue*)pval;
		*value = *p->value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
PointValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("point") == 0)
	{
		PointValue *p = (PointValue*)pval;
		*p->value = *value;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = "";
		*s->value << "(" << value->x << "," << value->y << ")";
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
PointValue::Duplicate(void) const
{
	return new PointValue(*this);
}



MessageValue::MessageValue(void)
{
	value = new BMessage;
	*type = "message";
}


MessageValue::MessageValue(const MessageValue &from)
{
	value = new BMessage(*from.value);
	*type = "message";
}


MessageValue::MessageValue(const int32 &what)
{
	value = new BMessage(what);
	*type = "message";
}


MessageValue::MessageValue(BMessage from)
{
	value = new BMessage(from);
	*type = "message";
}


MessageValue::~MessageValue(void)
{
	delete value;
}


bool
MessageValue::AcceptsType(char *type)
{
	return (strcasecmp(type, "message") == 0);
}


bool
MessageValue::ReturnsType(char *type)
{
	if (strcasecmp(type, "message") == 0)
		return true;
	return false;
}

		
status_t
MessageValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("message") == 0)
	{
		MessageValue *p = (MessageValue*)pval;
		*value = *p->value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
MessageValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("message") == 0)
	{
		MessageValue *p = (MessageValue*)pval;
		*p->value = *value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
MessageValue::Duplicate(void) const
{
	return new MessageValue(*this);
}



ListValue::ListValue(void)
{
	value = new BObjectList<PValue>(20, true);
	*type = "list";
}

ListValue::ListValue(const ListValue &from)
{
	value = new BObjectList<PValue>(20, true);
	*type = "list";
	*this = from;
}


ListValue::ListValue(BObjectList<PValue> from)
{
	value = new BObjectList<PValue>(20, true);
	*type = "list";
}


ListValue::~ListValue(void)
{
	delete value;
}


bool
ListValue::AcceptsType(char *type)
{
	return (strcasecmp(type, "list") == 0);
}


bool
ListValue::ReturnsType(char *type)
{
	return (strcasecmp(type, "list") == 0);
}

		
status_t
ListValue::SetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("list") == 0)
	{
		ListValue *from = dynamic_cast<ListValue*>(pval);
		if (!from)
			return B_BAD_VALUE;
		
		*this = *from->value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
ListValue::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("list") == 0)
	{
		ListValue *list = dynamic_cast<ListValue*>(pval);
		if (!list)
			return B_BAD_VALUE;
		
		*list = *this;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


PValue *
ListValue::Duplicate(void) const
{
	return new ListValue(*this);
}


ListValue &
ListValue::operator=(const ListValue &from)
{
	value->MakeEmpty();
	
	for (int32 i = 0; i < from.value->CountItems(); i++)
		value->AddItem(from.value->ItemAt(i)->Duplicate());
	
	return *this;
}


PValue *
CreatePValue(const char *type)
{
	if (!type)
		return NULL;
	
	if (strcasecmp(type, "StringValue") == 0)
		return new StringValue;
	else if (strcasecmp(type, "IntValue") == 0)
		return new IntValue;
	else if (strcasecmp(type, "BoolValue") == 0)
		return new BoolValue;
	else if (strcasecmp(type, "ColorValue") == 0)
		return new ColorValue;
	else if (strcasecmp(type, "RectValue") == 0)
		return new RectValue;
	else if (strcasecmp(type, "PointValue") == 0)
		return new PointValue;
	else if (strcasecmp(type, "FloatValue") == 0)
		return new FloatValue;
	else if (strcasecmp(type, "ListValue") == 0)
		return new ListValue;
	else if (strcasecmp(type, "MessageValue") == 0)
		return new MessageValue;
	else if (strcasecmp(type, "PValue") == 0)
		return new PValue;
	
	return NULL;
}

void
DestroyPValue(PValue *val)
{
	delete val;
}
