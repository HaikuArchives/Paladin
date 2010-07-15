#ifndef TERMINAL_WINDOW_H
#define TERMINAL_WINDOW_H

#include "DWindow.h"

#include <String.h>
#include <TextView.h>

class TerminalWindow : public DWindow
{
public:
			TerminalWindow(const char *commandline);
			~TerminalWindow(void);
	void	MessageReceived(BMessage *msg);
	void	RunCommand(void);
	void	FrameResized(float w, float h);
	
private:
	BTextView	*fTextView;
	BString		fCommand;
};

#endif
