// [zooey, 2005]: made MTextAddon really an abstract class
//==================================================================
//	MTextAddOn.h
//	Copyright 1996  Metrowerks Corporation, All Rights Reserved.
//==================================================================
//	This is a proxy class used by Editor add_ons.  It does not inherit from BView
//	but provides an abstract interface to a text engine.
 
#ifndef MTEXTADDON_H
#define MTEXTADDON_H

#include "HLibHekkel.h"

class BWindow;
struct entry_ref;

class IMPEXP_LIBHEKKEL MTextAddOn
{
public:
	virtual						~MTextAddOn();
	virtual	const char*			Text() = 0;
	virtual	int32				TextLength() const = 0;
	virtual	void				GetSelection(int32 *start, int32 *end) const = 0;
	virtual	void				Select(int32 newStart, int32 newEnd) = 0;
	virtual void				Delete() = 0;
	virtual void				Insert(const char* inText) = 0;
	virtual void				Insert(const char* text, int32 length) = 0;

	virtual	BWindow*			Window() = 0;
	virtual status_t			GetRef(entry_ref& outRef) = 0;
};

#if !__INTEL__
#pragma export on
#endif
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#if !__INTEL__
#pragma export reset
#endif

typedef long (*perform_edit_func)(MTextAddOn *addon);

#endif
