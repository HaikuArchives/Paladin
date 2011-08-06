#ifndef PMENU_H
#define PMENU_H

#include "PView.h"
#include <Menu.h>
#include <Window.h>
#include "PMenuItem.h"
#include "PMenuPriv.h"


class PMenuBackend;
class BMenu;

class PMenu : public PView
{
public:
							PMenu(void);
							PMenu(BMessage *msg);
							PMenu(const char *name);
							PMenu(const PMenu &from);
							~PMenu(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BMenu *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

