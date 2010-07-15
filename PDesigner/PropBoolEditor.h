#ifndef PROPBOOLEDITOR_H
#define PROPBOOLEDITOR_H

#include <CheckBox.h>
#include <String.h>

#include "AutoTextControl.h"
#include "PropEditor.h"


class PObject;
class PProperty;

class PropBoolEditor : public PropertyEditor
{
public:
								PropBoolEditor(PObject *obj, PProperty *prop);
								~PropBoolEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	void				SetTarget(BHandler *handler, BLooper *looper = NULL);
	
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);
	virtual	void				MessageReceived(BMessage *msg);
	
	
private:
	BCheckBox			*fEditor;
	BMessenger			*fMsgr;
	BString				*fPropName;
};

#endif
