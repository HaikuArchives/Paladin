/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "App.h"
#include "MainWindow.h"


App::App()
	:
	BApplication("application/x-vnd.dw-SymbolFinder")
{
	MainWindow* window = new MainWindow();
	window->Show();
}


int
main()
{
	App* app = new App();
	app->Run();
	delete app;

	return 0;
}
