#ifndef PARGS_H
#define PARGS_H

#include <Entry.h>
#include <Message.h>
#include <Messenger.h>
#include <TypeConstants.h>
#include <InterfaceDefs.h>
#include <String.h>

class PArgs
{
public:
						PArgs(void);
						PArgs(PArgs &from);
						~PArgs(void);
	
	PArgs &				operator=(const PArgs &from);
	void				SetTo(const PArgs &from);
	void				MakeEmpty(void);
	
	void				PrintToStream(void);
	
	status_t			CountFieldItems(const char *name, int32 *count);
	status_t			RemoveData(const char *name, int32 index = 0);
	status_t			RemoveName(const char *name);
	
	status_t			AddData(const char *name, type_code type,
								const void *data, int32_t numBytes);
	
	status_t			AddChar(const char *name, char value);
	status_t			AddRect(const char *name, BRect value);
	status_t			AddPoint(const char *name, BPoint value);
	status_t			AddString(const char *name, const char *value);
	status_t			AddString(const char *name, const BString &value);
	status_t			AddInt8(const char *name, int8 value);
	status_t			AddUInt8(const char *name, uint8 value);
	status_t			AddInt16(const char *name, int16 value);
	status_t			AddUInt16(const char *name, uint16 value);
	status_t			AddInt32(const char *name, int32 value);
	status_t			AddUInt32(const char *name, uint32 value);
	status_t			AddInt64(const char *name, int64 value);
	status_t			AddUInt64(const char *name, uint64 value);
	status_t			AddBool(const char *name, bool value);
	status_t			AddFloat(const char *name, float value);
	status_t			AddDouble(const char *name, double value);
	status_t			AddPointer(const char *name, const void *value);
	status_t			AddMessenger(const char *name, const BMessenger &value);
	status_t			AddRef(const char *name, const entry_ref &value);
	status_t			AddPArg(const char *name, const PArgs &value);
	status_t			AddColor(const char *name, const rgb_color &value);

	status_t			FindData(const char *name, type_code type,
								const void **data, int32_t *numBytes,
								const int32 index = 0) const;

	status_t			FindChar(const char *name, char *value, int32 index = 0) const;
	status_t			FindRect(const char *name, BRect *value, int32 index = 0) const;
	status_t			FindPoint(const char *name, BPoint *value, int32 index = 0) const;
	status_t			FindString(const char *name, const char **value, int32 index = 0) const;
	status_t			FindString(const char *name, BString *value, int32 index = 0) const;
	status_t			FindInt8(const char *name, int8 *value, int32 index = 0) const;
	status_t			FindUInt8(const char *name, uint8 *value, int32 index = 0) const;
	status_t			FindInt16(const char *name, int16 *value, int32 index = 0) const;
	status_t			FindUInt16(const char *name, uint16 *value, int32 index = 0) const;
	status_t			FindInt32(const char *name, int32 *value, int32 index = 0) const;
	status_t			FindUInt32(const char *name, uint32 *value, int32 index = 0) const;
	status_t			FindInt64(const char *name, int64 *value, int32 index = 0) const;
	status_t			FindUInt64(const char *name, uint64 *value, int32 index = 0) const;
	status_t			FindBool(const char *name, bool *value, int32 index = 0) const;
	status_t			FindFloat(const char *name, float *value, int32 index = 0) const;
	status_t			FindDouble(const char *name, double *value, int32 index = 0) const;
	status_t			FindPointer(const char *name,  void **value, int32 index = 0) const;
	status_t			FindMessenger(const char *name, BMessenger *value, int32 index = 0) const;
	status_t			FindRef(const char *name, entry_ref *value, int32 index = 0) const;
	status_t			FindPArg(const char *name, PArgs *value, int32 index = 0) const;
	status_t			FindColor(const char *name, rgb_color *value, int32 index = 0) const;
	
	status_t			ReplaceData(const char *name, type_code type,
									const void *data, int32_t numBytes,
									int32 index = 0);
						
	status_t			ReplaceChar(const char *name, char value, int32 index = 0);
	status_t			ReplaceRect(const char *name, BRect value, int32 index = 0);
	status_t			ReplacePoint(const char *name, BPoint value, int32 index = 0);
	status_t			ReplaceString(const char *name, const char *value, int32 index = 0);
	status_t			ReplaceString(const char *name, const BString &value, int32 index = 0);
	status_t			ReplaceInt8(const char *name, int8 value, int32 index = 0);
	status_t			ReplaceUInt8(const char *name, uint8 value, int32 index = 0);
	status_t			ReplaceInt16(const char *name, int16 value, int32 index = 0);
	status_t			ReplaceUInt16(const char *name, uint16 value, int32 index = 0);
	status_t			ReplaceInt32(const char *name, int32 value, int32 index = 0);
	status_t			ReplaceUInt32(const char *name, uint32 value, int32 index = 0);
	status_t			ReplaceInt64(const char *name, int64 value, int32 index = 0);
	status_t			ReplaceUInt64(const char *name, uint64 value, int32 index = 0);
	status_t			ReplaceBool(const char *name, bool value, int32 index = 0);
	status_t			ReplaceFloat(const char *name, float value, int32 index = 0);
	status_t			ReplaceDouble(const char *name, double value, int32 index = 0);
	status_t			ReplacePointer(const char *name, const void *value, int32 index = 0);
	status_t			ReplaceMessenger(const char *name, BMessenger value, int32 index = 0);
	status_t			ReplaceRef(const char *name, const entry_ref &value, int32 index = 0);
	status_t			ReplacePArg(const char *name, const PArgs &value, int32 index = 0);
	status_t			ReplaceColor(const char *name, const rgb_color &value, int32 index = 0);
	
	// This class is for when PArgs objects are used to store function parameters.
	// Order matters, so class is for tracking it. Protocol for using it is to create
	// a separate PArgs object to contain the information. Within that container,
	// each ArgOrderInfo record is kept in four fields which correspond to its
	// properties, i.e. reading one record will mean four separate Find* calls with the
	// same index. Convenience functions have been written for this.
	status_t			AddOrderInfo(const char *fieldName, type_code fieldType,
									int32 callIndex, int32 fieldIndex);
	status_t			SetOrderInfo(int32 &index, const char *fieldName,
									type_code fieldType, int32 callIndex,
									int32 fieldIndex);
	status_t			FindOrderInfo(int32 &index, BString &fieldName,
									type_code &fieldType, int32 &callIndex,
									int32 &fieldIndex);
	status_t			RemoveOrderInfo(const int32 &index);
	
	void				SetBackend(const BMessage &msg);
	BMessage			GetBackend(void) const;
	
private:
	BMessage			fBackend;
};

#endif
