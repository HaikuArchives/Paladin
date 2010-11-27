#include "PObjectBroker.h"
#include "PProperty.h"

#include <Locker.h>
#include "PWindow.h"
#include "PView.h"
#include "PControl.h"
#include "PBox.h"
#include "PButton.h"
#include "PCheckBox.h"
#include "PLabel.h"
#include "PListView.h"
#include "PRadioButton.h"
#include "PScrollBar.h"
#include "PTextControl.h"

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
	:	fQuitting(false)
{
	fObjectList = new BObjectList<PObject>(20,true);
	fObjInfoList = new BObjectList<PObjectInfo>(20,true);
	
	fObjInfoList->AddItem(new PObjectInfo("PObject","Generic Object",PObject::Instantiate,
											PObject::Create));
	fObjInfoList->AddItem(new PObjectInfo("PWindow","Window",PWindow::Instantiate,
											PWindow::Create));
	fObjInfoList->AddItem(new PObjectInfo("PView","View",PView::Instantiate,PView::Create));
	fObjInfoList->AddItem(new PObjectInfo("PButton","Button",PButton::Instantiate,PButton::Create));
	fObjInfoList->AddItem(new PObjectInfo("PCheckBox","Checkbox",PCheckBox::Instantiate,
											PCheckBox::Create));
	fObjInfoList->AddItem(new PObjectInfo("PBox","Box",PBox::Instantiate,PBox::Create));
	fObjInfoList->AddItem(new PObjectInfo("PControl","Generic Control",PControl::Instantiate,
											PControl::Create));
	fObjInfoList->AddItem(new PObjectInfo("PLabel","Label",PLabel::Instantiate,PLabel::Create));
	fObjInfoList->AddItem(new PObjectInfo("PRadioButton","RadioButton",PRadioButton::Instantiate,
											PRadioButton::Create));
	fObjInfoList->AddItem(new PObjectInfo("PScrollBar","ScrollBar",PScrollBar::Instantiate,
											PScrollBar::Create));
	fObjInfoList->AddItem(new PObjectInfo("PListView","List",
											PListView::Instantiate,PListView::Create));
	fObjInfoList->AddItem(new PObjectInfo("PTextControl","Text Control",PControl::Instantiate,
											PTextControl::Create));
}


PObjectBroker::~PObjectBroker(void)
{
	fQuitting = true;
	
	delete fObjectList;
	delete fObjInfoList;
}


PObject *
PObjectBroker::MakeObject(const char *type, BMessage *msg)
{
	if (!type)
		return NULL;
	
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

