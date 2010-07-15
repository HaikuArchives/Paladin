#ifndef FLOATINGRESIZEHANDLE_H
#define FLOATINGRESIZEHANDLE_H

#include <Window.h>

class FloatingResizeHandle : public BWindow
{
public:
			FloatingResizeHandle(BView *target, float size);
};


#endif
