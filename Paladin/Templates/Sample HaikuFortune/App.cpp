#include "App.h"

#include <FindDirectory.h>
#include <OS.h>
#include <Path.h>
#include <stdlib.h>

#include "FortuneFunctions.h"
#include "MainWindow.h"

App::App(void)
  :	BApplication("application/x-vnd.test-HaikuFortune")
{
	BPath path;
	
	// We have to use an #ifdef here because the fortune files under R5
	// and Zeta are in the system/etc/ directory, but in Haiku they're
	// kept in the system/data directory.
	#ifdef __HAIKU__
	find_directory(B_SYSTEM_DATA_DIRECTORY,&path);
	#else
	find_directory(B_BEOS_ETC_DIRECTORY,&path);
	#endif
	
	path.Append("fortunes");
	gFortunePath = path.Path();
	
	// If we want the rand() function to actually be pretty close to random
	// we will need to seed the random number generator with the time. If we
	// don't, we will get the same "random" numbers each time the program is
	// run. 
	srand(system_time());
	
	MainWindow *win = new MainWindow();
	win->Show();
}


int
main(void)
{
	srand(system_time());
	
	App *app = new App();
	app->Run();
	delete app;
	return 0;
}
