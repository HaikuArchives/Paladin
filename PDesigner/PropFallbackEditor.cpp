#include "PropFallbackEditor.h"

#include "PObject.h"
#include "PProperty.h"

PropFallbackEditor::PropFallbackEditor(PObject *obj, PProperty *prop)
	:	PropertyEditor(obj,prop),
		fLabel(NULL)
{
	BString label("FallbackEditor");
	if (GetProperty())
		label = GetProperty()->GetName();
	
	fLabel = new BStringView(BRect(0,0,1,1),"label",label.String(),B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
}


PropFallbackEditor::~PropFallbackEditor(void)
{
	// This is just in case this editor is deleted before being added to a window
	if (!fLabel->Parent())
		delete fLabel;
}


bool
PropFallbackEditor::HandlesType(const BString &type)
{
	return true;
}


PropertyEditor *
PropFallbackEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropFallbackEditor(obj,prop);
}


void
PropFallbackEditor::AttachedToWindow(void)
{
	if (!fLabel->Parent())
	{
		if (!Bounds().IsValid())
		{
			fLabel->ResizeToPreferred();
			
			ResizeTo(fLabel->Bounds().Width(), fLabel->Bounds().Height());
		}
		AddChild(fLabel);
	}
}

