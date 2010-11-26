#ifndef HANDLEWINDOW_H
#define HANDLEWINDOW_H

#include <Messenger.h>
#include <Window.h>

enum
{
	M_HANDLE_MOVED = 'hnmv',
	M_RESIZE_MOVED = 'rsmv'
};

class HandleWindow : public BWindow
{
public:
	HandleWindow(const BRect &frame, BBitmap *bitmap, BMessenger msgr, int32 what);
};

#endif
