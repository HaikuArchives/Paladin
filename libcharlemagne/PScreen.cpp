#include "PScreen.h"

#include <Application.h>
#include <Screen.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PScreenBackend : public BScreen
{
public:
			PScreenBackend(PObject *owner);


private:
	PObject *fOwner;
};


PScreen::PScreen(void)
	:	PObject()
{
	fType = "PScreen";
	fFriendlyType = "Screen";
	AddInterface("PScreen");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PScreen::PScreen(BMessage *msg)
	:	PObject(msg)
{
	fType = "PScreen";
	fFriendlyType = "Screen";
	AddInterface("PScreen");
	
	
	
	InitBackend();
}


PScreen::PScreen(const char *name)
	:	PObject(name)
{
	fType = "PScreen";
	fFriendlyType = "Screen";
	AddInterface("PScreen");
	
	InitMethods();
	InitBackend();
}


PScreen::PScreen(const PScreen &from)
	:	PObject(from)
{
	fType = "PScreen";
	fFriendlyType = "Screen";
	AddInterface("PScreen");
	
	InitMethods();
	InitBackend();
}


PScreen::~PScreen(void)
{
}


BArchivable *
PScreen::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PScreen"))
		return new PScreen(data);

	return NULL;
}


PObject *
PScreen::Create(void)
{
	return new PScreen();
}


PObject *
PScreen::Duplicate(void) const
{
	return new PScreen(*this);
}
status_t
PScreen::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BScreen *backend = (BScreen*)fBackend;
	if (str.ICompare("IsValid") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsValid());
	else if (str.ICompare("ColorSpace") == 0)
		((IntProperty*)prop)->SetValue(backend->ColorSpace());
	else if (str.ICompare("Frame") == 0)
		((RectProperty*)prop)->SetValue(backend->Frame());
	else
	{
		return PObject::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PScreen::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BoolValue boolval;
	ColorValue colorval;
	FloatValue floatval;
	IntValue intval;
	PointValue pointval;
	RectValue rectval;
	StringValue stringval;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;

	return PObject::SetProperty(name, value, index);
}


BScreen *
PScreen::GetBackend(void) const
{
	return fBackend;
}


void
PScreen::InitBackend(void)
{
	if (!fBackend)
		fBackend = new PScreenBackend(this);
	StringValue sv("A representation of the screen");
	SetProperty("Description", &sv);
}


void
PScreen::InitProperties(void)
{
	SetStringProperty("Description", "A representation of the screen");

	AddProperty(new BoolProperty("IsValid", true, "Is the screen object valid?"));
	AddProperty(new IntProperty("ColorSpace", B_RGB_32_BIT, "Color space of the screen"));
	AddProperty(new RectProperty("Frame", BRect(0,0,639, 479), "Size of the screen in pixels"));
}


void
PScreen::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PScreenBackend::PScreenBackend(PObject *owner)
	:	BScreen(),
		fOwner(owner)
{
}


