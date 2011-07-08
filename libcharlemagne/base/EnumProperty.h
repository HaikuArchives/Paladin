#ifndef ENUMPROPERTY_H
#define ENUMPROPERTY_H

#include "ObjectList.h"
#include "PValue.h"
#include "PProperty.h"

class EnumPair
{
public:
	EnumPair(void)
	{
		name = new BString();
		value = new int32;
	}
	EnumPair(const char *n,const int32 &v)
	{
		name = new BString(n);
		value = new int32(v);
	}
	~EnumPair(void)
	{
		delete name;
		delete value;
	}
	
	BString *name;
	int32	*value;
};

// The EnumProperty class is unusual in that when it is initially created, it doesn't have
// any values that can be set. Further setup is needed -- adding items to the value set.
// As such, this class is meant more as a base for other classes, such as a WindowFlagsProperty.
class EnumProperty : public PProperty
{
public:
							EnumProperty(void);
							EnumProperty(PValue *value);
							EnumProperty(BMessage *msg);
	virtual					~EnumProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
	
	virtual	status_t		SetValue(const int32 &value);
	virtual status_t		SetValue(const char *value);
	
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
			void			AddValuePair(const char *name, const int32 &value);
			void			RemoveValuePair(const char *name);
			void			RemoveValuePair(const int32 &value);
			BString			PairNameAt(const int32 &index) const;
			int32			PairValueAt(const int32 &index) const;
			int32			CountValuePairs(void) const;

protected:
	EnumPair *				FindItem(const char *name);
	EnumPair *				FindItem(const int32 &value);
	
private:
	BObjectList<EnumPair>	*fList;
	EnumPair				*fValueItem;
};


class EnumFlagProperty : public EnumProperty
{
public:
							EnumFlagProperty(void);
							EnumFlagProperty(PValue *value);
							EnumFlagProperty(BMessage *msg);
	virtual					~EnumFlagProperty(void);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		SetValue(PValue *value);
	
			status_t		SetValue(const int32 &value);
	
	virtual	status_t		GetValue(PValue *value);
	virtual	BString			GetValueAsString(void) const;
	
private:
	int32					fValue;
};

#endif
