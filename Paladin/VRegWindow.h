#ifndef VREGWINDOW_H
#define VREGWINDOW_H

#include "DWindow.h"
#include <MenuBar.h>
#include <TextView.h>

#include "AutoTextControl.h"

class VRegWindow : public DWindow
{
public:
						VRegWindow(void);
			void		MessageReceived(BMessage *msg);
			void		FrameResized(float w, float h);
			
private:
			void		RunSearch(void);
	AutoTextControl		*fRegexBox;
	BTextView			*fSourceView,
						*fMatchView;
};

#endif

