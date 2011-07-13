#ifndef PLISTVIEW_H
#define PLISTVIEW_H

#include "PView.h"
#include <Application.h>
#include <ListView.h>
#include <ListItem.h>
#include <stdio.h>
#include <Window.h>


class PListViewBackend;
class BListView;

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

			BListView *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

