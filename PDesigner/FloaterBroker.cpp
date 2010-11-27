#include "FloaterBroker.h"
#include "Floater.h"

static FloaterBroker *sFloaterBroker = NULL;

FloaterBroker::FloaterBroker(void)
	:	fList(20, true)
{
	fList.AddItem(new HandleFloater("ResizeHandle.png", FLOATER_RESIZE));
	fList.AddItem(new HandleFloater("MoveHandle.png", FLOATER_MOVE));
}


FloaterBroker::~FloaterBroker(void)
{
}

							
FloaterBroker *
FloaterBroker::GetInstance(void)
{
	return sFloaterBroker;
}


void
FloaterBroker::AttachAllFloaters(PView *view)
{
	for (int32 i = 0; i < fList.CountItems(); i++)
	{
		Floater *f = fList.ItemAt(i);
		f->FloaterAttached(view);
	}
}


void
FloaterBroker::DetachAllFloaters(void)
{
	for (int32 i = 0; i < fList.CountItems(); i++)
	{
		Floater *f = fList.ItemAt(i);
		f->FloaterDetached();
	}
}


void
FloaterBroker::NotifyFloaters(PView *view, const int32 &action)
{
	for (int32 i = 0; i < fList.CountItems(); i++)
	{
		Floater *f = fList.ItemAt(i);
		f->Update(view, action);
	}
}


void 
InitFloaterSystem(void)
{
	sFloaterBroker = new FloaterBroker();
}


void
ShutdownFloaterSystem(void)
{
	delete sFloaterBroker;
}


