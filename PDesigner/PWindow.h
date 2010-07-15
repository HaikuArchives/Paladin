#ifndef PWINDOW_H
#define PWINDOW_H

#include <Window.h>
#include <ListItem.h>
#include "ObjectItem.h"
#include "PObject.h"
#include "PProperty.h"

/* Window Property List:
		Active
		Description
		Feel
		Flags
		Floating (read-only)
		Frame
		Front (read-only)
		Look
		Minimized
		MinWidth
		MaxWidth
		MinHeight
		MaxHeight
		Modal (read-only)
		PulseRate
		Title
		ViewList	-- list of child views
		Visible
		Workspaces
*/

class PWindowBackend;
class PWindow;

class WindowItem : public ObjectItem
{
public:
				WindowItem(PWindow *win);
	PWindow *	GetWindow(void);
};

class PWindow : public PObject
{
public:
							PWindow(void);
							PWindow(BMessage *msg);
							PWindow(const char *name);
							PWindow(const PWindow &from);
							~PWindow(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
		
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		RunMethod(const char *name, const BMessage &args, BMessage &outdata);
	
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	
	BWindow *				GetWindow(void);
	
	WindowItem *			CreateWindowItem(void);
	
private:
	void					InitProperties(void);
	void					InitBackend(void);
	
	status_t				DoAddChild(const BMessage &args, BMessage &outdata);
	status_t				DoRemoveChild(const BMessage &args, BMessage &outdata);
	status_t				DoChildAt(const BMessage &args, BMessage &outdata);
	status_t				DoCountChildren(const BMessage &args, BMessage &outdata);
	status_t				DoFindView(const BMessage &args, BMessage &outdata);
	
	PWindowBackend			*fWindow;
};

#endif
