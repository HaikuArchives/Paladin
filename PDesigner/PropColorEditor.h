#ifndef PROPCOLOREDITOR_H
#define PROPCOLOREDITOR_H

#include "PropEditor.h"
#include <StringView.h>

class PObject;
class PProperty;
class ColorWell;

class PropColorEditor : public PropertyEditor
{
public:
								PropColorEditor(PObject *obj, PProperty *prop);
								~PropColorEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	void				SetTarget(BHandler *handler, BLooper *looper);
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);
	virtual	void				MessageReceived(BMessage *msg);

private:
	BStringView					*fLabel;
	ColorWell					*fColorWell;
	BMessenger					*fMsgr;
};

#endif
