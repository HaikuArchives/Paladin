#ifndef RUNARGS_WINDOW_H
#define RUNARGS_WINDOW_H

#include "DWindow.h"
#include "AutoTextControl.h"

class Project;

class RunArgsWindow : public DWindow
{
public:
			RunArgsWindow(Project *proj);
	bool	QuitRequested(void);

private:
	AutoTextControl	*fArgText;
	Project			*fProject;
};

#endif
