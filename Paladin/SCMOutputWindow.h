#ifndef SCMOUTPUTWINDOW_H
#define SCMOUTPUTWINDOW_H

#include <Button.h>

#include "DWindow.h"
#include "TextView.h"

class SCMOutputWindow : public DWindow
{
public:
			SCMOutputWindow(const char *title);
			
	void		FrameResized(float w, float h);
	void		MessageReceived(BMessage *msg);
	BTextView *	GetTextView(void);
	
private:
	BTextView	*fLog;
	BButton		*fClose;
};


void SCMOutputCallback(const char *text);

#endif
