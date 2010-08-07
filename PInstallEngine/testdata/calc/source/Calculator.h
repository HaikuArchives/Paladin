
#ifndef __CALCULATOR__
#define __CALCULATOR__

#include <Application.h>

class CalculatorApp : public BApplication
{

public:
					CalculatorApp();
virtual	void		AboutRequested();

private:

		BWindow		*wind;

};




#endif
