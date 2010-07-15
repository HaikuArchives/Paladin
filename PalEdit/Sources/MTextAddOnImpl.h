// [zooey, 2005]: renamed this from MTextAddOn
//==================================================================
//	MTextAddOn.h
//	Copyright 1996  Metrowerks Corporation, All Rights Reserved.
//==================================================================
//	This is a proxy class used by Editor add_ons.  It does not inherit from BView
//	but provides an abstract interface to a text engine.
 
#ifndef MTEXTADDONIMPL_H
#define MTEXTADDONIMPL_H

#include "MTextAddOn.h"

class PExtCmd;
class PText;
typedef PText MIDETextView;

class MTextAddOnImpl : public MTextAddOn
{
public:
								MTextAddOnImpl(
									MIDETextView&	inTextView);

								MTextAddOnImpl(
									MIDETextView&	inTextView,
									const char *extension);

	virtual						~MTextAddOnImpl();
	virtual	const char*			Text();
	virtual	int32				TextLength() const;
	virtual	void				GetSelection(
									int32 *start, 
									int32 *end) const;
	virtual	void				Select(
									int32 newStart, 
									int32 newEnd);
	virtual void				Delete();
	virtual void				Insert(
									const char* inText);
	virtual void				Insert(
									const char* text, 
									int32 length);

	virtual	BWindow*			Window();
	virtual status_t			GetRef(
									entry_ref&	outRef);

private:

	MIDETextView&				fText;
	PExtCmd						*fCmd;
	bool						fDirty;
};

#endif
