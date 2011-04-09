#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>

#include <Button.h>
#include <MenuBar.h>
#include <TextView.h>


class MainWindow : public BWindow
{
public:
						MainWindow(void);
			void		MessageReceived(BMessage *msg);
			bool		QuitRequested(void);
			
private:
			BMenuBar	*fMenuBar;
			BButton		*fFind,
						*fReplace,
						*fReplaceFind,
						*fReplaceAll;
						
			BTextView	*fFindBox,
						*fReplaceBox;
};


#endif
