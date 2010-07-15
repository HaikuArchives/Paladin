#ifndef MISCEDITORS_H
#define MISCEDITORS_H

#include <MenuField.h>
#include <String.h>
#include "PropEditor.h"
#include "PropEnumEditor.h"

class PObject;
class PProperty;

class WindowLookEditor : public PropEnumEditor
{
public:
								WindowLookEditor(PObject *obj, PProperty *prop);
			bool				HandlesType(const BString &type);
			PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
};

class WindowFeelEditor : public PropEnumEditor
{
public:
								WindowFeelEditor(PObject *obj, PProperty *prop);
			bool				HandlesType(const BString &type);
			PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
};

class WindowFlagsEditor : public PropEnumFlagEditor
{
public:
								WindowFlagsEditor(PObject *obj, PProperty *prop);
			bool				HandlesType(const BString &type);
			PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
};

class ViewFlagsEditor : public PropEnumFlagEditor
{
public:
								ViewFlagsEditor(PObject *obj, PProperty *prop);
			bool				HandlesType(const BString &type);
			PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
};

class ResizeModeEditor : public PropEnumFlagEditor
{
public:
								ResizeModeEditor(PObject *obj, PProperty *prop);
			bool				HandlesType(const BString &type);
			PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
};

#endif
