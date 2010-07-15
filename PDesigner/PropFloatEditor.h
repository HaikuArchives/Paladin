#ifndef PROPFLOATEDITOR_H
#define PROPFLOATEDITOR_H

#include <String.h>

#include "NumBox.h"
#include "PropEditor.h"


class PObject;
class PProperty;

class PropFloatEditor : public PropertyEditor
{
public:
								PropFloatEditor(PObject *obj, PProperty *prop);
								~PropFloatEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	void				SetTarget(BHandler *handler, BLooper *looper = NULL);
	
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);
	virtual	void				MessageReceived(BMessage *msg);
	
	
private:
	NumBox				*fEditor;
	BMessenger			*fMsgr;
	BString				*fPropName;
};

#endif
