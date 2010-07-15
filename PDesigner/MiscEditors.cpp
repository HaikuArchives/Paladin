#include "MiscEditors.h"

#include "PObject.h"
#include "PProperty.h"
#include "EnumProperty.h"
#include <stdio.h>

WindowLookEditor::WindowLookEditor(PObject *obj, PProperty *prop)
	:	PropEnumEditor(obj,prop)
{
}


bool
WindowLookEditor::HandlesType(const BString &type)
{
	return (type.ICompare("WindowLookProperty") == 0);
}


PropertyEditor *
WindowLookEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new WindowLookEditor(obj,prop);
}


WindowFeelEditor::WindowFeelEditor(PObject *obj, PProperty *prop)
	:	PropEnumEditor(obj,prop)
{
}


bool
WindowFeelEditor::HandlesType(const BString &type)
{
	return (type.ICompare("WindowFeelProperty") == 0);
}


PropertyEditor *
WindowFeelEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new WindowFeelEditor(obj,prop);
}


WindowFlagsEditor::WindowFlagsEditor(PObject *obj, PProperty *prop)
	:	PropEnumFlagEditor(obj,prop)
{
}


bool
WindowFlagsEditor::HandlesType(const BString &type)
{
	return (type.ICompare("WindowFlagsProperty") == 0);
}


PropertyEditor *
WindowFlagsEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new WindowFlagsEditor(obj,prop);
}


ViewFlagsEditor::ViewFlagsEditor(PObject *obj, PProperty *prop)
	:	PropEnumFlagEditor(obj,prop)
{
}


bool
ViewFlagsEditor::HandlesType(const BString &type)
{
	return (type.ICompare("ViewFlagsProperty") == 0);
}


PropertyEditor *
ViewFlagsEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new ViewFlagsEditor(obj,prop);
}


ResizeModeEditor::ResizeModeEditor(PObject *obj, PProperty *prop)
	:	PropEnumFlagEditor(obj,prop)
{
}


bool
ResizeModeEditor::HandlesType(const BString &type)
{
	return (type.ICompare("ResizeModeProperty") == 0);
}


PropertyEditor *
ResizeModeEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	return new ResizeModeEditor(obj,prop);
}


