#ifndef PWINDOW_H
#define PWINDOW_H

#include <Window.h>
#include <ListItem.h>
#include "PHandler.h"
#include "PComponents.h"

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

class PWindow : public PHandler
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
	
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	
	virtual	status_t		SendMessage(BMessage *msg);
	
	BWindow *				GetWindow(void);
	
private:
	friend class			PWindowBackend;
	
	void					InitProperties(void);
	void					InitBackend(void);
	
	PWindowBackend			*fWindow;
};

#endif
