#ifndef SCMIMPORTWINDOW_H
#define SCMIMPORTWINDOW_H

#include "DWindow.h"

#include <Button.h>
#include <CheckBox.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <StringView.h>
#include <TextView.h>

#include "AutoTextControl.h"
#include "SCMImporter.h"

class SCMProjectImporter;

class SCMImportWindow : public DWindow
{
public:
						SCMImportWindow(void);
			void		MessageReceived(BMessage *msg);
			void		FrameResized(float w, float h);
			
private:
			void		SetProvider(SCMProjectImporter *importer);
			void		UpdateCommand(void);
			void		DoImport(void);
			
	BMenuField		*fProviderField;
	BMenuField		*fSCMField;
	
	AutoTextControl	*fProjectBox;
	BCheckBox		*fAnonymousBox;
	
	AutoTextControl	*fUserNameBox,
					*fRepository;
	
	BStringView		*fCommandLabel;
	BTextView		*fCommandView;
	
	BButton			*fOK;
	
	SCMProjectImporterManager	fProviderMgr;
	SCMProjectImporter			*fProvider;
};


#endif
