#ifndef PLISTVIEW_H
#define PLISTVIEW_H

#include <ListView.h>
#include "PControl.h"

/*
	PListView Properties:
		All PView Properties
		
		ItemCount (read-only)
		SelectionType
*/

class PListView : public PView
{
public:
							PListView(void);
							PListView(BMessage *msg);
							PListView(const char *name);
							PListView(const PListView &from);
							~PListView(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
protected:
	virtual void			InitBackend(BView *view = NULL);

private:
	void					InitProperties(void);
};

#endif
