#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include "DWindow.h"

#include <FilePanel.h>
#include <ListView.h>

class ClickableStringView;
class BitmapButton;
class TypedRefFilter;

class StartWindow : public DWindow
{
public:
				StartWindow(void);
				~StartWindow(void);
		bool	QuitRequested(void);
		void	MessageReceived(BMessage *msg);
		
private:
		BitmapButton *	MakeButton(const char *name,
									const char *up, const char *down,
									int32 command);
		ClickableStringView *
						MakeLabel(BitmapButton *button, const char *label);
		
		BitmapButton	*fNewButton,
						*fOpenButton,
						*fOpenRecentButton,
						*fQuickImportButton,
						*fOnlineImportButton;
		
		BListView		*fListView;
		BFilePanel		*fOpenPanel;
		BFilePanel		*fImportPanel;
};

#endif
