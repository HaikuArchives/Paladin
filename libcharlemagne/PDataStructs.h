#ifndef PDATASTRUCTS_H
#define PDATASTRUCTS_H

#include "PData.h"

class PAppInfo : public PData
{
public:
							PAppInfo(void);
							PAppInfo(BMessage *msg);
							PAppInfo(const char *name);
							PAppInfo(const PAppInfo &from);
							~PAppInfo(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PData *		Create(void);
	virtual	PData *		Duplicate(void) const;
};

class PEntryRef : public PData
{
public:
							PEntryRef(void);
							PEntryRef(BMessage *msg);
							PEntryRef(const char *name);
							PEntryRef(const PEntryRef &from);
							~PEntryRef(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PData *			Create(void);
	virtual	PData *			Duplicate(void) const;
};

#endif
