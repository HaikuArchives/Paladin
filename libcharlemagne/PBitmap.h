#ifndef PBITMAP_H
#define PBITMAP_H

#include "PObject.h"

class PBitmapBackend;

class PBitmap : public PObject
{
public:
							PBitmap(void);
							PBitmap(BMessage *msg);
							PBitmap(const char *name);
							PBitmap(const PBitmap &from);
							~PBitmap(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			PBitmapBackend *GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);
			
			PBitmapBackend	*fBackend;
};

#endif

