#ifndef PTEXTVIEW_H
#define PTEXTVIEW_H

#include "PView.h"
#include <Application.h>
#include <TextView.h>
#include <stdio.h>
#include "PClipboard.h"
#include "PObjectBroker.h"
#include <Window.h>


class PTextViewBackend;
class BTextView;

class PTextView : public PView
{
public:
							PTextView(void);
							PTextView(BMessage *msg);
							PTextView(const char *name);
							PTextView(const PTextView &from);
							~PTextView(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BTextView *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

