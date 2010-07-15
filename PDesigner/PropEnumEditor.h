#ifndef PROPENUMEDITOR_H
#define PROPENUMEDITOR_H

#include <MenuField.h>
#include <Menu.h>
#include <MenuItem.h>
#include <String.h>
#include "PropEditor.h"

class PObject;
class PProperty;

class PropEnumEditor : public PropertyEditor
{
public:
								PropEnumEditor(PObject *obj, PProperty *prop);
								~PropEnumEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	void				SetTarget(BHandler *handler, BLooper *looper = NULL);
	
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);
	virtual	void				MessageReceived(BMessage *msg);
	
	
protected:
	virtual void				HandleEdit(int32 value);
	
	BMenuField			*fEditor;
	BMenu				*fMenu;
	BString				*fPropName;
	BMessenger			*fMsgr;
};

class PropEnumFlagEditor : public PropEnumEditor
{
public:
								PropEnumFlagEditor(PObject *obj, PProperty *prop);
								~PropEnumFlagEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
protected:
	virtual void				HandleEdit(int32 value);
};

#endif
