#ifndef PVALUE_H
#define PVALUE_H

#include <InterfaceDefs.h>
#include <Rect.h>
#include <String.h>
#include <Message.h>

#include "ObjectList.h"

class PObject;

class PValue
{
public:
							PValue(void);
		virtual				~PValue(void);
		
		virtual bool		AcceptsType(PValue *pval) { return true; }
		virtual bool		ReturnsType(PValue *pval) { return false; }
		
		virtual	status_t	SetValue(PValue *pval) { return B_OK; }
		virtual	status_t	GetValue(PValue *pval) { return B_OK; }
		
		virtual	PValue *	Duplicate(void) const { return new PValue(); }
		
				BString		*type;
};


class BoolValue : public PValue
{
public:
							BoolValue(void);
							BoolValue(const BoolValue &from);
							BoolValue(bool from);
							~BoolValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	bool 		operator==(const BoolValue &from) { return *value = *from.value; }
		inline	bool 		operator==(const bool &from) { return *value = from; }
		inline	BoolValue &	operator=(const BoolValue &from) { *value = *from.value; return *this; }
		inline	BoolValue &	operator=(const bool &from) { *value = from; return *this; }
		inline	bool		operator!(void) { return !(*value); }
		
				bool		*value;
};


class StringValue : public PValue
{
public:
							StringValue(void) { *type = "string"; value = new BString(); }
							StringValue(const StringValue &from);
							StringValue(BString from);
							StringValue(const char *from);
							~StringValue(void);
							
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	StringValue &	operator=(const StringValue &from) { *value = *from.value; return *this; }
		inline	StringValue &	operator=(const BString &from) { *value = from; return *this; }
		
		inline	bool 		operator==(const StringValue &from) { return *value == *from.value; }
		inline	bool 		operator==(const BString &from) { return *value == from; }
		inline	bool 		operator!=(const StringValue &from) { return *value != *from.value; }
		inline	bool 		operator!=(const BString &from) { return *value != from; }
		inline	bool		operator<(const BString &from) { return *value < from; }
		inline	bool		operator<(const StringValue &from)  { return *value < *from.value; }
		inline	bool		operator>(const BString &from) { return *value > from; }
		inline	bool		operator>(const StringValue &from)  { return *value > *from.value; }
		inline	bool		operator<=(const BString &from) { return *value <= from; }
		inline	bool		operator<=(const StringValue &from)  { return *value <= *from.value; }
		inline	bool		operator>=(const BString &from) { return *value >= from; }
		inline	bool		operator>=(const StringValue &from)  { return *value >= *from.value; }
				
				BString		*value;
};

class IntValue : public PValue
{
public:
							IntValue(void);
							IntValue(const IntValue &from);
							IntValue(int64 from);
							~IntValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	IntValue &	operator=(const IntValue &from) { *value = *from.value; return *this; }
		inline	IntValue &	operator=(const int64 &from) { *value = from; return *this; }
		
		inline	int64		operator+(const int64 &from) { return *value + from; }
		inline	int64		operator+(const IntValue &from)  { return *value + *from.value; }
		inline	int64		operator-(const int64 &from) { return *value - from; }
		inline	int64		operator-(const IntValue &from)  { return *value - *from.value; }
		inline	int64		operator*(const int64 &from) { return *value * from; }
		inline	int64		operator*(const IntValue &from)  { return *value * *from.value; }
		inline	int64		operator/(const int64 &from) { return *value / from; }
		inline	int64		operator/(const IntValue &from)  { return *value / *from.value; }
		inline	int64		operator%(const int64 &from) { return *value % from; }
		inline	int64		operator%(const IntValue &from)  { return *value % *from.value; }
		
		inline	bool		operator!(void) { return !(*value); }
		inline	bool 		operator==(const IntValue &from) { return *value == *from.value; }
		inline	bool 		operator==(const int64 &from) { return *value == from; }
		inline	bool 		operator!=(const IntValue &from) { return *value != *from.value; }
		inline	bool 		operator!=(const int64 &from) { return *value != from; }
		inline	bool		operator<(const int64 &from) { return *value < from; }
		inline	bool		operator<(const IntValue &from)  { return *value < *from.value; }
		inline	bool		operator>(const int64 &from) { return *value > from; }
		inline	bool		operator>(const IntValue &from)  { return *value > *from.value; }
		inline	bool		operator<=(const int64 &from) { return *value <= from; }
		inline	bool		operator<=(const IntValue &from)  { return *value <= *from.value; }
		inline	bool		operator>=(const int64 &from) { return *value >= from; }
		inline	bool		operator>=(const IntValue &from)  { return *value >= *from.value; }
		
				int64		*value;
};

class FloatValue : public PValue
{
public:
							FloatValue(void);
							FloatValue(const FloatValue &from);
							FloatValue(float from);
							~FloatValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	FloatValue &	operator=(const FloatValue &from) { *value = *from.value; return *this; }
		inline	FloatValue &	operator=(const float &from) { *value = from; return *this; }
		
		inline	float		operator+(const float &from) { return *value + from; }
		inline	float		operator+(const FloatValue &from)  { return *value + *from.value; }
		inline	float		operator-(const float &from) { return *value - from; }
		inline	float		operator-(const FloatValue &from)  { return *value - *from.value; }
		inline	float		operator*(const float &from) { return *value * from; }
		inline	float		operator*(const FloatValue &from)  { return *value * *from.value; }
		inline	float		operator/(const float &from) { return *value / from; }
		inline	float		operator/(const FloatValue &from)  { return *value / *from.value; }
		
		inline	bool		operator!(void) { return !(*value); }
		inline	bool		operator==(const FloatValue &from) { return *value == *from.value; }
		inline	bool 		operator==(const float &from) { return *value == from; }
		inline	bool 		operator!=(const FloatValue &from) { return *value != *from.value; }
		inline	bool 		operator!=(const float &from) { return *value != from; }
		inline	bool		operator<(const float &from) { return *value < from; }
		inline	bool		operator<(const FloatValue &from)  { return *value < *from.value; }
		inline	bool		operator>(const float &from) { return *value > from; }
		inline	bool		operator>(const FloatValue &from)  { return *value > *from.value; }
		inline	bool		operator<=(const float &from) { return *value <= from; }
		inline	bool		operator<=(const FloatValue &from)  { return *value <= *from.value; }
		inline	bool		operator>=(const float &from) { return *value >= from; }
		inline	bool		operator>=(const FloatValue &from)  { return *value >= *from.value; }
		
				float		*value;
};

class ColorValue : public PValue
{
public:
							ColorValue(void);
							ColorValue(const ColorValue &from);
							ColorValue(rgb_color from);
							ColorValue(uint8 red, uint8 green, uint8 blue, uint8 alpha = 255);
							~ColorValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
				void		SetValue(uint8 red, uint8 green, uint8 blue, uint8 alpha = 255);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		ColorValue &	operator=(const ColorValue &from);
		ColorValue &	operator=(const rgb_color &from);
		
		rgb_color		operator+(const rgb_color &from);
		rgb_color		operator+(const ColorValue &from);
		rgb_color		operator-(const rgb_color &from);
		rgb_color		operator-(const ColorValue &from);
		rgb_color		operator*(const rgb_color &from);
		rgb_color		operator*(const ColorValue &from);
		rgb_color		operator/(const rgb_color &from);
		rgb_color		operator/(const ColorValue &from);
		
		bool 			operator==(const ColorValue &from);
		bool 			operator==(const rgb_color &from);
		bool 			operator!=(const ColorValue &from);
		bool 			operator!=(const rgb_color &from);
		
		rgb_color		*value;
};


class RectValue : public PValue
{
public:
							RectValue(void);
							RectValue(const RectValue &from);
							RectValue(BRect from);
							~RectValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	BRect 		operator==(const RectValue &from) { return *value = *from.value; }
		inline	BRect 		operator==(const BRect &from) { return *value = from; }
		inline	RectValue &	operator=(const RectValue &from) { *value = *from.value; return *this; }
		inline	RectValue &	operator=(const BRect &from) { *value = from; return *this; }
		
				BRect		*value;
};


class PointValue : public PValue
{
public:
							PointValue(void);
							PointValue(const PointValue &from);
							PointValue(BPoint from);
							~PointValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	BPoint 		operator==(const PointValue &from) { return *value = *from.value; }
		inline	BPoint 		operator==(const BPoint &from) { return *value = from; }
		inline	PointValue &	operator=(const PointValue &from) { *value = *from.value; return *this; }
		inline	PointValue &	operator=(const BPoint &from) { *value = from; return *this; }
		
				BPoint		*value;
};


class MessageValue : public PValue
{
public:
								MessageValue(void);
								MessageValue(const MessageValue &from);
								MessageValue(const int32 &what);
								MessageValue(BMessage from);
								~MessageValue(void);
		
		virtual bool			AcceptsType(PValue *pval);
		virtual bool			ReturnsType(PValue *pval);
		
		virtual	status_t		SetValue(PValue *value);
		virtual	status_t		GetValue(PValue *value);
		
		virtual	PValue *		Duplicate(void) const;
		
		inline	BMessage 		operator==(const MessageValue &from) { return *value = *from.value; }
		inline	BMessage 		operator==(const BMessage &from) { return *value = from; }
		inline	MessageValue &	operator=(const MessageValue &from) { *value = *from.value; return *this; }
		inline	MessageValue &	operator=(const BMessage &from) { *value = from; return *this; }
		
				BMessage		*value;
};


class ListValue : public PValue
{
public:
							ListValue(void);
							ListValue(const ListValue &from);
							ListValue(BObjectList<PValue> from);
							~ListValue(void);
		
		virtual bool		AcceptsType(PValue *pval);
		virtual bool		ReturnsType(PValue *pval);
		
		virtual	status_t	SetValue(PValue *value);
		virtual	status_t	GetValue(PValue *value);
		
		virtual	PValue *	Duplicate(void) const;
		
		inline	ListValue &	operator=(const ListValue &from);
		
				BObjectList<PValue>	*value;
};

#endif
