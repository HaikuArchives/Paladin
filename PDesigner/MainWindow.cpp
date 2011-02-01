#include "MainWindow.h"

#include <Application.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <stdio.h>
#include <View.h>

#include "FloaterBroker.h"
#include "Floater.h"
#include "Globals.h"
#include "HookFunctions.h"
#include "MsgDefs.h"
#include "ObjectItem.h"
#include "ObjectWindow.h"
#include "PArgs.h"
#include "PObjectBroker.h"
#include "PropertyWindow.h"
#include "PView.h"
#include "PWindow.h"

enum
{
	M_NEW_PROJECT = 'nwpj',
	M_ADD_WINDOW = 'adwn',
	M_REMOVE_SELECTED = 'rmsl',
	M_TOGGLE_PROPERTY_WIN = 'tprw',
	M_OBJECT_SELECTED = 'obsl',
	M_QUIT_REQUESTED = 'qurq'
};

MainWindow::MainWindow(void)
	:	BWindow(BRect(5,25,250,350),"PDesigner",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS),
		fProject(NULL)
{
	BRect r(Bounds());
	r.bottom = 20;
	BMenuBar *bar = new BMenuBar(r,"menubar");
	AddChild(bar);
	
	r = Bounds();
	r.top = 21;
	BView *top = new BView(r, "top", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(top);
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	r = top->Bounds();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fListView = new BOutlineListView(r,"listview",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL);
	BScrollView *scroll = new BScrollView("listscroll",fListView,B_FOLLOW_ALL,0,true,true);
	top->AddChild(scroll);
	fListView->SetSelectionMessage(new BMessage(M_OBJECT_SELECTED));
	
	BMenu *menu = new BMenu("File");
	menu->AddItem(new BMenuItem("New Project",new BMessage(M_NEW_PROJECT),'N', B_COMMAND_KEY));
	bar->AddItem(menu);
	
	menu = new BMenu("Project");
	menu->AddItem(new BMenuItem("Add Window",new BMessage(M_ADD_WINDOW),'A'));
	
	BMessage *menumsg = new BMessage(M_ADD_CONTROL);
	menumsg->AddString("type","PView");
	menu->AddItem(new BMenuItem("Add View",menumsg,'V'));

	menumsg = new BMessage(M_ADD_CONTROL);
	menumsg->AddString("type","PButton");
	menu->AddItem(new BMenuItem("Add Button",menumsg));
	
	menumsg = new BMessage(M_ADD_CONTROL);
	menumsg->AddString("type","PCheckBox");
	menu->AddItem(new BMenuItem("Add Check Box",menumsg));
	
	menu->AddItem(new BMenuItem("Delete Item",new BMessage(M_REMOVE_SELECTED),'D'));
	bar->AddItem(menu);
	
	menu = new BMenu("Window");
	menu->AddItem(new BMenuItem("Toggle Properties",new BMessage(M_TOGGLE_PROPERTY_WIN)));
	bar->AddItem(menu);
	
	fPropertyWin = new PropertyWindow();
	fPropertyWin->Show();
	
	fObjectWin = new ObjectWindow(BMessenger(this));
	fObjectWin->Show();
}


bool
MainWindow::QuitRequested(void)
{
	// Closing down the application is a little tricky, so be careful when editing this function
	
	// This flag is because the property window only hides itself when the user clicks
	// its close button.
	gIsQuitting = true;
	
	// Make it look like we've closed while we're actually shutting down
	Hide();
	
	// Once the flag is set, now we tell the property window to close.
	fPropertyWin->PostMessage(B_QUIT_REQUESTED);
	
	// Finally tell the app to quit and quit this window
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_ADD_WINDOW:
		{
			AddWindow();
			break;
		}
		case M_ADD_CONTROL:
		{
			BString type;
			msg->FindString("type",&type);
			if (type.ICompare("PWindow") == 0)
				AddWindow();
			else
				AddControl(type);
			break;
		}
		case M_REMOVE_SELECTED:
		{
			int32 index = fListView->CurrentSelection();
			fListView->Deselect(index);
			UpdateProperties();
			
			ObjectItem *item = (ObjectItem*)fListView->RemoveItem(index);
			if (item)
			{
				PObject *obj = item->GetObject();
				delete item;
				delete obj;
			}
			break;
		}
		case M_TOGGLE_PROPERTY_WIN:
		{
			if (fPropertyWin->IsHidden())
				fPropertyWin->Show();
			else
				fPropertyWin->Hide();
			break;
		}
		case M_OBJECT_SELECTED:
		{
			UpdateFloaters();
			UpdateProperties();
			break;
		}
		case M_ACTIVATE_OBJECT:
		{
			uint64 id;
			if (msg->FindInt64("id",(int64*)&id) != B_OK)
				break;
			
			for (int32 i = 0; i < fListView->CountItems(); i++)
			{
				ObjectItem *item = (ObjectItem*) fListView->ItemAt(i);
				if (item->GetObject()->GetID() == id)
				{
					fListView->Select(i);
					break;
				}
			}
			break;
		}
		case M_UPDATE_PROPERTY_EDITOR:
		{
			fPropertyWin->PostMessage(msg);
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
MainWindow::AddWindow(void)
{
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	PWindow *pwin = dynamic_cast<PWindow*>(broker->MakeObject("PWindow"));
	if (pwin)
	{
		BString name("Window");
		name << pwin->GetID();
		pwin->AddProperty(new StringProperty("Name",name.String(),
											"The name for this window instance"),0,0);
		pwin->AddProperty(new BoolProperty("ReallyQuit", false), PROPERTY_HIDE_IN_EDITOR);
		pwin->SetStringProperty("Title",name.String());

		pwin->SetPointProperty("Location",BPoint(250,100));
		pwin->SetFloatProperty("Width",400);
		pwin->SetFloatProperty("Height",300);
		
		pwin->ConnectEvent("FrameMoved", PWindowFrameMoved);
		pwin->ConnectEvent("FrameResized", PWindowFrameResized);
		pwin->ConnectEvent("WindowActivated", PWindowWindowActivated);
		pwin->ConnectEvent("QuitRequested", PWindowQuitRequested);
		pwin->SetMsgHandler(M_QUIT_REQUESTED, PWindowMQuitRequested);
		
		fListView->AddItem(new ObjectItem(pwin));
		
		int32 index = fListView->FullListIndexOf(fListView->LastItem());
		fListView->Select(index);
		Activate();
	}
}


void
MainWindow::AddControl(const BString &type)
{
	if (type.CountChars() < 1)
		return;
	
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	PObject *pobj = broker->MakeObject(type.String());
	
	if (!pobj || !pobj->UsesInterface("PView"))
		return;
		
	PView *pview = dynamic_cast<PView*>(pobj);
	if (!pview)
		return;
	
	BString name(pview->GetType());
	name << pview->GetID();
	pview->AddProperty(new StringProperty("Name",name.String(),"The name for this view"),0,0);
	
	if (pview->GetType().Compare("PTextControl") == 0)
	{
		// Create each text control with some more useful defaults for the GUI
		pview->SetStringProperty("Text", "Text");
		pview->SetStringProperty("Label", "Label");
		
		PArgs in, out;
		pview->RunMethod("SetPreferredDivider", in.ListRef(), out.ListRef());
	}
	
	FloatValue pw, ph;
	pview->GetProperty("PreferredWidth", &pw);
	pview->GetProperty("PreferredHeight", &ph);
	
	if (*pw.value > 0.0)
		pview->SetFloatProperty("Width",*pw.value);
	else
		pview->SetFloatProperty("Width",100);
	
	if (*ph.value > 0.0)
		pview->SetFloatProperty("Height",*ph.value);
	else
		pview->SetFloatProperty("Height",100);
	
	// If we have a PWindow selected in the window, we will add the view as a child of the
	// selected window. If there is no selection, we will add the item at the end with no
	// parent
	
	ObjectItem *item = new ObjectItem(pview);
	int32 selection = fListView->FullListCurrentSelection();
	if (selection < 0)
		fListView->AddItem(item);
	
	ObjectItem *oitem = dynamic_cast<ObjectItem*>(fListView->FullListItemAt(selection));
	if (!oitem)
		return;
	
	PWindow *pwin = dynamic_cast<PWindow*>(oitem->GetObject());
	if (pwin)
	{
		PArgs args,out;
		
		if (pview->GetType().Compare("PView") == 0 && 
			pwin->RunMethod("CountChildren",args.ListRef(),out.ListRef()) == B_OK)
		{
			int32 count;
			if (out.FindInt32("count",&count) == B_OK && count == 0)
			{
				float winWidth, winHeight;
				pwin->GetFloatProperty("Width",winWidth);
				pwin->GetFloatProperty("Height",winHeight);
				pview->SetFloatProperty("Width",winWidth);
				pview->SetFloatProperty("Height",winHeight);
				pview->SetIntProperty("HResizingMode",RESIZE_BOTH);
				pview->SetIntProperty("VResizingMode",RESIZE_BOTH);
			}
			pview->SetColorProperty("BackColor",ui_color(B_PANEL_BACKGROUND_COLOR));
		}
		
		args.AddInt64("id",item->GetObject()->GetID());
		if (pwin->RunMethod("AddChild", args.ListRef(), out.ListRef()) == B_OK)
			fListView->AddUnder(item,oitem);
		else
			fListView->AddItem(item);
	}
	else
	{
		if (oitem->GetObject()->UsesInterface("PView"))
		{
			PView *parent = dynamic_cast<PView*>(oitem->GetObject());
			PArgs args,out;
			args.AddInt64("id",item->GetObject()->GetID());
			if(parent->RunMethod("AddChild", args.ListRef(), out.ListRef()) == B_OK)
				fListView->AddUnder(item,oitem);
			else
				fListView->AddItem(item);
		}
		else
		{
			// Not a view or window, so just add it on at the end
			fListView->AddItem(item);
		}
	}
	
	pview->ConnectEvent("FocusChanged", PViewFocusChanged);
	pview->ConnectEvent("MouseDown", PViewMouseDown);
	pview->SetMsgHandler(M_FLOATER_ACTION, PViewHandleFloaterMsg);
	
	pview->SetBoolProperty("Focus", true);
	
	fListView->Select(fListView->FullListIndexOf(item));
}


void
MainWindow::MakeEmpty(void)
{
	while (fListView->CountItems())
	{
		ObjectItem *item = (ObjectItem*)fListView->RemoveItem(0L);
		if (item)
		{
			delete item->GetObject();
			delete item;
		}
	}
}


void
MainWindow::UpdateProperties(void)
{
	int32 selection = fListView->FullListCurrentSelection();

	BMessenger msgr(fPropertyWin);
	BMessage msg(M_SET_OBJECT);
	
	if (selection < 0)
	{
		// If there is no selection, we empty the property window. 0 is a built-in invalid
		// object handle, just like NULL is an invalid pointer.
		msg.AddInt64("id",0);
	}
	else
	{
		ObjectItem *item = dynamic_cast<ObjectItem*>(fListView->FullListItemAt(selection));
		PObject *obj = item->GetObject();
		msg.AddInt64("id",obj->GetID());
	}
	
	msgr.SendMessage(&msg);
}


void
MainWindow::UpdateFloaters(void)
{
	ObjectItem *item = dynamic_cast<ObjectItem*>(fListView->FullListItemAt(
											fListView->FullListCurrentSelection()));
	
	FloaterBroker *broker = FloaterBroker::GetInstance();
	broker->DetachAllFloaters();
	
	if (item)
	{
		PView *view = dynamic_cast<PView*>(item->GetObject());
		broker->AttachAllFloaters(view);
	}
}
