#ifndef PVIEW_H
#define PVIEW_H

#include <ListItem.h>
#include <map>
#include <View.h>

#include "PComponents.h"
#include "PHandler.h"

/*
	PView Properties:
		ChildView
		Description
		Flags
		Focus
		Font
		Frame
		HighColor
		HScrollbar?
		IsPrinting (read-only)
		LineCapMode
		LineJoinMode
		LowColor
		MouseButtons (read-only)
		MousePos (read-only)
		Origin
		Parent
		PenPos
		PenSize
		PreferredHeight
		PreferredWidth
		ResizingMode
		Scale
		ViewBitmap?
		ViewColor
		ViewOverlay?
		Visible
		VScrollbar?
		Window (read-only)
*/

class PView;

enum
{
	RESIZE_NONE = 0,
	RESIZE_FIRST,
	RESIZE_SECOND,
	RESIZE_BOTH,
	RESIZE_CENTER
};


class PView : public PHandler
{
public:
							PView(bool skipBackend = false);
							PView(BMessage *msg, bool skipBackend = false);
							PView(const char *name, bool skipBackend = false);
							PView(const PView &from, bool skipBackend = false);
							~PView(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	virtual	status_t		SendMessage(BMessage *msg);
	
	virtual	BView *			GetView(void);
	
protected:
	virtual	void			InitBackend(void);
	virtual	void			InitMethods(void);
	
	void					SetHResizingMode(const int32 &value);
	void					SetVResizingMode(const int32 &value);
	int32					GetHResizingMode(void) const;
	int32					GetVResizingMode(void) const;
	
	BView					*fView;
	
private:
	void					InitProperties(void);
	bool					fSkipBackend;
};

#endif
