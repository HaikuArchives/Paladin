#ifndef PVIEW_H
#define PVIEW_H

#include <ListItem.h>
#include <View.h>
#include "ObjectItem.h"
#include "PComponents.h"

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

class ViewItem : public ObjectItem
{
public:
				ViewItem(PView *win);
	PView *		GetView(void);
};

class PView : public PObject
{
public:
							PView(void);
							PView(BMessage *msg);
							PView(const char *name);
							PView(const PView &from);
							~PView(void);
//			PView &			operator=(const PView &from);
			
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		RunMethod(const char *name, const BMessage &args, BMessage &outdata);
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	virtual	BView *			GetView(void);
	virtual ViewItem *		CreateViewItem(void);
	
protected:
	virtual	void			InitBackend(BView *view = NULL);
	virtual	void			InitMethods(void);
	
	status_t				DoAddChild(const BMessage &args, BMessage &outdata);
	status_t				DoRemoveChild(const BMessage &args, BMessage &outdata);
	status_t				DoChildAt(const BMessage &args, BMessage &outdata);
	
	BView					*fView;
	
private:
	void					InitProperties(void);
	
};

#endif
