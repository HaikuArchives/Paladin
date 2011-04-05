#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <View.h>

class MainView : public BView
{
public:
						MainView(BRect frame);
		void			MessageReceived(BMessage *msg);
private:

};


#endif
