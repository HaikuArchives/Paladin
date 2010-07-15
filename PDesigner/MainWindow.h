#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>
#include <OutlineListView.h>

class ObjectWindow;
class Project;
class PropertyWindow;

class MainWindow : public BWindow
{
public:
					MainWindow(void);
			bool	QuitRequested(void);
			void	MessageReceived(BMessage *msg);
private:
			void	AddWindow(void);
			void	AddControl(const BString &type);
			void	MakeEmpty(void);
			void	UpdateProperties(void);
			
			BOutlineListView	*fListView;
			Project				*fProject;
			PropertyWindow		*fPropertyWin;
			ObjectWindow		*fObjectWin;
};

#endif
