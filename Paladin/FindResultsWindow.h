#ifndef FINDRESULTSWINDOW_H
#define FINDRESULTSWINDOW_H

#include "DWindow.h"

#include 

class FindResultsWindow : public DWindow
{
public:
						FindResultsWindow(void);
			void		MessageReceived(BMessage *msg);
};


#endif
