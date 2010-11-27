#include "PProperty.h"
#include <ClassInfo.h>
#include "PObject.h"
#include "PObjectBroker.h"

PropertyRoster gPropertyRoster;

PProperty::PProperty(void)
	:	fReadOnly(false),
		fEnabled(true)
{
	fType = new BString();
	fName = new BString();
	fDescription = new BString();
}


PProperty::PProperty(const char *name)
	:	fReadOnly(false)
{
	fType = new BString();
	fName = new BString(name);
	fDescription = new BString();
}


PProperty::PProperty(BMessage *msg)
	:	BArchivable(msg)
{
	fType = new BString();
	fName = new BString();
	fDescription = new BString();
	
	BString str;
	if (msg->FindString("name",&str) == B_OK)
		*fName = str;
	
	if (msg->FindString("type",&str) == B_OK)
		*fType = str;
	
	if (msg->FindString("description",&str) == B_OK)
		*fDescription = str;
	
	bool b;
	if (msg->FindBool("readonly",&b) == B_OK)
		fReadOnly = b;
}


PProperty::PProperty(const PProperty &from)
{
	fType = new BString();
	fName = new BString();
	fDescription = new BString();
	
	if (!fReadOnly)
	{
		*fName = *from.fName;
		*fType = *from.fType;
	}
}


PProperty::~PProperty(void)
{
	delete fType;
	delete fName;
	delete fDescription;
}


PProperty *
PProperty::Create(void)
{
	return new PProperty();
}


PProperty *
PProperty::Duplicate(void)
{
	return new PProperty(*this);
}


BArchivable *
PProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PProperty"))
		return new PProperty(data);

	return NULL;
}


status_t
PProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = BArchivable::Archive(data, deep);
	data->AddString("class","PProperty");
	
	if (status == B_OK)
		status = data->AddString("name",*fName);
	
	if (status == B_OK)
		status = data->AddString("type",*fType);
	
	if (status == B_OK)
		status = data->AddString("description",*fDescription);
	
	if (status == B_OK)
		status = data->AddBool("readonly",fReadOnly);
	return status;
}


void
PProperty::SetName(const BString &name)
{
	*fName = name;
}


void
PProperty::SetName(const char *name)
{
	SetName(BString(name));
}


BString
PProperty::GetName(void) const
{
	return *fName;
}


bool
PProperty::IsReadOnly(void) const
{
	return false;
}


void
PProperty::SetReadOnly(const bool &value)
{
	fReadOnly = value;
}


void
PProperty::SetEnabled(const bool &value)
{
	fEnabled = value;
}


bool
PProperty::IsEnabled(void) const
{
	return fEnabled;
}

	
void
PProperty::SetType(const BString &type)
{
	if (!fReadOnly)
		*fType = type;
}


void
PProperty::SetType(const char *type)
{
	SetType(BString(type));
}


BString
PProperty::GetType(void) const
{
	return *fType;
}


status_t
PProperty::SetValue(PValue *data)
{
	// Base PObjects don't have a value
	return B_OK;
}


status_t
PProperty::GetValue(PValue *pval)
{
	// Does nothing because PObject instances lack a value
	return B_OK;
}


BString
PProperty::GetValueAsString(void) const
{
	return BString();
}


void
PProperty::SetDescription(const char *desc)
{
	*fDescription = desc;
}


void
PProperty::SetDescription(const BString &desc)
{
	*fDescription = desc;
}


BString
PProperty::GetDescription(void)
{
	return *fDescription;
}


StringProperty::StringProperty(void)
{
	fStringValue = new StringValue();
	
	SetType("StringProperty");
}


StringProperty::StringProperty(const char *name, const char *value, const char *desc)
{
	fStringValue = new StringValue();
	
	SetType("StringProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


StringProperty::StringProperty(const BString &name, const BString &value)
{
	fStringValue = new StringValue();
	
	SetType("StringProperty");
	SetName(name);
	SetValue(value);
}


StringProperty::StringProperty(PValue *value)
{
	fStringValue = new StringValue();
	
	SetType("StringProperty");
	SetValue(value);
}


StringProperty::StringProperty(BMessage *msg)
	:	PProperty(msg)
{
	fStringValue = new StringValue();
	
	BString str;
	if (msg->FindString("value",&str) == B_OK)
		*fStringValue = str;
}

	
StringProperty::~StringProperty(void)
{
	delete fStringValue;
}


PProperty *
StringProperty::Create(void)
{
	return new StringProperty();
}


PProperty *
StringProperty::Duplicate(void)
{
	return new StringProperty(*this);
}


BArchivable *
StringProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "StringProperty"))
		return new StringProperty(data);

	return NULL;
}


status_t
StringProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddString("value",*fStringValue->value);
	return status;
}

		
status_t
StringProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	return fStringValue->SetValue(value);
}


status_t
StringProperty::SetValue(const char *string)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fStringValue = BString(string);
	return B_OK;
}


status_t
StringProperty::SetValue(const BString &string)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fStringValue = string;
	return B_OK;
}


status_t
StringProperty::GetValue(PValue *pval)
{
	return fStringValue->GetValue(pval);
}


BString
StringProperty::GetValueAsString(void) const
{
	return *fStringValue->value;
}


BoolProperty::BoolProperty(void)
{
	fBoolValue = new BoolValue();
	SetType("BoolProperty");
}


BoolProperty::BoolProperty(PValue *value)
{
	fBoolValue = new BoolValue();
	SetType("BoolProperty");
	SetValue(value);
}


BoolProperty::BoolProperty(const char *name, const bool &value, const char *desc)
{
	fBoolValue = new BoolValue();
	SetType("BoolProperty");
	SetName(name);
	SetValue(value);
	SetDescription(NULL);
}


BoolProperty::BoolProperty(BMessage *msg)
	:	PProperty(msg)
{
	fBoolValue = new BoolValue();
	bool b;
	if (msg->FindBool("value",&b) == B_OK)
		*fBoolValue = b;
}

	
BoolProperty::~BoolProperty(void)
{
	delete fBoolValue;
}


PProperty *
BoolProperty::Create(void)
{
	return new BoolProperty();
}


PProperty *
BoolProperty::Duplicate(void)
{
	return new BoolProperty(*this);
}


BArchivable *
BoolProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "BoolProperty"))
		return new BoolProperty(data);

	return NULL;
}


status_t
BoolProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddBool("value",fBoolValue->value);
	return status;
}

		
status_t
BoolProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	return fBoolValue->SetValue(value);
}


status_t
BoolProperty::SetValue(bool b)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	*fBoolValue = b;
	return B_OK;
}


status_t
BoolProperty::GetValue(PValue *pval)
{
	return fBoolValue->GetValue(pval);
}


BString
BoolProperty::GetValueAsString(void) const
{
	return BString(fBoolValue->value ? "true" : "false");
}


IntProperty::IntProperty(void)
{
	fIntValue = new IntValue();
	SetType("IntProperty");
}


IntProperty::IntProperty(PValue *value)
{
	fIntValue = new IntValue();
	SetType("IntProperty");
	SetValue(value);
}


IntProperty::IntProperty(const char *name, const int64 &value, const char *desc)
{
	fIntValue = new IntValue();
	SetType("IntProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


IntProperty::IntProperty(BMessage *msg)
	:	PProperty(msg)
{
	fIntValue = new IntValue();
	int64 val;
	if (msg->FindInt64("value",&val) == B_OK)
		*fIntValue = val;
}

	
IntProperty::~IntProperty(void)
{
	delete fIntValue;
}


PProperty *
IntProperty::Create(void)
{
	return new IntProperty();
}


PProperty *
IntProperty::Duplicate(void)
{
	return new IntProperty(*this);
}


BArchivable *
IntProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "IntProperty"))
		return new IntProperty(data);

	return NULL;
}


status_t
IntProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddInt64("value",*fIntValue->value);
	return status;
}

		
status_t
IntProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	return fIntValue->SetValue(value);
}


status_t
IntProperty::SetValue(int64 val)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fIntValue = val;
	return B_OK;
}


status_t
IntProperty::GetValue(PValue *pval)
{
	return fIntValue->GetValue(pval);
}


BString
IntProperty::GetValueAsString(void) const
{
	BString str;
	str << *fIntValue->value;
	return str;
}


FloatProperty::FloatProperty(void)
{
	fFloatValue = new FloatValue();
	SetType("FloatProperty");
}


FloatProperty::FloatProperty(PValue *value)
{
	fFloatValue = new FloatValue();
	SetType("FloatProperty");
	SetValue(value);
}


FloatProperty::FloatProperty(const char *name, const float &value, const char *desc)
{
	fFloatValue = new FloatValue();
	SetType("FloatProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


FloatProperty::FloatProperty(BMessage *msg)
	:	PProperty(msg)
{
	fFloatValue = new FloatValue();
	float val;
	if (msg->FindFloat("value",&val) == B_OK)
		*fFloatValue = val;
}

	
FloatProperty::~FloatProperty(void)
{
	delete fFloatValue;
}


PProperty *
FloatProperty::Create(void)
{
	return new FloatProperty();
}


PProperty *
FloatProperty::Duplicate(void)
{
	return new FloatProperty(*this);
}


BArchivable *
FloatProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "FloatProperty"))
		return new FloatProperty(data);

	return NULL;
}


status_t
FloatProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddFloat("value",*fFloatValue->value);
	return status;
}

		
status_t
FloatProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	return fFloatValue->SetValue(value);
}


status_t
FloatProperty::SetValue(float f)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fFloatValue = f;
	return B_OK;
}


status_t
FloatProperty::GetValue(PValue *pval)
{
	return fFloatValue->GetValue(pval);
}


BString
FloatProperty::GetValueAsString(void) const
{
	BString str;
	str << *fFloatValue->value;
	return str;
}


ColorProperty::ColorProperty(void)
{
	fColorValue = new ColorValue();
	SetType("ColorProperty");
}


ColorProperty::ColorProperty(PValue *value)
{
	fColorValue = new ColorValue();
	SetType("ColorProperty");
	SetValue(value);
}


ColorProperty::ColorProperty(const char *name, const rgb_color &value, const char *desc)
{
	fColorValue = new ColorValue();
	SetType("ColorProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


ColorProperty::ColorProperty(const char *name, const uint8 &red, const uint8 &green,
							const uint8 &blue, const char *desc)
{
	fColorValue = new ColorValue();
	SetType("ColorProperty");
	SetName(name);
	SetDescription(desc);
	
	rgb_color c = { red,green,blue,255 };
	SetValue(c);
}


ColorProperty::ColorProperty(BMessage *msg)
	:	PProperty(msg)
{
	fColorValue = new ColorValue();
	rgb_color *c;
	ssize_t s;
	if (msg->FindData("value",B_RGB_COLOR_TYPE,(const void**)&c,&s) == B_OK)
		*fColorValue = *c;
}

	
ColorProperty::~ColorProperty(void)
{
	delete fColorValue;
}


PProperty *
ColorProperty::Create(void)
{
	return new ColorProperty();
}


PProperty *
ColorProperty::Duplicate(void)
{
	return new ColorProperty(*this);
}


BArchivable *
ColorProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "ColorProperty"))
		return new ColorProperty(data);

	return NULL;
}


status_t
ColorProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddData("value",B_RGB_COLOR_TYPE,(const void**)fColorValue->value,
							sizeof(rgb_color));
	return status;
}

		
status_t
ColorProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	return fColorValue->SetValue(value);
}


status_t
ColorProperty::SetValue(const rgb_color &value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fColorValue = value;
	return B_OK;
}


status_t
ColorProperty::GetValue(PValue *pval)
{
	return fColorValue->GetValue(pval);
}


BString
ColorProperty::GetValueAsString(void) const
{
	BString str;
	str << "Color(" << (int)fColorValue->value->red << "," << (int)fColorValue->value->green
		<< "," << (int)fColorValue->value->blue << "," << (int)fColorValue->value->alpha << ")";
	return str;
}


RectProperty::RectProperty(void)
{
	fRectValue = new RectValue();
	SetType("RectProperty");
}


RectProperty::RectProperty(PValue *value)
{
	fRectValue = new RectValue();
	SetType("RectProperty");
	SetValue(value);
}


RectProperty::RectProperty(const char *name, const BRect &value, const char *desc)
{
	fRectValue = new RectValue();
	SetType("RectProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


RectProperty::RectProperty(BMessage *msg)
	:	PProperty(msg)
{
	fRectValue = new RectValue();
	BRect b;
	if (msg->FindRect("value",&b) == B_OK)
		*fRectValue = b;
}

	
RectProperty::~RectProperty(void)
{
	delete fRectValue;
}


PProperty *
RectProperty::Create(void)
{
	return new RectProperty();
}


PProperty *
RectProperty::Duplicate(void)
{
	return new RectProperty(*this);
}


BArchivable *
RectProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "RectProperty"))
		return new RectProperty(data);

	return NULL;
}


status_t
RectProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddRect("value",*fRectValue->value);
	return status;
}

		
status_t
RectProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	return fRectValue->SetValue(value);
}


status_t
RectProperty::SetValue(const BRect &r)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fRectValue = r;
	return B_OK;
}


status_t
RectProperty::GetValue(PValue *pval)
{
	return fRectValue->GetValue(pval);
}


BString
RectProperty::GetValueAsString(void) const
{
	BString str;
	str << "BRect(" << fRectValue->value->left << "," << fRectValue->value->top
		<< "," << fRectValue->value->right << "," << fRectValue->value->bottom << ")";
	return str;
}


PointProperty::PointProperty(void)
{
	fPointValue = new PointValue();
	SetType("PointProperty");
}


PointProperty::PointProperty(PValue *value)
{
	fPointValue = new PointValue();
	SetType("PointProperty");
	SetValue(value);
}


PointProperty::PointProperty(const char *name, const BPoint &value, const char *desc)
{
	fPointValue = new PointValue();
	SetType("PointProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


PointProperty::PointProperty(BMessage *msg)
	:	PProperty(msg)
{
	fPointValue = new PointValue();
	BPoint b;
	if (msg->FindPoint("value",&b) == B_OK)
		*fPointValue = b;
}

	
PointProperty::~PointProperty(void)
{
	delete fPointValue;
}


PProperty *
PointProperty::Create(void)
{
	return new PointProperty();
}


PProperty *
PointProperty::Duplicate(void)
{
	return new PointProperty(*this);
}


BArchivable *
PointProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PointProperty"))
		return new PointProperty(data);

	return NULL;
}


status_t
PointProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddPoint("value",*fPointValue->value);
	return status;
}

		
status_t
PointProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	return fPointValue->SetValue(value);
}


status_t
PointProperty::SetValue(const BPoint &p)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	*fPointValue = p;
	return B_OK;
}


status_t
PointProperty::GetValue(PValue *pval)
{
	return fPointValue->GetValue(pval);
}


BString
PointProperty::GetValueAsString(void) const
{
	BString str;
	str << "BPoint(" << fPointValue->value->x << "," << fPointValue->value->y << ")";
	return str;
}


MessageProperty::MessageProperty(void)
{
	SetType("MessageProperty");
}


MessageProperty::MessageProperty(PValue *value)
{
	SetType("MessageProperty");
	SetValue(value);
}


MessageProperty::MessageProperty(const char *name, const BMessage &value, const char *desc)
{
	SetType("MessageProperty");
	SetName(name);
	SetValue(value);
	SetDescription(desc);
}


MessageProperty::MessageProperty(const char *name, const int32 &what, const char *desc)
{
	SetType("MessageProperty");
	SetName(name);
	
	BMessage msg(what);
	SetValue(msg);
	SetDescription(desc);
}


MessageProperty::MessageProperty(BMessage *msg)
	:	PProperty(msg)
{
	BMessage b;
	if (msg->FindMessage("value",&b) == B_OK)
		fMessageValue = b;
}

	
PProperty *
MessageProperty::Create(void)
{
	return new MessageProperty();
}


PProperty *
MessageProperty::Duplicate(void)
{
	return new MessageProperty(*this);
}


BArchivable *
MessageProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "MessageProperty"))
		return new MessageProperty(data);

	return NULL;
}


status_t
MessageProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	status = data->AddMessage("value",fMessageValue.value);
	return status;
}

		
status_t
MessageProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	return fMessageValue.SetValue(value);
}


status_t
MessageProperty::SetValue(const BMessage &p)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	fMessageValue = p;
	return B_OK;
}


status_t
MessageProperty::GetValue(PValue *pval)
{
	return fMessageValue.GetValue(pval);
}


BString
MessageProperty::GetValueAsString(void) const
{
	return BString("BMessage()");
}

class PMetaProperty
{
public:
	PMetaProperty(const char *typestr, MakeFromArchiveFunc objfunc,
				MakePropertyFunc func) { type = typestr; arcfunc = objfunc; createfunc = func; }
	
	MakeFromArchiveFunc arcfunc;
	MakePropertyFunc createfunc;
	BString type;
};


PropertyRoster::PropertyRoster(void)
	:	fPropertyList(20, true)
{
	fPropertyList.AddItem(new PMetaProperty("PProperty",PProperty::Instantiate,PProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("StringProperty",StringProperty::Instantiate,
												StringProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("BoolProperty",BoolProperty::Instantiate,
												BoolProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("IntProperty",IntProperty::Instantiate,
												IntProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("FloatProperty",FloatProperty::Instantiate,
												FloatProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("ColorProperty",ColorProperty::Instantiate,
												ColorProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("RectProperty",RectProperty::Instantiate,
												RectProperty::Create));
	fPropertyList.AddItem(new PMetaProperty("PointProperty",PointProperty::Instantiate,
												PointProperty::Create));
}


PropertyRoster::~PropertyRoster(void)
{
}


PProperty *
PropertyRoster::MakeProperty(const char *type, BMessage *msg) const
{
	if (!type)
		return NULL;
	
	PMetaProperty *info = NULL;
	for (int32 i = 0; i < fPropertyList.CountItems(); i++)
	{
		PMetaProperty *temp = fPropertyList.ItemAt(i);
		if (temp->type.ICompare(type) == 0)
		{
			info = temp;
			break;
		}
	}
	
	if (info)
	{
		if (msg)
			return (PProperty*)info->arcfunc(msg);
		else
			return info->createfunc();
	}
	
	return NULL;
}


int32
PropertyRoster::CountProperties(void) const
{
	return fPropertyList.CountItems();
}


BString
PropertyRoster::PropertyAt(const int32 &index) const
{
	BString str;
	PMetaProperty *info = fPropertyList.ItemAt(index);
	if (info)
		str = info->type;
	return str;
}


PMetaProperty *
PropertyRoster::FindProperty(const char *type)
{
	for (int32 i = 0; i < fPropertyList.CountItems(); i++)
	{
		PMetaProperty *pinfo = fPropertyList.ItemAt(i);
		if (pinfo->type == type)
			return pinfo;
	}
	
	return NULL;
}

