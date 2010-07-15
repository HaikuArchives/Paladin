#ifndef PROPRECTEDITOR_H
#define PROPRECTEDITOR_H

#include <String.h>
#include <StringView.h>

#include "NumBox.h"
#include "PropEditor.h"


class PObject;
class PProperty;

class PropRectEditor : public PropertyEditor
{
public:
								PropRectEditor(PObject *obj, PProperty *prop);
								~PropRectEditor(void);
	
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
	NumBox				*fLEditor,
						*fTEditor,
						*fREditor,
						*fBEditor;
	BMessenger			*fMsgr;
	BString				*fPropName;
};

#endif
