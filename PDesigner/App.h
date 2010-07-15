#ifndef APP_H
#define APP_H

#include <Application.h>

enum
{
	M_DELETE_OBJECT = 'dlob'
};

class MainWindow;

class App : public BApplication
{
public:
			App(void);
	bool	QuitRequested(void);
	void	MessageReceived(BMessage *msg);

private:
	MainWindow	*fMainWin;
};

#endif
