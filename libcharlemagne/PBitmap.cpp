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
	
	uint32		Flags(void);
	
	void		Initialize(const BRect &bounds, uint32 flags,
							color_space space, int32 bytesPerRow,
							screen_id screenID);
	void		Initialize(const BBitmap &source, uint32 flags);
	
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
	if (str.ICompare("AreaID") == 0)
		((IntProperty*)prop)->SetValue(backend->Area());
	else if (str.ICompare("BytesPerRow") == 0)
		((IntProperty*)prop)->SetValue(backend->BytesPerRow());
	else if (str.ICompare("ColorSpace") == 0)
		((IntProperty*)prop)->SetValue((int64)backend->ColorSpace());
	else if (str.ICompare("Flags") == 0)
		((IntProperty*)prop)->SetValue(backend->Flags());
	else if (str.ICompare("Frame") == 0)
		((RectProperty*)prop)->SetValue(backend->Bounds());
	else if (str.ICompare("IsValid") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsValid());
	else if (str.ICompare("Locked") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsLocked());
	else
		return PObject::GetProperty(name, value, index);

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
	
	PBitmapBackend *backend = fBackend;
	
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

	if (str.ICompare("Locked") == 0)
	{
		prop->GetValue(&boolval);
		if (*boolval.value)
			backend->Lock();
		else
			backend->Unlock();
	}
	else
		return PObject::SetProperty(name, value, index);
	
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
}


void
PBitmap::InitProperties(void)
{
	SetStringProperty("Description", "A representation of the screen");
	
	AddProperty(new IntProperty("AreaID", -1, "The area containing the bitmap's data."),
				PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("BytesPerRow", -1,
								"The number of bytes in one of the bitmap's rows."),
				PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("ColorSpace", B_RGB_32_BIT,
								"Color space of the bitmap"),
				PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("Flags", 0, "Behavior flags for the bitmap"),
				PROPERTY_READ_ONLY);
	AddProperty(new RectProperty("Frame", BRect(0,0,639,479),
								"Size of the bitmap in pixels"),
				PROPERTY_READ_ONLY);
	AddProperty(new BoolProperty("IsValid", true, "Is the bitmap object valid?"),
				PROPERTY_READ_ONLY);
	AddProperty(new BoolProperty("Locked", false, "The lock state of the bitmap."));
}


void
PBitmap::InitMethods(void)
{
	PMethodInterface pmi;
	
}


PBitmapBackend::PBitmapBackend(PObject *owner)
	:	fOwner(owner),
		fBitmap(NULL)
{
}


status_t
PBitmapBackend::InitCheck(void) const
{
	return fBitmap ? fBitmap->InitCheck() : B_NO_INIT;
}


bool
PBitmapBackend::IsValid(void) const
{
	return fBitmap ? fBitmap->IsValid() : false;
}


area_id
PBitmapBackend::Area(void) const
{
	return fBitmap ? fBitmap->Area() : -1;
}


void *
PBitmapBackend::Bits(void) const
{
	return fBitmap ? fBitmap->Bits() : NULL;
}


int32
PBitmapBackend::BitsLength(void) const
{
	return fBitmap ? fBitmap-> BitsLength() : 0;
}


int32
PBitmapBackend::BytesPerRow(void) const
{
	return fBitmap ? fBitmap->BytesPerRow() : 0;
}


color_space
PBitmapBackend::ColorSpace(void) const
{
	return fBitmap ? fBitmap->ColorSpace() : B_NO_COLOR_SPACE;
}


BRect
PBitmapBackend::Bounds(void) const
{
	return fBitmap ? fBitmap->Bounds() : BRect(0.0, 0.0, -1.0, -1.0);
}


uint32
PBitmapBackend::Flags(void)
{
	return fBitmap ? fBitmap->Flags() : 0;
}


void
PBitmapBackend::Initialize(const BRect &bounds, uint32 flags,
							color_space space, int32 bytesPerRow,
							screen_id screenID)
{
	if (!fBitmap)
		fBitmap = new BBitmap(bounds, flags, space, bytesPerRow,
								screenID);
}


void
PBitmapBackend::Initialize(const BBitmap &source, uint32 flags)
{
	if (!fBitmap)
		fBitmap = new BBitmap(source, flags);
}


void
PBitmapBackend::SetBits(const void *data, int32 length,
					int32 offset, color_space colorSpace)
{
	if (fBitmap)
		fBitmap->SetBits(data, length, offset, colorSpace);
}


void
PBitmapBackend::AddChild(BView *view)
{
	if (fBitmap)
		fBitmap->AddChild(view);
}


bool
PBitmapBackend::RemoveChild(BView *view)
{
	return fBitmap ? fBitmap->RemoveChild(view) : false;
}


int32
PBitmapBackend::CountChildren(void) const
{
	return fBitmap ? fBitmap->CountChildren() : -1;
}


BView *
PBitmapBackend::ChildAt(int32 index) const
{
	return fBitmap ? fBitmap->ChildAt(index) : NULL;
}


BView *
PBitmapBackend::FindView(const char *name) const
{
	return fBitmap ? fBitmap->FindView(name) : NULL;
}


BView *
PBitmapBackend::FindView(BPoint pt) const
{
	return fBitmap ? fBitmap->FindView(pt) : NULL;
}


bool
PBitmapBackend::Lock(void)
{
	return fBitmap ? fBitmap->Lock() : false;
}


void
PBitmapBackend::Unlock(void)
{
	if (fBitmap)
		fBitmap->Unlock();
}


bool
PBitmapBackend::IsLocked(void) const
{
	return fBitmap ? fBitmap->IsLocked() : false;
}

