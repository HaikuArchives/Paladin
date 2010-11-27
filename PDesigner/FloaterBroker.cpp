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
FloaterBroker::GetBrokerInstance(void)
{
	return sFloaterBroker;
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


