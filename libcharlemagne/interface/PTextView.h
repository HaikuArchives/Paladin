#ifndef PTEXTVIEW_H
#define PTEXTVIEW_H

#include "PView.h"



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


protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

};

#endif

