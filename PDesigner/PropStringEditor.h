#ifndef PROPSTRINGEDITOR_H
#define PROPSTRINGEDITOR_H

#include <String.h>

#include "AutoTextControl.h"
#include "PropEditor.h"


class PObject;
class PProperty;

class PropStringEditor : public PropertyEditor
{
public:
								PropStringEditor(PObject *obj, PProperty *prop);
								~PropStringEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	void				SetTarget(BHandler *handler, BLooper *looper = NULL);
	
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);
	virtual	void				MessageReceived(BMessage *msg);
	
	
private:
	AutoTextControl		*fEditor;
	BString				*fPropName;
	BMessenger			*fMsgr;
};

#endif
