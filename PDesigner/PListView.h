#ifndef PLISTVIEW_H
#define PLISTVIEW_H

#include <ListView.h>
#include "PControl.h"

/*
	PListItem Properties:
		All PObject Properties
		
		Enabled
		Expanded
		Height
		OutlineLevel (read-only)
		Selected
		Width
*/

class PListItem : public PObject
{
public:
							PListItem(void);
							PListItem(BMessage *msg);
							PListItem(const char *name);
							PListItem(const PListItem &from);
							~PListItem(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
			BListItem *		GetListItem(void);
	
protected:
	virtual void			InitBackend(void);
	BListItem				*fListItem;

private:
	void					InitProperties(void);
};


/*
	PStringItem Properties:
		All PListItem Properties
		
		Text
*/

class PStringItem : public PListItem
{
public:
							PStringItem(void);
							PStringItem(BMessage *msg);
							PStringItem(const char *name);
							PStringItem(const PStringItem &from);
							~PStringItem(void);
			
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
protected:
	virtual void			InitBackend(void);
	
private:
	void					InitProperties(void);
};


/*
	PListView Properties:
		All PView Properties
		
		Items
		ItemCount (read-only)
		InvocationMessage
		SelectionMessage
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
	
	virtual	bool			AddProperty(PProperty *p, uint32 flags = 0);
	virtual PProperty *		RemoveProperty(const int32 &index);
	virtual	void			RemoveProperty(PProperty *p);
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
protected:
	virtual void			InitBackend(BView *view = NULL);

private:
	void					InitProperties(void);
};

#endif
