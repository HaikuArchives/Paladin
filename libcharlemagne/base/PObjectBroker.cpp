#include "PObjectBroker.h"
#include "PProperty.h"

#include <Locker.h>
#include <stdio.h>

#include "PApplication.h"
#include "PControl.h"
#include "PBox.h"
#include "PButton.h"
#include "PCheckBox.h"
#include "PColorControl.h"
#include "PHandler.h"
#include "PLabel.h"
#include "PListView.h"
#include "PProgressBar.h"
#include "PRadioButton.h"
#include "PScrollBar.h"
#include "PSlider.h"
#include "PTextControl.h"
#include "PTextView.h"
#include "PView.h"
#include "PWindow.h"

static PObjectBroker *sBroker = NULL;
static uint64 sNextID = 1;
BLocker sIDLock;

void
InitObjectSystem(void)
{
	if (!sBroker)
		sBroker = new PObjectBroker();
}

void
ShutdownObjectSystem(void)
{
	if (sBroker)
	{
		delete sBroker;
		sBroker = NULL;
	}
}

PObjectBroker::PObjectBroker(void)
	:	BLooper("PObjectBroker"),
		fQuitting(false),
		pApp(NULL)
{
	fObjectList = new BObjectList<PObject>(20,true);
	fObjInfoList = new BObjectList<PObjectInfo>(20,true);
	
	fObjInfoList->AddItem(new PObjectInfo("PObject","Generic Object",PObject::Instantiate,
											PObject::Create));
	fObjInfoList->AddItem(new PObjectInfo("PApplication","Application",PApplication::Instantiate,
											PApplication::Create));
	fObjInfoList->AddItem(new PObjectInfo("PHandler","Handler",PHandler::Instantiate,
											PHandler::Create));
	fObjInfoList->AddItem(new PObjectInfo("PWindow","Window",PWindow::Instantiate,
											PWindow::Create));
	fObjInfoList->AddItem(new PObjectInfo("PView","View",PView::Instantiate,PView::Create));
	fObjInfoList->AddItem(new PObjectInfo("PButton","Button",PButton::Instantiate,PButton::Create));
	fObjInfoList->AddItem(new PObjectInfo("PCheckBox","Checkbox",PCheckBox::Instantiate,
											PCheckBox::Create));
	fObjInfoList->AddItem(new PObjectInfo("PBox","Box",PBox::Instantiate,PBox::Create));
	fObjInfoList->AddItem(new PObjectInfo("PColorControl","Color Picker",PColorControl::Instantiate,
											PColorControl::Create));
	fObjInfoList->AddItem(new PObjectInfo("PControl","Generic Control",PControl::Instantiate,
											PControl::Create));
	fObjInfoList->AddItem(new PObjectInfo("PLabel","Label",PLabel::Instantiate,PLabel::Create));
	fObjInfoList->AddItem(new PObjectInfo("PProgressBar","Progress Bar",PProgressBar::Instantiate,
											PProgressBar::Create));
	fObjInfoList->AddItem(new PObjectInfo("PRadioButton","RadioButton",PRadioButton::Instantiate,
											PRadioButton::Create));
	fObjInfoList->AddItem(new PObjectInfo("PScrollBar","ScrollBar",PScrollBar::Instantiate,
											PScrollBar::Create));
	fObjInfoList->AddItem(new PObjectInfo("PListView","List",
											PListView::Instantiate,PListView::Create));
	fObjInfoList->AddItem(new PObjectInfo("PSlider","Slider",
											PSlider::Instantiate,PSlider::Create));
	fObjInfoList->AddItem(new PObjectInfo("PTextControl","Text Control", PTextControl::Instantiate,
											PTextControl::Create));
	fObjInfoList->AddItem(new PObjectInfo("PTextView","Text View",PTextView::Instantiate,
											PTextView::Create));
}


PObjectBroker::~PObjectBroker(void)
{
	fQuitting = true;
	
	if (pApp)
		fObjectList->RemoveItem(pApp, false);
	
	delete fObjectList;
	delete fObjInfoList;
	
	delete pApp;
}


PObject *
PObjectBroker::MakeObject(const char *type, BMessage *msg)
{
	if (!type)
		return NULL;
	
	if (pApp && BString(type).ICompare("PApplication") == 0)
		return pApp;
	
	PObjectInfo *info = NULL;
	for (int32 i = 0; i < fObjInfoList->CountItems(); i++)
	{
		PObjectInfo *temp = fObjInfoList->ItemAt(i);
		if (temp->type.ICompare(type) == 0)
		{
			info = temp;
			break;
		}
	}
	
	if (info)
	{
		PObject *obj = msg ? (PObject*)info->arcfunc(msg) : info->createfunc();
		fObjectList->AddItem(obj);
		
		if (!pApp && obj->GetType().Compare("PApplication") == 0)
			pApp = obj;
		
		return obj;
	}
	
	return NULL;
}


int32
PObjectBroker::CountTypes(void) const
{
	return fObjInfoList->CountItems();
}


BString
PObjectBroker::TypeAt(const int32 &index) const
{
	PObjectInfo *info = fObjInfoList->ItemAt(index);
	BString str;
	if (info)
		str = info->type;
	
	return str;
}


BString
PObjectBroker::FriendlyTypeAt(const int32 &index) const
{
	PObjectInfo *info = fObjInfoList->ItemAt(index);
	BString str;
	if (info)
		str = info->friendlytype;
	
	return str;
}


PObject *
PObjectBroker::FindObject(const uint64 &id)
{
	// 0 is a NULL object id
	if (!id)
		return NULL;
	
	for (int32 i = 0; i < fObjectList->CountItems(); i++)
	{
		PObject *obj = fObjectList->ItemAt(i);
		if (obj && obj->GetID() == id)
			return obj;
	}
	
	return NULL;
}


PObjectBroker *
PObjectBroker::GetBrokerInstance(void)
{
	if (!sBroker)
		InitObjectSystem();
	
	return sBroker;
}


void
PObjectBroker::RegisterObject(PObject *obj)
{
	if (obj)
	{
		sIDLock.Lock();
		obj->fObjectID = sNextID++;
		sIDLock.Unlock();
	}
}


void
PObjectBroker::UnregisterObject(PObject *obj)
{
	if (obj && !fQuitting)
	{
		sIDLock.Lock();
		fObjectList->RemoveItem(obj,false);
		sIDLock.Unlock();
	}
}


void
PObjectBroker::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case POBJECT_BROKER_DELETE_OBJECT:
		{
			int64 id;
			if (msg->FindInt64("id", &id) == B_OK)
			{
				PObject *obj = FindObject(id);
				delete obj;
			}
			break;
		}
		default:
			BLooper::MessageReceived(msg);
	}
}


PObjectInfo *
PObjectBroker::FindObjectInfo(const char *type)
{
	for (int32 i = 0; i < fObjInfoList->CountItems(); i++)
	{
		PObjectInfo *oinfo = fObjInfoList->ItemAt(i);
		if (oinfo->type == type)
			return oinfo;
	}
	
	return NULL;
}


PObjectBroker *
GetBrokerInstance(void)
{
	return PObjectBroker::GetBrokerInstance();
}

