#ifndef FLOATERBROKER_H
#define FLOATERBROKER_H

#include "ObjectList.h"
#include "PView.h"

class Floater;

class FloaterBroker
{
public:
							FloaterBroker(void);
							~FloaterBroker(void);
							
	static	FloaterBroker *	GetInstance(void);
	
			void			AttachAllFloaters(PView *view);
			void			DetachAllFloaters(void);
			void			NotifyFloaters(PView *view, const int32 &action);
			
private:
	BObjectList<Floater>	fList;
};

void InitFloaterSystem(void);
void ShutdownFloaterSystem(void);


#endif
