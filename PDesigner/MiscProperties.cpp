#include "MiscProperties.h"
#include <View.h>

WindowLookProperty::WindowLookProperty(void)
	:	EnumProperty()
{
	Init();
}


WindowLookProperty::WindowLookProperty(const char *name, window_look look,
									const char *description)
	:	EnumProperty()
{
	Init();
	SetName(name);
	SetDescription(description);
	SetValue(look);
}


WindowLookProperty::WindowLookProperty(PValue *value)
	:	EnumProperty(value)
{
	Init();
}


WindowLookProperty::WindowLookProperty(BMessage *msg)
	:	EnumProperty(msg)
{
	Init();
}


PProperty *
WindowLookProperty::Create(void)
{
	return new WindowLookProperty();
}


PProperty *
WindowLookProperty::Duplicate(void)
{
	return new WindowLookProperty(*this);
}


BArchivable *
WindowLookProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "WindowLookProperty"))
		return new WindowLookProperty(data);

	return NULL;
}


void
WindowLookProperty::Init(void)
{
	SetType("WindowLookProperty");
	
	AddValuePair("Bordered",B_BORDERED_WINDOW_LOOK);
	AddValuePair("No Border",B_NO_BORDER_WINDOW_LOOK);
	AddValuePair("Normal",B_TITLED_WINDOW_LOOK);
	AddValuePair("Document",B_DOCUMENT_WINDOW_LOOK);
	AddValuePair("Modal",B_MODAL_WINDOW_LOOK);
	AddValuePair("Floating",B_FLOATING_WINDOW_LOOK);
}


WindowFeelProperty::WindowFeelProperty(void)
	:	EnumProperty()
{
	Init();
}


WindowFeelProperty::WindowFeelProperty(const char *name, window_feel feel,
									const char *description)
	:	EnumProperty()
{
	Init();
	SetName(name);
	SetDescription(description);
	SetValue(feel);
}


WindowFeelProperty::WindowFeelProperty(PValue *value)
	:	EnumProperty(value)
{
	Init();
}


WindowFeelProperty::WindowFeelProperty(BMessage *msg)
	:	EnumProperty(msg)
{
	Init();
}


PProperty *
WindowFeelProperty::Create(void)
{
	return new WindowFeelProperty();
}


PProperty *
WindowFeelProperty::Duplicate(void)
{
	return new WindowFeelProperty(*this);
}


BArchivable *
WindowFeelProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "WindowFeelProperty"))
		return new WindowFeelProperty(data);

	return NULL;
}


void
WindowFeelProperty::Init(void)
{
	SetType("WindowFeelProperty");

	AddValuePair("Normal",B_NORMAL_WINDOW_FEEL);
	AddValuePair("Floating Subset",B_FLOATING_SUBSET_WINDOW_FEEL);
	AddValuePair("Floating App",B_FLOATING_APP_WINDOW_FEEL);
	AddValuePair("Floating All",B_FLOATING_ALL_WINDOW_FEEL);
	AddValuePair("Modal Subset",B_MODAL_SUBSET_WINDOW_FEEL);
	AddValuePair("Modal App",B_MODAL_APP_WINDOW_FEEL);
	AddValuePair("Modal All",B_MODAL_ALL_WINDOW_FEEL);
}


WindowFlagsProperty::WindowFlagsProperty(void)
	:	EnumFlagProperty()
{
	Init();
}


WindowFlagsProperty::WindowFlagsProperty(const char *name, int32 flags,
									const char *description)
	:	EnumFlagProperty()
{
	Init();
	SetName(name);
	SetDescription(description);
	SetValue(flags);
}


WindowFlagsProperty::WindowFlagsProperty(PValue *value)
	:	EnumFlagProperty(value)
{
	Init();
}


WindowFlagsProperty::WindowFlagsProperty(BMessage *msg)
	:	EnumFlagProperty(msg)
{
	Init();
}


PProperty *
WindowFlagsProperty::Create(void)
{
	return new WindowFlagsProperty();
}


PProperty *
WindowFlagsProperty::Duplicate(void)
{
	return new WindowFlagsProperty(*this);
}


BArchivable *
WindowFlagsProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "WindowFlagsProperty"))
		return new WindowFlagsProperty(data);

	return NULL;
}


void
WindowFlagsProperty::Init(void)
{
	SetType("WindowFlagsProperty");
	
	AddValuePair("No Close Button",B_NOT_CLOSABLE);
	AddValuePair("No Zoom Button",B_NOT_ZOOMABLE);
	AddValuePair("Not Minimizable",B_NOT_MINIMIZABLE);
	AddValuePair("Not Movable",B_NOT_MOVABLE);
	AddValuePair("Fixed Size",B_NOT_RESIZABLE);
	AddValuePair("Fixed Width",B_NOT_H_RESIZABLE);
	AddValuePair("Fixed Height",B_NOT_V_RESIZABLE);
	AddValuePair("Quit App on Close",B_QUIT_ON_WINDOW_CLOSE);
	AddValuePair("Avoid Front",B_AVOID_FRONT);
	AddValuePair("Avoid Focus",B_AVOID_FOCUS);
	AddValuePair("Accept First Click",B_WILL_ACCEPT_FIRST_CLICK);
	AddValuePair("Outline Resize",B_OUTLINE_RESIZE);
	AddValuePair("Asynchronous Controls",B_ASYNCHRONOUS_CONTROLS);
	AddValuePair("Views Can Overlap",B_VIEWS_CAN_OVERLAP);
	AddValuePair("No Workspace Activation",B_NO_WORKSPACE_ACTIVATION);
	AddValuePair("Not Anchored on Activate",B_NOT_ANCHORED_ON_ACTIVATE);
}


ViewFlagsProperty::ViewFlagsProperty(void)
	:	EnumFlagProperty()
{
	Init();
}


ViewFlagsProperty::ViewFlagsProperty(const char *name, int32 flags,
									const char *description)
	:	EnumFlagProperty()
{
	Init();
	SetName(name);
	SetDescription(description);
	SetValue(flags);
}


ViewFlagsProperty::ViewFlagsProperty(PValue *value)
	:	EnumFlagProperty(value)
{
	Init();
}


ViewFlagsProperty::ViewFlagsProperty(BMessage *msg)
	:	EnumFlagProperty(msg)
{
	Init();
}


PProperty *
ViewFlagsProperty::Create(void)
{
	return new ViewFlagsProperty();
}


PProperty *
ViewFlagsProperty::Duplicate(void)
{
	return new ViewFlagsProperty(*this);
}


BArchivable *
ViewFlagsProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "ViewFlagsProperty"))
		return new ViewFlagsProperty(data);

	return NULL;
}


void
ViewFlagsProperty::Init(void)
{
	SetType("ViewFlagsProperty");
	
	AddValuePair("Will Draw",B_WILL_DRAW);
	AddValuePair("Full Update on Resize",B_FULL_UPDATE_ON_RESIZE);
	AddValuePair("Navigable",B_NAVIGABLE);
	AddValuePair("Navigable Jump",B_NAVIGABLE_JUMP);
	AddValuePair("Pulse Needed",B_PULSE_NEEDED);
	AddValuePair("Frame Events",B_FRAME_EVENTS);
	AddValuePair("Subpixel Precision",B_SUBPIXEL_PRECISE);
	AddValuePair("Draw on Children",B_DRAW_ON_CHILDREN);
	AddValuePair("Input Method Aware",B_INPUT_METHOD_AWARE);
	
	#ifdef __HAIKU__
	AddValuePair("Supports Layout",B_SUPPORTS_LAYOUT);
	AddValuePair("Invalidate After Layout",B_INVALIDATE_AFTER_LAYOUT);
	#endif
}


ResizeModeProperty::ResizeModeProperty(void)
	:	EnumFlagProperty()
{
	Init();
}


ResizeModeProperty::ResizeModeProperty(const char *name, int32 flags,
									const char *description)
	:	EnumFlagProperty()
{
	Init();
	SetName(name);
	SetDescription(description);
	SetValue(flags);
}


ResizeModeProperty::ResizeModeProperty(PValue *value)
	:	EnumFlagProperty(value)
{
	Init();
}


ResizeModeProperty::ResizeModeProperty(BMessage *msg)
	:	EnumFlagProperty(msg)
{
	Init();
}


PProperty *
ResizeModeProperty::Create(void)
{
	return new ResizeModeProperty();
}


PProperty *
ResizeModeProperty::Duplicate(void)
{
	return new ResizeModeProperty(*this);
}


BArchivable *
ResizeModeProperty::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "ResizeModeProperty"))
		return new ResizeModeProperty(data);

	return NULL;
}


void
ResizeModeProperty::Init(void)
{
	SetType("ResizeModeProperty");
	
	AddValuePair("Follow Left",B_FOLLOW_LEFT);
	AddValuePair("Follow Right",B_FOLLOW_RIGHT);
	AddValuePair("Follow Left + Right",B_FOLLOW_LEFT_RIGHT);
	AddValuePair("Follow H Center",B_FOLLOW_H_CENTER);
	AddValuePair("Follow Top",B_FOLLOW_TOP);
	AddValuePair("Follow Bottom",B_FOLLOW_BOTTOM);
	AddValuePair("Follow Top + Bottom",B_FOLLOW_TOP_BOTTOM);
	AddValuePair("Follow V Center",B_FOLLOW_V_CENTER);
	AddValuePair("Follow All",B_FOLLOW_ALL);
	AddValuePair("Follow None",B_FOLLOW_NONE);
}


