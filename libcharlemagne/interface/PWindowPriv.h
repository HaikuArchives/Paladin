#ifndef PWINDOW_PRIVATE
#define PWINDOW_PRIVATE

#include <Window.h>
#include "PObject.h"

class PWindowBackend : public BWindow
{
public:
				PWindowBackend(PObject *owner);
				~PWindowBackend(void);
	
	void		FrameMoved(BPoint pt);
	void		FrameResized(float w, float h);
	void		MenusBeginning(void);
	void		MenusEnded(void);
	void		WindowActivated(bool active);
	void		ScreenChanged(BRect frame, color_space mode);
	void		WorkspaceActivated(int32 workspace, bool active);
	void		WorkspacesChanged(uint32 oldspace, uint32 newspace);
	bool		QuitRequested(void);
	void		MessageReceived(BMessage *msg);
	
	void		SetCodeFeel(window_feel feel);
	window_feel	CodeFeel(void) const;
	
	PObject *	GetOwner(void);
	
private:
	PObject 	*fOwner;
	bool		fQuitFlag;
	window_feel	fCodeFeel;
};

#endif
