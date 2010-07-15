#include "ObjectItem.h"
#include "PObject.h"

ObjectItem::ObjectItem(PObject *obj, const char *namebase)
	:	BStringItem(""),
		fObject(obj)
{
	SetName(namebase);
}


void
ObjectItem::SetName(const char *namebase)
{
	if (fObject)
	{
		StringProperty *name = dynamic_cast<StringProperty*>(fObject->FindProperty("Name"));
		if (!name)
			name = dynamic_cast<StringProperty*>(fObject->FindProperty("Title"));
		
		if (name)
		{
			BString text = namebase ? namebase : "Object";
			text << ": " << name->GetValueAsString();
			SetText(text.String());
		}
	}
}


PObject *
ObjectItem::GetObject(void)
{
	return fObject;
}
