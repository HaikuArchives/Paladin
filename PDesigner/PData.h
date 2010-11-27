#ifndef PDATA_H
#define PDATA_H


#include <Archivable.h>
#include <Message.h>
#include <String.h>
#include "ObjectList.h"
#include "PProperty.h"

enum
{
	PROPERTY_READ_ONLY			= 0x00000001,
	PROPERTY_ALLOW_MULTIPLE		= 0x00000010,
	PROPERTY_HIDE_IN_EDITOR		= 0x00000100
};


class PropertyData
{
public:
	PropertyData(PProperty *p, uint32 f) { value = p; flags = f; }
	~PropertyData(void) { delete value; }
	
	PProperty 	*value;
	uint32 		flags;
};

class PData : public BArchivable
{
public:
							PData(void);
							PData(BMessage *msg);
							PData(const char *name);
							PData(const PData &from);
			PData &		operator=(const PData &from);
			PProperty *		operator[](const char *name);
			PProperty *		operator[](const BString &name);
	virtual					~PData(void);
	
	// Create() is needed for the object broker's use. Always uses a class' default constructor
	static	PData *			Create(void);
	virtual	PData *			Duplicate(void) const;
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	
			int32			CountProperties(const char *name = NULL) const;
			PProperty *		PropertyAt(const int32 &index) const;
			int32			IndexOfProperty(PProperty *p) const;
			
			PProperty *		FindProperty(const BString &name, const int32 &index = 0) const;
			PProperty *		FindProperty(const char *name, const int32 &index = 0) const;
	
	virtual	bool			AddProperty(PProperty *p, uint32 flags = 0,int32 index = -1);
	virtual PProperty *		RemoveProperty(const int32 &index);
	virtual	void			RemoveProperty(PProperty *p);
			uint32			PropertyFlagsAt(const int32 &index) const;
	virtual	void			SetFlagsForProperty(PProperty *p, const int32 &index);
			uint32			FlagsForProperty(PProperty *p) const;
	
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;

	virtual	status_t		SetStringProperty(const char *name, const char *value, const int32 &index = 0);
	virtual	status_t		SetIntProperty(const char *name, const int64 &value, const int32 &index = 0);
	virtual	status_t		SetBoolProperty(const char *name, const bool &value, const int32 &index = 0);
	virtual	status_t		SetFloatProperty(const char *name, const float &value, const int32 &index = 0);
	virtual	status_t		SetRectProperty(const char *name, const BRect &value, const int32 &index = 0);
	virtual	status_t		SetPointProperty(const char *name, const BPoint &value, const int32 &index = 0);
	virtual	status_t		SetColorProperty(const char *name, const rgb_color &value, const int32 &index = 0);
	virtual	status_t		SetColorProperty(const char *name, const uint8 &red, const uint8 &green,
											const uint8 &blue, const uint8 &alpha = 255,
											const int32 &index = 0);
	
	virtual	status_t		GetStringProperty(const char *name, BString &value, const int32 &index = 0);
	virtual	status_t		GetIntProperty(const char *name, int64 &value, const int32 &index = 0);
	virtual	status_t		GetBoolProperty(const char *name, bool &value, const int32 &index = 0);
	virtual	status_t		GetFloatProperty(const char *name, float &value, const int32 &index = 0);
	virtual	status_t		GetRectProperty(const char *name, BRect &value, const int32 &index = 0);
	virtual	status_t		GetPointProperty(const char *name, BPoint &value, const int32 &index = 0);
	virtual	status_t		GetColorProperty(const char *name, rgb_color &value, const int32 &index = 0);
	
			BString			GetType(void) const;
			BString			GetFriendlyType(void) const;
			
	virtual	void			PrintToStream(void);
	
protected:
	BString					fType;
	BString					fFriendlyType;
	
private:
	BObjectList<PropertyData>	*fPropertyList;
};


#endif
