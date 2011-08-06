#ifndef PMENUPRIV_H
#define PMENUPRIV_H

#include "PObject.h"
#include <Menu.h>
#include <MenuItem.h>

class PMenuBackend : public BMenu
{
public:
			PMenuBackend(PObject *owner);

	void	FrameMoved(BPoint param1);
	void	FrameResized(float param1, float param2);
	void	KeyUp(const char *bytes, int32 count);
	void	MouseMoved(BPoint param1, uint32 param2, const BMessage * param3);
	void	Draw(BRect param1);
	void	AttachedToWindow(void);
	void	Pulse(void);
	void	MouseDown(BPoint param1);
	void	AllAttached(void);
	void	MakeFocus(bool param1);
	void	AllDetached(void);
	void	MouseUp(BPoint param1);
	void	WindowActivated(bool param1);
	void	DrawAfterChildren(BRect param1);
	void	DetachedFromWindow(void);
	void	KeyDown(const char *bytes, int32 count);

	PObject *	GetOwner(void) const
				{
					return fOwner;
				}
private:
	PObject *fOwner;
};


class PMenuItemBackend : public BMenuItem
{
public:
				PMenuItemBackend(PObject *owner);

	PObject *	GetOwner(void) const
				{
					return fOwner;
				}
private:
	PObject *fOwner;
};


#endif

