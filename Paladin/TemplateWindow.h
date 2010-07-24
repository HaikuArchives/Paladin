#ifndef TEMPLATE_WINDOW_H
#define TEMPLATE_WINDOW_H

#include <Button.h>
#include <CheckBox.h>
#include <MenuField.h>
#include "DWindow.h"

#include "AutoTextControl.h"
#include "PathBox.h"
#include "TemplateManager.h"

class TemplateWindow : public DWindow
{
public:
				TemplateWindow(const BRect &frame);
				~TemplateWindow(void);
		bool	QuitRequested(void);
		void	MessageReceived(BMessage *msg);

private:
		void	CheckTemplates(void);
		BMenuField		*fTemplateField;
		
		AutoTextControl	*fNameBox;
		AutoTextControl	*fTargetBox;
		PathBox			*fPathBox;
		BCheckBox		*fCreateFolder;
		
		BButton			*fOK;
		TemplateManager	fTempList;
		BMenuField		*fSCMChooser;
};

#endif
