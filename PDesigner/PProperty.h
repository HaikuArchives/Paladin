#ifndef PPROPERTY_H
#define PPROPERTY_H

#include <Archivable.h>
#include <Message.h>
#include <String.h>

#include "PValue.h"

class PObject;

class PProperty : public BArchivable
{
public:
							PProperty(void);
							PProperty(const char *name);
							PProperty(BMessage *msg);
							PProperty(const PProperty &from);
	virtual					~PProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	void			SetName(const BString &name);
			void			SetName(const char *name);
			BString			GetName(void) const;
	
			bool			IsReadOnly(void) const;
			void			SetReadOnly(const bool &value);
	
	virtual	void			SetEnabled(const bool &value);
			bool			IsEnabled(void) const;
	
	virtual	void			SetType(const BString &type);
			void			SetType(const char *type);
			BString			GetType(void) const;
	
	virtual	status_t		SetValue(PValue *value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
	virtual	void			SetDescription(const char *desc);
	virtual	void			SetDescription(const BString &desc);
	virtual	BString			GetDescription(void);

private:
	BString					*fType,
							*fName,
							*fDescription;
	bool					fReadOnly,
							fEnabled;
};


class StringProperty : public PProperty
{
public:
							StringProperty(void);
							StringProperty(const char *name, const char *value,
											const char *desc = NULL);
							StringProperty(const BString &name, const BString &value);
							StringProperty(PValue *value);
							StringProperty(BMessage *msg);
							~StringProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(const BString &value);
			status_t		SetValue(const char *value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	StringValue				*fStringValue;
};


class BoolProperty : public PProperty
{
public:
							BoolProperty(void);
							BoolProperty(PValue *value);
							BoolProperty(const char *name, const bool &value,
										const char *desc = NULL);
							BoolProperty(BMessage *msg);
	virtual					~BoolProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(bool value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	BoolValue				*fBoolValue;
};


class IntProperty : public PProperty
{
public:
							IntProperty(void);
							IntProperty(PValue *value);
							IntProperty(const char *name, const int64 &value,
										const char *desc = NULL);
							IntProperty(BMessage *msg);
	virtual					~IntProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(int64 value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	IntValue				*fIntValue;
};


class FloatProperty : public PProperty
{
public:
							FloatProperty(void);
							FloatProperty(PValue *value);
							FloatProperty(const char *name, const float &value,
										const char *desc = NULL);
							FloatProperty(BMessage *msg);
	virtual					~FloatProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(float value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	FloatValue				*fFloatValue;
};


class ColorProperty : public PProperty
{
public:
							ColorProperty(void);
							ColorProperty(PValue *value);
							ColorProperty(const char *name, const rgb_color &value,
										const char *desc = NULL);
							ColorProperty(const char *name, const uint8 &red, const uint8 &green,
										const uint8 &blue, const char *desc = NULL);
							ColorProperty(BMessage *msg);
	virtual					~ColorProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(const rgb_color &value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	ColorValue				*fColorValue;
};


class RectProperty : public PProperty
{
public:
							RectProperty(void);
							RectProperty(PValue *value);
							RectProperty(const char *name, const BRect &value,
										const char *desc = NULL);
							RectProperty(BMessage *msg);
	virtual					~RectProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(const BRect &value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	RectValue				*fRectValue;
};


class PointProperty : public PProperty
{
public:
							PointProperty(void);
							PointProperty(PValue *value);
							PointProperty(const char *name, const BPoint &value,
										const char *desc = NULL);
							PointProperty(BMessage *msg);
	virtual					~PointProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(const BPoint &value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	PointValue				*fPointValue;
};


class MessageProperty : public PProperty
{
public:
							MessageProperty(void);
							MessageProperty(PValue *value);
							MessageProperty(const char *name, const BMessage &value,
										const char *desc = NULL);
							MessageProperty(const char *name, const int32 &what,
										const char *desc = NULL);
							MessageProperty(BMessage *msg);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
			status_t		SetValue(const BMessage &value);
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	MessageValue				fMessageValue;
};


#endif
