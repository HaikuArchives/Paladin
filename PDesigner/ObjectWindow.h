#ifndef OBJECTWINDOW_H
#define OBJECTWINDOW_H

#include <Window.h>
#include <Messenger.h>

class ObjectWindow : public BWindow
{
public:
	ObjectWindow(const BMessenger &target);

private:
	BMessenger	fMsgr;
};


#endif
