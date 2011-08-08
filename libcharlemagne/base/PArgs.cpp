#include "PArgs.h"

PArgs::PArgs(void)
{
}


PArgs::PArgs(PArgs &from)
{
	*this = from;
}


PArgs::~PArgs(void)
{
}


PArgs &
PArgs::operator=(const PArgs &from)
{
	fBackend = from.fBackend;
	
	return *this;
}


void
PArgs::SetTo(const PArgs &from)
{
	*this = from;
}


void
PArgs::MakeEmpty(void)
{
	fBackend.MakeEmpty();
}


void
PArgs::PrintToStream(void)
{
	fBackend.PrintToStream();
}


status_t
PArgs::CountFieldItems(const char *name, int32 *count)
{
	type_code type;
	return fBackend.GetInfo(name, &type, count);
}


status_t
PArgs::RemoveData(const char *name, int32 index)
{
	return fBackend.RemoveData(name, index);
}


status_t
PArgs::RemoveName(const char *name)
{
	return fBackend.RemoveName(name);
}


#pragma mark - Add methods


status_t
PArgs::AddData(const char *name, type_code type, const void *data, int32_t numBytes)
{
	return fBackend.AddData(name, type, data, numBytes, false);
}


status_t
PArgs::AddChar(const char *name, char value)
{
	return fBackend.AddData(name, B_CHAR_TYPE, &value, sizeof(char));
}


status_t
PArgs::AddRect(const char *name, BRect value)
{
	return fBackend.AddRect(name, value);
}


status_t
PArgs::AddPoint(const char *name, BPoint value)
{
	return fBackend.AddPoint(name, value);
}


status_t
PArgs::AddString(const char *name, const char *value)
{
	return fBackend.AddString(name, value);
}


status_t
PArgs::AddString(const char *name, const BString &value)
{
	return fBackend.AddString(name, value);
}


status_t
PArgs::AddInt8(const char *name, int8 value)
{
	return fBackend.AddInt8(name, value);
}


status_t
PArgs::AddUInt8(const char *name, uint8 value)
{
	return fBackend.AddUInt8(name, value);
}


status_t
PArgs::AddInt16(const char *name, int16 value)
{
	return fBackend.AddInt16(name, value);
}


status_t
PArgs::AddUInt16(const char *name, uint16 value)
{
	return fBackend.AddUInt16(name, value);
}


status_t
PArgs::AddInt32(const char *name, int32 value)
{
	return fBackend.AddInt32(name, value);
}


status_t
PArgs::AddUInt32(const char *name, uint32 value)
{
	return fBackend.AddUInt32(name, value);
}


status_t
PArgs::AddInt64(const char *name, int64 value)
{
	return fBackend.AddInt64(name, value);
}


status_t
PArgs::AddUInt64(const char *name, uint64 value)
{
	return fBackend.AddUInt64(name, value);
}


status_t
PArgs::AddBool(const char *name, bool value)
{
	return fBackend.AddBool(name, value);
}


status_t
PArgs::AddFloat(const char *name, float value)
{
	return fBackend.AddFloat(name, value);
}


status_t
PArgs::AddDouble(const char *name, double value)
{
	return fBackend.AddDouble(name, value);
}


status_t
PArgs::AddPointer(const char *name, const void *value)
{
	return fBackend.AddPointer(name, value);
}


status_t
PArgs::AddMessenger(const char *name, const BMessenger &value)
{
	return fBackend.AddMessenger(name, value);
}


status_t
PArgs::AddRef(const char *name, const entry_ref &value)
{
	return fBackend.AddRef(name, &value);
}


status_t
PArgs::AddPArg(const char *name, const PArgs &value)
{
	return fBackend.AddMessage(name, &value.fBackend);
}


status_t
PArgs::AddColor(const char *name, const rgb_color &value)
{
	return fBackend.AddData(name, B_RGB_COLOR_TYPE, &value, sizeof(rgb_color));
}


#pragma mark - Find methods


status_t
PArgs::FindData(const char *name, type_code type, const void **data,
				int32_t *numBytes, const int32 index) const
{
	return fBackend.FindData(name, type, index, data, (ssize_t*)numBytes);
}


status_t
PArgs::FindChar(const char *name, char *value, int32 index) const
{
	ssize_t size;
	return fBackend.FindData(name, B_CHAR_TYPE, index, (const void **)&value, &size);
}


status_t
PArgs::FindRect(const char *name, BRect *value, int32 index) const
{
	return fBackend.FindRect(name, index, value);
}


status_t
PArgs::FindPoint(const char *name, BPoint *value, int32 index) const
{
	return fBackend.FindPoint(name, index, value);
}


status_t
PArgs::FindString(const char *name, const char **value, int32 index) const
{
	return fBackend.FindString(name, index, value);
}


status_t
PArgs::FindString(const char *name, BString *value, int32 index) const
{
	return fBackend.FindString(name, index, value);
}


status_t
PArgs::FindInt8(const char *name, int8 *value, int32 index) const
{
	return fBackend.FindInt8(name, index, value);
}


status_t
PArgs::FindUInt8(const char *name, uint8 *value, int32 index) const
{
	return fBackend.FindUInt8(name, index, value);
}


status_t
PArgs::FindInt16(const char *name, int16 *value, int32 index) const
{
	return fBackend.FindInt16(name, index, value);
}


status_t
PArgs::FindUInt16(const char *name, uint16 *value, int32 index) const
{
	return fBackend.FindUInt16(name, index, value);
}


status_t
PArgs::FindInt32(const char *name, int32 *value, int32 index) const
{
	return fBackend.FindInt32(name, index, value);
}


status_t
PArgs::FindUInt32(const char *name, uint32 *value, int32 index) const
{
	return fBackend.FindUInt32(name, index, value);
}


status_t
PArgs::FindInt64(const char *name, int64 *value, int32 index) const
{
	return fBackend.FindInt64(name, index, value);
}


status_t
PArgs::FindUInt64(const char *name, uint64 *value, int32 index) const
{
	return fBackend.FindUInt64(name, index, value);
}


status_t
PArgs::FindBool(const char *name, bool *value, int32 index) const
{
	return fBackend.FindBool(name, index, value);
}


status_t
PArgs::FindFloat(const char *name, float *value, int32 index) const
{
	return fBackend.FindFloat(name, index, value);
}


status_t
PArgs::FindDouble(const char *name, double *value, int32 index) const
{
	return fBackend.FindDouble(name, index, value);
}


status_t
PArgs::FindPointer(const char *name,  void **value, int32 index) const
{
	return fBackend.FindPointer(name, index, value);
}


status_t
PArgs::FindMessenger(const char *name, BMessenger *value, int32 index) const
{
	return fBackend.FindMessenger(name, index, value);
}


status_t
PArgs::FindRef(const char *name, entry_ref *value, int32 index) const
{
	return fBackend.FindRef(name, index, value);
}


status_t
PArgs::FindPArg(const char *name, PArgs *value, int32 index) const
{
	return fBackend.FindMessage(name, index, &value->fBackend);
}


status_t
PArgs::FindColor(const char *name, rgb_color *value, int32 index) const
{
	ssize_t size;
	return fBackend.FindData(name, B_RGB_COLOR_TYPE, index, 
							(const void **)&value, &size);
}


#pragma mark - Replacement methods


status_t
PArgs::ReplaceData(const char *name, type_code type, const void *data,
				int32_t numBytes, int32 index)
{
	return fBackend.ReplaceData(name, type, index, data, numBytes);
}


status_t
PArgs::ReplaceChar(const char *name, char value, int32 index)
{
	return fBackend.ReplaceData(name, B_CHAR_TYPE, index, &value, sizeof(char));
}


status_t
PArgs::ReplaceRect(const char *name, BRect value, int32 index)
{
	return fBackend.ReplaceRect(name, index, value);
}


status_t
PArgs::ReplacePoint(const char *name, BPoint value, int32 index)
{
	return fBackend.ReplacePoint(name, index, value);
}


status_t
PArgs::ReplaceString(const char *name, const char *value, int32 index)
{
	return fBackend.ReplaceString(name, index, value);
}


status_t
PArgs::ReplaceString(const char *name, const BString &value, int32 index)
{
	return fBackend.ReplaceString(name, index, value);
}


status_t
PArgs::ReplaceInt8(const char *name, int8 value, int32 index)
{
	return fBackend.ReplaceInt8(name, index, value);
}


status_t
PArgs::ReplaceUInt8(const char *name, uint8 value, int32 index)
{
	return fBackend.ReplaceUInt8(name, index, value);
}


status_t
PArgs::ReplaceInt16(const char *name, int16 value, int32 index)
{
	return fBackend.ReplaceInt16(name, index, value);
}


status_t
PArgs::ReplaceUInt16(const char *name, uint16 value, int32 index)
{
	return fBackend.ReplaceUInt16(name, index, value);
}


status_t
PArgs::ReplaceInt32(const char *name, int32 value, int32 index)
{
	return fBackend.ReplaceInt32(name, index, value);
}


status_t
PArgs::ReplaceUInt32(const char *name, uint32 value, int32 index)
{
	return fBackend.ReplaceUInt32(name, index, value);
}


status_t
PArgs::ReplaceInt64(const char *name, int64 value, int32 index)
{
	return fBackend.ReplaceInt64(name, index, value);
}


status_t
PArgs::ReplaceUInt64(const char *name, uint64 value, int32 index)
{
	return fBackend.ReplaceUInt64(name, index, value);
}


status_t
PArgs::ReplaceBool(const char *name, bool value, int32 index)
{
	return fBackend.ReplaceBool(name, index, value);
}


status_t
PArgs::ReplaceFloat(const char *name, float value, int32 index)
{
	return fBackend.ReplaceFloat(name, index, value);
}


status_t
PArgs::ReplaceDouble(const char *name, double value, int32 index)
{
	return fBackend.ReplaceDouble(name, index, value);
}


status_t
PArgs::ReplacePointer(const char *name, const void *value, int32 index)
{
	return fBackend.ReplacePointer(name, index, value);
}


status_t
PArgs::ReplaceMessenger(const char *name, BMessenger value, int32 index)
{
	return fBackend.ReplaceMessenger(name, index, value);
}


status_t
PArgs::ReplaceRef(const char *name, const entry_ref &value, int32 index)
{
	return fBackend.ReplaceRef(name, index, &value);
}


status_t
PArgs::ReplacePArg(const char *name, const PArgs &value, int32 index)
{
	return fBackend.ReplaceMessage(name, index, &value.fBackend);
}


status_t
PArgs::ReplaceColor(const char *name, const rgb_color &value, int32 index)
{
	return fBackend.ReplaceData(name, B_RGB_COLOR_TYPE, index, &value, sizeof(rgb_color));
}


status_t
PArgs::AddOrderInfo(const char *fieldName, type_code fieldType,
								int32 callIndex, int32 fieldIndex)
{
	if (!fieldName)
		return B_ERROR;
	
	status_t status;
	
	status = AddString("fieldname", fieldName);
	if (status != B_OK)
		return status;
	
	status = AddInt32("type", fieldType);
	if (status != B_OK)
		return status;
	
	status = AddInt32("callindex", callIndex);
	if (status != B_OK)
		return status;
	
	status = AddInt32("fieldindex", fieldIndex);
	
	return status;
}


status_t
PArgs::SetOrderInfo(int32 &index, const char *fieldName, type_code fieldType,
					int32 callIndex, int32 fieldIndex)
{
	if (!fieldName)
		return B_ERROR;
	
	status_t status;
	
	status = ReplaceString("fieldname", fieldName, index);
	if (status != B_OK)
		return status;
	
	status = ReplaceInt32("type", fieldType, index);
	if (status != B_OK)
		return status;
	
	status = ReplaceInt32("callindex", callIndex, index);
	if (status != B_OK)
		return status;
	
	status = ReplaceInt32("fieldindex", fieldIndex, index);
	
	return status;
}


status_t
PArgs::FindOrderInfo(int32 &index, BString &fieldName, type_code &fieldType,
					int32 &callIndex, int32 &fieldIndex)
{
	status_t status;
	
	status = FindString("fieldname", &fieldName, index);
	if (status != B_OK)
		return status;
	
	status = FindInt32("type", (int32*)&fieldType, index);
	if (status != B_OK)
		return status;
	
	status = FindInt32("callindex", &callIndex, index);
	if (status != B_OK)
		return status;
	
	status = FindInt32("fieldindex", &fieldIndex, index);
	
	return status;
}


status_t
PArgs::RemoveOrderInfo(const int32 &index)
{
	status_t status;
	
	status = RemoveData("fieldname", index);
	if (status != B_OK)
		return status;
	
	status = RemoveData("type", index);
	if (status != B_OK)
		return status;
	
	status = RemoveData("callindex", index);
	if (status != B_OK)
		return status;
	
	status = RemoveData("fieldindex", index);
	
	return status;
}


void
PArgs::SetBackend(const BMessage &msg)
{
	fBackend = msg;
}


BMessage
PArgs::GetBackend(void) const
{
	return fBackend;
}

