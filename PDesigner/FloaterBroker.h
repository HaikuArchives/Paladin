#ifndef FLOATERBROKER_H
#define FLOATERBROKER_H

#include "ObjectList.h"

class Floater;

class FloaterBroker
{
public:
							FloaterBroker(void);
							~FloaterBroker(void);
							
	static	FloaterBroker *	GetBrokerInstance(void);

private:
	BObjectList<Floater>	fList;
};

void InitFloaterSystem(void);
void ShutdownFloaterSystem(void);


#endif
