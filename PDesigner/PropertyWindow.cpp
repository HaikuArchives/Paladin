#include "PropertyWindow.h"
#include <Screen.h>
#include <stdio.h>

#include "AutoTextControl.h"
#include "Globals.h"
#include "MsgDefs.h"
#include "PObjectBroker.h"
#include "PropEditor.h"

enum
{
	M_PROPERTY_CHANGED = 'prch'
};

PropertyWindow::PropertyWindow(void)
	:	BWindow(BRect(100,100,300,500),"Properties",B_FLOATING_WINDOW, B_ASYNCHRONOUS_CONTROLS |
																	B_FRAME_EVENTS),
		fObject(NULL)
{
	BRect screen(BScreen().Frame());
	MoveTo(screen.right - Bounds().Width() - 5,25);
	
	fTop = new BView(Bounds(),"top",B_FOLLOW_ALL,B_WILL_DRAW);
	fTop->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(fTop);
	
	fIDView = new BStringView(BRect(10,10,11,11), "idview", NULL,
										B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fTop->AddChild(fIDView);
}


PropertyWindow::~PropertyWindow(void)
{
	PropertyEditorBroker::GetBrokerInstance()->DetachAllEditors();
}


bool
PropertyWindow::QuitRequested(void)
{
	if (!IsHidden())
		Hide();
	
	if (gIsQuitting.GetValue())
		return true;
	
	return false;
}


void
PropertyWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_SET_OBJECT:
		{
			uint64 id;
			if (msg->FindInt64("id",(int64*)&id) == B_OK)
				SetObject(id);
			break;
		}
		case M_UPDATE_PROPERTY_EDITOR:
		{
			uint64 id;
			BString name;
			if (msg->FindInt64("id",(int64*)&id) == B_OK)
			{
				int32 i = 0;
				while (msg->FindString("name",i++, &name) == B_OK)
					UpdateEditor(id,name);
			}
			break;
		}
		case M_OWNER_CHANGED:
		{
			uint64 id;
			BString name, ownerType;
			if (msg->FindInt64("id",(int64*)&id) == B_OK &&
				msg->FindString("ownertype", &ownerType) == B_OK)
			{
				if (fObject->GetID() != id)
					break;
				
				int32 i = 0;
				while (msg->FindString("name",i++, &name) == B_OK)
					UpdateEditor(id,name);
			}
		}
		default:
		{
			BWindow::MessageReceived(msg);
		}
	}
}


void
PropertyWindow::SetObject(const uint64 &id)
{
	PropertyEditorBroker::GetBrokerInstance()->DetachAllEditors();
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj)
		return;
	
	fObject = obj;
	
	DisableUpdates();
	
	BString label;
	label << "ID: " << fObject->GetID();
	fIDView->SetText(label.String());
	fIDView->ResizeToPreferred();
	
	BPoint addPoint(0.0,fIDView->Frame().bottom + 5.0);
	for (int32 i = 0; i < obj->CountProperties(); i++)
	{
		PProperty *prop = obj->PropertyAt(i);
		uint32 flags = obj->FlagsForProperty(prop);
		if ( (flags & PROPERTY_READ_ONLY) || (flags & PROPERTY_HIDE_IN_EDITOR) )
			continue;
		
		// This will be NULL if a property is of a type that there isn't an editor for
		PropertyEditor *editor = GetEditorForProperty(obj,prop);
		if (editor)
		{
			// editors are responsible for resizing themselves to fit the width and be
			// conservative on height
			fTop->AddChild(editor);
			
			editor->MoveTo(addPoint);
			addPoint = editor->Frame().LeftBottom();
			addPoint.y += 1.0;
		}
	}
	
	EnableUpdates();
}


void
PropertyWindow::UpdateEditor(const uint64 &id, const BString &name)
{
	if (!fObject || fObject->GetID() != id)
		return;
	
	PProperty *prop = fObject->FindProperty(name);
	if (!prop)
		return;
	
	// Find the editor for the property in question.
	int32 count = fTop->CountChildren();
	PropertyEditor *editor = NULL;
	for(int32 i = 0;  i < count; i++)
	{
		PropertyEditor *child = dynamic_cast<PropertyEditor*>(fTop->ChildAt(i));
		if (child && child->GetProperty() == prop)
		{
			editor = child;
			break;
		}
	}
	
	if (editor)
		editor->Update();
}


