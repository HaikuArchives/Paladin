#ifndef PROPPOINTEDITOR_H
#define PROPPOINTEDITOR_H

#include <String.h>
#include <StringView.h>

#include "NumBox.h"
#include "PropEditor.h"


class PObject;
class PProperty;

class PropPointEditor : public PropertyEditor
{
public:
								PropPointEditor(PObject *obj, PProperty *prop);
								~PropPointEditor(void);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
	virtual	void				SetTarget(BHandler *handler, BLooper *looper = NULL);
	
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);
	virtual	void				MessageReceived(BMessage *msg);
	
	virtual	void				FrameResized(float w, float h);
private:
	BStringView			*fLabel;
	NumBox				*fXEditor,
						*fYEditor;
	BMessenger			*fMsgr;
	BString				*fPropName;
};

#endif
