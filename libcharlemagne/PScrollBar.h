#ifndef PSCROLLBAR_H
#define PSCROLLBAR_H

#include "PView.h"

/*
	Scrollbar Properties:
		LargeStep
		Min
		Max
		Orientation (read-only)
		Proportion
		SmallStep
		Target
		Value
*/

// This class exists to serve as a parent class for the H and V scrollbar child classes
class PScrollBar : public PView
{
public:
	virtual					~PScrollBar(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
protected:
							PScrollBar(void);
							PScrollBar(BMessage *msg);
							PScrollBar(const char *name);
							PScrollBar(const PScrollBar &from);
							
	virtual void			InitBackend(void);
	
private:
	void					InitProperties(void);

	PView					*fPViewTarget;
};

class PVScrollBar : public PView
{
public:
							PVScrollBar(void);
							PVScrollBar(BMessage *msg);
							PVScrollBar(const char *name);
							PVScrollBar(const PVScrollBar &from);
	virtual					~PVScrollBar(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(void);
	
private:
	void					InitPVScrollBar(void);
	void					InitProperties(void);
};



#endif
