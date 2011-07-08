#include "PBitmap.h"

#include <Application.h>
#include <Bitmap.h>
#include <Screen.h>

#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"
#include "PView.h"

int32_t PBitmapInitialize(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PBitmapInitializeFrom(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PBitmapSetBits(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PBitmapAddChild(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PBitmapRemoveChild(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PBitmapCountChildren(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

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
	void		Initialize(PBitmapBackend *source, uint32 flags);
	
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
	else if (str.ICompare("InitStatus") == 0)
		((IntProperty*)prop)->SetValue(backend->InitCheck());
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
	
	pmi.AddArg("bounds", PARG_RECT, "Size of the bitmap");
	pmi.AddArg("flags", PARG_INT32, "Bitmap flags");
	pmi.AddArg("space", PARG_INT32, "Color space of the bitmap");
	pmi.AddArg("bytesperrow", PARG_INT32, "Optional: number of bytes per row",
				PMIFLAG_OPTIONAL);
	pmi.AddArg("screenID", PARG_INT32, "Optional: ID of the screen for the bitmap",
				PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("Initialize", PBitmapInitialize, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("bitmapID", PARG_INT64, "ID of the source PBitmap");
	pmi.AddArg("flags", PARG_INT32, "Behavior flags for the bitmap");
	AddMethod(new PMethod("InitializeFrom", PBitmapInitializeFrom, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("data", PARG_POINTER, "Pointer to the data from which to copy");
	pmi.AddArg("length", PARG_INT32, "Number of bytes to copy");
	pmi.AddArg("offset", PARG_INT32, "Offset to start copying");
	pmi.AddArg("colorspace", PARG_INT32, "Color space of the source data");
	AddMethod(new PMethod("SetBits", PBitmapSetBits, &pmi));
	
	pmi.AddArg("id", PARG_INT64, "ID of the child PView to add");
	AddMethod(new PMethod("AddChild", PBitmapAddChild, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("id", PARG_INT64, "ID of the child PView to remove");
	AddMethod(new PMethod("RemoveChild", PBitmapRemoveChild, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddReturnValue("value", PARG_INT32, "Number of children found");
	AddMethod(new PMethod("CountChildren", PBitmapCountChildren, &pmi));
	pmi.MakeEmpty();
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
PBitmapBackend::Initialize(PBitmapBackend *source, uint32 flags)
{
	if (!fBitmap && source && source->fBitmap)
		fBitmap = new BBitmap(*source->fBitmap, flags);
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


int32_t
PBitmapInitialize(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PBitmapBackend *backend = (PBitmapBackend*)parent->GetBackend();
	
	PArgs args(in);
	
	BRect bounds;
	if (args.FindRect("bounds", &bounds) != B_OK)
		return B_ERROR;
	
	int32 flags;
	if (args.FindInt32("flags", &flags) != B_OK)
		return B_ERROR;
	
	int32 colorSpace;
	if (args.FindInt32("space", &colorSpace) != B_OK)
		return B_ERROR;
	
	int32 bytesPerRow;
	if (args.FindInt32("bytesperrow", &bytesPerRow) != B_OK)
		bytesPerRow = B_ANY_BYTES_PER_ROW;
	
	int32 screenID;
	if (args.FindInt32("screenID", &screenID) != B_OK)
		screenID = B_MAIN_SCREEN_ID.id;
	
	screen_id idStruct;
	idStruct.id = screenID;
	
	backend->Initialize(bounds, flags, (color_space)colorSpace,
						bytesPerRow, idStruct);
	
	return B_OK;
}


int32_t
PBitmapInitializeFrom(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PBitmapBackend *backend = (PBitmapBackend*)parent->GetBackend();
	
	PArgs args(in);
	int64 sourceID;
	if (args.FindInt64("id", &sourceID) != B_OK)
		return B_ERROR;
	
	int32 flags;
	if (args.FindInt32("flags", &flags) != B_OK)
		return B_ERROR;
	
	PObject *sourceObj = BROKER->FindObject(sourceID);
	if (!sourceObj->UsesInterface("PBitmap"))
		return B_BAD_DATA;
	
	PBitmap *sourceBitmap = (PBitmap*)sourceObj;
	
	backend->Initialize(sourceBitmap->GetBackend(), flags);
	
	return B_OK;
}


int32_t
PBitmapSetBits(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PBitmapBackend *backend = (PBitmapBackend*)parent->GetBackend();
	
	PArgs args(in);
	void *data;
	if (args.FindPointer("data", &data) != B_OK)
		return B_ERROR;
	
	int32 length;
	if (args.FindInt32("length", &length) != B_OK)
		return B_ERROR;
	
	int32 offset;
	if (args.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;
	
	int32 colorSpace;
	if (args.FindInt32("colorspace", &colorSpace) != B_OK)
		return B_ERROR;
	
	backend->SetBits(data, length, offset, (color_space)colorSpace);
	
	return B_OK;
}


int32_t
PBitmapAddChild(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PBitmapBackend *backend = (PBitmapBackend*)parent->GetBackend();
	
	PArgs args(in);
	int64 childID;
	if (args.FindInt64("id", &childID) != B_OK)
		return B_ERROR;
	
	PObject *childObj = BROKER->FindObject(childID);
	if (!childObj->UsesInterface("PView"))
		return B_BAD_DATA;
	
	PView *childPView = (PView*)childObj;
	
	status_t status = B_OK;
	if (backend->Lock())
	{
		if (backend->Flags() & B_BITMAP_ACCEPTS_VIEWS)
			backend->AddChild(childPView->GetView());
		else
			status = B_NOT_ALLOWED;
		
		backend->Unlock();
	}
	else
		status = B_ERROR;
	
	return status;
}


int32_t
PBitmapRemoveChild(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PBitmapBackend *backend = (PBitmapBackend*)parent->GetBackend();
	
	PArgs args(in);
	int64 childID;
	if (args.FindInt64("id", &childID) != B_OK)
		return B_ERROR;
	
	PObject *childObj = BROKER->FindObject(childID);
	if (!childObj->UsesInterface("PView"))
		return B_BAD_DATA;
	
	PView *childPView = (PView*)childObj;
	
	status_t status = B_OK;
	if (backend->Lock())
	{
		if (backend->Flags() & B_BITMAP_ACCEPTS_VIEWS)
			backend->RemoveChild(childPView->GetView());
		else
			status = B_NOT_ALLOWED;
		
		backend->Unlock();
	}
	else
		status = B_ERROR;
	
	return status;
}


int32_t
PBitmapCountChildren(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PBitmapBackend *backend = (PBitmapBackend*)parent->GetBackend();
	
	PArgs args(out);
	int32 count = -1;
	
	if (backend->Lock())
	{
		count = backend->CountChildren();
		backend->Unlock();
	}
	else
		return B_ERROR;
	
	args.MakeEmpty();
	args.AddInt32("value", count);
	
	return B_OK;
}


