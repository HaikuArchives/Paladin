#ifndef PBITMAP_H
#define PBITMAP_H

#include "PObject.h"
#include <Clipboard.h>

/*
	PBitmap
		Properties:
			AcceptsChildren
			BitsLength (read-only)
			Bounds (read-only)
			BytesPerRow (read-only)
			ChildCount (read-only)
			ColorSpace (read-only)
			IsLocked
			IsValid  (read-only)
			
		Methods:
			AddChild
			Bits
			ChildAt
			FindView
			Initialize
			RemoveChild
			SetBits
*/

class PBitmap : public PObject
{
public:
							PBitmap(void);
							PBitmap(BMessage *msg);
							PBitmap(const char *name);
							PBitmap(const PBitmap &from);
							~PBitmap(void);
			
	static	BArchivable *	Instantiate(BMessage *data);

	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	BBitmap *				GetBackend(void);
	
private:
	
	void					InitBackend(void);
	void					InitProperties(void);
	void					InitMethods(void);
	
	BBitmap					*fBitmap;
};

#endif
