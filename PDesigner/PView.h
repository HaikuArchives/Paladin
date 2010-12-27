#ifndef PVIEW_H
#define PVIEW_H

#include <ListItem.h>
#include <map>
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

using std::map;

enum
{
	RESIZE_NONE = 0,
	RESIZE_FIRST,
	RESIZE_SECOND,
	RESIZE_BOTH,
	RESIZE_CENTER
};


class ViewItem : public ObjectItem
{
public:
				ViewItem(PView *win);
	PView *		GetView(void);
};

typedef std::map<int32, MethodFunction> MsgHandlerMap;

class PView : public PObject
{
public:
							PView(void);
							PView(BMessage *msg);
							PView(const char *name);
							PView(const PView &from);
							~PView(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	virtual	BView *			GetView(void);
	virtual ViewItem *		CreateViewItem(void);
	
	virtual	void			SetMsgHandler(const int32 &constant, MethodFunction handler);
			MethodFunction	GetMsgHandler(const int32 &constant);
			void			RemoveMsgHandler(const int32 &constant);
	
	virtual	status_t		RunMessageHandler(const int32 &constant, PArgList &args);
			void			ConvertMsgToArgs(BMessage &in, PArgList &out);
	
protected:
	virtual	void			InitBackend(BView *view = NULL);
	virtual	void			InitMethods(void);
	
	void					SetHResizingMode(const int32 &value);
	void					SetVResizingMode(const int32 &value);
	int32					GetHResizingMode(void) const;
	int32					GetVResizingMode(void) const;
	
	BView					*fView;
	
private:
	void					InitProperties(void);
	
	MsgHandlerMap			fMsgHandlerMap;
};

#endif
