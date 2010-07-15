#include "EnumProperty.h"

EnumProperty::EnumProperty(void)
	:	fValueItem(NULL)
{
	fList = new BObjectList<EnumPair>(20,true);
	SetType("EnumProperty");
}


EnumProperty::EnumProperty(PValue *value)
	:	fValueItem(NULL)
{
	fList = new BObjectList<EnumPair>(20,true);
	SetType("EnumProperty");
	SetValue(value);
}


EnumProperty::EnumProperty(BMessage *msg)
	:	PProperty(msg),
		fValueItem(NULL)
{
	fList = new BObjectList<EnumPair>(20,true);
	
	int32 i = 0;
	BString name;
	int32 value;
	while (msg->FindString("pairname",i,&name) == B_OK)
	{
		msg->FindInt32("pairvalue",i,&value);
		AddValuePair(name.String(),value);
		i++;
	}
	
	if (msg->FindInt32("value",&value) == B_OK)
		fValueItem = FindItem(value);
	
}

	
EnumProperty::~EnumProperty(void)
{
	delete fList;
}


PProperty *
EnumProperty::Create(void)
{
	return new EnumProperty();
}


PProperty *
EnumProperty::Duplicate(void)
{
	return new EnumProperty(*this);
}


BArchivable *
EnumProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "EnumProperty"))
		return new EnumProperty(data);

	return NULL;
}


status_t
EnumProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	for (int32 i = 0; i < fList->CountItems(); i++)
	{
		EnumPair *item = fList->ItemAt(i);
		status = data->AddString("pairname",*item->name);
		if (status != B_OK)
			break;
		status = data->AddInt32("pairvalue",*item->value);
		if (status != B_OK)
			break;
	}
	
	if (fValueItem)
		status = data->AddInt32("value",*fValueItem->value);
	
	if (status == B_OK)
		status = data->AddString("class","EnumProperty");
	
	return status;
}

		
status_t
EnumProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	if (value->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)value;
		fValueItem = FindItem(*i->value);
	}
	else if (value->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)value;
		fValueItem = FindItem(s->value->String());
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
EnumProperty::SetValue(const int32 &value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	fValueItem = FindItem(value);
	return B_OK;
}


status_t
EnumProperty::SetValue(const char *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	if (!value || strlen(value) < 1)
		fValueItem = NULL;
	else
		fValueItem = FindItem(value);
	return B_OK;
}


status_t
EnumProperty::GetValue(PValue *pval)
{
	if (!pval || !fValueItem)
		return B_ERROR;
	
	if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*i->value = *fValueItem->value;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*b->value = (*fValueItem->value == 0) ? false : true;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*f->value = *fValueItem->value;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		s->value->SetTo(*fValueItem->name);
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


BString
EnumProperty::GetValueAsString(void) const
{
	return (fValueItem) ? *fValueItem->name : BString();
}


void
EnumProperty::AddValuePair(const char *name, const int32 &value)
{
	if (!name)
		return;
	
	EnumPair *item = FindItem(name);
	if (item)
	{
		*item->value = value;
		return;
	}
	item = new EnumPair(name,value);
	fList->AddItem(item);
}


void
EnumProperty::RemoveValuePair(const char *name)
{
	if (!name)
		return;
	
	EnumPair *item = FindItem(name);
	if (!item)
		return;
	
	fList->RemoveItem(item);
}


void
EnumProperty::RemoveValuePair(const int32 &value)
{
	EnumPair *item = FindItem(value);
	if (!item)
		return;
	
	fList->RemoveItem(item);
}


BString
EnumProperty::PairNameAt(const int32 &index) const
{
	EnumPair *item = fList->ItemAt(index);
	return (item) ? *item->name : BString();
}


int32
EnumProperty::PairValueAt(const int32 &index) const
{
	EnumPair *item = fList->ItemAt(index);
	return (item) ? *item->value : -1;
}


int32
EnumProperty::CountValuePairs(void) const
{
	return fList->CountItems();
}


EnumPair *
EnumProperty::FindItem(const char *name)
{
	if (!name)
		return NULL;
	
	for (int32 i = 0; i < fList->CountItems(); i++)
	{
		EnumPair *item = fList->ItemAt(i);
		if (item->name->ICompare(name) == 0)
			return item;
	}
	
	return NULL;
}


EnumPair *
EnumProperty::FindItem(const int32 &value)
{
	for (int32 i = 0; i < fList->CountItems(); i++)
	{
		EnumPair *item = fList->ItemAt(i);
		if (*item->value == value)
			return item;
	}
	
	return NULL;
}



EnumFlagProperty::EnumFlagProperty(void)
	:	fValue(0)
{
	SetType("EnumFlagProperty");
}


EnumFlagProperty::EnumFlagProperty(PValue *value)
	:	fValue(0)
{
	SetType("EnumFlagProperty");
	SetValue(value);
}


EnumFlagProperty::EnumFlagProperty(BMessage *msg)
	:	EnumProperty(msg),
		fValue(0)
{
	if (msg->FindInt32("flagset",&fValue) != B_OK)
		fValue = 0;
}

	
EnumFlagProperty::~EnumFlagProperty(void)
{
}


PProperty *
EnumFlagProperty::Create(void)
{
	return new EnumFlagProperty();
}


PProperty *
EnumFlagProperty::Duplicate(void)
{
	return new EnumFlagProperty(*this);
}


BArchivable *
EnumFlagProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "EnumFlagProperty"))
		return new EnumFlagProperty(data);

	return NULL;
}


status_t
EnumFlagProperty::Archive(BMessage *data, bool deep) const
{
	status_t status = PProperty::Archive(data,deep);
	if (status != B_OK)
		return status;
	
	if (fValue)
		status = data->AddInt32("flagset",fValue);
	
	if (status == B_OK)
		status = data->AddString("class","EnumFlagProperty");
	
	return status;
}

		
status_t
EnumFlagProperty::SetValue(PValue *value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	if (value->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)value;
		fValue = *i->value;
	}
	else if (value->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)value;
		EnumPair *pair = FindItem(s->value->String());
		if (!pair)
			return B_BAD_VALUE;
		fValue = *pair->value;
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


status_t
EnumFlagProperty::SetValue(const int32 &value)
{
	if (IsReadOnly())
		return B_READ_ONLY;
	
	fValue = value;
	return B_OK;
}


status_t
EnumFlagProperty::GetValue(PValue *pval)
{
	if (!pval)
		return B_ERROR;
	
	if (pval->type->ICompare("int") == 0)
	{
		IntValue *i = (IntValue*)pval;
		*i->value = fValue;
	}
	else if (pval->type->ICompare("bool") == 0)
	{
		BoolValue *b = (BoolValue*)pval;
		*b->value = fValue ? true : false;
	}
	else if (pval->type->ICompare("float") == 0)
	{
		FloatValue *f = (FloatValue*)pval;
		*f->value = fValue;
	}
	else if (pval->type->ICompare("string") == 0)
	{
		StringValue *s = (StringValue*)pval;
		*s->value = GetValueAsString();
	}
	else
		return B_BAD_VALUE;
	
	return B_OK;
}


BString
EnumFlagProperty::GetValueAsString(void) const
{
	BString out;
	for (int32 i = 0; i < CountValuePairs(); i++)
	{
		if (fValue & PairValueAt(i))
			out << " | " << PairNameAt(i);
	}
	
	if (out.FindFirst(" | ") == 0)
		out.RemoveFirst(" | ");
	
	return out;
}

