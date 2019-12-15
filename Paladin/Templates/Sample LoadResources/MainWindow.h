#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>

class MainWindow : public BWindow
{
public:
						MainWindow(void);
			bool		QuitRequested(void);
};

#endif
