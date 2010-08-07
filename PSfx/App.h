#ifndef APP_H
#define APP_H

#include <Application.h>

#define M_QUIT_PROGRAM 'qupr'

class App : public BApplication
{
public:
			App(void);
	void	MessageReceived(BMessage *msg);
	void	RefsReceived(BMessage *msg);
	void	ReadyToRun(void);
};

void RegisterWindow(void);
void DeregisterWindow(void);
int32 CountRegisteredWindows(void);

#endif
