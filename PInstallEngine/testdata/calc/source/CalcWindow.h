
#ifndef __CALC_WINDOW__
#define __CALC_WINDOW__

#include <Window.h>
#include <Beep.h>

#include "CalcView.h"



class CalcWindow : public BWindow
{

public:
					CalcWindow(BPoint pt);
	virtual	bool	QuitRequested();

private:

	CalcView *calc_view;

};

#endif
