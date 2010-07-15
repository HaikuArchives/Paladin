#ifndef PROPFALLBACKEDITOR_H
#define PROPFALLBACKEDITOR_H

#include "PropEditor.h"
#include <StringView.h>

class PObject;
class PProperty;

class PropFallbackEditor : public PropertyEditor
{
public:
								PropFallbackEditor(PObject *obj, PProperty *prop);
								~PropFallbackEditor(void);
	
	virtual	bool				HandlesType(const BString &type);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				AttachedToWindow(void);

private:
	BStringView					*fLabel;
};

#endif
