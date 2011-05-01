#include "PBitmap.h"

#include <Application.h>
#include <Bitmap.h>
#include <Screen.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

class PBitmapBackend
{
public:
				PBitmapBackend(PObject *owner);
	status_t	InitCheck(void) const;
	bool		IsValid(void) const;
	
	area_id		Area(void) const;
	void *		Bits(void) const;
	int32		BitsLength(void) const;
	int32		BytesPerRow(void) const;
	color_space	ColorSpace(void) const;
	BRect		Bounds(void) const;
	
	status_t	SetDrawingFlags(uint32 flags);
	uint32		Flags(void);
	
	void		Set(const BRect &frame);
	
	void		SetBits(const void *data, int32 length,
						int32 offset, color_space colorSpace);
	
	void		AddChild(BView *view);
	bool		RemoveChild(BView *view);
	int32		CountChildren(void) const;
	BView *		ChildAt(int32 index) const;
	BView *		FindView(const char *name) const;
	BView *		FindView(BPoint pt) const;
	bool		Lock(void);
	void		Unlock(void);
	bool		IsLocked(void) const;
	
private:
	PObject *fOwner;
	BBitmap	*fBitmap;
};


PBitmap::PBitmap(void)
	:	PObject()
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PBitmap::PBitmap(BMessage *msg)
	:	PObject(msg)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	
	
	
	InitBackend();
}


PBitmap::PBitmap(const char *name)
	:	PObject(name)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	
	InitMethods();
	InitBackend();
}


PBitmap::PBitmap(const PBitmap &from)
	:	PObject(from)
{
	fType = "PBitmap";
	fFriendlyType = "Bitmap";
	AddInterface("PBitmap");
	
	InitMethods();
	InitBackend();
}


PBitmap::~PBitmap(void)
{
}


BArchivable *
PBitmap::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PBitmap"))
		return new PBitmap(data);

	return NULL;
}


PObject *
PBitmap::Create(void)
{
	return new PBitmap();
}


PObject *
PBitmap::Duplicate(void) const
{
	return new PBitmap(*this);
}


status_t
PBitmap::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	PBitmapBackend *backend = fBackend;
	if (str.ICompare("IsValid") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsValid());
	else if (str.ICompare("ColorSpace") == 0)
		((IntProperty*)prop)->SetValue(backend->ColorSpace());
	else if (str.ICompare("Bounds") == 0)
		((RectProperty*)prop)->SetValue(backend->Bounds());
	else
	{
		return PObject::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PBitmap::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BScreen *backend = (BScreen*)fBackend;
	
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


PBitmapBackend *
PBitmap::GetBackend(void) const
{
	return fBackend;
}


void
PBitmap::InitBackend(void)
{
	if (!fBackend)
		fBackend = new PBitmapBackend(this);
	StringValue sv("A representation of the screen");
	SetProperty("Description", &sv);
}


void
PBitmap::InitProperties(void)
{
	SetStringProperty("Description", "A representation of the screen");

	AddProperty(new BoolProperty("IsValid", true, "Is the screen object valid?"));
	AddProperty(new IntProperty("ColorSpace", B_RGB_32_BIT, "Color space of the screen"));
	AddProperty(new RectProperty("Frame", BRect(0,0,639, 479), "Size of the screen in pixels"));
}


void
PBitmap::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PBitmapBackend::PBitmapBackend(PObject *owner)
	:	fOwner(owner)
{
}


status_t
PBitmapBackend::InitCheck(void) const
{
}


bool
PBitmapBackend::IsValid(void) const
{
}


area_id
PBitmapBackend::Area(void) const
{
}


void *
PBitmapBackend::Bits(void) const
{
}


int32
PBitmapBackend::BitsLength(void) const
{
}


int32
PBitmapBackend::BytesPerRow(void) const
{
}


color_space
PBitmapBackend::ColorSpace(void) const
{
}


BRect
PBitmapBackend::Bounds(void) const
{
}


status_t
PBitmapBackend::SetDrawingFlags(uint32 flags)
{
}


uint32
PBitmapBackend::Flags(void)
{
}


void
PBitmapBackend::Set(const BRect &frame)
{
}


void
PBitmapBackend::SetBits(const void *data, int32 length,
					int32 offset, color_space colorSpace)
{
}


void
PBitmapBackend::AddChild(BView *view)
{
}


bool
PBitmapBackend::RemoveChild(BView *view)
{
}


int32
PBitmapBackend::CountChildren(void) const
{
}


BView *
PBitmapBackend::ChildAt(int32 index) const
{
}


BView *
PBitmapBackend::FindView(const char *name) const
{
}


BView *
PBitmapBackend::FindView(BPoint pt) const
{
}


bool
PBitmapBackend::Lock(void)
{
}


void
PBitmapBackend::Unlock(void)
{
}


bool
PBitmapBackend::IsLocked(void) const
{
}

