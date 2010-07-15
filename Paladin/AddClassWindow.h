#ifndef ADDCLASSWINDOW_H
#define ADDCLASSWINDOW_H

#include <ListView.h>
#include <ListItem.h>
#include <Window.h>


class AddClassWindow : public BWindow
{
public:
							AddClassWindow();

			void			MessageReceived(BMessage *msg);
private:
			void			PopulateList(void);
			
			BListView		*fClassList;
};

#endif
